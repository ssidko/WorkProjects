#include "ControlUnit.h"
#include <QMessageBox>

#define MAX_COM_PORT_NUMBER			50
#define RESPONSE_CODE				0x6948


ControlUnit::ControlUnit(void) : com_handle(NULL), com_name(""), opened(false)
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
		SetupComm(handle, 1500, 1500);

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
		::CancelIo(handle);
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
	o.hEvent = ::CreateEventA(NULL, FALSE, FALSE, NULL);
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
	o.hEvent = ::CreateEventA(NULL, FALSE, FALSE, NULL);
	if (!o.hEvent) {
		return false;
	}
	if (!::ReadFile(handle, buff, size, &rw, &o)) {
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
	DWORD rw = 0;
	DWORD err = 0;
	HANDLE handle = NULL;
	QString com_name = "";
	for (int i = 0; i < MAX_COM_PORT_NUMBER; i++) {
		com_name = "\\\\.\\COM" + QString::number(i, 10);
		handle = OpenComPort(com_name.toLocal8Bit().data());
		if (handle == INVALID_HANDLE_VALUE) continue;
		if (IsControlUnit(handle)) {
			::CloseHandle(handle);
			return true;
		}
		::CloseHandle(handle);
	}
	com_name = "";
	return false;
}

bool ControlUnit::Open()
{
	DWORD rw = 0;
	Close();
	for (int i = 0; i < MAX_COM_PORT_NUMBER; i++) {
		com_name = "\\\\.\\COM" + QString::number(i, 10);
		com_handle = OpenComPort(com_name.toLocal8Bit().data());
		if (com_handle != INVALID_HANDLE_VALUE) {
			if (IsControlUnit(com_handle)) {
				opened = true;
				return true;
			}
			::CloseHandle(com_handle);
			com_handle = NULL;
		}
	}
	com_name = "";
	return false;
}

void ControlUnit::Close()
{
	com_name = "";
	opened = false;
	if (com_handle && (com_handle != INVALID_HANDLE_VALUE)) {
		::CloseHandle(com_handle);
		com_handle = NULL;
	}
}

bool ControlUnit::SenCommand(QString cmd)
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
	QString com_name = "\\\\.\\COM4";
	HANDLE com_handle = OpenComPort(com_name.toLocal8Bit().data());
	if (com_handle != INVALID_HANDLE_VALUE) {
		CancelIo(com_handle);
		HANDLE ev = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if (ev) {
				OVERLAPPED ov;
				memset(&ov, 0x00, sizeof(OVERLAPPED));
				::ResetEvent(ev);
				ov.hEvent = ev;

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
				
				memset(&ov, 0x00, sizeof(OVERLAPPED));
				::ResetEvent(ev);
				ov.hEvent = ev;

				if (!::ReadFile(com_handle, buff, 2, &rw, &ov)) {
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
