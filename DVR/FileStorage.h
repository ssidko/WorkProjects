#pragma once

#ifndef _FILE_SORAGE_H
#define _FILE_SORAGE_H

#include <vector>
#include <map>
#include <list>
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
		DWORD end_sync_counter;
		LONGLONG first_offset;
		LONGLONG last_offset;
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
		DWORD EndSyncCounter(void) { return end_sync_counter; }
		LONGLONG FirstFrameOffset(void) { return first_offset; }
		LONGLONG LastFrameOffset(void) { return last_offset; }
		LONGLONG Size(void) {return file.GetSize();}
		std::string Name(void) {return name;}
		bool Rename(const std::string &new_name);
		DWORD Camera(void) { return camera; }
	};

	class FileStorage
	{
	private:
		std::string out_directory;
		std::vector<VideoFile *> files;
		std::map< DWORD, std::list<VideoFile *> > storage;
	public:
		FileStorage(int cam_count, const std::string &out_directory_path);
		~FileStorage(void);
		bool SaveFrameSequence(std::vector<BYTE> &sequence_buffer, FrameSequenceInfo &sequence_info);
		VideoFile *CreateNewFile(FrameSequenceInfo &sequence_info);
		void CloseFile(VideoFile *vfile);
		void CloseAllFiles(void);
	};
}

#endif

