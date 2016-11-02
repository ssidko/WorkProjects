#include "G2fdbRecovery.h"
#include "G2fdbVolume.h"

using namespace G2FDB;

int G2FDB::StartRecovery(const std::string & dvr_volume, const std::string & out_directory, const dvr::Timestamp & start_time, const dvr::Timestamp & end_time)
{
	G2fdbVolume volume(dvr_volume); 
	if (!volume.Open()) { 
		return -1;
	}

	FrameSequence sequence;
	LONGLONG offset = 0;

	sequence.data.reserve(MAX_FRAME_SEQUANCE_SIZE);

	while (volume.ReadFrameSequence(sequence, 60)) {


		sequence.Clear();
	}

	return -1;
}
