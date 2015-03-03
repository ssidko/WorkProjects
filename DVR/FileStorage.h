#pragma once

#ifndef _FILE_SORAGE_H
#define _FILE_SORAGE_H

#include <vector>
#include <string>
#include <windows.h>
#include "File.h"
#include "Frame.h"
#include "Timestamp.h"

namespace DHFS
{
#define MAX_SEQUENCE_SIZE		(DWORD)32*1024*1024
#define MAX_VIDEO_FILE_SIZE		(DWORD)512*1024*1024

	class VideoFile
	{
	private:
		int camera;
		Timestamp start_time;
		Timestamp end_time;
		std::string name;
		W32Lib::FileEx file;
	public:
		VideoFile(std::string full_path);
		~VideoFile(void);
		bool Create(void);
		bool SaveFrameSequence(BYTE *buffer, FrameSequence &sequence);
		Timestamp StartTime(void) {return start_time;}
		Timestamp EndTime(void) {return end_time;}
		LONGLONG Size(void) {return file.GetSize();}
	};

	class FileStorage
	{
	private:
		std::string raw_directory;
		std::string mkv_directory;
		std::vector<VideoFile *> files;
		BYTE *frame_sequence_buffer;
	public:
		FileStorage(int cam_count, std::string &raw_directory_path, std::string &mkv_directory_path);
		~FileStorage(void);
		bool SaveFrameSequence(W32Lib::FileEx &dhfs_volume, FrameSequence &sequence);
		VideoFile *CreateNewFile(FrameSequence &sequence);
		void GenerateFileName(std::string &new_file_name, FrameSequence &sequence);
	};
}

#endif

