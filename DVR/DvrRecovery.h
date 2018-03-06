#pragma once

#ifndef _DVR_RECOVERY_H
#define _DVR_RECOVERY_H

#include <string>
#include <stdint.h>

#include "dvr.h"

typedef struct DvrRecoveryTask {
	std::string io_name;
	uint64_t io_offset;
	uint64_t io_size;
	std::string output_dir;
	dvr::Timestamp start_time;
	dvr::Timestamp end_time;
};

#endif // _DVR_RECOVERY_H
