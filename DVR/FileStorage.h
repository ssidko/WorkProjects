#pragma once

#ifndef _FILE_SORAGE_H
#define _FILE_SORAGE_H

#include <vector>
#include <string>
#include <windows.h>
#include "File.h"
#include "DhfsOnDisk.h"
#include "Timestamp.h"
#include "utility.h"

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
		void Close(void);
		bool SaveFrameSequence(std::vector<BYTE> &sequence_buffer, FrameSequenceInfo &sequence_info);
		Timestamp StartTime(void) {return start_time;}
		Timestamp EndTime(void) {return end_time;}
		LONGLONG Size(void) {return file.GetSize();}
		std::string Name(void) {return name;}
		bool Rename(const std::string &new_name);
		unsigned int CameraNumber(void) { return camera; }
	};

	class FileStorage
	{
	private:
		std::string out_directory;
		std::vector<VideoFile *> files;
	public:
		FileStorage(int cam_count, const std::string &out_directory_path);
		~FileStorage(void);
		bool SaveFrameSequence(std::vector<BYTE> &sequence_buffer, FrameSequenceInfo &sequence_info);
		VideoFile *CreateNewFile(FrameSequenceInfo &sequence_info);
		void CloseFile(unsigned int file_index);
		void CloseAllFiles(void);
	};
}

#endif

