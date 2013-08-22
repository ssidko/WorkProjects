#include "mxf.h"

namespace mxf
{

#define MXF_CLIP_FOLDER			_T("CLIP")
#define MXF_VIDEO_FOLDER		_T("VIDEO")
#define MXF_AUDIO_FOLDER		_T("AUDIO")

#define MXF_SIGNATURE_1			(DWORD)0x342B0E06
#define MXF_SIGNATURE_2			(DWORD)0x01010502

#define MXF_TAIL_SIZE			(DWORD)224

typedef struct _MXF_HEADER {
	DWORD sign1;
	DWORD sign2;
	BYTE unk1[40];
	DWORD tail_offset;

	bool IsValid(void)
	{
		return ((sign1 == MXF_SIGNATURE_1) && (sign2 == MXF_SIGNATURE_2));
	}
	
	DWORD TailOffset(void)
	{
		return Be2Le(&tail_offset);
	}
} MXF_HEADER;

bool CreateSubFolders(TCHAR *path, String &video_path, String &audio_path)
{
	String path_str = path;
	if (path_str[path_str.length()-1] != _T('\\')) {
		path_str += _T('\\');
	}
	video_path = path_str;
	video_path += MXF_VIDEO_FOLDER;
	if (!::CreateDirectory(video_path.c_str(), NULL)) {
		if (::GetLastError() != ERROR_ALREADY_EXISTS) {
			return FALSE;
		}
	}
	audio_path = path_str;
	audio_path += MXF_AUDIO_FOLDER;
	if (!::CreateDirectory(audio_path.c_str(), NULL)) {
		if (::GetLastError() != ERROR_ALREADY_EXISTS) {
			return FALSE;
		}
	}
	return TRUE;
}

int Run(PhysicalDrive &drive, String &video_path, String &audio_path)
{
	return 0;
}

} // namespace mxf {

int mxf_main(DWORD drive_number, TCHAR *out_dir)
{
	assert(out_dir);
	PhysicalDrive drive(drive_number);
	if (!drive.Open()) {
		return -1;
	}
	String video_path = _T("");
	String audio_path = _T("");
	if (mxf::CreateSubFolders(out_dir, video_path, audio_path)) {
		return -1;
	}

	return mxf::Run(drive, video_path, audio_path);
}


