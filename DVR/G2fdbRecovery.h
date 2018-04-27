#ifndef _G2FDB_RECOVERY_H
#define _G2FDB_RECOVERY_H

#include <string>
#include "File.h"
#include "dvr.h"
#include "G2fdbVolume.h"

namespace G2FDB
{
	int StartRecovery(const dvr::RecoveryTask &task);
	void StartRcoveryByMetadata(const dvr::RecoveryTask &task);
	void TestRecovery(void);
}

#endif // _G2FDB_RECOVERY_H