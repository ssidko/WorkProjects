#ifndef _SHA256_H
#define _SHA256_H

#include "zfs.h"

void zio_checksum_SHA256(const void *buf, uint64_t size, zio_cksum_t *zcp);

#endif // _SHA256_H
