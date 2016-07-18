#ifndef _ORBITA_H
#define _ORBITA_H

#include <windows.h>
#include <string>
#include <BufferedFile.h>
#include "Timestamp.h"
#include "File.h"

namespace Orbita
{
	using namespace dvr;

	#pragma pack(push)
	#pragma pack(1)

	typedef struct _TIME_STAMP {
		BYTE year;
		BYTE month;
		BYTE day;
		BYTE hours;
		BYTE minutes;
		BYTE seconds;
		Timestamp Timestamp(void) { return dvr::Timestamp(2000 + year, month, day, hours, minutes, seconds); }
	} TIME_STAMP;

	typedef struct _HEADER {
		BYTE channel : 4;		// 1-16
		BYTE mask_1 : 4;		// 0x3
		BYTE sub_type : 4;		// 0,1,5 ...	
		BYTE mask_2 : 4;		// 0x3
		WORD frame_type;		// "dc", "wb"
		bool IsValid(void)
		{
			if ((mask_1 == 3) && (mask_2 == 3)) {
				if ((frame_type == 'cd') || (frame_type == 'bw')) {
					return true;
				}
			}	
			return false;
		}
	} HEADER;

	typedef struct _HEADER_dc{
		HEADER header;
		DWORD sign_2;			// "H264"
		DWORD size;
		BYTE subframes_count;
		BYTE unk_1[11];
		bool IsValid(void)
		{
			if (header.IsValid() && (header.frame_type == 'cd') && (sign_2 == '462H')) {
				return true;
			}
			return false;
		}
	} HEADER_dc;

	typedef struct _HEADER_0dc : public HEADER_dc {
		TIME_STAMP timestamp;
		BYTE unk_4[10];
	} HEADER_0dc;

	typedef struct _HEADER_wb{
		HEADER header;
		WORD size_1;
		WORD size_2;
		bool IsValid(void)
		{
			if (header.IsValid() && (header.frame_type == 'bw') && (size_1 == size_2)) {
				return true;
			}
			return false;
		}
	} HEADER_wb;

	#pragma pack(pop)

	typedef struct _FRAME_DESCRIPTOR {
		LONGLONG offset;
		DWORD channel;
		DWORD frame_type;
		DWORD sub_type;
		Timestamp timestamp;
		DWORD size;
		void Clear(void) { memset(this, 0x00, sizeof(FRAME_DESCRIPTOR)); }
	} FRAME_DESCRIPTOR;

	typedef struct _FRAME_SEQUENCE {
		Timestamp last_timestamp;
		FRAME_DESCRIPTOR first_frame;
		FRAME_DESCRIPTOR last_frame;
		std::vector<BYTE> buffer;
		DWORD frames_count;

		void Clear(void) {
			last_timestamp.Clear();
			first_frame.Clear();
			last_frame.Clear();
			buffer.clear();
			frames_count = 0;
		}

		bool IsNextFrame(FRAME_DESCRIPTOR &frame) {
			if (last_frame.channel == frame.channel) {
				if (frame.timestamp.Seconds() && last_timestamp.Seconds()) {
					if (frame.timestamp.Seconds() >= last_timestamp.Seconds()) {
						if ((frame.timestamp.Seconds() - last_timestamp.Seconds()) <= 5) {
							return true;
						}
					}
					return false;
				}	
				return true;
			}
			return false;
		}
	} FRAME_SEQUENCE;

	class Scanner
	{
	private:
		BufferedFile io;
	public:
		Scanner(const std::string &io_name);
		~Scanner();
		bool Open() { return io.Open(); }
		void AlignIoPointer(void);
		void SetPointer(const LONGLONG &frame_offset) {io.SetPointer(frame_offset);}
		bool IsValidFrameHeader(HEADER *header);
		DWORD HeaderExtraSize(HEADER *header);
		DWORD FrameDataSize(HEADER *header);
		Timestamp FrameTimestamp(HEADER *header);
		bool NextFrameHeader(LONGLONG &frame_offset);
		bool NextFrameHeaderWithTimestamp(void);
		bool ReadFrame(std::vector<BYTE> &buffer, FRAME_DESCRIPTOR &frame);
		bool NextFrameSequence(FRAME_SEQUENCE &sequence);
		bool NextOneSecondsSequence(FRAME_SEQUENCE &sequence);
	};

	class Storage
	{
	private:
		DWORD max_file_size;
		std::string directory;
		std::string mkvmerge_app_path;
		std::vector<W32Lib::FileEx *> files;
	public:
		Storage(const std::string &out_directory);
		~Storage() { CloseAll(); }
		void CloseAll(void);
		void CloseFile(DWORD index);
		void ToMkv(DWORD index);
		bool CreateNewFile(FRAME_SEQUENCE &sequence);
		bool Save(FRAME_SEQUENCE &sequence);

	};

	int Main(const std::string &io_name, const std::string &out_dir);

}

#endif // _ORBITA_H


