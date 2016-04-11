#include "Hikvision.h"
#include <vector>
#include "utility.h"
#include "BufferedFile.h"

DWORD HIKV::FRAME_HEADER::DataSize()
{
	DWORD size = 0;
	switch ( type ) {
	case 0xBA :
		return 16;
	case 0xBC :
	case 0xBD :
	case 0xE0 :
		return BeToLe((WORD)data_size);
	default :
		return 0;	
	}	
}

LONGLONG HIKV::HikVolume::GoToNextFrame(void)
{
	LONGLONG init_offset = io.Pointer();
	LONGLONG cur_offset = 0;
	BYTE signature[] = { 0x00, 0x00, 0x01 };

	while (cur_offset = io.Find(signature, sizeof(signature))) {	
	
	
		int x = 0;	
	}



	return -1;
}

bool HIKV::HikVolume::ReadFrame(std::vector<BYTE> &buffer)
{
	FRAME_HEADER *fheader = nullptr;
	size_t initial_size = buffer.size();
	LONGLONG initial_offset = io.Pointer();
	DWORD data_size = 0;

	buffer.resize(buffer.size() + sizeof(FRAME_HEADER));	
	if (io.Read(buffer.data(), sizeof(FRAME_HEADER)) != sizeof(FRAME_HEADER)) {
		fheader = (FRAME_HEADER *)&buffer[initial_size];
		data_size = fheader->DataSize();
		if (data_size) {
			
		
		
		
		}
	}

	io.SetPointer(initial_offset);
	buffer.resize(initial_size);

	return false;
}

int HIKV::StartRecovering(const std::string &dhfs_volume, const std::string &out_directory, const Timestamp &start_time, const Timestamp & end_time)
{
	BYTE signature[] = { 0x00, 0x00, 0x01 };
	LONGLONG pos = 0;
	HikVolume vol("\\\\.\\PhysicalDrive2");
	if (vol.Open()) {
		vol.SetPointer(156400 * 512LL);

		while ((pos = vol.GoToNextFrame()) != -1) {
		


			vol.SetPointer(++pos);
		
		}	
	}

	return 0;
}
