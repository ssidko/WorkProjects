#ifndef _LZJB_H
#define _LZJB_H

#include <stdint.h>

int lzjb_decompress(void *srd, void *dst, size_t s_len, size_t d_len, int n);

#endif // _LZJB_H