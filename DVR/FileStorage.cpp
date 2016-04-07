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
void DHFS::VideoFile::Close(void)
{
	file.Close();
}

bool DHFS::VideoFile::SaveFrameSequence(std::vector<BYTE> &sequence_buffer, FrameSequenceInfo &sequence_info)
{
	if (start_time.Seconds() == 0) {
		camera = sequence_info.start_frame.camera;
		start_time = sequence_info.start_frame.timestamp;
	}
	end_time = sequence_info.end_frame.timestamp;
	return (file.Write(&sequence_buffer[0], sequence_buffer.size()) == sequence_buffer.size());
}

bool DHFS::VideoFile::Rename(const std::string &new_name)
{
	if (file.Rename(new_name.data())) {
		name = new_name;
		return true;
	}
	return false;
}

DHFS::FileStorage::FileStorage(int cam_count, const std::string &out_directory_path) : out_directory(out_directory_path)
{
	if (cam_count == 0) {
		files.resize(64, NULL);
	} else {
		files.resize(cam_count, NULL);
	}
}

DHFS::FileStorage::~FileStorage(void)
{
	CloseAllFiles();
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
			CloseFile(sequence_info.start_frame.camera);
			vfile = NULL;
		} else if (sequence_info.start_frame.timestamp.Seconds() < vfile->EndTime().Seconds()) {
			if ((vfile->EndTime().Seconds() - sequence_info.start_frame.timestamp.Seconds()) > 10) {
				CloseFile(sequence_info.start_frame.camera);
				vfile = NULL;
			}
		} 


		//else if (sequence_info.start_frame.timestamp.Seconds() < vfile->EndTime().Seconds()) {
		//	/*skip this sequence*/
		//	return true;
		//} else if ((sequence_info.start_frame.timestamp.Seconds() - vfile->EndTime().Seconds()) > (LONGLONG)2*60) {
		//	/*save in next file*/
		//	//delete vfile;
		//	//vfile = NULL;
		//	CloseFile(sequence_info.start_frame.camera);
		//	vfile = NULL;
		//}
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
	std::string file_name;
	//GenerateFileName(file_name, sequence_info);
	file_name += out_directory;
	file_name += std::to_string(sequence_info.start_frame.camera+1);
	file_name += ".tmp";
	VideoFile *new_file = new VideoFile(file_name.c_str());
	if (new_file) {
		if (new_file->Create()) {
			return new_file;
		}
	}
	return NULL;
}

void DHFS::FileStorage::CloseFile(unsigned int file_index)
{
	VideoFile *vfile = files[file_index];
	if (vfile) {

		vfile->Close();

		//std::stringstream file_name;
		//file_name << out_directory << "[" << file_index + 1 << "]-" << vfile->StartTime().String() << "-=-" << vfile->EndTime().String() /*<< ".avi"*/;

		//vfile->Rename(file_name.str() + ".h264");

		//Convert2Avi(file_name.str() + ".h264", file_name.str() + ".avi");

		std::stringstream file_name;
		file_name << out_directory << "[" << file_index + 1 << "]-" << vfile->StartTime().String() << "-=-" << vfile->EndTime().String() << ".avi";

		Convert2Avi(vfile->Name(), file_name.str());


		delete files[file_index];
		files[file_index] = NULL;
	}
}

void DHFS::FileStorage::CloseAllFiles(void)
{
	for (DWORD i = 0; i < files.size(); ++i) {
		CloseFile(i);
	}
}

