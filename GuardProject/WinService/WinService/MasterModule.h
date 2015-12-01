#pragma once
#ifndef _MASTER_MODULE_H
#define _MASTER_MODULE_H

#include "ComPort.h"
#include "Message.h"

class MasterModule
{
private:
	ComPort com;
	DWORD last_error;
public:
	MasterModule();
	~MasterModule();

	bool Open(std::string &com_name);
	void Close();

	bool WaitForMessage(Message &msg);
};

#endif // _MASTER_MODULE_H

