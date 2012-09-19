#include <windows.h>
#include <tchar.h>

namespace DM 
{
	class Exception	{
	private:
	public:
		Exception() {};
		~Exception() {};
	}

	typedef enum (
		DWORD kRead << 1;
		DWORD kWrite << 2;
		DWORD kSharedRead << 3;
		DWORD kSharedWrite << 4;
	);

	class IFile {
	private:
	public:
		IFile() {};
		virtual ~IFile() = 0;

		virtual Open() = 0;
		virtual Close() = 0;
		virtual Read(BYTE *buff, DWORD size) = 0;
		virtual Write(BYTE *buff, DWORD size) = 0;
		virtual SetPointer(LONGLONG &ptr) = 0;
	};

	class IBlockDevice {
	public:
		virtual Read();
		virtual Write();
		virtual SetPointer();
		virtual Open();
		virtual Close();
	};

} // End of namespace DM