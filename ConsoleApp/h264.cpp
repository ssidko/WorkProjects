#include "h264.h"

DWORD h264dvr::min_time = 0;
DWORD h264dvr::max_time = 0;

template<typename T, size_t BUFFER_SIZE>
LONGLONG FindInFile(T &file_io, BYTE *str, DWORD size)
{
	if (!str || !size || (size > BUFFER_SIZE)) return -1;

	BYTE r_buffer[BUFFER_SIZE];
	DWORD rw;
	LONGLONG saved_fptr;
	LONGLONG curr_fprt;

	file_io.GetPointer(saved_fptr);
	curr_fprt = saved_fptr;

	while ((rw = file_io.Read(r_buffer, BUFFER_SIZE)) && (rw >= size))
	{
		for (DWORD i = 0; (i + size) <= rw; i++) {
			if (CmpMemBlck(&r_buffer[i], str, size)) {
				curr_fprt += i;
				file_io.SetPointer(curr_fprt);
				return curr_fprt;			
			}		
		}
		curr_fprt += (rw - size + 1);
		file_io.SetPointer(curr_fprt);
	}

	// Восстанавливаем файловый указатель
	file_io.SetPointer(saved_fptr);

	return -1;
}

h264dvr::PHEADER h264dvr::ReadFrame(FileEx &file, std::vector<BYTE> &frame)
{
	h264dvr::PHEADER header = NULL;
	size_t frame_start_index = frame.size();
	frame.resize(frame_start_index + sizeof(HEADER));
	if (file.Read(&frame[frame_start_index], sizeof(HEADER)) == sizeof(HEADER)) {
		header = (PHEADER)&frame[frame_start_index];
		if (header->IsValidHeader()) {

			frame.resize(frame_start_index + sizeof(HEADER) + header->data_size);
			header = (PHEADER)&frame[frame_start_index];
			if (file.Read(&frame[frame_start_index + sizeof(HEADER)], header->data_size) == header->data_size) {
				header = (PHEADER)&frame[frame_start_index];
				return header;
			} else {
				frame.resize(frame_start_index);
				return NULL;
			}			
		}	
	}
	return false;
}

void h264dvr::SaveAllTimestampsToFile(void)
{
	FileEx out_file("K:\\38023\\timestamps.bin");
	BufferedFile disk("\\\\.\\PhysicalDrive0");
	if (disk.Open() && out_file.Create()) {
	
		LONGLONG offset = 0;
		BYTE marker[] = { 0x00, 0x00, 0x00, 0x00, 0x01 };
		std::vector<BYTE> frame;
		PHEADER header = NULL;
	
		while ( (offset = FindInFile<BufferedFile, 32*1024>(disk, marker, sizeof(marker))) > 0 ) {
			int x = 0;
			disk.SetPointer(offset + 1);
		
		
		}
	
	
	}
}

int h264dvr::main(/*TCHAR *file_name, LONGLONG offset, TCHAR *out_dir*/void)
{
	SaveAllTimestampsToFile();
	//h264dvr::min_time = MIN_TIMESTAMP;
	//h264dvr::max_time = MAX_TIMESTAMP;

	//std::time_t t = h264dvr::min_time;
	//char time_str[32] = { 0 };
	//ctime_s(time_str, sizeof(time_str), &t);


	//FileEx file(_T("F:\\38023\\20150612_160000_ps.h264"));
	//FileEx out_file(_T("F:\\38023\\out.h264"));
	//if (file.Open() && out_file.Create()) {
	//	
	//	int buff_size = 512;
	//	BYTE *buff = new BYTE[buff_size];

	//	LONGLONG offset = 0;
	//	BYTE marker[] = {0x00, 0x00, 0x00, 0x00, 0x01};
	//	std::vector<BYTE> frame;
	//	PHEADER header = NULL;
	//	while ( (offset = file.Find(marker, sizeof(marker))) > 0x00 ) {
	//		file.SetPointer(--offset);
	//		while (header = ReadFrame(file, frame)) {
	//			if (header->channel_number == 0x00) {
	//				out_file.Write(&frame[sizeof(HEADER)], header->data_size);
	//			}	

	//			offset += (sizeof(HEADER) + header->data_size);
	//			file.SetPointer(offset);

	//			frame.clear();
	//			int x = 0;
	//		}
	//		offset+=2;
	//		file.SetPointer(offset);
	//		frame.clear();
	//	}	
	//}

	return 0;
}