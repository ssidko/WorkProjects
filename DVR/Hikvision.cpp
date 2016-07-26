#include "Hikvision.h"
#include <vector>
#include "utility.h"
#include "BufferedFile.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <ios>

bool HIKV::FRAME_HEADER::IsValid(void)
{
	if ((signature[0] == 0x00) && (signature[1] == 0x00) && (signature[2] == 0x01)) {
		switch (type) {
		case 0xBA:
		case 0xBC:
		case 0xBD:
		case 0xE0:
			return true;
		default:
			return false;
		}	
	}
	return false;
}

void HIKV::FrameSequence::Clear(void)
{
	offset = 0;
	frames_count = 0;
	start_time.Clear();
	end_time.Clear();
	buffer.clear();
}

LONGLONG HIKV::HikVolume::FindNextFrame(void)
{
	LONGLONG init_offset = io.Pointer();
	LONGLONG cur_offset = 0;
	BYTE signature[] = { 0x00, 0x00, 0x01, 0xBA };
	return io.Find(signature, sizeof(signature));
}


bool HIKV::HikVolume::ReadFrame(std::vector<BYTE> &buffer, FrameInfo &frame)
{
	FRAME_HEADER *header = nullptr;
	size_t origin_size = buffer.size();
	size_t data_pos = origin_size + sizeof(FRAME_HEADER);
	LONGLONG offset = io.Pointer();
	DWORD data_size = 0;

	buffer.resize(buffer.size() + sizeof(FRAME_HEADER));	
	if (io.Read(&buffer[origin_size], sizeof(FRAME_HEADER)) == sizeof(FRAME_HEADER)) {
		header = (FRAME_HEADER *)&buffer[origin_size];
		if (header->IsValid()) {
			if (header->type == 0xBA) {
				data_size = 16;
				frame.full_size = sizeof(FRAME_HEADER) + data_size;
			} else {
				buffer.resize(buffer.size() + sizeof(WORD));
				if (io.Read(&buffer[origin_size + sizeof(FRAME_HEADER)], sizeof(WORD)) != sizeof(WORD)) {
					goto _error;
				}
				data_size = BeToLe(*((WORD *)&buffer[origin_size + sizeof(FRAME_HEADER)]));
				if (data_size == 0x00) {
					goto _error;
				}
				data_pos = origin_size + sizeof(FRAME_HEADER) + sizeof(WORD);
				frame.full_size = sizeof(FRAME_HEADER) + sizeof(WORD) + data_size;
			}
			buffer.resize(buffer.size() + data_size);
			if (io.Read(&buffer[data_pos], data_size) == data_size) {
				header = (FRAME_HEADER *)&buffer[origin_size];
				frame.offset = offset;
				frame.frame_type = header->type;
				frame.data_size = data_size;
				if (header->type == 0xBC) {
					if ( (*(WORD *)(&buffer[data_pos + 6]) == 0x4B48) && (*(WORD *)(&buffer[data_pos + 22]) == 0x4B48) ) {						
						DWORD raw = BeToLe(*((DWORD *)&buffer[data_pos + 10]));
						TIMESTAMP *stmp = (TIMESTAMP *)&raw;
						frame.time_stamp = stmp->TimeStamp();
					}				
				}
				return true;
			}		
		}
	}

_error:

	io.SetPointer(offset);
	buffer.resize(origin_size);

	return false;
}

bool HIKV::HikVolume::NextFrameSequence(FrameSequence &sequence)
{
	LONGLONG offset;
	FrameInfo frame;
	while ((offset = FindNextFrame()) != -1) {

		while (ReadFrame(sequence.buffer, frame)) {
		
		
		}

	}
	return false;
}

bool HIKV::HikVolume::SaveFrameSequenceToFile(std::string &file_name, FrameSequence &sequence)
{
	FrameInfo frame;
	size_t buffer_max_size = 100 * 1024 * 1024;
	LONGLONG last_frame_offset = 0;

	W32Lib::FileEx out_file(file_name.c_str());
	if (!out_file.Create()) {
		throw std::ios_base::failure("Error create file.");
	}

	frame.Clear();
	sequence.Clear();

	while (ReadFrame(sequence.buffer, frame)) {

		last_frame_offset = frame.offset;

		if (sequence.frames_count == 0) {
			sequence.offset = frame.offset;
		}
		
		if (frame.time_stamp.Seconds() != 0) {
			if (sequence.start_time.Seconds() == 0) {
				sequence.start_time = frame.time_stamp;
			}
			sequence.end_time = frame.time_stamp;		
		}
	
		++sequence.frames_count;

		if (sequence.buffer.size() >= buffer_max_size) {
			out_file.Write(sequence.buffer.data(), sequence.buffer.size());
			sequence.buffer.clear();
		}
	}

	if (sequence.buffer.size()) {
		out_file.Write(sequence.buffer.data(), sequence.buffer.size());
		sequence.buffer.clear();
	}

	if (last_frame_offset) {
		SetPointer(last_frame_offset + 1);
	}

	return (sequence.frames_count != 0);
}

bool HIKV::HikVolume::SaveFramesInfoToFile(std::string &file_name) 
{
	std::fstream file(file_name.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
	if (!file.is_open()) {
		return false;
	}

	LONGLONG offset = 0;
	FrameInfo frame;
	std::vector<BYTE> buffer;
	char lba_str[32] = { 0 };

	while ((offset = FindNextFrame()) != -1) {
		while (ReadFrame(buffer, frame)) {

			switch (frame.frame_type) {
			case kHikPrivateData_1:
				sprintf_s(lba_str, sizeof(lba_str), "%011lld: ", frame.offset / 512);
				file << lba_str << frame.time_stamp.String() << "\n";
				break;
			case kPS_frame:
				break;
			case kPES_frame:
				break;
			case kTypeCode_BD:
				break;
			default:
				break;
			}
			buffer.clear();
			file.flush();
		}
		SetPointer(frame.offset + 1);
	}

	file.flush();
	file.close();

	return true;
}

int HIKV::StartRecovering(const std::string &dhfs_volume, const std::string &out_directory, const dvr::Timestamp &start_time, const dvr::Timestamp &end_time)
{
	DWORD error;
	DWORD rw = 0;
	HikVolume vol(dhfs_volume.c_str());
	if (vol.Open()) {

		LONGLONG offset;
		LONGLONG file_counter = 0;
		FrameInfo frame;
		FrameSequence sequence;
		std::string file_name = out_directory + "temp.h264";

		//vol.SetPointer(353168518*512LL);

		while ((offset = vol.FindNextFrame()) != -1) {

			if (!vol.SaveFrameSequenceToFile(file_name, sequence)) {

				vol.SetPointer(offset + 1);	

			} else if (sequence.end_time.Seconds() - sequence.start_time.Seconds()) {
				
				std::stringstream new_name;
				new_name << out_directory << sequence.start_time.String() << "-=-" << sequence.end_time.String() << "--[" << sequence.offset << "]" << ".avi";
				//new_name << out_directory << "[" << file_counter++ << "]--" << sequence.start_time.String() << "-=-" << sequence.end_time.String() << ".h264";

				::Convert2Avi(file_name, new_name.str());

				//W32Lib::FileEx out_file(file_name.c_str());
				//if (out_file.Open()) {					
				//	out_file.Close();
				//	bool result = out_file.Rename(new_name.str().c_str());
				//	if (!result) {
				//		error = ::GetLastError();
				//	}	

				//	int x = 0;
				//}
			
			}
		}
	}

	return 0;
}
