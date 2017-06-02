#ifndef _LZ4_H
#define _LZ4_H

// 
// Returns 0 on success
// and non-zero on failure
// 
int lz4_decompress_zfs(void *s_start, void *d_start, size_t s_len, size_t d_len, int n);

#endif // _LZ4_H

