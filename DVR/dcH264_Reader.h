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

		bool ReadFrame(dvr::Frame &frame);
		bool ReadFrameSequence(dvr::FrameSequence &sequence);

	private:
		BufferedFile io;
	};

	int main(void);
}

#endif // _DCH264_READER_H

