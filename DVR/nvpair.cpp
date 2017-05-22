#include "nvpair.h"

#include <assert.h>

#include <intrin.h> // for byte swap; exclusive for Visual C++

#define MAX_VALUE_SIZE					2147483647
#define	XDR_NVPAIR_MIN_SIZE				((size_t)(5 * 4))


size_t DecodeXdrNVPair(XdrReader &xdr)
{
	if (xdr.Length() < XDR_NVPAIR_MIN_SIZE) {
		return 0;
	}




	try {

		size_t encode_size = xdr.UInt32();
		size_t decode_size = xdr.UInt32();
		std::string name = xdr.String();
		uint32_t value_type = xdr.UInt32();
		uint32_t nelem = xdr.UInt32();

		size_t nvp_size = 0;
		nvpair_base *nvp = nullptr;

		switch (value_type) {
		case DATA_TYPE_BYTE:
		case DATA_TYPE_INT8:
		case DATA_TYPE_UINT8:
		case DATA_TYPE_INT16:
		case DATA_TYPE_UINT16:
		case DATA_TYPE_INT32:
		case DATA_TYPE_UINT32:
			nvp = new NVInteger(name, xdr.UInt32());
			nvp_size += 4;
			break;
		case DATA_TYPE_INT64:
		case DATA_TYPE_UINT64:
			nvp = new NVInteger(name, xdr.Int64());
			nvp_size += 4;
			break;
		case DATA_TYPE_STRING:
			nvp = new NVString(name, xdr.String());
			nvp_size += 4; 
			nvp_size += ALIGN4(((NVString *)nvp)->value.size());
			break;
		case DATA_TYPE_NVLIST:
			
			break;
		case DATA_TYPE_NVLIST_ARRAY:
			
			break;
		case DATA_TYPE_BYTE_ARRAY:
		case DATA_TYPE_INT16_ARRAY:
		case DATA_TYPE_UINT16_ARRAY:
		case DATA_TYPE_INT32_ARRAY:
		case DATA_TYPE_UINT32_ARRAY:
		case DATA_TYPE_INT64_ARRAY:
		case DATA_TYPE_UINT64_ARRAY:
		case DATA_TYPE_STRING_ARRAY:
		case DATA_TYPE_HRTIME:
		case DATA_TYPE_BOOLEAN_VALUE:
		case DATA_TYPE_BOOLEAN_ARRAY:
		case DATA_TYPE_INT8_ARRAY:	
		case DATA_TYPE_UINT8_ARRAY:	
		case DATA_TYPE_DOUBLE:	

		case DATA_TYPE_BOOLEAN:

		default:
			return false;		
		
		}
	
	
	} catch (const std::out_of_range &ex) {
		return false;	
	}

	return false;
}

bool DecodeXdrNVList(uint8_t *buff, size_t size)
{
	assert(buff);
	assert(size);




	return false;
}