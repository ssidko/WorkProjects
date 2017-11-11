#ifndef _DCH264_RECOVERY_H
#define _DCH264_RECOVERY_H


#include <string>
#include <vector>
#include "dcH264_Reader.h"
#include "dvr.h"

namespace dcH264 {

	class VideoFile
	{
	public:
		VideoFile(const std::string &file_path, size_t buffer_size);
		~VideoFile();
		bool Create();
		void Close(void);
		bool SaveSequence(const dvr::FrameSequence &sequence);
		int Camera(void);
		dvr::Timestamp StartTime();
		dvr::Timestamp EndTime();
		uint64_t LastOffset(void);

	private:
		int camera;
		dvr::Timestamp start_time;
		dvr::Timestamp end_time;
		uint64_t last_offset;
		std::vector<uint8_t> buffer;
	};

	class VideoStorage
	{
	public:
		VideoStorage(const std::string &storage_dir_path);
		~VideoStorage();
		bool Open(void);
		void Close();
		bool SaveSequence(const dvr::FrameSequence &sequence);
	
	private:
		std::string storage_path;
	};

	void StartRecovery(void);

}

class dcH264_Recovery
{
public:
	dcH264_Recovery();
	~dcH264_Recovery();
};

#endif // _DCH264_RECOVERY_H

