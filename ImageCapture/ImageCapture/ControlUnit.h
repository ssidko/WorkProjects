#ifndef CONTROL_UNIT
#define CONTROL_UNIT

#include <QString>
#include <Windows.h>

class ControlUnit
{
private:
	HANDLE handle;
	QString com_name;
	static HANDLE OpenComPort(const char *name);
	static bool WriteComPort(HANDLE handle, const void *buff, int size);
	static bool ReadComPort(HANDLE handle, void *buff, int size);
	static bool IsControlUnit(HANDLE handle);
public:
	ControlUnit(void);
	~ControlUnit(void);
	static bool IsAvailable(void);
	bool Open();
	void Close();
	bool SenCommand(QString cmd);
};

#endif

