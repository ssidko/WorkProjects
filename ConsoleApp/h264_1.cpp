#include "h264_1.h"
#include <W32Lib.h>


namespace h264_1
{


	bool NextFrame(FileEx &file, LONGLONG &start_offset, FRAME_DESCRIPTOR &frame)
	{
		if (!file.SetPointer(start_offset)) {
			return false;
		}

		HEADER header = {0};
		BYTE magic[] = {0x00, 0x00, 0x01, 0xFC};

		while (file.Find(magic, sizeof(magic)) != (-1)) {

		}
		return false;
	}

	int main(TCHAR *file_name, LONGLONG offset, TCHAR *out_dir)
	{
		FileEx file(_T("J:\\Work\\36829\\2222"));
		if (file.Open()) {
			LONGLONG offset = 0;
			FRAME_DESCRIPTOR frame = {0};
			while (NextFrame(file, offset, frame)) {

			}
		}
		return 0;
	}

}