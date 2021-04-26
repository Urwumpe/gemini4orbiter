// ==============================================================
// ORBITER vessel module: NASA Manned Manuveuring Unit (MMU)
// ==============================================================

#include "../../include/OrbiterAPI.h"
#include "../../include/VesselAPI.h"
#include "../../include/multi_stage3.h"
#include <cmath>

const double TETHER_OPERATING_SPEED = 0.128;
const double TETHER_MAX_ANGLE = RAD*90.0;

//PROPELLANT_HANDLE main_tank;
//THRUSTER_HANDLE thrusters[12];


void SetEVA (VESSEL *vessel)
{
// ==============================================================
// These numbers are based off of the MS Space Simulator MMU
	vessel->SetSize (1.2);
	vessel->SetCOG_elev (0.75);
	vessel->SetEmptyMass (200.0);
	vessel->SetMaxFuelMass (20.0);
	vessel->SetFuelMass(20.0);
	vessel->SetISP (350.0*SEC);
	vessel->SetMaxThrust (ENGINE_MAIN, 31.5);
	vessel->SetMaxThrust (ENGINE_RETRO, 31.5);
	vessel->SetMaxThrust (ENGINE_HOVER, 0);
	vessel->SetMaxThrust (ENGINE_ATTITUDE, 31.5);
// ==============================================================
	vessel->SetPMI (_V(.35,.13,0.35));
	vessel->SetCrossSections (_V(1.64,.77,1.64));
// ==============================================================
	vessel->SetCW (0.3, 0.3, .3, .3);
	vessel->SetRotDrag (_V(0.7,0.7,1.2));
	vessel->SetPitchMomentScale (0);
	vessel->SetBankMomentScale (0);
	vessel->SetLiftCoeffFunc (0); 
	vessel->SetDockParams (_V(-.09,-.1,.23), _V(0,0,1), _V(0,1,0));
// ==============================================================
	vessel->ClearMeshes();
	vessel->ClearExhaustRefs();
	vessel->ClearAttExhaustRefs();
	vessel->AddMesh (oapiLoadMeshGlobal("gem-eva1"));
//	vessel->SetClipRadius(.1);
	//vessel->AddMesh (oapiLoadMeshGlobal("gem_tether"));
}

class Gemini_EVA:public IStage
{
public:
	Gemini_EVA();
	void Init ();
	void Exit (VESSEL *vessel);
	void Setup (VESSEL *vessel, double stage_sep_time);
	void ParseLine (VESSEL *vessel, char *line, void *vs);
	void SaveState (VESSEL *vessel, FILEHANDLE scn);
	void Timestep (VESSEL *vessel, double simt);
	void Animate (VESSEL *vessel, double simt);
	void VisualCreated (VESSEL *vessel, VISHANDLE vis, int refcount);
	void VisualDestroyed (VESSEL *vessel, VISHANDLE vis, int refcount);
	int ConsumeKey (VESSEL *vessel, const char *keystate);
	int ConsumeBufferedKey (VESSEL *vessel, DWORD key, bool down, char *kstate);
	void Message(VESSEL *vessel, int msg, double value);
private:
	void SetStage(VESSEL *vessel);
	OBJHANDLE gvessel;
};

Gemini_EVA::Gemini_EVA()
{
	Init();
}
void Gemini_EVA::Init ()
{
}

void Gemini_EVA::Exit (VESSEL *vessel)
{
}

void Gemini_EVA::Setup (VESSEL *vessel, double stage_sep_time)
{
	char name[256];
	char vname[256];
	UINT i;
	strcpy (vname,vessel->GetName());
	for (i=0;i<strlen(vname)+1;i++)
		name[i]=0;
	for (i=0;i<strlen(vname)-4;i++)
		name[i]=vname[i];
	gvessel=oapiGetVesselByName(name);
	SetStage(vessel);
}

void Gemini_EVA::ParseLine (VESSEL *vessel, char *line, void *vs)
{
}

void Gemini_EVA::SaveState (VESSEL *vessel, FILEHANDLE scn)
{
}

