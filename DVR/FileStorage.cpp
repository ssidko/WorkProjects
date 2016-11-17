#include <exception>
#include <assert.h>
#include <sstream>
#include "FileStorage.h"
#include <cmath>


//
// class VideoFile
//

DHFS::VideoFile::VideoFile(std::string full_path) : 
	camera(-1),
	name(full_path),
	file(full_path.c_str()),
	end_sync_counter(0),
	first_offset(0),
	last_offset(0)
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
		first_offset = sequence_info.start_frame.offset;
	}
	end_time = sequence_info.end_frame.timestamp;
	end_sync_counter = sequence_info.end_sync_counter;
	last_offset = sequence_info.end_frame.offset;
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


//
// class FileStorage
//

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

bool DHFS::FileStorage::SaveFrameSequence(std::vector<BYTE>& sequence_buffer, FrameSequenceInfo & sequence_info)
{
	const LONGLONG max_disatance = 10LL * 1024 * 1024 * 1024;
	const LONGLONG fps = 25;

	size_t delta_time = 0;
	size_t delta_counter = 0;
	size_t time_threshold = 30 *60;

	VideoFile *out_file = nullptr;
	std::list<VideoFile *> &files = storage[sequence_info.Camera()];
	std::list<VideoFile *> for_remove;

	for (VideoFile *vfile : files) {

		assert(vfile);

		if (sequence_info.start_frame.timestamp.Seconds() >= vfile->EndTime().Seconds()) {
			delta_time = sequence_info.start_frame.timestamp.Seconds() - vfile->EndTime().Seconds();
			if ((delta_time) <= time_threshold) {
				if ((sequence_info.start_sync_counter > vfile->EndSyncCounter())) {
					
					delta_counter = sequence_info.start_sync_counter - vfile->EndSyncCounter();
					if (delta_time == 0x00) {
						if (delta_counter < (2 * fps)) {
							out_file = vfile;
							break;
						}
					} else {
						if ((std::fdim(delta_time * fps, delta_counter)) <= (2 * fps)) {
							out_file = vfile;
							break;
						}		
					}

				} else {
					int x = 0;
				}
			}
		}			

		if ((sequence_info.start_frame.offset - vfile->LastFrameOffset()) >= max_disatance) {
			for_remove.push_back(vfile);
		}
	}

	for (VideoFile *vfile : for_remove) {
		CloseFile(vfile);
		files.remove(vfile);	
	}
	for_remove.clear();


	if (out_file && (out_file->Size() >= MAX_VIDEO_FILE_SIZE)) {
		CloseFile(out_file);
		files.remove(out_file);
		out_file = nullptr;
	}

	if (!out_file) {
		out_file = CreateNewFile(sequence_info);
		if (out_file) {
			files.push_back(out_file);
		}
	}

	if (out_file) {
		return out_file->SaveFrameSequence(sequence_buffer, sequence_info);
	} else {
		return false;
	}
}

DHFS::VideoFile *DHFS::FileStorage::CreateNewFile(FrameSequenceInfo &sequence_info)
{
	static LONGLONG file_counter = 0;
	std::string file_name;
	//GenerateFileName(file_name, sequence_info);
	file_name += out_directory;
	file_name += std::to_string(sequence_info.start_frame.camera+1);
	file_name += "--";
	file_name += std::to_string(file_counter);
	file_name += ".tmp";
	VideoFile *new_file = new VideoFile(file_name.c_str());
	if (new_file) {
		++file_counter;
		if (new_file->Create()) {
			return new_file;
		}
	}
	return NULL;
}

void DHFS::FileStorage::CloseFile(VideoFile *vfile)
{
	if (vfile) {

		std::string old_name = vfile->Name();

		vfile->Close();

		std::stringstream file_name;
		file_name << out_directory << "[" << vfile->Camera() + 1 << "]-";
		file_name << vfile->StartTime().String() << "-=-" << vfile->EndTime().String();
		file_name << "-=-" << std::to_string(vfile->FirstFrameOffset());
		file_name << "--" << std::to_string(vfile->LastFrameOffset()) << ".avi";

		//Convert2Avi(vfile->Name(), file_name.str());
		ConvertToMkv(vfile->Name(), file_name.str());

		// Only rename
		//DWORD error = 0;
		//W32Lib::FileEx out_file(vfile->Name().c_str());
		//if (out_file.Open()) {
		//	out_file.Close();
		//	bool result = out_file.Rename(file_name.str().str().c_str());
		//	if (!result) {
		//		error = ::GetLastError();
		//	}
		//}

		::DeleteFileA(old_name.data());

		delete vfile;
	}
}

void DHFS::FileStorage::CloseAllFiles(void)
{
	for (auto pair : storage) {
		for (auto file : pair.second) {
			CloseFile(file);		
		}
		pair.second.clear();		
	}
	storage.clear();
}

