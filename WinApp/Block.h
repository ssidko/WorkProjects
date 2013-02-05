#ifndef _BLOCK
#define _BLOCK

#include "common.h"

BEGIN_NAMESPACE_FR

class Block
{
	LONGLONG base_offset;
public:
	Block(void);
	~Block(void);
};

END_NAMESPACE_FR

#endif // _BLOCK
