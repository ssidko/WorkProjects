#ifndef _DHFS
#define _DHFS

#include <windows.h>
#include "Timestamp.h"
#include "File.h"

class Frame
{
public:
	Frame(void);
	~Frame(void);

	static Frame *GetFrame(W32Lib::FileEx &file)
};

#endif //_DHFS

