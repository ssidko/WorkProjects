#include "WfsScanner.h"

using namespace WFS;

#define IO_BUFFER_SIZE				(DWORD)256*512
#define BUFFER_SIZE					(DWORD)32*512	

WFS::Scanner::Scanner(const std::string &file_name) : io(file_name, IO_BUFFER_SIZE), buffer_size(BUFFER_SIZE), buffer(nullptr)
{
}

WFS::Scanner::~Scanner()
{
	Close();
}

BOOL WFS::Scanner::Open(void)
{
	buffer = new BYTE[buffer_size];
	if (io.Open() && (buffer != nullptr)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void WFS::Scanner::Close(void)
{
	if (buffer != nullptr) {
		delete[] buffer;
		buffer = nullptr;
	}
}

void WFS::Scanner::SetPointer(const LONGLONG &offset)
{
	if (!io.SetPointer(offset)) {
		throw std::exception("Error: WFS::Scanner::SetPointer().");
	}
}

BOOL WFS::Scanner::NextFrameHeader(LONGLONG &offset)
{
	DWORD rw = 0;
	DWORD *sign = nullptr;
	DWORD sign_size = sizeof(DWORD);
	DWORD header_type = WFS::kUnknownType;

	if (!buffer) { return FALSE; }	
	memset(buffer, 0x00, buffer_size);

	io.Pointer(offset);
	while ((rw = io.Read(buffer, buffer_size)) >= sign_size) {
		
		for (DWORD i = 0; (i + sign_size) < rw; ++i) {
			sign = (DWORD *)&buffer[i];
			if ((*sign & WFS_FRAME_HEADER_MASK) == WFS_FRAME_HEADER_SIGNATURE) {
				io.SetPointer(offset);
				header_type = buffer[i+3] & 0x0F;
				return TRUE;
			}		
			++offset;
		}

		io.SetPointer(offset);
	}
	return FALSE;
}

BOOL WFS::Scanner::NextFrame(LONGLONG &offset)
{

	return FALSE;
}
