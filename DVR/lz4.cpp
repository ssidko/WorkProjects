#include <stdint.h>
#include <cstring>
#include "lz4.h"

#define LZ4_ARCH64			0

/*
* Architecture-specific macros
*/
#if LZ4_ARCH64
#define	STEPSIZE 8
#define	UARCH U64
#define	AARCH A64
#define	LZ4_COPYSTEP(s, d)	A64(d) = A64(s); d += 8; s += 8;
#define	LZ4_COPYPACKET(s, d)	LZ4_COPYSTEP(s, d)
#define	LZ4_SECURECOPY(s, d, e)	if (d < e) LZ4_WILDCOPY(s, d, e)
#define	HTYPE U32
#define	INITBASE(base)		const uint8_t* const base = ip
#else /* !LZ4_ARCH64 */
#define	STEPSIZE 4
#define	UARCH U32
#define	AARCH A32
#define	LZ4_COPYSTEP(s, d)	A32(d) = A32(s); d += 4; s += 4;
#define	LZ4_COPYPACKET(s, d)	LZ4_COPYSTEP(s, d); LZ4_COPYSTEP(s, d);
#define	LZ4_SECURECOPY		LZ4_WILDCOPY
#define	HTYPE const uint8_t *
#define	INITBASE(base)		const int base = 0
#endif /* !LZ4_ARCH64 */

#define	LZ4_WILDCOPY(s, d, e) do { LZ4_COPYPACKET(s, d) } while (d < e);

/* Basic types */
#define	BYTE	uint8_t
#define	U16		uint16_t
#define	U32		uint32_t
#define	S32		int32_t
#define	U64		uint64_t

typedef struct _U16_S {
	U16 v;
} U16_S;
typedef struct _U32_S {
	U32 v;
} U32_S;
typedef struct _U64_S {
	U64 v;
} U64_S;

#define	A64(x) (((U64_S *)(x))->v)
#define	A32(x) (((U32_S *)(x))->v)
#define	A16(x) (((U16_S *)(x))->v)

#define	COPYLENGTH 8
#define	LASTLITERALS 5
#define	MFLIMIT (COPYLENGTH + MINMATCH)
#define	MINLENGTH (MFLIMIT + 1)

#define	MAXD_LOG 16
#define	MAX_DISTANCE ((1 << MAXD_LOG) - 1)

#define	ML_BITS					4
#define	ML_MASK					((1U<<ML_BITS)-1)
#define	RUN_BITS				(8-ML_BITS)
#define	RUN_MASK				((1U<<RUN_BITS)-1)

#define	LZ4_READ_LITTLEENDIAN_16(d, s, p) { d = (s) - A16(p); }

#define	expect(expr, value)    (expr)
#define	unlikely(expr)			expect((expr) != 0, 0)

#define	BE_IN8(xa)				*((uint8_t *)(xa))

#define	BE_IN16(xa)				(((uint16_t)BE_IN8(xa) << 8) | BE_IN8((uint8_t *)(xa)+1))

#define	BE_IN32(xa)				(((uint32_t)BE_IN16(xa) << 16) | BE_IN16((uint8_t *)(xa)+2))

static int LZ4_uncompress_unknownOutputSize(const char *source, char *dest, int isize, int maxOutputSize)
{
	/* Local Variables */
	const uint8_t *ip = (const uint8_t *)source;
	const uint8_t *const iend = ip + isize;
	const uint8_t *ref;

	uint8_t *op = (uint8_t *)dest;
	uint8_t *const oend = op + maxOutputSize;
	uint8_t *cpy;

	size_t dec32table[] = { 0, 3, 2, 3, 0, 0, 0, 0 };
#if LZ4_ARCH64
	size_t dec64table[] = { 0, 0, 0, (size_t)-1, 0, 1, 2, 3 };
#endif

	/* Main Loop */
	while (ip < iend) {
		unsigned token;
		size_t length;

		/* get runlength */
		token = *ip++;
		if ((length = (token >> ML_BITS)) == RUN_MASK) {
			int s = 255;
			while ((ip < iend) && (s == 255)) {
				s = *ip++;
				length += s;
			}
		}
		/* copy literals */
		cpy = op + length;
		/* CORNER-CASE: cpy might overflow. */
		if (cpy < op)
			goto _output_error;	/* cpy was overflowed, bail! */
		if ((cpy > oend - COPYLENGTH) ||
			(ip + length > iend - COPYLENGTH)) {
			if (cpy > oend)
				/* Error: writes beyond output buffer */
				goto _output_error;
			if (ip + length != iend)
				/*
				* Error: LZ4 format requires to consume all
				* input at this stage
				*/
				goto _output_error;
			(void)memcpy(op, ip, length);
			op += length;
			/* Necessarily EOF, due to parsing restrictions */
			break;
		}
		LZ4_WILDCOPY(ip, op, cpy);
		ip -= (op - cpy);
		op = cpy;

		/* get offset */
		LZ4_READ_LITTLEENDIAN_16(ref, cpy, ip);
		ip += 2;
		if (ref < (uint8_t * const)dest)
			/*
			* Error: offset creates reference outside of
			* destination buffer
			*/
			goto _output_error;

		/* get matchlength */
		if ((length = (token & ML_MASK)) == ML_MASK) {
			while (ip < iend) {
				int s = *ip++;
				length += s;
				if (s == 255)
					continue;
				break;
			}
		}
		/* copy repeated sequence */
		if (unlikely(op - ref < STEPSIZE)) {
#if LZ4_ARCH64
			size_t dec64 = dec64table[op - ref];
#else
			const int dec64 = 0;
#endif
			op[0] = ref[0];
			op[1] = ref[1];
			op[2] = ref[2];
			op[3] = ref[3];
			op += 4;
			ref += 4;
			ref -= dec32table[op - ref];
			A32(op) = A32(ref);
			op += STEPSIZE - 4;
			ref -= dec64;
		}
		else {
			LZ4_COPYSTEP(ref, op);
		}
		cpy = op + length - (STEPSIZE - 4);
		if (cpy > oend - COPYLENGTH) {
			if (cpy > oend)
				/*
				* Error: request to write outside of
				* destination buffer
				*/
				goto _output_error;
			LZ4_SECURECOPY(ref, op, (oend - COPYLENGTH));
			while (op < cpy)
				*op++ = *ref++;
			op = cpy;
			if (op == oend)
				/*
				* Check EOF (should never happen, since
				* last 5 uint8_ts are supposed to be literals)
				*/
				goto _output_error;
			continue;
		}
		LZ4_SECURECOPY(ref, op, cpy);
		op = cpy;	/* correction */
	}

	/* end of decoding */
	return (int)(((char *)op) - dest);

	/* write overflow error detected */
_output_error:
	return (int)(-(((char *)ip) - source));
}

int lz4_decompress_zfs(void *s_start, void *d_start, size_t s_len, size_t d_len, int n)
{
	const char *src = (char *)s_start;
	uint32_t bufsiz = BE_IN32(src);

	/* invalid compressed buffer size encoded at start */
	if (bufsiz + sizeof(bufsiz) > s_len)
		return (1);

	/*
	* Returns 0 on success (decompression function returned non-negative)
	* and non-zero on failure (decompression function returned negative.
	*/
	return (LZ4_uncompress_unknownOutputSize(&src[sizeof(bufsiz)], (char *)d_start, bufsiz, d_len) < 0);
}