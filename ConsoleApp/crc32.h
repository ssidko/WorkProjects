#ifndef _CRC32_H
#define _CRC32_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FINAL 0xffffffff

uint32_t crc32(const uint8_t *buf, size_t size, uint32_t start_crc = 0);

#endif // _CRC32_H