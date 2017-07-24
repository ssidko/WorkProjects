#ifndef _CRC32_H
#define _CRC32_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint32_t crc32buf(const char *buf, size_t len);

#endif // _CRC32_H