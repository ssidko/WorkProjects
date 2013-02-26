#include "String.h"
#include "List.h"

inline WORD Be2Le(WORD *be)
{
	WORD le;
	((BYTE *)&le)[0] = ((BYTE *)be)[1];
	((BYTE *)&le)[1] = ((BYTE *)be)[0];
	return le;
}

inline DWORD Be2Le(DWORD *be)
{
	WORD le;
	((BYTE *)&le)[0] = ((BYTE *)be)[3];
	((BYTE *)&le)[1] = ((BYTE *)be)[2];
	((BYTE *)&le)[2] = ((BYTE *)be)[1];
	((BYTE *)&le)[3] = ((BYTE *)be)[0];
	return le;
}