#include "dcH264_Reader.h"
#include <assert.h>
#include <algorithm>
#include "utility.h"

using namespace dcH264;

int dcH264::main(void)
{
	FRAME_HEADER *hdr = nullptr;
	dvr::Frame frame;
	std::string drive_name = "\\\\.\\PhysicalDrive0";
	LONGLONG disk_size = GetPhysicalDriveSize(drive_name);
	LONGLONG start_offset = 16046629LL * 512;

	Reader reader(drive_name, disk_size);
	if (reader.Open()) {

		size_t max_frame_size = 0;
		reader.SetOffset(start_offset);
		
		while (reader.GoToNextFrame()) {
		
			LONGLONG frame_offset = reader.Offset();
			LONGLONG last_frame_offset = frame_offset;
			size_t last_frame_size = 0;

			try {
			
				while (reader.ReadFrame(frame)) {
					max_frame_size = std::max<size_t>(last_frame_size, max_frame_size);
					last_frame_offset = frame.offset;
					last_frame_size = frame.buffer.size();
					int x = 0;
				}

				reader.SetOffset(last_frame_offset + 1);

			} catch (std::exception& ex) {

				int x = 0;
				
			}		
		
		}

	}

	return 0;
}

inline LONGLONG Align(const LONGLONG value, LONGLONG align)
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

bool dcH264::Reader::GoToNextFrame()
{
	const size_t chunk_size = 512;
	std::vector<uint8_t> buffer(chunk_size, 0x00);
	dcH264::FRAME_HEADER *header = nullptr;
	
	// Align io offset
	LONGLONG offs = io.Pointer();
	LONGLONG aligned = Align(offs, 8);
	io.SetPointer(Align(io.Pointer(), 8));
	
	size_t readed = 0;
	LONGLONG offset = io.Pointer();
	while (readed = io.Read(buffer.data(), buffer.size())) {
		
		size_t loop = readed / 8;
		for (size_t i = 0; i < loop; i++) {

			header = (dcH264::FRAME_HEADER *)&buffer[i * 8];
			if (header->IsValid()) {				
				io.SetPointer(offset);					
				return true;
			}
			offset += 8;
		}
		offset = io.Pointer();
	}
	return false;
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

	frame.camera = hdr->camera - '0';
	if ((hdr->frame_type == 'cd') && (hdr->subtype == '0')) {
		if (hdr->dc.subtype_0.timestamp.IsValid()) {
			frame.time = hdr->dc.subtype_0.timestamp.Timestamp();
		} else {
			goto _error;
		}
	}

	//
	// Read FRAME_PAYLOAD
	//
	
	size_t frame_size = hdr->FrameSize();
	if (frame_size == 0 || frame_size > 0xFFFFFF) {
		goto _error;
	}

	frame.buffer.resize(frame_size);
	hdr = (FRAME_HEADER *)&frame.buffer[0];

	if (frame_size > sizeof(FRAME_HEADER)) {		
		size_t tail_size = frame_size - sizeof(FRAME_HEADER);
		if (tail_size != io.Read(&frame.buffer[sizeof(FRAME_HEADER)], tail_size)) {
			goto _error;
		}		
	}

	if (hdr->frame_type == 'cd') {
		if (hdr->PayloadSize() < 4) {
			goto _error;
		}
		uint32_t *start_prefix = (uint32_t *)hdr->Payload();
		if (*start_prefix != 0x1000000) {
			goto _error;
		}		
	}

	//
	// Align file pointer to 8 byte.
	//

	if (!io.SetPointer(Align(frame.offset + frame_size, 8))) {
		goto _error;
	}

	return true;

_error:
	frame.Clear();
	return false;
}

bool dcH264::Reader::ReadFrameSequence(dvr::FrameSequence &sequence, size_t max_size)
{
	dvr::Frame frame;

	while (GoToNextFrame()) {
		if (ReadFrame(frame)) {
			AppendFirstFrame(sequence, frame);
			while (ReadFrame(frame)) {
			
			
				if (sequence.camera == frame.camera) {
				
					AppendFrame(sequence, frame);
				}
			
			
			}


		}
	
	}



	return sequence.frames_count ? true : false;
}

void dcH264::AppendFirstFrame(dvr::FrameSequence & sequence, dvr::Frame & frame)
{
	sequence.Clear();
	sequence.offset = frame.offset;
	sequence.camera = frame.camera;
	AppendFrame(sequence, frame);
}

void dcH264::AppendFrame(dvr::FrameSequence & sequence, dvr::Frame & frame)
{
	sequence.frames_count += 1;
	if (frame.time) {
		if (!sequence.start_time) {
			sequence.start_time = frame.time;
		} 
		sequence.end_time = frame.time;
	}
	FRAME_HEADER *header = (FRAME_HEADER *)&frame.buffer[0];
	sequence.buffer.resize(header->PayloadSize());
	std::memcpy(&sequence.buffer[0], header->Payload(), header->PayloadSize());
}