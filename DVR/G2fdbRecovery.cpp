#include "G2fdbRecovery.h"
#include <sstream>
#include "utility.h"

using namespace G2FDB;

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

		while (volume.ReadFrameSequence(sequence, max_sequence_size, max_delta_time)) {
			storage.SaveFrameSequence(sequence);
			sequence.Clear();
		}
		return 0;
	}
	return -1;
}

#define G2FDB_UNIT_SIGNATURE	"G2FrameDbIdxSeg"

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
	constexpr const size_t signature_size = sizeof(G2FDB_UNIT_SIGNATURE) - 1 ;
	
	char unit_signature[signature_size] = {0};
	std::vector<frame_descriptor_t> descriptors(descriptors_count);

	G2fdbVolume volume(task.io_name);
	BufferedFile io(task.io_name, 128*1024);
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
			io.Read(unit_signature, signature_size);
			if (std::memcmp(unit_signature, G2FDB_UNIT_SIGNATURE, signature_size) != 0) {
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
	task.output_dir = "F:\\43889\\video-500Gb";
	task.io_name = "\\\\.\\PhysicalDrive2";
	task.io_offset = 4196352ull * 512;

	if (auto size = GetPhysicalDriveSize2(task.io_name)) {
		task.io_size = size.value() - task.io_offset;
		//G2FDB::StartRecovery(task);
		G2FDB::StartRcoveryByMetadata(task);
	}	
}

