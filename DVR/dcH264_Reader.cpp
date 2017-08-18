#include "dcH264_Reader.h"
#include <assert.h>

using namespace dcH264;

int dcH264::main(void)
{
	FRAME_HEADER *hdr = nullptr;
	dvr::Frame frame;
	LONGLONG disk_size = 1953525168ll * 512ll;

	Reader reader("\\\\.\\PhysicalDrive2", disk_size);
	if (reader.Open()) {

		std::vector<uint8_t> frame_buffer;
		reader.SetOffset(0x6C71A83780);
		if (reader.ReadFrame(frame)) {
			hdr = (FRAME_HEADER *)&frame.buffer[0];

			auto paylad = hdr->Payload();

			int x = 0;
		
		
		}
		int x = 0;
	}

	return 0;
}

inline LONGLONG Align(const LONGLONG &value, size_t align)
{
	return  ((value + (align - 1)) & ~(align - 1));
}

dcH264::Reader::Reader(const std::string &file_name, const LONGLONG &file_size) : io(file_name, 128 * 1024, file_size)
{
}

bool dcH264::Reader::Open()
{
	return io.Open();
}

bool dcH264::Reader::SetOffset(const LONGLONG &offset)
{
	return io.SetPointer(offset);
}

LONGLONG dcH264::Reader::Offset(void)
{
	return io.Pointer();
}

bool dcH264::Reader::ReadFrame(dvr::Frame &frame)
{
	FRAME_HEADER *hdr = nullptr;

	frame.Clear();
	frame.offset = io.Pointer();

	//
	// Read FRAME_HEADER
	//

	frame.buffer.resize(sizeof(FRAME_HEADER));
	hdr = (FRAME_HEADER *)&frame.buffer[0];

	if (sizeof(FRAME_HEADER) != io.Read(&frame.buffer[0], sizeof(FRAME_HEADER))) {
		goto _error;
	}
	if (!hdr->IsValid()) {
		goto _error;
	}

	//
	// Read FRAME_PAYLOAD
	//
	
	size_t frame_size = hdr->FrameSize();

	frame.buffer.resize(frame_size);
	hdr = (FRAME_HEADER *)&frame.buffer[0];

	if (frame_size > sizeof(FRAME_HEADER)) {		
		size_t tail_size = frame_size - sizeof(FRAME_HEADER);
		if (tail_size != io.Read(&frame.buffer[sizeof(FRAME_HEADER)], tail_size)) {
			goto _error;
		}		
	}

	//
	// Align file pointer.
	//

	if (!io.SetPointer(Align(frame.offset + frame_size, 8))) {
		goto _error;
	}

	frame.camera = hdr->camera - '0';
	if ((hdr->frame_type == 'cd') && (hdr->subtype == '0')) {
		frame.time = hdr->dc.subtype_0.timestamp.Timestamp();		
	}

	return true;

_error:
	frame.Clear();
	return false;
}

bool AppendFrameToSequence(dvr::FrameSequence &sequence, dvr::Frame &frame)
{
	FRAME_HEADER *header = (FRAME_HEADER *)&frame.buffer[0];

	if (sequence.frames_count == 0) {
		sequence.camera = frame.camera;
	}

	if (sequence.camera == frame.camera) {
	
		if (frame.time.Seconds()) {
		
		
		
		
		}



		size_t old_size = sequence.buffer.size();
		sequence.buffer.resize(old_size + header->PayloadSize());
		std::memcpy(&sequence.buffer[old_size], header->Payload(), header->PayloadSize());
	}

	return false;
}

bool dcH264::Reader::ReadFrameSequence(dvr::FrameSequence &sequence)
{
	dvr::Frame frame;
	FRAME_HEADER *header = nullptr;

	sequence.Clear();

	if (ReadFrame(frame)) {

		header = (FRAME_HEADER *)&frame.buffer[0];

		sequence.offset = frame.offset;
		sequence.camera = frame.camera;
		sequence.frames_count += 1;
		sequence.start_time = frame.time;
		sequence.end_time = frame.time;

		sequence.buffer.resize(header->PayloadSize());
		std::memcpy(&sequence.buffer[0], header->Payload(), header->PayloadSize());
	
		while (ReadFrame(frame)) {

			header = (FRAME_HEADER *)&frame.buffer[0];

			if (frame.time.Seconds()) {
				
				if (sequence.start_time.Seconds == 0) {
					sequence.start_time = frame.time;
				}
						
			
			}


			size_t old_size = sequence.buffer.size();
			sequence.buffer.resize(old_size + header->PayloadSize());
			std::memcpy(&sequence.buffer[old_size], header->Payload(), header->PayloadSize());

		}
	}


	return sequence.frames_count ? true : false;
}