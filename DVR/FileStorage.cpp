#include <exception>
#include <assert.h>
#include <sstream>
#include "FileStorage.h"

DHFS::VideoFile::VideoFile(std::string full_path) : camera(-1), name(full_path), file(full_path.c_str())
{
	start_time.Clear();
	end_time.Clear();
}

DHFS::VideoFile::~VideoFile( void )
{
	file.Close();
}

bool DHFS::VideoFile::Create(void)
{
	return file.Create();
}

bool DHFS::VideoFile::SaveFrameSequence(std::vector<BYTE> &sequence_buffer, FrameSequenceInfo &sequence_info)
{
	if (start_time.Seconds() == 0) {
		start_time = sequence_info.start_frame.timestamp;
	}
	end_time = sequence_info.end_frame.timestamp;
	return (file.Write(&sequence_buffer[0], sequence_buffer.size()) == sequence_buffer.size());
}

DHFS::FileStorage::FileStorage(int cam_count, const std::string &raw_directory_path, const std::string &mkv_directory_path) :
	raw_directory(raw_directory_path),
	mkv_directory(mkv_directory_path)
{
	if (cam_count == 0) {
		files.resize(64, NULL);
	} else {
		files.resize(cam_count, NULL);
	}
}

DHFS::FileStorage::~FileStorage(void)
{
}

bool DHFS::FileStorage::SaveFrameSequence(std::vector<BYTE> &sequence_buffer, FrameSequenceInfo &sequence_info)
{
	if (sequence_info.size > MAX_SEQUENCE_SIZE) {
		throw std::exception();
	}

	assert(sequence_info.start_frame.camera <= files.size());
	VideoFile *vfile = files[sequence_info.start_frame.camera];

	if (vfile) {
		if (vfile->Size() >= MAX_VIDEO_FILE_SIZE) {
			delete vfile;
			vfile = NULL;
		} else if (sequence_info.start_frame.timestamp < vfile->EndTime()) {
			/*skip this sequence*/
			return true;
		} else if ((sequence_info.start_frame.timestamp - vfile->EndTime()) > (LONGLONG)2*60) {
			/*save in next file*/
			delete vfile;
			vfile = NULL;
		}
	}

	if (vfile == NULL) {
		vfile = CreateNewFile(sequence_info);
		if (vfile) {
			files[sequence_info.start_frame.camera] = vfile;
		} else {
			return false;
		}
	}

	return vfile->SaveFrameSequence(sequence_buffer, sequence_info);
}

DHFS::VideoFile *DHFS::FileStorage::CreateNewFile(FrameSequenceInfo &sequence_info)
{
	std::string new_file_name;
	GenerateFileName(new_file_name, sequence_info);
	VideoFile *new_file = new VideoFile(new_file_name.c_str());
	if (new_file) {
		if (new_file->Create()) {
			return new_file;
		} else {
			delete new_file;
		}
	}
	return NULL;
}

void DHFS::FileStorage::GenerateFileName(std::string &new_file_name, FrameSequenceInfo &sequence_info)
{
	std::stringstream stream;
	stream << raw_directory << "\\" << /*sequence.offset << " " <<*/ "[" << sequence_info.start_frame.camera << "] ";
	stream << sequence_info.start_frame.timestamp.String() << ".h264";
	new_file_name = stream.str();
}
