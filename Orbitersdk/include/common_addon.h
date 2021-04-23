#ifndef __COMMON_ADDON_H
#define __COMMON_ADDON_H

#include "orbitersdk.h"

class COMMON_VESSEL: public VESSEL {
public:
	COMMON_VESSEL (OBJHANDLE hObj);
	virtual void* GetSystemInterface(const char* pszSystem);
	virtual int SendMessage(const char* pszMessage, void* aMessage);
};

COMMON_VESSEL::COMMON_VESSEL(OBJHANDLE hObj)
: VESSEL(hObj)
{
}

void* COMMON_VESSEL::GetSystemInterface(const char* pszSystem)
{
	return 0;
}

int COMMON_VESSEL::SendMessage(const char* pszMessage, void* aMessage)
{
	return 0;
}


#endif // !__COMMON_ADDON_H
