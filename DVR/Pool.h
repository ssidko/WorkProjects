#ifndef _POOL_H
#define _POOL_H

#include <stdint.h>
#include "File.h"
#include "zfs.h"

class Pool
{
private:
	W32Lib::FileEx io;

public:
	Pool(const char* io_name, uint64_t offset_in_bytes);
	~Pool();

	bool Open() { return false; };

};

#endif // _POOL_H

