#include "Calc.h"

using namespace std;

enum ElementType {
	kNumber,
	kMinus,
	kPlus,
	kMul,
	kDev,
	kOpenBracket,
	kCloseBracket
};

typedef struct _ELEMENT {
	DWORD type;
	double value;
} ELEMENT;

int CalcMain(void)
{
	String str = _T("2+3*4-6/3"); // 12
	return 0;
}