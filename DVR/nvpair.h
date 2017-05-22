#ifndef _NVPAIR_H
#define _NVPAIR_H

#include <list>
#include <vector>
#include "XdrReader.h"

/*
* XDR encoding functions
*
* An xdr packed nvlist is encoded as:
*
*  - encoding methode and host endian (4 bytes)
*  - nvl_version (4 bytes)
*  - nvl_nvflag (4 bytes)
*
*  - encoded nvpairs, the format of one xdr encoded nvpair is:
*	- encoded size of the nvpair (4 bytes)
*	- decoded size of the nvpair (4 bytes)
*	- name string, (4 + sizeof(NV_ALIGN4(string))
*	  a string is coded as size (4 bytes) and data
*	- data type (4 bytes)
*	- number of elements in the nvpair (4 bytes)
*	- data
*
*  - 2 zero's for end of the entire list (8 bytes)
*/

#pragma pack(push, 1)

struct nvlist_header {
	char encoding;
	char endian;
	char reserved1;
	char reserved2;
	uint32_t version;
	uint32_t flag;
};

struct xdr_nvpair {
	uint32_t encoded_size;
	uint32_t decoded_size;
	uint32_t name_size;
};

#pragma pack(pop)

typedef enum {
	DATA_TYPE_UNKNOWN = 0,			// 0x00
	DATA_TYPE_BOOLEAN,				// 0x01
	DATA_TYPE_BYTE,					// 0x02
	DATA_TYPE_INT16,				// 0x03
	DATA_TYPE_UINT16,				// 0x04
	DATA_TYPE_INT32,				// 0x05
	DATA_TYPE_UINT32,				// 0x06
	DATA_TYPE_INT64,				// 0x07
	DATA_TYPE_UINT64,				// 0x08
	DATA_TYPE_STRING,				// 0x09
	DATA_TYPE_BYTE_ARRAY,			// 0x0A
	DATA_TYPE_INT16_ARRAY,			// 0x0B
	DATA_TYPE_UINT16_ARRAY,			// 0x0C
	DATA_TYPE_INT32_ARRAY,			// 0x0D
	DATA_TYPE_UINT32_ARRAY,			// 0x0E
	DATA_TYPE_INT64_ARRAY,			// 0x0F
	DATA_TYPE_UINT64_ARRAY,			// 0x10
	DATA_TYPE_STRING_ARRAY,			// 0x11
	DATA_TYPE_HRTIME,				// 0x12
	DATA_TYPE_NVLIST,				// 0x13
	DATA_TYPE_NVLIST_ARRAY,			// 0x14	
	DATA_TYPE_BOOLEAN_VALUE,		// 0x15
	DATA_TYPE_INT8,					// 0x16
	DATA_TYPE_UINT8,				// 0x17
	DATA_TYPE_BOOLEAN_ARRAY,		// 0x18
	DATA_TYPE_INT8_ARRAY,			// 0x19
	DATA_TYPE_UINT8_ARRAY,			// 0x20
	DATA_TYPE_DOUBLE				// 0x21
} data_type;

enum ValueType {
	kInteger = 1,
	kString,
	kNVList
};

struct nvpair_base
{
	std::string name;
	ValueType type;

	nvpair_base(const std::string &name_, ValueType type_) : name(name_), type(type_) {}
	virtual ~nvpair_base()
	{
		int x = 0;
	}
};

template<typename T, ValueType vt>
struct nvpair : public nvpair_base
{
	T value;
	nvpair(const std::string &name_, const T &value_ = T()) : nvpair_base(name_, vt), value(value_) {}
};

typedef nvpair<unsigned long long, kInteger> NVInteger;
typedef nvpair<std::string, kString> NVString;
typedef nvpair<std::list<nvpair_base *>, kNVList> NVList;

size_t DecodeXdrNVPair(XdrReader &xdr, nvpair_base *nvp);
bool DecodeXdrNVList(uint8_t *buff, size_t size);

#endif // _NVPAIR_H