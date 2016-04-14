#include "Hikvision.h"
#include <vector>
#include "utility.h"
#include "BufferedFile.h"

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

LONGLONG HIKV::HikVolume::GoToNextFrame(void)
{
	LONGLONG init_offset = io.Pointer();
	LONGLONG cur_offset = 0;
	BYTE signature[] = { 0x00, 0x00, 0x01 };
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

				return true;
			}		
		} else { 
			int x = 0;
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
	while ((offset = GoToNextFrame()) != -1) {

		while (ReadFrame(sequence.buffer, frame)) {
		
		
		}

	}
	return false;
}

void HIKV::SaveToFile(const std::string &file_name, std::vector<BYTE> &buffer)
{
	DWORD rw = 0;
	W32Lib::FileEx out_file(file_name.c_str());
	if (out_file.Open() || out_file.Create()) {
		if (out_file.SetPointer(0, FILE_END) == 0x00) {
			rw = ::GetLastError();		
		}
		rw = out_file.Write(buffer.data(), buffer.size());
		out_file.Close();
	}
}

int HIKV::StartRecovering(const std::string &dhfs_volume, const std::string &out_directory, const Timestamp &start_time, const Timestamp &end_time)
{
	DWORD rw = 0;
	HikVolume vol("\\\\.\\PhysicalDrive2");
	if (vol.Open()) {

		LONGLONG offset;
		FrameInfo frame;
		FrameSequence sequence;
		vol.SetPointer(353168518*512LL);

		while ((offset = vol.GoToNextFrame()) != -1) {

			while (vol.ReadFrame(sequence.buffer, frame)) {
				++sequence.frames_count;





				frame.Clear();
			}

			int x = 0;

		}
	}

	return 0;
}