void Gemini_EVA::Timestep (VESSEL *vessel, double simt)
{
	VESSEL *gemvessel;
	if (gvessel != 0) {
		gemvessel = oapiGetVesselInterface(gvessel);
		TetherCalc2(gemvessel, vessel, 7.62);
	}
}

void Gemini_EVA::Animate (VESSEL *vessel, double simt)
{
}

void Gemini_EVA::VisualCreated (VESSEL *vessel, VISHANDLE vis, int refcount)
{
}

void Gemini_EVA::VisualDestroyed (VESSEL *vessel, VISHANDLE vis, int refcount)
{
}

int Gemini_EVA::ConsumeKey (VESSEL *vessel, const char *keystate)
{
	return 0;
}

int Gemini_EVA::ConsumeBufferedKey (VESSEL *vessel, DWORD key, bool down, char *kstate)
{
	OBJHANDLE hvessel;
	if (!down) return 0; // only process keydown events
	if (KEYMOD_SHIFT (kstate)) 
	{
		if (key == OAPI_KEY_B) // "Jettison Beacon" 
		{
			AddImpulse(vessel,_V(0,1,0),_V(0,0,-1));
			return 1;			
		}
	}
	else if (KEYMOD_CONTROL (kstate)) 
	{
		// insert ctrl key combinations here
	}
	else 
	{ // unmodified keys
		if (key == OAPI_KEY_E) { // "Abort Emergency"
			char name[256];
			char vname[256];
			UINT i;
			strcpy (vname,vessel->GetName());
			for (i=0;i<strlen(vname)+1;i++)
				name[i]=0;

			for (i=0;i<strlen(vname)-4;i++)
				name[i]=vname[i];
			hvessel=oapiGetVesselByName(name);
			if (hvessel != 0)
			{
				OBJHANDLE hVessel;
				hVessel = vessel->GetDockStatus(vessel->GetDockHandle(0));
				if (hVessel != 0) {
					oapiDeleteVessel(hVessel);
				}
				vessel->ClearMeshes();
				oapiSetFocusObject(hvessel);

				//oapiDeleteVessel(vessel->GetHandle());
				//((Multi_Stage*)oapiGetVesselInterface(hVessel))->Message(100,0);
			}

			return 1;
		}

		if (key == OAPI_KEY_D) // "Detach" 
		{
			OBJHANDLE hVessel;
			VESSEL *aVessel;
			hVessel = vessel->GetDockStatus(vessel->GetDockHandle(0));
			if (hVessel != 0) {
				aVessel = oapiGetVesselInterface(hVessel);
				aVessel->Undock(ALLDOCKS, vessel->GetHandle());
				vessel->AddForce(_V(0,0,-1000),_V(0,0,0));
			}
			//AddImpulse(vessel,_V(1,0,0),_V(1000,1,1));
			//sprintf(oapiDebugString(),"B");
			return 1;			
		}
	}	
	return 0;
}

void Gemini_EVA::Message(VESSEL *vessel, int msg, double value)
{
}

void Gemini_EVA::SetStage(VESSEL *vessel)
{
	SetEVA(vessel);
}



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
//	sprintf(oapiDebugString(),"Exit");
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
	newStage = new Gemini_EVA();
	((Multi_Stage*)vessel)->SetStage(newStage,0);
}


DLLCLBK void ovcTimestep (VESSEL *vessel, double simt)
{
	((Multi_Stage*)vessel)->Timestep(simt);
}

DLLCLBK void ovcSetStateEx (VESSEL *vessel, const void *status)
{
	((Multi_Stage*)vessel)->DefSetStateEx (status);	        // default initialisation
}

