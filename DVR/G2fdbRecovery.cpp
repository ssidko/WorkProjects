#include "G2fdbRecovery.h"

using namespace G2FDB;

int G2FDB::StartRecovery(const std::string & dvr_volume, const std::string & out_directory, const dvr::Timestamp & start_time, const dvr::Timestamp & end_time)
{
	G2fdbVolume volume(dvr_volume); 
	VideoStorage storage;
	FrameSequence sequence;

	if (volume.Open() && storage.Open(out_directory)) {

		sequence.Clear();
		sequence.data.reserve(MAX_FRAME_SEQUANCE_SIZE);

		while (volume.ReadFrameSequence(sequence, 60)) {

			storage.SaveFrameSequence(sequence);
			sequence.Clear();
		}

		return 0;
	}

	return -1;
}

//
//	class VideoStorage
//

bool G2FDB::VideoStorage::IsDirectoryExist(const std::string & directory)
{
	return false;
}

bool G2FDB::VideoStorage::Open(const std::string & storage_directory)
{
	base_directory = storage_directory;

	while (base_directory.size()) {
		char ch = base_directory.back();
		if (isspace(ch)) {
			base_directory.pop_back();
		} else {
			if (ch != '\\') {
				base_directory += "\\";
			}
			break;
		}
	}

	bool result = ::CreateDirectoryA(base_directory.c_str(), NULL);

	if (!result) {
		if (ERROR_ALREADY_EXISTS == ::GetLastError()) {
			result = true;
		}	
	}

	return result;
}

bool G2FDB::VideoStorage::SaveFrameSequence(FrameSequence & sequennce)
{
	return false;
}

//
//	class VideoFile
//