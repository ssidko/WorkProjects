#ifndef _HIKVISION_H
#define _HIKVISION_H

#include <string>
#include "dvr.h"
#include "BufferedFile.h"
#include "utility.h"

namespace HIKV
{
	using namespace dvr;

#pragma pack(push)
#pragma pack(1)

	typedef struct _TIMESTAMP {
		DWORD seconds	: 4;
		DWORD minutes	: 6;
		DWORD hours		: 5;
		DWORD day		: 5;
		DWORD month		: 4;
		DWORD year		: 8;
		Timestamp TimeStamp (void) { return Timestamp(year + 2000, month, day, hours, minutes, seconds << 2); }
	} TIMESTAMP;

	typedef struct _FRAME_HEADER {
		BYTE signature[3];			// {0x00, 0x00, 0x01} synchro bytes
		BYTE type;					// enum FrameType
		bool IsValid(void);
	} FRAME_HEADER;

	typedef struct _FRAME_TYPE_0E {
		FRAME_HEADER header;
		uint16_t data_size;
		uint16_t flags;
		uint8_t meta_data_size;
	} FRAME_TYPE_0E;

#pragma pack(pop)

	typedef enum {
		kPS_frame			= 0xBA,
		kPES_frame			= 0xE0,
		kHikPrivateData_1	= 0xBC, // contain TimeStamp
		kTypeCode_BD		= 0xBD,
	} FrameType;

	typedef struct _FrameInfo {
		LONGLONG offset;
		BYTE frame_type;
		DWORD data_size;
		DWORD full_size;
		Timestamp time_stamp;
		_FrameInfo(void) { Clear(); }
		void Clear(void) {
			offset = 0;
			frame_type = 0;
			data_size = 0;
			full_size = 0;
			time_stamp.Clear();
		}
	} FrameInfo;

	struct Frame {
		LONGLONG offset;
		std::vector<BYTE> data;

		void Clear(void);
		BYTE Type(void);
		Timestamp TimeStamp(void);
		size_t H264DataOffset(void);
		size_t H264DataSize(void);
	};

	typedef struct _FrameSequence {
		LONGLONG offset;
		DWORD frames_count;
		Timestamp start_time;
		Timestamp end_time;
		std::vector<BYTE> buffer;
		void Clear(void);
	} FrameSequence;

	class HikVolume
	{
	private:
		BufferedFile io;
	public: 
		HikVolume(const std::string &file_name) : io(file_name, 256*512) {}
		~HikVolume(void) {}

		bool Open(void) { return io.Open(); }
		void SetPointer(const LONGLONG &offset) { io.SetPointer(offset); }
		LONGLONG Pointer(void) { return io.Pointer(); }
		LONGLONG FindNextFrame(void);
		bool ReadFrame(Frame &frame);
		bool ReadFrame(std::vector<BYTE> &buffer, FrameInfo &frame);
		bool NextFrameSequence(FrameSequence &sequence);
		bool SaveFrameSequenceToFile(std::string &file_name, FrameSequence &sequence);
		bool SaveFramesInfoToFile(std::string &file_name);
	};

	int StartRecovery(const std::string &dhfs_volume, const std::string &out_directory, const dvr::Timestamp &start_time = dvr::Timestamp(), const dvr::Timestamp &end_time = dvr::Timestamp());
}



#endif // _HIKVISION_H

