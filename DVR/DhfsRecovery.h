#ifndef _DHFS_RECOVERY_H
#define _DHFS_RECOVERY_H

#include "dvr.h"
#include "DhfsVolume.h"
#include "File.h"
#include "utility.h"

#include <string>
#include <map>
#include <list>

namespace DHFS
{

	class VFile
	{
	private:
		size_t camera{0};
		size_t width{0};
		size_t height{0};
		Timestamp start_time;
		Timestamp end_time;
		LONGLONG last_frame_offset{0};
		W32Lib::FileEx io;
	public:
		VFile(const std::string file_name) :io(file_name.c_str()) {}
		bool Create(void);
		void Close(void);
		std::string FileName(void) { return std::string(io.GetName()); }
		std::string Description(void);
		size_t Camera(void) { return camera; }
		size_t Width(void) { return width; }
		size_t Height(void) { return height; }
		Timestamp StartTime(void) { return start_time; }
		Timestamp EndTime(void) { return end_time; }
		LONGLONG Size(void);
		LONGLONG LastFrameOffset(void) { return last_frame_offset; }
		bool SaveFrameSequence(FrameSequence &sequence);
	};

	class Storage
	{
	private:
		size_t max_file_size;
		std::string base_directory;
		std::map<std::string, std::map<size_t, std::list<VFile *>>> storage;

		bool CreateDirectory(const std::string &directory);
	public:
		Storage(const std::string &storage_directory, LONGLONG max_file_size_);
		~Storage();
		bool Open(void);
		void Close(void);
		void CloseVideoFile(VFile &file);
		void ConvertVideoFile(VFile &file);
		bool SaveFrameSequence(FrameSequence &sequence);
		bool SaveFrameSequenceEx(FrameSequence &sequence, LONGLONG max_delta_time);
	};

	int StartRecovery(const std::string &dvr_volume, const std::string &out_directory, const dvr::Timestamp &start_time = dvr::Timestamp(), const dvr::Timestamp &end_time = dvr::Timestamp());

}

#endif // _DHFS_RECOVERY_H

