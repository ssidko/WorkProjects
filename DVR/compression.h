#ifndef _COMPRESSION_H
#define _COMPRESSION_H

#include "zfs_type.h"

int lzjb_decompress(void *s_start, void *d_start, uint32_t s_len, uint32_t d_len);


#endif // _COMPRESSION_H
