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

bool DHFS::VideoFile::SaveFrameSequence(BYTE *buffer, FrameSequence &sequence)
{
	if (start_time.Seconds() == 0) {
		camera = sequence.camera;
		start_time = sequence.start_time; 
	}
	assert(camera == sequence.camera);
	end_time = sequence.end_time;
	return file.Write(buffer, sequence.size);
}

DHFS::FileStorage::FileStorage(int cam_count, std::string &raw_directory_path, std::string &mkv_directory_path) :
	raw_directory(raw_directory_path),
	mkv_directory(mkv_directory_path),
	frame_sequence_buffer(NULL)
{
	if (cam_count == 0) {
		files.resize(64, NULL);
	} else {
		files.resize(cam_count, NULL);
	}
	frame_sequence_buffer = new BYTE[MAX_SEQUENCE_SIZE];
	assert(frame_sequence_buffer);
}

DHFS::FileStorage::~FileStorage(void)
{
	if (frame_sequence_buffer) {
		delete[] frame_sequence_buffer;
	}
}

bool DHFS::FileStorage::SaveFrameSequence(W32Lib::FileEx &dhfs_volume, FrameSequence &sequence)
{
	if (sequence.size > MAX_SEQUENCE_SIZE) {
		throw std::exception();
	}

	assert(sequence.camera <= files.size());
	VideoFile *vfile = files[sequence.camera];

	if (vfile) {
		if (vfile->Size() >= MAX_VIDEO_FILE_SIZE) {
			delete vfile;
			vfile = NULL;
		} else if (sequence.start_time < vfile->EndTime()) {
			/*skip this sequence*/
			return true;
		} else if ((sequence.start_time - vfile->EndTime()) > (LONGLONG)2*60) {
			/*save in next file*/
			delete vfile;
			vfile = NULL;
		}
	}

	if (vfile == NULL) {
		vfile = CreateNewFile(sequence);
		if (vfile) {
			files[sequence.camera] = vfile;
		} else {
			return false;
		}
	}

	if (dhfs_volume.SetPointer(sequence.offset)) {
		if (dhfs_volume.Read(frame_sequence_buffer, sequence.size) == sequence.size) {
			return vfile->SaveFrameSequence(frame_sequence_buffer, sequence);
		}
	}

	return false;
}

DHFS::VideoFile *DHFS::FileStorage::CreateNewFile(FrameSequence &sequence)
{
	std::string new_file_name;
	GenerateFileName(new_file_name, sequence);
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

void DHFS::FileStorage::GenerateFileName(std::string &new_file_name, FrameSequence &sequence)
{
	std::stringstream stream;
	stream << raw_directory << "\\" << /*sequence.offset << " " <<*/ "[" << sequence.camera << "] ";
	stream << sequence.start_time.String() << ".264";
	new_file_name = stream.str();
}
