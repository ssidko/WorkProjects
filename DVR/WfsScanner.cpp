#include "WfsScanner.h"

using namespace WFS;

#define IO_BUFFER_SIZE				(DWORD)256*512	


WFS::Scanner::Scanner(const std::string &file_name) : io(file_name, IO_BUFFER_SIZE)
{
	if (!io.Open()) {
		throw std::exception("Не удалось открыть файл.");
	}
}

WFS::Scanner::~Scanner()
{
}

void WFS::Scanner::NextFrame(const LONGLONG &offset)
{


}
