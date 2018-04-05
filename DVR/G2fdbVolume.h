#ifndef _G2FDB_VOLUME_H
#define _G2FDB_VOLUME_H

#include "windows.h"
#include "G2fdbOnDisk.h"
#include "dvr.h"
#include "BufferedFile.h"
#include <vector>

namespace G2FDB
{
	using namespace std;
	using namespace dvr;

	struct Frame {
		uint64_t offset = 0;
		uint32_t camera = 0;
		uint32_t width = 0;
		uint32_t height = 0;
		Timestamp time;
		std::vector<BYTE> data;

		void Clear(void);
		DWORD HeaderSize(void);
		DWORD FullSize(void);
	};

#define MAX_FRAME_SEQUANCE_SIZE					((DWORD) 128*1024*1024)

	struct FrameSequence {
		uint64_t offset;
		uint32_t camera;
		uint32_t width = 0;
		uint32_t height = 0;
		size_t frames_count;
		Timestamp start_time;
		Timestamp end_time;
		vector<BYTE> data;

		void Clear(void);
		void AddData(BYTE *frame_data, size_t size);
		void AddFirstFrame(Frame &first_frame);
		void AddFrame(Frame &frame);
	};

	class G2fdbVolume 
	{
	private:
		BufferedFile io;

		bool IsValidFrameHeader(const FRAME_HEADER &header);
		size_t SignatureOffset(void);
	public:
		G2fdbVolume(const std::string &volume_file);
		~G2fdbVolume();
		bool Open(void);
		bool SetPointer(const LONGLONG &offset);
		
		// С текущей позиции ищет фрейм.
		//
		// В случае успеха считывает его и сохраняет в обьекте frame.
		// Файловый указатель устанавливается сразу же за фреймом.
		//
		// Если найти фрейм не удалось - файловый указатель не определен.
		bool FindAndReadFrame(G2FDB::Frame &frame);
		bool FindAndReadFrame(dvr::Frame &frame);
		
		bool ReadFrame(G2FDB::Frame &frame);
		bool ReadFrame(dvr::Frame &frame);
		bool ReadFrameSequence(G2FDB::FrameSequence &sequence, size_t max_delta_time = 1);
		bool ReadFrameSequence(dvr::FrameSequence &sequence, size_t max_size, size_t max_delta_time = 1);
	};
}

#endif // _G2FDB_VOLUME_H

