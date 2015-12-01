#ifndef _COM_PORT_H
#define _COM_PORT_H

#include <windows.h>

#include <list>
#include <string>

class ComPort
{
private:
	DWORD last_error;
	HANDLE handle;
	std::string name;
public:
	ComPort();
	ComPort(const char *com_port_name);
	~ComPort();

	static void AvailableComPorts(std::list<std::string> &com_list);
	bool Open(DWORD baud_rate = CBR_9600);
	bool Open(const char *com_port_name);
	void Close();
	bool Write(const void *buff, DWORD size);
	bool Read(void *buff, DWORD size, DWORD timeout);
	bool Read(void *buff, DWORD size);
	bool WaitForInputData(DWORD timeout);
	bool WaitForInputData(void);
	void Test(void);
};

#endif // _COM_PORT_H