#include "WfsScanner.h"

using namespace WFS;

#define IO_BUFFER_SIZE				(DWORD)256*512
#define BUFFER_SIZE					(DWORD)32*512	

WFS::Scanner::Scanner(const std::string &file_name) : io(file_name, IO_BUFFER_SIZE), buffer_size(BUFFER_SIZE), buffer(nullptr)
{
}

WFS::Scanner::~Scanner()
{
	Close();
}

BOOL WFS::Scanner::Open(void)
{
	buffer = new BYTE[buffer_size];
	if (io.Open() && (buffer != nullptr)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void WFS::Scanner::Close(void)
{
	if (buffer != nullptr) {
		delete[] buffer;
		buffer = nullptr;
	}
}

BOOL WFS::Scanner::ReadFrame(FrameSequence &seq, FrameInfo &readed_frame)
{
	DWORD first_read_count = MAX_FRAME_HEADER_SIZE;
	DWORD second_read_count = 0;
	DWORD original_size = seq.buffer.size();
	FRAME_HEADER *header = nullptr;
	FRAME_0D *frame_0d = nullptr;
	FRAME_0A *frame_0a = nullptr;
	FRAME_0C *frame_0c = nullptr;

	readed_frame.Clear();
	io.Pointer(readed_frame.offset);
	
	seq.buffer.resize(original_size + first_read_count);
	if (io.Read(&seq.buffer[original_size], first_read_count) == first_read_count) {

		header = (FRAME_HEADER *)&seq.buffer[original_size];
		readed_frame.type = header->type;
		if (header->IsValid()) {

			switch (readed_frame.type)
			{
			case WFS::kType_0D:
				frame_0d = (FRAME_0D *)&seq.buffer[original_size];
				readed_frame.size = frame_0d->data_size + sizeof(FRAME_0D);
				break;
			case WFS::kType_0A:
				frame_0a = (FRAME_0A *)&seq.buffer[original_size];
				readed_frame.size = frame_0a->data_size + sizeof(FRAME_0A);
				break;
			case WFS::kType_0C:
				frame_0c = (FRAME_0C *)&seq.buffer[original_size];
				readed_frame.size = frame_0c->data_size + sizeof(FRAME_0C);
				break;
			default:
				seq.buffer.resize(original_size);
				io.SetPointer(readed_frame.offset);
				return FALSE;
			}

			if (readed_frame.size < MAX_FRAME_SIZE) {
				seq.buffer.resize(original_size + readed_frame.size);
				if (readed_frame.size > first_read_count) {
					second_read_count = readed_frame.size - first_read_count;
					if (io.Read(&seq.buffer[original_size + first_read_count], second_read_count) != second_read_count) {
						seq.buffer.resize(original_size);
						io.SetPointer(readed_frame.offset);
						return FALSE;
					}
				} else {
					io.SetPointer(readed_frame.offset + readed_frame.size);
				}
				seq.frame_counter++;
				seq.size += readed_frame.size;
				if (readed_frame.type == WFS::kType_0C) {
					frame_0c = (FRAME_0C *)&seq.buffer[original_size];
					if (seq.start_time.Seconds() == 0x00) {
						seq.start_time.seconds = frame_0c->time.seconds;
						seq.start_time.minutes = frame_0c->time.minutes;
						seq.start_time.hours = frame_0c->time.hours;
						seq.start_time.day = frame_0c->time.day;
						seq.start_time.month = frame_0c->time.month;
						seq.start_time.year = frame_0c->time.year + 2000;

						seq.offset = readed_frame.offset;
					}
					seq.end_time.seconds = frame_0c->time.seconds;
					seq.end_time.minutes = frame_0c->time.minutes;
					seq.end_time.hours = frame_0c->time.hours;
					seq.end_time.day = frame_0c->time.day;
					seq.end_time.month = frame_0c->time.month;
					seq.end_time.year = frame_0c->time.year + 2000;
				}
				return TRUE;
			}
		}		
	}

	seq.buffer.resize(original_size);
	io.SetPointer(readed_frame.offset);
	return FALSE;
}

void WFS::Scanner::SetPointer(const LONGLONG &offset)
{
	if (!io.SetPointer(offset)) {
		throw std::exception("Error: WFS::Scanner::SetPointer().");
	}
}

BOOL WFS::Scanner::NextFrameHeader(FrameInfo &info)
{
	DWORD rw = 0;
	DWORD *sign = nullptr;
	DWORD sign_size = sizeof(DWORD);
	DWORD header_type = WFS::kUnknownType;

	if (!buffer) { return FALSE; }	
	memset(buffer, 0x00, buffer_size);

	io.Pointer(info.offset);
	while ((rw = io.Read(buffer, buffer_size)) >= sign_size) {
		for (DWORD i = 0; (i + sign_size) < rw; ++i) {
			sign = (DWORD *)&buffer[i];
			if ((*sign & WFS_FRAME_HEADER_SIGNATURE_MASK) == WFS_FRAME_HEADER_SIGNATURE) {
				io.SetPointer(info.offset);
				info.type = buffer[i + 3] & 0x0F;
				return TRUE;
			}		
			++info.offset;
		}
		io.SetPointer(info.offset);
	}
	return FALSE;
}

BOOL WFS::Scanner::NextFrameWithTimestamp(void)
{
	FrameInfo info;
	while (NextFrameHeader(info)) {
		if (info.type == WFS::kType_0C) {
			return TRUE;
		} else {
			io.SetPointer(++info.offset);
		}
	}
	return FALSE;
}

BOOL WFS::Scanner::NextFrameSequence(FrameSequence &sequence)
{
	FrameInfo frame;
	LONGLONG prev_offset = 0;

	sequence.Clear();
	while (NextFrameWithTimestamp()) {
		while (ReadFrame(sequence, frame)) {
			prev_offset = frame.offset;
		}
		if (sequence.frame_counter) {
			return TRUE;
		}
		sequence.Clear();
		io.SetPointer(io.Pointer() + 1);
	}
	return FALSE;
}
