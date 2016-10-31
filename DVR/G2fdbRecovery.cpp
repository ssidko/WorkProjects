#include "G2fdbRecovery.h"
#include "G2fdbVolume.h"

using namespace G2FDB;

int G2FDB::StartRecovery(const std::string & dvr_volume, const std::string & out_directory, const dvr::Timestamp & start_time, const dvr::Timestamp & end_time)
{
	G2fdbVolume volume(dvr_volume); 
	if (!volume.Open()) { 
		return -1;
	}

	Frame frame;
	FrameSequence sequence;
	LONGLONG offset = 0;

	if (volume.ReadFrameSequence(sequence)) {


		sequence.Clear();
	}

	return -1;
}
