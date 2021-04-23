#ifndef __COMMON_ADDON2_H
#define __COMMON_ADDON2_H

#include "orbitersdk.h"

class COMMON_VESSEL2: public VESSEL2 {
public:
	COMMON_VESSEL2 (OBJHANDLE hObj, int fmodel);
	virtual void* GetSystemInterface(const char* pszSystem);
	virtual int SendMessage(const char* pszMessage, void* aMessage);
};

COMMON_VESSEL2::COMMON_VESSEL2(OBJHANDLE hObj, int fmodel)
: VESSEL2(hObj, fmodel)
{
}

void* COMMON_VESSEL2::GetSystemInterface(const char* pszSystem)
{
	return 0;
}

int COMMON_VESSEL2::SendMessage(const char* pszMessage, void* aMessage)
{
	return 0;
}


#endif // !__COMMON_ADDON2_H
