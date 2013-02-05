#ifndef _PAGE
#define _PAGE

#include "common.h"

BEGIN_NAMESPACE_FR

class PageTemplate
{
#pragma pack(push)
#pragma pack(1)
	typedef struct CHUNK
	{
		BYTE *data;
		BYTE *vs_data;
	} CHUNK, *PCHUNK;
#pragma pack(pop)

private:

public:
	PageTemplate(void);
	~PageTemplate(void);


};

class Page
{
private:

public:
	Page(/*TCHAR*/void);
	~Page(void);
};

END_NAMESPACE_FR

#endif // _PAGE
