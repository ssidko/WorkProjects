#include "h264.h"

DWORD h264dvr::min_time = 0;
DWORD h264dvr::max_time = 0;

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

int h264dvr::main(TCHAR *file_name, LONGLONG offset, TCHAR *out_dir)
{
	h264dvr::min_time = MIN_TIMESTAMP;
	h264dvr::max_time = MAX_TIMESTAMP;

	std::time_t t = h264dvr::min_time;
	char time_str[32] = { 0 };
	ctime_s(time_str, sizeof(time_str), &t);


	FileEx file(_T("F:\\38023\\20150612_160000_ps.h264"));
	FileEx out_file(_T("F:\\38023\\out.h264"));
	if (file.Open() && out_file.Create()) {
		
		int buff_size = 512;
		BYTE *buff = new BYTE[buff_size];

		LONGLONG offset = 0;
		BYTE marker[] = {0x00, 0x00, 0x00, 0x00, 0x01};
		std::vector<BYTE> frame;
		PHEADER header = NULL;
		while ( (offset = file.Find(marker, sizeof(marker))) > 0x00 ) {
			file.SetPointer(--offset);
			while (header = ReadFrame(file, frame)) {
				if (header->channel_number == 0x00) {
					out_file.Write(&frame[sizeof(HEADER)], header->data_size);
				}	

				offset += (sizeof(HEADER) + header->data_size);
				file.SetPointer(offset);

				frame.clear();
				int x = 0;
			}
			offset+=2;
			file.SetPointer(offset);
			frame.clear();
		}	
	}
	

	return 0;
}