// ==============================================================
// ORBITER vessel module: Mercury Redstone
// ==============================================================

#include "orbitersdk.h"
#include "multi_stage.h"
#include "beacon.h"

// Initialisation
DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	Stack = new Multi_Stage();
	return new VESSEL (hvessel, flightmodel);
}

// Cleanup
DLLCLBK void ovcExit (VESSEL *vessel)
{
	if (Stack) delete Stack;
	if (vessel) delete vessel;
}

// Keyboard interface handler
DLLCLBK int ovcConsumeKey (VESSEL *vessel, const char *keystate)
{
	Stack->ConsumeKey(vessel,keystate);
	return 0;
}

DLLCLBK int ovcConsumeBufferedKey (VESSEL *vessel, DWORD key, bool down, char *kstate)
{
	if (!down) return 0; // only process keydown events

	return Stack->ConsumeBufferedKey(vessel, key, down, kstate);
}

// Set the capabilities of the vessel class
DLLCLBK void ovcSetClassCaps (VESSEL *vessel, FILEHANDLE cfg)
{
	IStage *newStage;
	newStage = new Beacon();
	Stack->SetStage(newStage,vessel,0);
}


DLLCLBK void ovcTimestep (VESSEL *vessel, double simt)
{
	Stack->Timestep(vessel,simt);
}

DLLCLBK void ovcLoadState (VESSEL *vessel, FILEHANDLE scn, VESSELSTATUS *def_vs)
{
	IStage *newStage;
    char *line;
	int status = 0;
	int jettisoned=0;
	double sts_sat_x = 0.0;
	double sts_sat_y = 0.0;
	double sts_sat_z = 0.0;
	bool configurationset = false;
	newStage = new Beacon();
	Stack->SetStage(newStage,vessel,0);
	while (oapiReadScenario_nextline (scn, line)) {
		if (configurationset)
		{
			newStage->ParseLine(vessel,line,def_vs);
		}
		else
		{
			if (!strnicmp (line, "CONFIGURATION", 13)) {
				sscanf (line+13, "%d", &status);
				switch (status) {
				case 0:
					newStage = new Beacon();
					break;
				}
				Stack->SetStage(newStage,vessel,0);
				configurationset = true;
			} else {
				vessel->ParseScenarioLine (line, def_vs);
			}
		}
    }
	newStage->Setup(vessel,0);
	Stack->SetCurrentStage(newStage);
}
//

// Save status to scenario file
DLLCLBK void ovcSaveState (VESSEL *vessel, FILEHANDLE scn)
{
	vessel->SaveDefaultState(scn);
	Stack->SaveState(vessel,scn);
}

DLLCLBK void ovcSetState (VESSEL *vessel, const VESSELSTATUS *status)
{
	vessel->DefSetState (status);	        // default initialisation
}
