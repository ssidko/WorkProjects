#ifndef _G2FDB_RECOVERY_H
#define _G2FDB_RECOVERY_H

#include "Timestamp.h"
#include <string>

namespace G2FDB
{
	int StartRecovery(const std::string &dvr_volume, const std::string &out_directory, const dvr::Timestamp &start_time, const dvr::Timestamp &end_time);
}

#endif // _G2FDB_RECOVERY_H