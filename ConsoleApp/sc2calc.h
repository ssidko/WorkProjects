#include "windows.h"

typedef basic_string<TCHAR> TString; 

namespace sc2calc {

	typedef struct _COST {
		DWORD minerals;
		DWORD gass;
		_COST (DWORD m, DWORD g) : minerals(m), gass(g) {}
	} COST;

	class Object {
	private:
		TString class_name;
		TString object_name;

	public:
		Object() : class_name(_T("")), object_name(_T("")) {}
		virtual ~Object();

		const TString &ClassName() const {return class_name;}
		const TString &ObjectName() const {return object_name;}
	};


	enum EntityType {
		kUnit,
		kBuilding,
		kResource,
		kNature
	};

	class Entity {};

	class World {};

	class Player {};

}
