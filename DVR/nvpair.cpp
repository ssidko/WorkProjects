#include "nvpair.h"
#include <assert.h>
#include <intrin.h> // for byte swap; exclusive for Visual C++


#define MAX_VALUE_SIZE					2147483647
#define	XDR_NVPAIR_MIN_SIZE				((size_t)(5 * 4))


size_t DecodeNVPair(XdrReader &xdr, nvpair_base *&nvp)
{
	if (xdr.Length() < XDR_NVPAIR_MIN_SIZE) {
		return 0;
	}

	nvp = nullptr;
	NVList * nvlist = nullptr;

	size_t encode_size = xdr.UInt32();
	size_t decode_size = xdr.UInt32();

	if (encode_size == 0) {
		if (decode_size == 0) {
			return 8;
		} else {
			assert(false);
		}
	}

	std::string name = xdr.String();
	uint32_t value_type = xdr.UInt32();
	uint32_t nelem = xdr.UInt32();

	size_t nvp_size = 4 + 4 + 4 + ALIGN4(name.length()) + 4 + 4;

	switch (value_type) {
	case DATA_TYPE_BYTE:
	case DATA_TYPE_INT8:
	case DATA_TYPE_UINT8:
	case DATA_TYPE_INT16:
	case DATA_TYPE_UINT16:
	case DATA_TYPE_INT32:
	case DATA_TYPE_UINT32:
		assert(nelem == 1);
		nvp = new NVInteger(name, xdr.UInt32());
		nvp_size += 4;
		break;

	case DATA_TYPE_HRTIME:
	case DATA_TYPE_INT64:
	case DATA_TYPE_UINT64:
		assert(nelem == 1);
		nvp = new NVInteger(name, xdr.Int64());
		nvp_size += 8;
		break;

	case DATA_TYPE_STRING:
		assert(nelem == 1);
		nvp = new NVString(name, xdr.String());
		nvp_size += 4; 
		nvp_size += ALIGN4(((NVString *)nvp)->value.size());
		break;

	case DATA_TYPE_NVLIST:
		assert(nelem == 1);
		nvp = new NVList(name);
		nvp_size += DecodeEmbeddedNVList(xdr, *((NVList *)nvp));
		break;

	case DATA_TYPE_NVLIST_ARRAY:
		nvp = new NVList(name);
		for (int i = 0; i < nelem; i++) {
			nvlist = new NVList(name + "[" + std::to_string(i) + "]");		
			(*(NVList *)nvp).pairs.push_back(nvlist);
			nvp_size += DecodeEmbeddedNVList(xdr, *nvlist);
		}
		break;

	case DATA_TYPE_BOOLEAN:
		assert(nelem == 0);
		nvp = new NVBool(name, false);
		break;

	case DATA_TYPE_BOOLEAN_VALUE:
		assert(nelem == 1);
		nvp = new NVBool(name, xdr.UInt32());
		nvp_size += 4;
		break;

	case DATA_TYPE_DOUBLE:
		assert(nelem == 1);

	case DATA_TYPE_BYTE_ARRAY:
	case DATA_TYPE_INT16_ARRAY:
	case DATA_TYPE_UINT16_ARRAY:
	case DATA_TYPE_INT32_ARRAY:
	case DATA_TYPE_UINT32_ARRAY:
	case DATA_TYPE_INT64_ARRAY:
	case DATA_TYPE_UINT64_ARRAY:
	case DATA_TYPE_STRING_ARRAY:
	case DATA_TYPE_BOOLEAN_ARRAY:
	case DATA_TYPE_INT8_ARRAY:	
	case DATA_TYPE_UINT8_ARRAY:	

	default:
		assert(false);		
	}

	if (nvp_size == encode_size) {
		assert(nvp);
		return nvp_size;
	} else {
		assert(false);
	}	

	return 0;
}

size_t DecodeEmbeddedNVList(XdrReader &xdr, NVList &nvlist)
{
	uint32_t version = xdr.UInt32();
	uint32_t nv_flags = xdr.UInt32();

	size_t nvl_size = 4 + 4;
	size_t nvp_size = 0;

	nvpair_base *nvp = nullptr;

	while (nvp_size = DecodeNVPair(xdr, nvp)) {
		nvl_size += nvp_size;
		if (nvp_size == 8) {			
			/*End of nvlist*/
			break; 
		} else {
			nvlist.pairs.push_back(nvp);
		}
	}

	return nvl_size;
}

bool DecodeXdrNVList(NVList &nvlist, uint8_t *buff, size_t size)
{
	assert(buff);
	assert(size);

	try {
		XdrReader xdr(buff, size);
		nvpair_base *nvp = nullptr;
		size_t sz = DecodeEmbeddedNVList(xdr, nvlist);
		return true;
	}
	catch (const std::exception &ex) {
		return false;
	}
}