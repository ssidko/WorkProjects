#pragma once
#include "W32Lib.h"

#define MOV_ATOM_TYPE_FILE_TYPE							(DWORD)'pytf'	// 'ftyp'
#define MOV_ATOM_TYPE_FREE								(DWORD)'eerf'	// 'free'
#define MOV_ATOM_TYPE_MEDIA_DATA						(DWORD)'tadm'	// 'mdat'

#define MP4_END_SIGNATURE								"</NonRealTimeMeta>\r\n"

typedef struct _MOV_ATOM {
	DWORD size;
	DWORD type;
} MOV_ATOM, *PMOV_ATOM;

typedef struct _MP4_HEADER {
	MOV_ATOM ftyp;
	BYTE ftyp_data[16];
	MOV_ATOM free;
	MOV_ATOM mdat;
	BYTE mdat_data[1];
} MP4_HEADER, *PMP4_HEADER;

class Mp4File : public FileEx
{
public:
	Mp4File(TCHAR *name) : FileEx(name) {}
	~Mp4File(void) {}

	BOOL CheckStructure();
};
