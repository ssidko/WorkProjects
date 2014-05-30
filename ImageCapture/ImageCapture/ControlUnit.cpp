#include "ControlUnit.h"
#include <QMessageBox>

#include <Cfgmgr32.h>
#include <Setupapi.h>

#pragma comment(lib, "Setupapi.lib")

DEFINE_GUID(GUID_DEVCLASS_PORTS, 0x4d36e978L, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18);

#define MAX_COM_PORT_NUMBER			50
#define RESPONSE_CODE				0x6948
#define GET_ID_CMD					"GetID"
#define GET_VERSION_CMD				"GetVer"

ControlUnit::ControlUnit(void) : com_handle(INVALID_HANDLE_VALUE), com_name(""), opened(false)
{
}

ControlUnit::~ControlUnit(void)
{
	Close();
}

HANDLE ControlUnit::OpenComPort(const char *name)
{
	HANDLE handle = ::CreateFileA(name, GENERIC_READ|GENERIC_WRITE, 0/*exclusive access*/, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (handle != INVALID_HANDLE_VALUE) {
		SetupComm(handle, 2048, 2048);

		COMMTIMEOUTS com_time_outs;
		com_time_outs.ReadIntervalTimeout = 0xFFFFFFFF;
		com_time_outs.ReadTotalTimeoutMultiplier = 0;
		com_time_outs.ReadTotalTimeoutConstant = 1000;
		com_time_outs.WriteTotalTimeoutMultiplier = 0;
		com_time_outs.WriteTotalTimeoutConstant = 1000;
		if(!SetCommTimeouts(handle, &com_time_outs)) {
			::CloseHandle(handle);
			return INVALID_HANDLE_VALUE;
		}
		DCB com_param;
		memset(&com_param, 0x00, sizeof(com_param));
		if (!GetCommState(handle, &com_param)) {
			::CloseHandle(handle);
			return INVALID_HANDLE_VALUE;
		}
		com_param.BaudRate = CBR_9600;
		com_param.ByteSize = 8;
		com_param.StopBits = ONESTOPBIT;
		com_param.Parity = NOPARITY;
		com_param.fAbortOnError = TRUE;
		com_param.fDtrControl = DTR_CONTROL_DISABLE;
		com_param.fRtsControl = RTS_CONTROL_DISABLE;
		com_param.fInX = FALSE;
		com_param.fOutX = FALSE;
		com_param.XonLim = 128;
		com_param.XoffLim = 128;
		com_param.fNull = FALSE;
		if (!SetCommState(handle, &com_param)) {
			::CloseHandle(handle);
			return INVALID_HANDLE_VALUE;
		}
		::PurgeComm(handle, PURGE_RXCLEAR|PURGE_TXABORT);
		::CancelIo(handle);
		::Sleep(2000); // Без этого не работает. Скорее всего что ардуина не увспевает сконфигурироваться.
		return handle;
	}
	return INVALID_HANDLE_VALUE;
}

bool ControlUnit::WriteComPort(HANDLE handle, const void *buff, int size)
{
	DWORD rw = 0;
	OVERLAPPED o;
	DWORD wait_result;
	bool ret = false;
	memset(&o, 0x00, sizeof(o));
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
	FlushFileBuffers(handle);
	::CloseHandle(o.hEvent);
	return ret;
}

bool ControlUnit::ReadComPort(HANDLE handle, void *buff, int size)
{
	DWORD rw = 0;
	OVERLAPPED o;
	DWORD wait_result;
	bool ret = false;
	memset(&o, 0x00, sizeof(o));
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
	::CloseHandle(o.hEvent);
	return ret;
}

bool ControlUnit::IsControlUnit(HANDLE handle)
{
	short response = 0;
	if (!WriteComPort(handle, "Hello\n", 6)) return false;
	if (!ReadComPort(handle, &response, 2)) return false;
	return (response == RESPONSE_CODE);
}

bool ControlUnit::IsAvailable(void)
{
	foreach (QString com, AvailableComPorts()) {
		HANDLE handle = OpenComPort(com.toLocal8Bit().data());
		if (handle != INVALID_HANDLE_VALUE) {
			if (IsControlUnit(handle)) {
				::CloseHandle(handle);
				return true;
			}
			else
				return false;
		}
	}
	return false;
}

QStringList ControlUnit::AvailableComPorts(void)
{
	QStringList com_list;
	QString dev_name_prefix = "\\\\.\\";
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

			QStringList registry_key_list;
			registry_key_list << QString("PortName") << QString("PortNumber");

			QString portName;
			foreach (const QString &registry_key, registry_key_list) {
				DWORD buff_size = 0;
				DWORD dataType = 0;
				QByteArray out_buff;
				forever {
					const LONG ret = ::RegQueryValueEx(key, reinterpret_cast<const wchar_t *>(registry_key.utf16()), NULL, &dataType, reinterpret_cast<unsigned char *>(out_buff.data()), &buff_size);
					if (ret == ERROR_MORE_DATA) {
						out_buff.resize(buff_size);
						continue;
					}
					else if (ret == ERROR_SUCCESS) {
						if (dataType == REG_SZ) {
							portName = dev_name_prefix + QString::fromUtf16(((const wchar_t *)(out_buff.constData())));
							com_list.push_back(portName);
						}
						else if (dataType == REG_DWORD) {
							portName = dev_name_prefix + QStringLiteral("COM%1").arg(*(PDWORD(out_buff.constData())));
							com_list.push_back(portName);
						}
					}
					break;
				}
			}
			::RegCloseKey(key);
						
		}
	}
	return com_list;
}

