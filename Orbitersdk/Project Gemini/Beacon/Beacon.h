#ifndef __BEACON_H
#define __BEACON_H
#include "../../include/OrbiterAPI.h"
#include "../../include/VesselAPI.h"
#include "../../include/multi_stage3.h"

const double MASS_Beacon  = 44*KG;
const VECTOR3 OFS_Beacon  = _V(0,0,0);


void AddBeacon(VESSEL *vessel)
{
	vessel->AddMesh (oapiLoadMeshGlobal("beacon"), &OFS_Beacon);
};


class Beacon:public IStage
{
public:
	void Init ();
	void Exit (VESSEL *vessel);
	void Setup (VESSEL *vessel, double stage_sep_time);
	void ParseLine (VESSEL *vessel, char *line, VESSELSTATUS *def_vs);
	void SaveState (VESSEL *vessel, FILEHANDLE scn);
	void Timestep (VESSEL *vessel, double simt);
	void Animate (VESSEL *vessel, double simt);
	void VisualCreated (VESSEL *vessel, VISHANDLE vis, int refcount);
	void VisualDestroyed (VESSEL *vessel, VISHANDLE vis, int refcount);
	int ConsumeKey (VESSEL *vessel, const char *keystate);
	int ConsumeBufferedKey (VESSEL *vessel, DWORD key, bool down, char *kstate);
private:
	void SetStage(VESSEL *vessel);
	PROPELLANT_HANDLE main_tank;
	THGROUP_HANDLE light_group;
	THRUSTER_HANDLE th_light[2];
};

void Beacon::Init ()
{
};

void Beacon::Exit (VESSEL *vessel)
{
};

void Beacon::Setup (VESSEL *vessel, double stage_sep_time)
{
	SetStage(vessel);
};

void Beacon::ParseLine (VESSEL *vessel, char *line, VESSELSTATUS *def_vs)
{
	vessel->ParseScenarioLine (line, def_vs);
};
void Beacon::SaveState (VESSEL *vessel, FILEHANDLE scn)
{
	oapiWriteScenario_int (scn, "CONFIGURATION", 0);
};

void Beacon::Timestep (VESSEL *vessel, double simt)
{
};

void Beacon::Animate (VESSEL *vessel, double simt)
{
};

void Beacon::VisualCreated (VESSEL *vessel, VISHANDLE vis, int refcount)
{
};

void Beacon::VisualDestroyed (VESSEL *vessel, VISHANDLE vis, int refcount)
{
};

int Beacon::ConsumeKey (VESSEL *vessel, const char *keystate)
{
	if (KEYMOD_SHIFT (keystate)) 
	{
		return 0; // shift combinations are reserved
	}
	else if (KEYMOD_CONTROL (keystate)) 
	{
		// insert ctrl key combinations here
	}
	else 
	{ // unmodified keys
		if (KEYDOWN (keystate, OAPI_KEY_DIVIDE)) { // "Disable Laterals"
			oapiSetAttitudeMode(vessel->GetHandle(),ATTMODE_ROT);
			return 1;
		}
	}
	return 0;
};
int Beacon::ConsumeBufferedKey (VESSEL *vessel, DWORD key, bool down, char *kstate)
{
	if (KEYMOD_SHIFT (kstate)) 
	{

	}
	else if (KEYMOD_CONTROL (kstate)) 
	{
		// insert ctrl key combinations here
	}
	else 
	{ // unmodified keys
	}	
	return 0;
};
void Beacon::SetStage(VESSEL *vessel)
{
	UINT eng1 = 0;
	UINT eng2 = 0;
	vessel->SetSize (.5);
	vessel->SetCOG_elev (.05);
	vessel->SetMaxFuelMass(.01);
	vessel->SetEmptyMass  (MASS_Beacon);
	vessel->SetCW (0.1, 0.1, 0.1, 0.1);
	vessel->SetRotDrag (_V(2.0,2.0,2.0));
	vessel->SetPitchMomentScale (0);
	vessel->SetBankMomentScale (0);
	vessel->SetLiftCoeffFunc (0); 
	vessel->ClearMeshes();
	vessel->ClearExhaustRefs();
	vessel->ClearAttExhaustRefs();
	AddBeacon(vessel);
	main_tank = vessel->CreatePropellantResource(0.72);
	th_light[0] = vessel->CreateThruster(_V(0,0,.1), _V(0,0,-1), .01, main_tank, 200*SEC);
	vessel->AddExhaust(th_light[0],.3,.1);
	th_light[1] = vessel->CreateThruster(_V(0,0,-.1), _V(0,0,1), .01, main_tank, 200*SEC);
	vessel->AddExhaust(th_light[1],.3,.1);

	light_group = vessel->CreateThrusterGroup(th_light,2,THGROUP_USER);
	vessel->SetThrusterGroupLevel(light_group,1.0);
};



#endif