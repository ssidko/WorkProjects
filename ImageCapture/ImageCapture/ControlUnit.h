#ifndef CONTROL_UNIT
#define CONTROL_UNIT

#include <QString>
#include <Windows.h>

//			
// TODO: Реализовать механизм получения доступных в системе COM портов по правильному,
//		 а не через тупой перебор :).
//

class ControlUnit
{
private:
	HANDLE com_handle;
	QString com_name;
	bool opened;
	static HANDLE OpenComPort(const char *name);
	static bool WriteComPort(HANDLE handle, const void *buff, int size);
	static bool ReadComPort(HANDLE handle, void *buff, int size);
	static bool IsControlUnit(HANDLE handle);
public:
	ControlUnit(void);
	~ControlUnit(void);
	static bool IsAvailable(void);
	bool IsOpened(void) {return opened;}
	bool Open();
	void Close();
	bool SenCommand(QString cmd);
	bool Testing(void);
};

#endif

