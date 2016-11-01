#ifndef _G2FDB_VOLUME_H
#define _G2FDB_VOLUME_H

#include "windows.h"
#include "G2fdbOnDisk.h"
#include "Timestamp.h"
#include "BufferedFile.h"
#include <vector>

namespace G2FDB
{
	using namespace std;
	using namespace dvr;

	struct Frame {
		LONGLONG offset;
		DWORD camera;
		Timestamp time;
		std::vector<BYTE> data;

		void Clear(void);
		DWORD HeaderSize(void);
		DWORD FullSize(void);
	};

#define MAX_FRAME_SEQUANCE_SIZE					((DWORD) 512*1024*1024)

	struct FrameSequence {
		LONGLONG offset;
		DWORD camera;
		DWORD frames_count;
		Timestamp start_time;
		Timestamp end_time;
		vector<BYTE> data;

		void Clear(void);
		void AddData(std::vector<BYTE> &new_data);
		void AddFirstFrame(Frame &first_frame);
		void AddFrame(Frame &frame);
	};

	class G2fdbVolume 
	{
	private:
		BufferedFile io;

		bool IsValidFrameHeader(const FRAME_HEADER &header);
	public:
		G2fdbVolume(const std::string &volume_file);
		~G2fdbVolume();
		bool Open(void);
		bool SetPointer(LONGLONG &offset);
		size_t SignatureOffset(void);
		
		// � ������� ������� ���� ��������� �����.
		//
		// � ������ ������ ��������� ��� � ��������� � ������� frame.
		// �������� ��������� ��������������� ����� �� �� �������.
		//
		// ���� ����� ����� �� ������� - �������� ��������� �� ���������.
		bool FindNextFrame(Frame &frame);
		
		bool ReadFrame(Frame &frame);
		bool ReadFrameSequence(FrameSequence &sequence);
	};
}

#endif // _G2FDB_VOLUME_H

