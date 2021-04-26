// ==============================================================
// ORBITER vessel module: Mercury Redstone
// ==============================================================

#include "..\..\include\orbitersdk.h"
#include "..\..\include\multi_stage6.h"
#include "gatv.h"


void InitialCaps(VESSEL2 *vessel) {
	IStage *newStage;
	newStage = new GATV();
	((Multi_Stage*)vessel)->SetStage(newStage,oapiGetSimTime());
}


void SetConfiguration(int status, VESSEL2 *vessel)
{
	IStage *newStage;
	switch (status) {
	case 0:
		newStage = new GATV();
		((Multi_Stage*)vessel)->SetStage(newStage,oapiGetSimTime());
		break;
	default:
		InitialCaps(vessel);
		break;
	}
};

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	Multi_Stage *Vessel;
	Vessel = new Multi_Stage(hvessel, flightmodel);
	Vessel->SetConfiguration(SetConfiguration);
	Vessel->SetInitialCaps(InitialCaps);
	return Vessel;
}

// Cleanup
DLLCLBK void ovcExit (VESSEL *vessel)
{
	if (vessel) delete (Multi_Stage*) vessel;
}

