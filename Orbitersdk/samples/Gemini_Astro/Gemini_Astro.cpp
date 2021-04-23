// ==============================================================
// ORBITER vessel module: NASA Manned Manuveuring Unit (MMU)
// ==============================================================

#include "../../include/orbitersdk.h"
#include "../../include/multi_stage2.h"
#include "gemini_astro.h"

// ==============================================================
// API interface
// ==============================================================



// Initialisation
DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	return new Multi_Stage(hvessel);
}

// Cleanup
DLLCLBK void ovcExit (VESSEL *vessel)
{
	if (vessel) delete (Multi_Stage*) vessel;
}

// Keyboard interface handler
DLLCLBK int ovcConsumeKey (VESSEL *vessel, const char *keystate)
{
	((Multi_Stage*)vessel)->ConsumeKey(keystate);
	return 0;
}

DLLCLBK int ovcConsumeBufferedKey (VESSEL *vessel, DWORD key, bool down, char *kstate)
{
	if (!down) return 0; // only process keydown events

	return ((Multi_Stage*)vessel)->ConsumeBufferedKey(key, down, kstate);
}

// Set the capabilities of the vessel class
DLLCLBK void ovcSetClassCaps (VESSEL *vessel, FILEHANDLE cfg)
{
	IStage *newStage;
	newStage = new Gemini_Astro();
	((Multi_Stage*)vessel)->SetStage(newStage,oapiGetSimTime());
}

	
DLLCLBK void ovcSetState (VESSEL *vessel, const VESSELSTATUS *status)
{
	vessel->DefSetState (status);	        // default initialisation
}

DLLCLBK void ovcTimestep (VESSEL *vessel, double simt)
{
	((Multi_Stage*)vessel)->Timestep(simt);
}