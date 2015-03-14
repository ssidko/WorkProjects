#include <assert.h>
#include "Frame.h"
#include <sstream>

using namespace DHFS;


void DHFS::_FrameInfo::ToString( std::string &info_str )
{
	std::stringstream stream;
	stream << offset << " ";
	stream << timestamp.String() << " ";
	stream << "camera: " << camera << " ";
	stream << "counter: " << counter << " ";
	stream << "\n";
	info_str = stream.str();
}

bool DHFS::_FrameSequenceInfo::IsYourNextFrame(FrameInfo &next_frame)
{
	LONGLONG delta_time = 2;
	if (start_frame.camera == next_frame.camera) {
		if (next_frame.timestamp.Seconds() >= end_frame.timestamp.Seconds()) {
			if ((next_frame.timestamp.Seconds() - end_frame.timestamp.Seconds()) <= delta_time) {
				return true;
			}
		}
	}
	return false;
}

DHFS::Volume::Volume(std::string &volume_file_name) : io(volume_file_name, 4096*1024)
{
}

bool DHFS::Volume::Open()
{
	return io.Open();
}


void DHFS::Volume::SetPointer( LONGLONG &pointer )
{
	io.SetPointer(pointer);
}

bool DHFS::Volume::IsValidHeader(FRAME_HEADER &header)
{
	if (header.magic == FRAME_HEADER_MAGIC) {
		if ((header.size > (sizeof(FRAME_HEADER) + sizeof(FRAME_FOOTER))) && (header.size <= FRAME_MAX_SIZE)) {
			return true;
		}
	}
	return false;
}

bool DHFS::Volume::ReadFrame(std::vector<BYTE> &buffer, FrameInfo &info)
{
	FRAME_HEADER *header = NULL;
	FRAME_FOOTER *footer = NULL;
	size_t size = buffer.size();
	LONGLONG offset = io.Pointer();

	info.Clear();
	buffer.resize(buffer.size() + sizeof(FRAME_HEADER), 0);
	if ( io.Read(&buffer[size], sizeof(FRAME_HEADER)) == sizeof(FRAME_HEADER) ) {
		header = (FRAME_HEADER *)&buffer[size];

		info.camera = header->camera;
		info.offset = offset;
		info.counter = header->counter;
		info.size = header->size;
		header->time.Timestamp(info.timestamp);

		if ( IsValidHeader(*header) ) {
			size_t tail_size = (header->size - sizeof(FRAME_HEADER));
			buffer.resize(buffer.size() + tail_size);
			if ( io.Read(&buffer[size + sizeof(FRAME_HEADER)], tail_size) == tail_size ) {
				footer = (FRAME_FOOTER *)&buffer[buffer.size() - sizeof(FRAME_FOOTER)];
				if ( (footer->magic == FRAME_FOOTER_MAGIC) && (footer->size == info.size) ) {
					return true;
				} else {
					info.Clear();
					return false;
				}
			}
		}
	}
	buffer.resize(size);
	io.SetPointer(offset);

	return false;
}

bool DHFS::Volume::NextFrame(std::vector<BYTE> &buffer, FrameInfo &info)
{
	LONGLONG offset = 0;
	DWORD header_magic = FRAME_HEADER_MAGIC;

	info.Clear();
	while ( (offset = io.Find((BYTE *)&header_magic, sizeof(header_magic))) >= 0 ) {
		if (ReadFrame(buffer, info)) {
			return true;
		} else {
			offset++;
			io.SetPointer(offset);
		}
	}
	return false;
}

bool DHFS::Volume::NextFrameSequence(std::vector<BYTE> &sequence_buffer, FrameSequenceInfo &sequence_info)
{
	FrameInfo frame_info;

	frame_info.Clear();
	sequence_info.Clear();
	sequence_buffer.clear();

	while (NextFrame(sequence_buffer, frame_info)) {
		if (sequence_info.frame_counter == 0) {
			sequence_info.start_frame = frame_info;
			sequence_info.end_frame = frame_info;
		}
		do {
			if (sequence_info.IsYourNextFrame(frame_info)) {
				sequence_info.frame_counter++;
				sequence_info.end_frame = frame_info;
				sequence_info.size += frame_info.size;
				continue;
			} else {
				if (sequence_info.frame_counter > 1) {
					sequence_buffer.resize(sequence_info.size);
					io.SetPointer(sequence_info.start_frame.offset + sequence_info.size);
					return true;
				} else {
					sequence_buffer.clear();
					sequence_info.Clear();
					break;
				}
			}
		} while (ReadFrame(sequence_buffer, frame_info));
	}

	return false;
}

void DHFS::Volume::Test(void)
{
	//FrameInfo info;
	FrameSequenceInfo info;
	std::vector<BYTE> buffer;
	buffer.reserve(4*1024*1024);

	while (NextFrameSequence(buffer, info)) {
		buffer.clear();
		int x = 0;
	}
}

#include "File.h"

void DHFS::Volume::SaveFrameInfo(void)
{
	FrameInfo frame_info;
	std::string str;
	std::vector<BYTE> buffer;
	buffer.reserve(4*1024*1024);

	W32Lib::FileEx log("E:\\37025\\frames.txt");
	if (log.Create()) {
		while (NextFrame(buffer, frame_info)) {
			frame_info.ToString(str);
			log.Write((void *)str.c_str(), str.size());
			str.clear();
			buffer.clear();
		}
	}
}