/*void RotateTether()
{
}

void UpdateMesh (VESSEL *vessel)
{
	int grp_ofs;
	double delta;
	float da =0; 
	MESHGROUP_TRANSFORM mt;

	if (delta = panel_proc-vpanel_proc) { 
		grp_ofs = 0;
		da = (float)(delta*PANEL_MAX_ANGLE);
		mt.nmesh = 2;
		mt.transform = MESHGROUP_TRANSFORM::ROTATE;
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.ref.x = .72;
		mt.P.rotparam.ref.y = 0;
		mt.P.rotparam.ref.z = 0;
		mt.P.rotparam.angle = -da;
		for (grp_ofs=0;grp_ofs<12;grp_ofs++)
		{
			mt.ngrp = grp_ofs; 
			vessel->MeshgroupTransform (Stack->hVis, mt);
		}

		mt.P.rotparam.ref.x -= 0.48*cos(panel_proc*PANEL_MAX_ANGLE);
		mt.P.rotparam.ref.y =  0;
		mt.P.rotparam.ref.z += 0.48*sin(panel_proc*PANEL_MAX_ANGLE);
		mt.P.rotparam.angle = 2*da;
		for (grp_ofs=0;grp_ofs<4;grp_ofs++)
		{
			mt.ngrp = grp_ofs; 
			vessel->MeshgroupTransform (Stack->hVis, mt);
		}

		for (grp_ofs=6;grp_ofs<10;grp_ofs++)
		{
			mt.ngrp = grp_ofs; 
			vessel->MeshgroupTransform (Stack->hVis, mt);
		}

		mt.P.rotparam.ref.x -= 0.48*cos(panel_proc*PANEL_MAX_ANGLE);
		mt.P.rotparam.ref.y =  0;
		mt.P.rotparam.ref.z -= 0.48*sin(panel_proc*PANEL_MAX_ANGLE);
		mt.P.rotparam.angle = -2*da;
		for (grp_ofs=0;grp_ofs<2;grp_ofs++)
		{
			mt.ngrp = grp_ofs; 
			vessel->MeshgroupTransform (Stack->hVis, mt);
		}

		for (grp_ofs=6;grp_ofs<8;grp_ofs++)
		{
			mt.ngrp = grp_ofs; 
			vessel->MeshgroupTransform (Stack->hVis, mt);
		}

//SECOND PANEL
		da = (float)(delta*PANEL_MAX_ANGLE);
		mt.nmesh = 1;
		mt.transform = MESHGROUP_TRANSFORM::ROTATE;
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.ref.x = -.72;
		mt.P.rotparam.ref.y = 0;
		mt.P.rotparam.ref.z = 0;
		mt.P.rotparam.angle = da;
		for (grp_ofs=0;grp_ofs<12;grp_ofs++)
		{
			mt.ngrp = grp_ofs; 
			vessel->MeshgroupTransform (Stack->hVis, mt);
		}

		mt.P.rotparam.ref.x += 0.48*cos(panel_proc*PANEL_MAX_ANGLE);
		mt.P.rotparam.ref.y =  0;
		mt.P.rotparam.ref.z += 0.48*sin(panel_proc*PANEL_MAX_ANGLE);
		mt.P.rotparam.angle = -2*da;
		for (grp_ofs=2;grp_ofs<6;grp_ofs++)
		{
			mt.ngrp = grp_ofs; 
			vessel->MeshgroupTransform (Stack->hVis, mt);
		}

		for (grp_ofs=8;grp_ofs<12;grp_ofs++)
		{
			mt.ngrp = grp_ofs; 
			vessel->MeshgroupTransform (Stack->hVis, mt);
		}

		mt.P.rotparam.ref.x += 0.48*cos(panel_proc*PANEL_MAX_ANGLE);
		mt.P.rotparam.ref.y =  0;
		mt.P.rotparam.ref.z -= 0.48*sin(panel_proc*PANEL_MAX_ANGLE);
		mt.P.rotparam.angle = 2*da;
		for (grp_ofs=4;grp_ofs<6;grp_ofs++)
		{
			mt.ngrp = grp_ofs; 
			vessel->MeshgroupTransform (Stack->hVis, mt);
		}

		for (grp_ofs=10;grp_ofs<12;grp_ofs++)
		{
			mt.ngrp = grp_ofs; 
			vessel->MeshgroupTransform (Stack->hVis, mt);
		}

		vpanel_proc = panel_proc;
	}

}
*/