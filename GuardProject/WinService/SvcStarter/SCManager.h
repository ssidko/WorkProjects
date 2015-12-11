#ifndef _SCMANAGER_H
#define _SCMANAGER_H

#include <windows.h>

class SCManager
{
private:
	HANDLE handle;
public:
	SCManager();
	~SCManager();
	bool Open(void);
	void Close(void);
};

#endif // _SCMANAGER_H

