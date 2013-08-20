#include "mxf.h"

#define MXF_VIDEO_FOLDER		_T("VIDEO")
#define MXF_AUDIO_FOLDER		_T("AUDIO")

typedef struct _MXF_HEADER {
	
};

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

int mxf_run(PhysicalDrive &drive, String &video_path, String &audio_path)
{

}


int mxf_main(DWORD drive_number, TCHAR *out_dir)
{
	assert(out_dir);
	PhysicalDrive drive(drive_number);
	if (!drive.Open()) {
		return -1;
	}
	String video_path = _T("");
	String audio_path = _T("");
	if (!CreateSubFolders(out_dir, video_path, audio_path)) {
		return -1;
	}

	return mxf_run();
}

