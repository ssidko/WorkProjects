#include "ComPort.h"

#include <windows.h>
#include <Setupapi.h>

#include <string>
#include <vector>

#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "Advapi32.lib")

ComPort::ComPort() : name(""), handle(INVALID_HANDLE_VALUE)
{
}

ComPort::ComPort(const char *com_port_name) : name(com_port_name), handle(INVALID_HANDLE_VALUE), last_error(0)
{
}

ComPort::~ComPort()
{
	Close();
}

void ComPort::AvailableComPorts(std::list<std::string> &com_list)
{
	com_list.clear();
	std::string dev_name_prefix = "\\\\.\\";
	HDEVINFO dev_info_set = ::SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (dev_info_set != INVALID_HANDLE_VALUE) {
		DWORD index = 0;
		SP_DEVINFO_DATA dev_info;
		memset(&dev_info, 0x00, sizeof(SP_DEVINFO_DATA));
		dev_info.cbSize = sizeof(SP_DEVINFO_DATA);
		while (::SetupDiEnumDeviceInfo(dev_info_set, index++, &dev_info)) {
			HKEY key = ::SetupDiOpenDevRegKey(dev_info_set, &dev_info, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
			if (key == INVALID_HANDLE_VALUE) {
				continue;
			}

			std::list<std::string> registry_key_list;
			registry_key_list.push_back("PortName");
			registry_key_list.push_back("PortNumber");

			std::string portName;
			auto it = registry_key_list.begin();
			while (it != registry_key_list.end()) {
				DWORD dataType = 0;
				DWORD buff_size = 16;
				std::vector<BYTE> out_buff;
				out_buff.resize(buff_size);
				std::string key_name = (*it++);
				while (true) {
					const LONG ret = ::RegQueryValueExA(key, key_name.c_str(), NULL, &dataType, out_buff.data(), &buff_size);
					if (ret == ERROR_MORE_DATA) {
						out_buff.resize(buff_size);
						continue;
					}
					else if (ret == ERROR_SUCCESS) {
						if (dataType == REG_SZ) {
							portName = dev_name_prefix + (const char *)out_buff.data();
							com_list.push_back(portName);
						}
						else if (dataType == REG_DWORD) {
							portName = dev_name_prefix + "COM" + std::to_string(*((PDWORD)out_buff.data()));
							com_list.push_back(portName);
						}
					}
					break;
				}
			}
			::RegCloseKey(key);
		}
	}
}

bool ComPort::Open(DWORD baud_rate)
{
	last_error = 0;
	handle = ::CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0/*exclusive access*/, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (handle != INVALID_HANDLE_VALUE) {
		SetupComm(handle, 2048, 2048);
		COMMTIMEOUTS com_time_outs;
		com_time_outs.ReadIntervalTimeout = 0xFFFFFFFF;
		com_time_outs.ReadTotalTimeoutMultiplier = 0;
		com_time_outs.ReadTotalTimeoutConstant = 1000;
		com_time_outs.WriteTotalTimeoutMultiplier = 0;
		com_time_outs.WriteTotalTimeoutConstant = 1000;
		if (SetCommTimeouts(handle, &com_time_outs)) {
			DCB com_param;
			memset(&com_param, 0x00, sizeof(com_param));
			com_param.DCBlength = sizeof(DCB);
			if (GetCommState(handle, &com_param)) {
				com_param.BaudRate = baud_rate;
				com_param.ByteSize = 8;
				com_param.StopBits = ONESTOPBIT;
				com_param.Parity = NOPARITY;
				//com_param.fAbortOnError = TRUE;
				//com_param.fDtrControl = DTR_CONTROL_DISABLE;
				//com_param.fRtsControl = RTS_CONTROL_DISABLE;
				//com_param.fInX = FALSE;
				//com_param.fOutX = FALSE;
				//com_param.XonLim = 128;
				//com_param.XoffLim = 128;
				//com_param.fNull = FALSE;
				if (SetCommState(handle, &com_param)) {
					if (SetCommMask(handle, EV_RXCHAR)) {
						::Sleep(2000); // Ардуине нужно время чтобы загрузиться.
						return true;
					}
				}
			}
		}
		::CloseHandle(handle);
	}
	last_error = ::GetLastError();
	return false;
}

bool ComPort::Open(const char *com_port_name)
{
	name = com_port_name;
	return Open();
}

void ComPort::Close()
{
	//::PurgeComm(handle, PURGE_RXCLEAR | PURGE_TXABORT);
	::CancelIo(handle);
	if (handle != INVALID_HANDLE_VALUE) {
		::CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}
	last_error = 0;
}

bool ComPort::Write(const void *buff, DWORD size)
{
	DWORD rw = 0;
	OVERLAPPED o = {0};
	DWORD wait_result;
	bool ret = false;

	o.hEvent = ::CreateEventA(NULL, TRUE, FALSE, NULL);
	if (!o.hEvent) {
		return false;
	}

	if (!::WriteFile(handle, buff, size, &rw, &o)) {
		if (ERROR_IO_PENDING == ::GetLastError()) {
			wait_result = WaitForSingleObject(o.hEvent, 1000);
			switch (wait_result) {
			case WAIT_OBJECT_0:
				if (::GetOverlappedResult(handle, &o, &rw, FALSE)) {
					ret = true;
					break;
				}
			default:
				::CancelIo(handle);
				ret = false;
				break;
			}
		} else {
			ret = false;
		}
	} else {
		ret = true;
	}

	::FlushFileBuffers(handle);
	::CloseHandle(o.hEvent);

	return ret;
}

bool ComPort::Read(void *buff, DWORD size)
{
	DWORD rw = 0;
	OVERLAPPED o = {0};
	DWORD wait_result;
	bool ret = false;

	o.hEvent = ::CreateEventA(NULL, TRUE, FALSE, NULL);
	if (!o.hEvent) {
		return false;
	}

	if (!::ReadFile(handle, buff, size, &rw, &o)) {
		if (ERROR_IO_PENDING == ::GetLastError()) {
			wait_result = WaitForSingleObject(o.hEvent, 1000);
			switch (wait_result) {
			case WAIT_OBJECT_0:
				if (::GetOverlappedResult(handle, &o, &rw, TRUE)) {
					if (rw) {
						ret = true;
					}
					break;
				}
			default:
				::CancelIo(handle);
				ret = false;
				break;
			}
		} else {
			ret = false;
		}
	} else {
		ret = true;
	}
	::CloseHandle(o.hEvent);
	return ret;
}

bool ComPort::WaitForInputData(void)
{
	bool ret = false;
	DWORD result = 0;
	DWORD event_type = 0;
	OVERLAPPED sync = { 0 };

	last_error = 0;
	sync.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (sync.hEvent) {

		if (::WaitCommEvent(handle, &event_type, &sync)) {
			if (event_type == EV_RXCHAR) {
				ret = true;
			} else {
				ret = false;
			}
		}

		if ((ret == false) && (ERROR_IO_PENDING == ::GetLastError())) {
			result = ::WaitForSingleObject(sync.hEvent, INFINITE);
			switch (result) {
			case WAIT_OBJECT_0:
				ret = true;
				break;
			default:
				ret = false;
				break;
			}
		}		

		::CloseHandle(sync.hEvent);
	}
	last_error = ::GetLastError();
	return ret;
}

void ComPort::Test(void)
{
	if (this->Open()) {
		//Message msg;
		//BYTE *ptr = (BYTE *)&msg;
		//int bytes_count = 0;
		//BYTE byte;

		//while (com_port->WaitForInputData()) {
		//	while (com_port->Read(&byte, 1)) {

		//		if (bytes_count == 0x00) {
		//			ptr = (BYTE *)&msg;
		//			if (byte == MESSAGE_HEADER) {
		//				ptr[bytes_count] = byte;
		//				bytes_count++;
		//			}
		//		}
		//		else {
		//			ptr[bytes_count] = byte;
		//			bytes_count++;
		//			if (bytes_count == sizeof(Message)) {
		//				if (msg.IsValidMessage()) {
		//					//
		//					// Message recived.
		//					//
		//					//emit(MessageRecived(msg));
		//				}
		//				bytes_count = 0;
		//			}
		//		}
		//	}
		//	int x = 0;
		//}
	}
}
