#ifndef _FLATCHER_H
#define _FLATCHER_H

#include <stdint.h>
#include "zfs.h"

void fletcher_2_native(const void *buf, uint64_t size, zio_cksum_t *zcp);
void fletcher_4_native(const void *buf, uint64_t size, zio_cksum_t *zcp);

#endif // _FLATCHER_H
