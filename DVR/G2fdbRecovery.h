#ifndef _G2FDB_RECOVERY_H
#define _G2FDB_RECOVERY_H

#include <string>
#include "File.h"
#include "Timestamp.h"
#include "G2fdbVolume.h"

namespace G2FDB
{
	class VideFile
	{
	private:
	public:
		VideFile() {}
		~VideFile() {}
	};

	class VideoStorage
	{
	private:
		std::string base_directory;
		bool IsDirectoryExist(const std::string &directory);
	public:
		VideoStorage() {}
		~VideoStorage() {}
		bool Open(const std::string &storage_directory);
		bool SaveFrameSequence(FrameSequence &sequennce);
	};

	int StartRecovery(const std::string &dvr_volume, const std::string &out_directory, const dvr::Timestamp &start_time, const dvr::Timestamp &end_time);
}

#endif // _G2FDB_RECOVERY_H