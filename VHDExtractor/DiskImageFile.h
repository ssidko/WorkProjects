#ifndef IMAGEDISKFILE_H
#define IMAGEDISKFILE_H

#include "windows.h"

class DiskImageFile
{
public:
	enum FileFormat {
		kUnknownFileFormat,
		kVdiFile,
		kVhdFile
	};

	virtual ~DiskImageFile() {};
	virtual bool Open(void) = 0;
	virtual void Close(void) = 0;
	virtual DWORD BlockSize(void) = 0;
	virtual DWORD BlocksCount(void) = 0;
	virtual bool ReadBlock(DWORD block_num, char *block_buff) = 0;
	virtual FileFormat ImageFileFormat(void) = 0;
};

#endif

