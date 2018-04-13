#include "G2fdbRecovery.h"
#include <sstream>
#include "utility.h"

using namespace G2FDB;

int G2FDB::StartRecovery(const std::string & dvr_volume, const std::string & out_directory, const dvr::Timestamp & start_time, const dvr::Timestamp & end_time)
{
	G2fdbVolume volume(dvr_volume); 
	VideoStorage storage;
	FrameSequence sequence;
	size_t max_delta_time = 60; // seconds

	if (volume.Open() && storage.Open(out_directory)) {

		sequence.Clear();
		sequence.data.reserve(MAX_FRAME_SEQUANCE_SIZE);

		//volume.SetPointer(500449675890LL);

		while (volume.ReadFrameSequence(sequence, max_delta_time)) {
			storage.SaveFrameSequence(sequence);
			sequence.Clear();
		}

		return 0;
	}

	return -1;
}

int G2FDB::StartRecovery(const dvr::RecoveryTask &task)
{	
	G2fdbVolume volume(task.io_name);
	dvr::VideoStorage storage(task.output_dir);
	dvr::FrameSequence sequence;

	const size_t max_sequence_size = 100 * 1024 * 1024;
	const size_t max_delta_time = 1;

	if (volume.Open() && storage.Open()) {
		
		sequence.Clear();
		volume.SetPointer(task.io_offset);

		try {

			while (volume.ReadFrameSequence(sequence, max_sequence_size, max_delta_time)) {
				storage.SaveFrameSequence(sequence);
				sequence.Clear();
			}
			return 0;

		} catch (std::system_error &exc) {
			std::string error_description = exc.what();
			return -1;
		}

	}
	return -1;
}

void G2FDB::StartRcoveryByMetadata(const dvr::RecoveryTask &task)
{
	const size_t unit_size = 256 * 1024 * 1024;
	const size_t descriptors_offset = 0x100000;
	const size_t data_offset = 0x500000;
	const size_t descriptors_size = data_offset - descriptors_offset;
	const size_t descriptors_count = descriptors_size / sizeof(frame_descriptor_t);

	const uint64_t volume_offset = task.io_offset;
	const uint64_t volume_size = task.io_size;
	const uint64_t units_count = volume_size / unit_size;

	const size_t signature_offset = 128;
	const size_t signature_size = std::strlen("G2FrameDbIdxSeg");
	
	std::string unit_signature = "G2FrameDbIdxSeg";
	std::vector<uint8_t> sign_buff(signature_size);
	std::vector<frame_descriptor_t> descriptors(descriptors_count);

	G2fdbVolume volume(task.io_name);
	BufferedFile io(task.io_name);
	dvr::VideoStorage storage(task.output_dir);
	dvr::Frame frame;
	dvr::FrameSequence sequence;

	if (volume.Open() && io.Open() && storage.Open()) {

		for (size_t unit = 0; unit <  units_count; unit ++) {
			
			uint64_t unit_offset = volume_offset + unit * unit_size;
		
			//
			// Check signature "G2FrameDbIdxSeg"
			//
			io.SetPointer(unit_offset + signature_offset);
			io.Read(sign_buff.data(), signature_size);
			if (std::memcmp(&unit_signature[0], sign_buff.data(), signature_size) != 0) {
				continue;
			}
		
			//
			// Read descriptors
			//
			io.SetPointer(unit_offset + descriptors_offset);
			io.Read(descriptors.data(), descriptors_size);

			frame.Clear();
			sequence.Clear();

			for (auto & descriptor : descriptors) {
			
				if (descriptor.signature == FRAME_HEADER_SIGNATURE) {
				
					frame_descriptor_t frame_header = {0};
					uint64_t frame_offset = unit_offset + data_offset + descriptor.frame_offset;

					io.SetPointer(frame_offset);
					io.Read(&(frame_header), sizeof(frame_descriptor_t));

					if (std::memcmp(&descriptor, &frame_header, sizeof(frame_descriptor_t)) != 0) {
						continue;
					}

					volume.SetPointer(frame_offset);
					if (volume.ReadFrame(frame)) {

						if (sequence.frames_count) {

							if ((frame.camera == sequence.camera) &&
								(frame.time.Seconds() >= sequence.end_time.Seconds()) &&
								((frame.time.Seconds() - sequence.end_time.Seconds()) <= 1))
							{
								sequence.AddFrame(frame);							
							} else {							
								storage.SaveFrameSequence(sequence);
								sequence.Clear();
								sequence.AddFirstFrame(frame);
							}
						
						} else {						
							sequence.AddFirstFrame(frame);						
						}
					
					}
				
				}
			
			}

			if (sequence.frames_count) {
				storage.SaveFrameSequence(sequence);
				sequence.Clear();
			}
		
		}
	
	}
}

void G2FDB::TestRecovery(void)
{
	dvr::RecoveryTask task;
	task.io_name = "\\\\.\\PhysicalDrive0";
	task.io_offset = 21495808ull * 512;
	task.io_size = 1932027311ull * 512;
	task.output_dir = "F:\\43889\\video-2";

	G2FDB::StartRecovery(task);
	G2FDB::StartRcoveryByMetadata(task);
}


//
//	class VideoStorage
//

bool G2FDB::VideoStorage::CreateDirectory(const std::string &directory)
{
	bool result = ::CreateDirectoryA(directory.c_str(), NULL);

	if (!result) {
		if (ERROR_ALREADY_EXISTS == ::GetLastError()) {
			result = true;
		}
	}

	return result;
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

bool G2FDB::VideoStorage::SaveFrameSequence(FrameSequence & sequence)
{
	// 2016-05-28
	char date[16] = {0};
	sprintf_s(date, sizeof(date), "%04u-%02u-%02u", sequence.start_time.year, sequence.start_time.month, sequence.start_time.day);

	std::string date_dir = base_directory + date + "\\";
	std::string cam_dir = date_dir + "CAM-" + std::to_string(sequence.camera) + "\\";
	std::string temp_file_name = cam_dir + "dvr.tmp";

	if (CreateDirectory(date_dir) && CreateDirectory(cam_dir)) {
		
		W32Lib::FileEx temp_file(temp_file_name.c_str());
		if (temp_file.Create()) {
			
			if (temp_file.Write(&sequence.data[0], sequence.data.size())) {

				std::stringstream file_name;
				file_name << cam_dir << "[" << sequence.camera << "]-";
				file_name << sequence.start_time.ToString() << "-=-" << sequence.end_time.ToString();
				file_name << "-=-" << std::to_string(sequence.offset);
				file_name << ".avi";

				Convert2Avi(temp_file_name, file_name.str());

				return true;			
			}
		}	
	}

	return false;
}
