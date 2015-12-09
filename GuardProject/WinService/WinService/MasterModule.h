#pragma once
#ifndef _MASTER_MODULE_H
#define _MASTER_MODULE_H

#include "ComPort.h"
#include "Message.h"

class MasterModule
{
private:
	enum {
		kIdentificationAnswerTimeout = 1000,
	};
private:
	ComPort com;
	DWORD last_error;
	bool opened;
public:
	MasterModule();
	~MasterModule();

	bool Identify(void);
	DWORD LastError(void) { return last_error; }
	bool Open(const std::string &com_name);
	void Close(void);
	bool Opened(void) { return opened; }

	bool WaitForMessage(Message &msg);
	bool WaitForMessage(Message &msg, DWORD timeout);
};

#endif // _MASTER_MODULE_H

