#ifndef IMAGEDISKFILE_H
#define IMAGEDISKFILE_H

#include "windows.h"

class DiskImageFile
{
public:
	virtual ~DiskImageFile() {};
	virtual bool Open() = 0;
	virtual void Close() = 0;
	virtual DWORD BlockSize() = 0;
	virtual DWORD BlocksCount() = 0;
	virtual bool ReadBlock(DWORD block_num, char *block_buff) = 0;
};

#endif

