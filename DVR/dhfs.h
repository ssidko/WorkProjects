#ifndef _DHFS
#define _DHFS

#include <windows.h>
#include "Timestamp.h"
#include "File.h"

#define BEGIN_NAMESPACE_DHFS		namespace DHFS {
#define END_NAMESPACE_DHFS			}

BEGIN_NAMESPACE_DHFS

#define FRAME_HEADER_MAGIC			((DWORD)0x56414844)
#define FRAME_FOOTER_MAGIC			((DWORD)0x76616864)
	
#pragma pack(push)
#pragma pack(1)
	
typedef struct _TIMESTAMP {
	DWORD seconds:6;
	DWORD minutes:6;
	DWORD hours:5;
	DWORD day:5;
	DWORD month:4;
	DWORD year:6;	
} TIMESTAMP;

typedef struct _FRAME_HEADER {
	DWORD magic;					// FRAME_HEADER_MAGIC
	WORD flags;
	WORD camera;
	DWORD counter;
	DWORD size;
	TIMESTAMP time;
	BYTE unk1[8];
	DWORD unk2;
	BYTE data[1];
} FRAME_HEADER;

typedef struct _FRAME_FOOTER {
	DWORD magic;					// FRAME_FOOTER_MAGIC
	DWORD size;
} FRAME_FOOTER;

#pragma pack(pop)

bool NextFrame(W32Lib::FileEx &file);
int StartRecovering(void);

END_NAMESPACE_DHFS

#endif // _DHFS