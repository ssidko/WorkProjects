#include "lzjb.h"

#define NBBY    8
#define MATCH_BITS      6
#define MATCH_MIN       3
#define OFFSET_MASK     ((1 << (16 - MATCH_BITS)) - 1)

int lzjb_decompress(void *s_start, void *d_start, size_t s_len, size_t d_len, int n)
{
	uint8_t *src = (uint8_t *)s_start;
	uint8_t *dst = (uint8_t *)d_start;
	uint8_t *d_end = (uint8_t *)d_start + d_len;
	uint8_t *cpy;
	uint8_t copymap = 0;
	int copymask = 1 << (NBBY - 1);

	while (dst < d_end) {
		if ((copymask <<= 1) == (1 << NBBY)) {
			copymask = 1;
			copymap = *src++;
		}
		if (copymap & copymask) {
			int mlen = (src[0] >> (NBBY - MATCH_BITS)) + MATCH_MIN;
			int offset = ((src[0] << NBBY) | src[1]) & OFFSET_MASK;
			src += 2;
			if ((cpy = dst - offset) < (uint8_t *)d_start)
				return (-1);
			while (--mlen >= 0 && dst < d_end)
				*dst++ = *cpy++;
		}
		else {
			*dst++ = *src++;
		}
	}
	return (0);
}
