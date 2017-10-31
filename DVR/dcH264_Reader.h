#ifndef _DCH264_READER_H
#define _DCH264_READER_H

#include <string>
#include "BufferedFile.h"
#include "dcH264_OnDisk.h"

namespace dcH264 
{

	class Reader
	{
	public:
		Reader(const std::string &file_name, const LONGLONG &file_size);
		~Reader() = default;

		bool Open();

		bool SetOffset(const LONGLONG &offset);
		LONGLONG Offset(void);

		bool GoToNextFrame();
		bool ReadFrame(dvr::Frame &frame);
		bool ReadFrameSequence(dvr::FrameSequence &sequence, size_t max_size = 32*1024*1024);

	private:
		BufferedFile io;
		dvr::Frame tmp_frame;
	};

	int main(void);
	void AppendFirstFrame(dvr::FrameSequence &sequence, dvr::Frame &frame);
	void AppendFrame(dvr::FrameSequence &sequence, dvr::Frame &frame);
}

#endif // _DCH264_READER_H

