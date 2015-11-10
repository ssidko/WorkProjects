#ifndef _COM_PORT_H
#define _COM_PORT_H

#include <windows.h>

#include <list>
#include <string>



class ComPort
{
private:
	HANDLE handle;
	std::string name;
public:
	ComPort(const char *com_port_name);
	~ComPort();

	static void AvailableComPorts(std::list<std::string> &com_list);
	bool Open();
	bool Write(const void *buff, DWORD size);
	bool Read(void *buff, DWORD size);
};

#endif // _COM_PORT_H