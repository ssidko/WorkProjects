#pragma once

#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <list>

namespace dvr
{
	class Timestamp
	{
	public:
		WORD year = 0;
		BYTE month = 0;
		BYTE day = 0;
		BYTE hours = 0;
		BYTE minutes = 0;
		BYTE seconds = 0;

		Timestamp(WORD year_ = 0, BYTE month_ = 0, BYTE day_ = 0, BYTE hours_ = 0, BYTE mins_ = 0, BYTE sec_ = 0);
		void Clear(void);
		bool Valid(void) const;
		ULONGLONG Seconds(void) const;
		std::string ToString(void) const;
		void ToString(std::string &str) const;
		std::string Date(char separator = '-') const;
		std::string Time(char separator = '-') const;

		bool operator>(const Timestamp &t);
		bool operator>=(const Timestamp &t);
		bool operator<(const Timestamp &t);
		bool operator<=(const Timestamp &t);
		bool operator==(const Timestamp &t);
		ULONGLONG operator-(const Timestamp &t);
		operator bool(void);
	};

#pragma pack(push,1)

	struct Date {
		size_t year = 0;
		uint8_t month = 0;
		uint8_t day = 0;
	};

	struct Time {
		uint8_t hours = 0;
		uint8_t minutes = 0;
		uint8_t seconds = 0;
	};

#pragma pack(pop)

	struct RecoveryTask {
		std::string io_name;
		uint64_t io_offset;
		uint64_t io_size;
		std::string output_dir;
		dvr::Timestamp start_time;
		dvr::Timestamp end_time;
	};

	struct Frame {
		uint64_t offset = 0;
		uint32_t camera = 0;
		uint32_t width = 0;
		uint32_t height = 0;
		dvr::Timestamp time;
		std::vector<uint8_t> buffer;

		void Clear(void);
	};

	struct FrameSequence {
		uint64_t first_frame_offset = 0;
		uint64_t last_frame_offset = 0;
		uint32_t camera = 0;
		uint32_t width = 0;
		uint32_t height = 0;
		size_t frames_count = 0;
		dvr::Timestamp start_time;
		dvr::Timestamp end_time;
		std::vector<uint8_t> buffer;
		
		void Clear(void);
		void AddFirstFrame(Frame &frame);
		void AddFrame(Frame &frame);
	};

	class VideoFile 
	{
	private:
		std::string io_path;

		uint64_t first_frame_offset = 0;
		uint64_t last_frame_offset = 0;
		uint32_t camera = 0;
		uint32_t width = 0;
		uint32_t height = 0;
		dvr::Timestamp start_time;
		dvr::Timestamp end_time;
		uint64_t file_size = 0;
	public:
		VideoFile(const std::string &file_path, FrameSequence &seq);
		
		void Close(void) {}
		void AppendFrameSequence(FrameSequence &seq);

		const std::string & FilePath(void) { return io_path; }
		uint64_t FirstFrameOffset(void) { return first_frame_offset; }
		uint64_t LastFrameOffset(void) { return last_frame_offset; }
		const Timestamp & StartTimestamp(void) { return start_time; }
		const Timestamp & EndTimestamp(void) { return end_time; }
		uint32_t Camera(void) { return camera; }
		uint32_t Height(void) { return height; }
		uint32_t Width(void) { return width; }
		bool IsValidWidthHeight(void) { return width && height; }
		uint64_t Size(void) { return file_size; }
	};

	class VideoStorage
	{
	private:
		std::string dir_path;
		size_t max_file_size = 512*1024*1024;
		uint64_t max_distance = 10 * 1024 * 1024 * 1024;

		std::map<std::string, std::map<uint32_t, VideoFile *>> storage;
		std::list<VideoFile *> recent_files;
	public:
		VideoStorage(const std::string &path);
		~VideoStorage(void);
		bool Open(void);
		void Close(void);
		bool SaveFrameSequence(FrameSequence &seq);
	private:
		VideoFile * CreateNewFile(FrameSequence &seq);
		void CloseFile(VideoFile *file);
		void CloseAllFiles(void);
		VideoFile * GetVideoFile(const std::string &date, uint32_t camera);
	};


}

#endif