#include "dcH264_Reader.h"
#include <assert.h>
#include <algorithm>
#include "utility.h"

using namespace dcH264;

int dcH264::main(void)
{
	FRAME_HEADER *hdr = nullptr;
	dvr::Frame frame;
	std::string out_dir = "F:\\43035\\";
	std::string drive_name = "\\\\.\\PhysicalDrive0";
	LONGLONG disk_size = GetPhysicalDriveSize(drive_name);
	LONGLONG start_offset = 16046629LL * 512;

	Reader reader(drive_name, disk_size);
	if (reader.Open()) {

		dvr::FrameSequence sequence;

		size_t max_frame_size = 0;
		size_t min_frame_size = -1;
		reader.SetOffset(start_offset);

		const size_t cam_id = 5;
		const LONGLONG file_max_size = 50 * 1024 * 1024;
		W32Lib::FileEx *out_file = nullptr;
		dvr::Timestamp last_timestamp;
		
		while (reader.ReadFrameSequence(sequence)) {

			if (sequence.camera == cam_id) {
				
				if (out_file) {
					if (out_file->GetSize() >= file_max_size) {
						delete out_file;
						out_file = nullptr;
					}
				}
			
				if (out_file == nullptr) {
					std::string file_name = std::to_string(sequence.camera) + " -=- " + sequence.start_time.ToString() + " -=- " + std::to_string((long long)sequence.offset) + ".h264";
					std::string file_path = out_dir + file_name;
					out_file = new W32Lib::FileEx(file_path.c_str());
					if (!out_file->Create()) {
						throw std::exception();
					}
				}

				if (sequence.start_time) {
					if (!last_timestamp) {
						last_timestamp = sequence.end_time;
					} else {					
						if (sequence.end_time < last_timestamp) {
							throw std::exception();						
						}
						last_timestamp = sequence.end_time;
					}
				}

				out_file->Write(sequence.buffer.data(), sequence.buffer.size());

			
			
			} else {

				//int x = 0;
				//std::string file_name = std::to_string(sequence.camera) + " -=- " + sequence.start_time.ToString() + " -- " + sequence.end_time.ToString() + " -=- " + std::to_string((long long)sequence.offset) + ".h264";
				//std::string file_path = out_dir + file_name;
				//W32Lib::FileEx out_file(file_path.c_str());
				//if (out_file.Create()) {
				//	out_file.Write(sequence.buffer.data(), sequence.buffer.size());
				//}
				//int y = 0;

				continue;

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
	if (frame_size > 0xFFFFFF) {
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

		uint32_t start_prefix_code = *(uint32_t *)hdr->Payload();	
		//
		// start_prefix_code can be x00x00x00x01 or x00x00x01
		//
		if (start_prefix_code != 0x01000000 && (start_prefix_code & 0x00FFFFFF) != 0x00010000) {
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
	size_t trusted_size = 0;
	LONGLONG last_frame_offset = 0;

	while (GoToNextFrame()) {
		last_frame_offset = io.Pointer();
		if (ReadFrame(tmp_frame)) {

			AppendFirstFrame(sequence, tmp_frame);
			last_frame_offset = tmp_frame.offset;
			trusted_size = 0;

			while (ReadFrame(tmp_frame)) {
				
				last_frame_offset = tmp_frame.offset;
				trusted_size = sequence.buffer.size();
			
				if (sequence.buffer.size() + tmp_frame.buffer.size() > max_size) {
					break;
				}
				if (sequence.camera != tmp_frame.camera) {
					break;
				}
				if (tmp_frame.time && sequence.end_time) {
					if (tmp_frame.time < sequence.end_time) {
						break;
					}
				}

				AppendFrame(sequence, tmp_frame);

			}

			//
			// Если последний фрейм был прочитан успешно необходимо файловый указатель
			// установить на него чтобы он был прочитан последующим чтением.
			//
			if (tmp_frame.buffer.size()) {
				io.SetPointer(last_frame_offset);			
			} else {
				io.SetPointer(last_frame_offset + 1);
			}

			if (sequence.frames_count > 2) {
				// 
				// Отбрасываем последний фрейм т.к. в большинстве случаев он повреждён.
				//
				sequence.frames_count -= 1;
				sequence.buffer.resize(trusted_size);
				return true;			
			}
		} else {
			io.SetPointer(last_frame_offset + 1);
		}			
	}

	return false;
}

void dcH264::AppendFirstFrame(dvr::FrameSequence & sequence, dvr::Frame & frame)
{
	sequence.Clear();
	sequence.offset = frame.offset;
	sequence.camera = frame.camera;
	sequence.frames_count = 0;
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
	size_t origin_size = sequence.buffer.size();
	FRAME_HEADER *header = (FRAME_HEADER *)&frame.buffer[0];


	//
	// Save without header
	// 
	sequence.buffer.resize(origin_size + header->PayloadSize());
	std::memcpy(&sequence.buffer[origin_size], header->Payload(), header->PayloadSize());

	//
	// Save with header
	// 
	//sequence.buffer.resize(origin_size + frame.buffer.size());
	//std::memcpy(&sequence.buffer[origin_size], frame.buffer.data(), frame.buffer.size());
}