bool ControlUnit::Open(void)
{
	Close();
	char buff[16];
	foreach (QString com, AvailableComPorts()) {
		com_handle = OpenComPort(com.toLocal8Bit().data());
		if (com_handle != INVALID_HANDLE_VALUE) {
			if (IsControlUnit(com_handle)) {
				com_name = com;
				memset(buff, 0x00, sizeof(buff));
				if (SendCommand(GET_ID_CMD))
					if (ReadComPort(com_handle, buff, 8))
						id = buff;

				memset(buff, 0x00, sizeof(buff));
				if (SendCommand(GET_VERSION_CMD))
					if (ReadComPort(com_handle, buff, 8))
						version = buff;

				opened = true;
				return true;
			}
		}	
	}
	return false;
}

void ControlUnit::Close(void)
{
	com_name = "";
	id = "";
	version = "";
	opened = false;
	if (com_handle != INVALID_HANDLE_VALUE) {
		::CloseHandle(com_handle);
		com_handle = INVALID_HANDLE_VALUE;
	}
}

bool ControlUnit::SendCommand(QString cmd)
{
	cmd += "\n";
	return 	WriteComPort(com_handle, cmd.toLocal8Bit().data(), cmd.size());
}

bool ControlUnit::Testing( void )
{
	DWORD rw;
	DWORD error_code = 0;
	DWORD ret_code;
	char buff[10] = {0};
	bool status = false;
	QString com_name = "\\\\.\\COM12";
	HANDLE com_handle = OpenComPort(com_name.toLocal8Bit().data());
	if (com_handle != INVALID_HANDLE_VALUE) {

		//::Sleep(2000);

		DWORD comm_mask;
		status = ::GetCommMask(com_handle, &comm_mask);
		comm_mask |= EV_RXCHAR;
		status = ::SetCommMask(com_handle, comm_mask);

		CancelIo(com_handle);
		HANDLE ev = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if (ev) {
				OVERLAPPED ov;
				memset(&ov, 0x00, sizeof(OVERLAPPED));
				ov.hEvent = ev;
				::ResetEvent(ov.hEvent);

				if (!::WriteFile(com_handle, "Hello\n", 6, &rw, &ov)) {
					error_code = ::GetLastError();
					if (ERROR_IO_PENDING == error_code) {
						// Операция еще выполняется, необходимо подождать.
						ret_code = WaitForSingleObject(ev, 5000);
						switch (ret_code) {
							case WAIT_OBJECT_0: // 0
								if (GetOverlappedResult(com_handle, &ov, &rw, FALSE)) {
									// Read completed successfully.
									error_code = 0;
									status = true;
								} else {
									// Error
									error_code = ::GetLastError();
									status = false;
								}
								break;
							default:
								// An error has occurred in WaitForSingleObject.
								// This usually indicates a problem with the
								// OVERLAPPED structure's event handle.
								error_code = ::GetLastError();
								status = false;
								break;
						}
					} else {
						// Произошла ошибка.
						error_code = ::GetLastError();
						status = false;
					}
				} else {
					// WriteFile completed immediately..
					error_code = 0;
					status = true;
				}

				FlushFileBuffers(com_handle);

				memset(&ov, 0x00, sizeof(OVERLAPPED));
				ov.hEvent = ev;
				::ResetEvent(ov.hEvent);

				if (!::ReadFile(com_handle, buff, 2, &rw, &ov)) {
					error_code = ::GetLastError();
					if (ERROR_IO_PENDING == error_code) {
						// Операция еще выполняется, необходимо подождать.
						ret_code = WaitForSingleObject(ev, 30000);
						switch (ret_code) {
							case WAIT_OBJECT_0: // 0
								if (GetOverlappedResult(com_handle, &ov, &rw, TRUE)) {
									// Read completed successfully.
									error_code = 0;
									status = true;
								} else {
									// Error
									error_code = ::GetLastError();
									status =false;
								}
								break;
							case WAIT_TIMEOUT: // 258
							case WAIT_FAILED: 
							default:
								// Error
								error_code = ::GetLastError();
								status = false;
								break;
						}
					} else {
						// Произошла ошибка.
						error_code = ::GetLastError();
						status = false;
					}
				} else {
					// В буфере уже прочитанные данны. Можно юзать.
					error_code = 0;
					status = true;
				}
				CloseHandle(ev);
		}
		::CloseHandle(com_handle);
	}
	return status;
}
