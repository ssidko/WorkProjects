#include "ControlUnit.h"

#define MAX_COM_PORT_NUMBER			50
#define RESPONSE_CODE				0x6948

ControlUnit::ControlUnit(void) : handle(NULL), com_name(""), opened(false)
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
		if (!SetCommState(handle, &com_param)) {
			::CloseHandle(handle);
			return INVALID_HANDLE_VALUE;
		}
		return handle;
	}
	return INVALID_HANDLE_VALUE;
}

bool ControlUnit::WriteComPort(HANDLE handle, const void *buff, int size)
{
	DWORD rw = 0;
	OVERLAPPED o;
	memset(&o, 0x00, sizeof(o));
	HANDLE ev = ::CreateEventA(NULL, FALSE, FALSE /*nonsignaled*/, NULL);
	if (!ev) return false;
	o.hEvent = ev;
	if (!::WriteFile(handle, buff, size, &rw, &o)) {
		if (ERROR_IO_PENDING != ::GetLastError()) {
			return false;
		}
	}
	return (WAIT_OBJECT_0 == WaitForSingleObject(ev, 1000));
}

bool ControlUnit::ReadComPort(HANDLE handle, void *buff, int size)
{
	DWORD rw = 0;
	OVERLAPPED o;
	memset(&o, 0x00, sizeof(o));
	HANDLE ev = ::CreateEventA(NULL, FALSE, FALSE /*nonsignaled*/, NULL);
	if (!ev) return false;
	o.hEvent = ev;
	if (!::ReadFile(handle, buff, size, &rw, &o)) {
		if (ERROR_IO_PENDING != ::GetLastError()) {
			return false;
		}
	}
	return (WAIT_OBJECT_0 == ::WaitForSingleObject(ev, 1000));
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
		handle = OpenComPort(com_name.toLocal8Bit().data());
		if (handle != INVALID_HANDLE_VALUE) {
			if (IsControlUnit(handle)) {
				opened = true;
				return true;
			}
			::CloseHandle(handle);
		}
	}
	com_name = "";
	return false;
}

void ControlUnit::Close()
{
	com_name = "";
	if (handle && (handle != INVALID_HANDLE_VALUE)) {
		handle = NULL;
		::CloseHandle(handle);
		opened = false;
	}
}

bool ControlUnit::SenCommand(QString cmd)
{
	cmd += "\n";
	return 	WriteComPort(handle, cmd.toLocal8Bit().data(), cmd.size());
}
