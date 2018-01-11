#include "flatcher.h"

void fletcher_2_native(const void *buf, uint64_t size, zio_cksum_t *zcp)
{
	const uint64_t *ip = (const uint64_t *)buf;
	const uint64_t *ipend = ip + (size / sizeof(uint64_t));
	uint64_t a0, b0, a1, b1;

	for (a0 = b0 = a1 = b1 = 0; ip < ipend; ip += 2) {
		a0 += ip[0];
		a1 += ip[1];
		b0 += a0;
		b1 += a1;
	}

	ZIO_SET_CHECKSUM(zcp, a0, a1, b0, b1);
}

void fletcher_2_byteswap(const void *buf, uint64_t size, zio_cksum_t *zcp)
{
	const uint64_t *ip = (const uint64_t *)buf;
	const uint64_t *ipend = ip + (size / sizeof(uint64_t));
	uint64_t a0, b0, a1, b1;

	for (a0 = b0 = a1 = b1 = 0; ip < ipend; ip += 2) {
		a0 += BSWAP_64(ip[0]);
		a1 += BSWAP_64(ip[1]);
		b0 += a0;
		b1 += a1;
	}

	ZIO_SET_CHECKSUM(zcp, a0, a1, b0, b1);
}

void fletcher_4_native(const void *buf, uint64_t size, zio_cksum_t *zcp)
{
	const uint32_t *ip = (const uint32_t *)buf;
	const uint32_t *ipend = ip + (size / sizeof(uint32_t));
	uint64_t a, b, c, d;

	for (a = b = c = d = 0; ip < ipend; ip++) {
		a += ip[0];
		b += a;
		c += b;
		d += c;
	}

	ZIO_SET_CHECKSUM(zcp, a, b, c, d);
}

void fletcher_4_byteswap(const void *buf, uint64_t size, zio_cksum_t *zcp)
{
	const uint32_t *ip = (const uint32_t *)buf;
	const uint32_t *ipend = ip + (size / sizeof(uint32_t));
	uint64_t a, b, c, d;

	for (a = b = c = d = 0; ip < ipend; ip++) {
		a += BSWAP_32(ip[0]);
		b += a;
		c += b;
		d += c;
	}

	ZIO_SET_CHECKSUM(zcp, a, b, c, d);
}

void fletcher_4_incremental_native(const void *buf, uint64_t size, zio_cksum_t *zcp)
{
	const uint32_t *ip = (const uint32_t *)buf;
	const uint32_t *ipend = ip + (size / sizeof(uint32_t));
	uint64_t a, b, c, d;

	a = zcp->word[0];
	b = zcp->word[1];
	c = zcp->word[2];
	d = zcp->word[3];

	for (; ip < ipend; ip++) {
		a += ip[0];
		b += a;
		c += b;
		d += c;
	}

	ZIO_SET_CHECKSUM(zcp, a, b, c, d);
}

void fletcher_4_incremental_byteswap(const void *buf, uint64_t size, zio_cksum_t *zcp)
{
	const uint32_t *ip = (const uint32_t *)buf;
	const uint32_t *ipend = ip + (size / sizeof(uint32_t));
	uint64_t a, b, c, d;

	a = zcp->word[0];
	b = zcp->word[1];
	c = zcp->word[2];
	d = zcp->word[3];

	for (; ip < ipend; ip++) {
		a += BSWAP_32(ip[0]);
		b += a;
		c += b;
		d += c;
	}

	ZIO_SET_CHECKSUM(zcp, a, b, c, d);
}
