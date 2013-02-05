#ifndef _THREAD
#define _THREAD

#include "W32Lib.h"

class Thread
{
private:
	HANDLE handle;			// thread handle
	DWORD id;				// thread identifier

	static DWORD WINAPI StaticThreadProc(LPVOID param)
	{
		return ((Thread *)param)->ThreadProc();
	}

public:
	Thread(void) : handle(NULL), id(0) {}
	~Thread(void) {if (handle) ::CloseHandle(handle);}

	//
	// Создаёт поток с стандартными параметрами
	//  suspend = FALSE - поток стартует сразу
	//  suspend = TRUE - для запуска потока нужно вызвать Resume()
	//
	BOOL Create(BOOL suspend = FALSE)
	{
		return CreateEx(NULL, 0, (suspend ? CREATE_SUSPENDED : 0));
 	}

	BOOL CreateEx(LPSECURITY_ATTRIBUTES security_attr, DWORD stack_size, DWORD creation_flags)
	{
		if (handle = ::CreateThread(security_attr, stack_size, Thread::StaticThreadProc, (LPVOID)this, creation_flags, &id))
			return TRUE;
		else
			return FALSE;
	}

	//
	// Thread's suspend count is incremented
	// Если произошла ошбка то возвращает (DWORD)-1
	//
	DWORD Suspend(void)
	{
		return ::SuspendThread(handle);
	}

	//
	// Decrements a thread's suspend count
	// Если произошла ошбка то возвращает (DWORD)-1
	//
	DWORD Resume(void)
	{
		return ::ResumeThread(handle);
	}

	virtual DWORD ThreadProc(void) = 0;
};

#endif
