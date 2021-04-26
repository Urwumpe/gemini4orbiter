#ifndef __GATV_H
#define __GATV_H

#include "..\..\include\orbitersdk.h"
#include "..\..\include\multi_stage6.h"

const double  AGENA_LENGTH       = 8.09;
const VECTOR3 AGENA_OFS_FLAME    = { 0.0, 0.0     , -AGENA_LENGTH/2};
const VECTOR3 AGENA_OFS          = { 0.0, 0.0     , 0};
const VECTOR3 AGENA_OFS_ANT      = { 0.0, 1.514   , AGENA_LENGTH /2 - (AGENA_LENGTH - 7.385)};
const double  AGENA_MASS         = 6821*KG;
const double  AGENA_MASS_EMPTY   = 558*KG;
const double  AGENA_MASS_FUEL_SPS = 60*KG;
const double  AGENA_MASS_FUEL_RCS = 16*KG;
const double  AGENA_MASS_FUEL    = AGENA_MASS-(AGENA_MASS_EMPTY+AGENA_MASS_FUEL_SPS+AGENA_MASS_FUEL_RCS);
const double  AGENA_ISP_SL       = 0*SEC;
const double  AGENA_ISP_VAC      = 300*SEC;
const double  AGENA_ISP_RCS      = 40*SEC;
const double  AGENA_THRUST_VAC   = 7257*KGF;
const double  AGENA_THRUST_SPS_1 = 90.1*KGF;
const double  AGENA_THRUST_SPS_2 = 7.25*KGF;
const double  AGENA_THRUST_RSC   = 4.5359247*KGF;

class GATV:public IStage
{
public:
	GATV();
	void Init               ();
	void Exit               (VESSEL2 *vessel);
	void Setup              (VESSEL2 *vessel, double stage_sep_time);
	void ParseLine          (VESSEL2 *vessel, char *line, void *vs);
	void SaveState          (VESSEL2 *vessel, FILEHANDLE scn);
	void PreStep            (VESSEL2 *vessel,  double simt, double simdt, double mjd);
	void PostStep           (VESSEL2 *vessel, double simt, double simdt, double mjd);
	void Animate            (VESSEL2 *vessel, double simt);
	void VisualCreated      (VESSEL2 *vessel, VISHANDLE vis, int refcount);
	void VisualDestroyed    (VESSEL2 *vessel, VISHANDLE vis, int refcount);
	int  ConsumeDirectKey   (VESSEL2 *vessel, char *keystate);
	int  ConsumeBufferedKey (VESSEL2 *vessel, DWORD key, bool down, char *keystate);
	void Message            (VESSEL2 *vessel, int msg, double value);
	void FocusChanged       (VESSEL2 *vessel, bool getfocus);
private:
	void SetStage(VESSEL2 *vessel);
	PROPELLANT_HANDLE fuel_agena;
	PROPELLANT_HANDLE fuel_sps;
	PROPELLANT_HANDLE fuel_rcs;
	THRUSTER_HANDLE thruster_main[5];
	THRUSTER_HANDLE thruster_rcs[10];
	bool pitch_yaw_plus;
};

GATV::GATV() 
{
	Init();
}

void GATV::Init ()
{
	fuel_agena = 0;
	pitch_yaw_plus = false;
}

void GATV::Exit (VESSEL2 *vessel)
{
}

void GATV::Setup (VESSEL2 *vessel, double stage_sep_time)
{
	SetStage(vessel);
}

void GATV::ParseLine (VESSEL2 *vessel, char *line, void *vs)
{
}

void GATV::SaveState (VESSEL2 *vessel, FILEHANDLE scn)
{
}

void GATV::PreStep (VESSEL2 *vessel,  double simt, double simdt, double mjd) {
}

void GATV::PostStep (VESSEL2 *vessel, double simt, double simdt, double mjd){
}


void GATV::Animate (VESSEL2 *vessel, double simt)
{
}

void GATV::VisualCreated (VESSEL2 *vessel, VISHANDLE vis, int refcount)
{
}

void GATV::VisualDestroyed (VESSEL2 *vessel, VISHANDLE vis, int refcount)
{
}

void GATV::Message(VESSEL2 *vessel, int msg, double value)
{
	if (msg == 501) { 
		sprintf(oapiDebugString(),"501 - Agena PPS On");
		vessel->SetThrusterLevel(thruster_main[0], 1.0);
		return;
	}
	if (msg == 500) { 
		sprintf(oapiDebugString(),"500 - Agena PPS Cutoff");
		vessel->SetThrusterLevel(thruster_main[0], 0.0);
		return;
	}
	if (msg == 560) { 
		sprintf(oapiDebugString(),"560 - Agena SPS-1 On");
		vessel->SetThrusterLevel(thruster_main[1], 1.0);
		vessel->SetThrusterLevel(thruster_main[2], 1.0);
		return;
	}
	if (msg == 551) { 
		sprintf(oapiDebugString(),"551 - Agena SPS-2 On");
		vessel->SetThrusterLevel(thruster_main[3], 1.0);
		vessel->SetThrusterLevel(thruster_main[4], 1.0);
		return;
	}
	if (msg == 550) { 
		sprintf(oapiDebugString(),"550 - SPS Cutoff");
		vessel->SetThrusterLevel(thruster_main[1], 0.0);
		vessel->SetThrusterLevel(thruster_main[2], 0.0);
		vessel->SetThrusterLevel(thruster_main[3], 0.0);
		vessel->SetThrusterLevel(thruster_main[4], 0.0);
		return;
	}
	if (msg == 410) { 
		sprintf(oapiDebugString(),"410 - Pitch Yaw Minus");
		pitch_yaw_plus = false;
		return;
	}
	if (msg == 411) { 
		sprintf(oapiDebugString(),"411 - Pitch Yaw Plus");
		pitch_yaw_plus = true;
		return;
	}

	if (msg == 440) { 
		sprintf(oapiDebugString(),"440 - Yaw Off");
		vessel->SetThrusterLevel(thruster_rcs[2], 0.0);
		vessel->SetThrusterLevel(thruster_rcs[3], 0.0);
		vessel->SetThrusterLevel(thruster_rcs[4], 0.0);
		vessel->SetThrusterLevel(thruster_rcs[5], 0.0);
		return;
	}

	if (msg == 441) { 
		if (pitch_yaw_plus) {
			sprintf(oapiDebugString(),"441 - Yaw On Plus");
			vessel->SetThrusterLevel(thruster_rcs[2], 1.0);
			vessel->SetThrusterLevel(thruster_rcs[3], 1.0);
		} else {
			sprintf(oapiDebugString(),"441 - Yaw On Minus");
			vessel->SetThrusterLevel(thruster_rcs[4], 1.0);
			vessel->SetThrusterLevel(thruster_rcs[5], 1.0);
		}
		return;
	}

	if (msg == 430) { 
		sprintf(oapiDebugString(),"430 - Pitch Off");
		vessel->SetThrusterLevel(thruster_rcs[0], 0.0);
		vessel->SetThrusterLevel(thruster_rcs[1], 0.0);
		return;
	}

	if (msg == 431) { 
		if (pitch_yaw_plus) {
			sprintf(oapiDebugString(),"431 - Pitch On Plus");
			vessel->SetThrusterLevel(thruster_rcs[0], 1.0);
		} else {
			sprintf(oapiDebugString(),"431 - Pitch On Minus");
			vessel->SetThrusterLevel(thruster_rcs[1], 1.0);
		}
		return;
	}
	sprintf(oapiDebugString(),"Agena Command %d not recognized", msg);
}

void GATV::FocusChanged (VESSEL2 *vessel, bool getfocus){
}

int GATV::ConsumeDirectKey (VESSEL2 *vessel, char *keystate)
{
	return 0;
}

int GATV::ConsumeBufferedKey (VESSEL2 *vessel, DWORD key, bool down, char *kstate)
{
	THRUSTER_HANDLE th_group[4];
	if (KEYMOD_SHIFT (kstate)) 
	{

	}
	else if (KEYMOD_CONTROL (kstate)) 
	{
		// insert ctrl key combinations here
	}
	else 
	{ // unmodified keys
		if (key == OAPI_KEY_1) { 
			th_group[0] = thruster_main[0];
			vessel->CreateThrusterGroup(th_group,1,THGROUP_MAIN);
			return 1;
		}
		if (key == OAPI_KEY_2) { 
			th_group[0] = thruster_main[1];
			th_group[1] = thruster_main[2];
			vessel->CreateThrusterGroup(th_group,2,THGROUP_MAIN);
			return 1;
		}
		if (key == OAPI_KEY_3) { 
			th_group[0] = thruster_main[3];
			th_group[1] = thruster_main[4];
			vessel->CreateThrusterGroup(th_group,2,THGROUP_MAIN);
			return 1;
		}
		if (key == OAPI_KEY_P) { 
			return 1;
		}
	}	
	return 0;
}

void GATV::SetStage(VESSEL2 *vessel)
{
	THRUSTER_HANDLE th_group[4];
// ==============================================================
// These numbers are based off of the MS Space Simulator MMU
	vessel->SetSize (AGENA_LENGTH);
	vessel->SetCOG_elev (0.75);
	vessel->SetEmptyMass (AGENA_MASS_EMPTY);
// ==============================================================
	vessel->SetPMI (_V(2.55,2.55,.35));
	vessel->SetCrossSections (_V(11.5,11.5,2.3));
// ==============================================================
	vessel->SetCW (0.1, 1.5, 0.3, 0.3);
	vessel->SetRotDrag (_V(0.7,0.7,0.7));
	vessel->SetPitchMomentScale (-1e-1);
	vessel->SetBankMomentScale (-1e-1);
	vessel->SetLiftCoeffFunc (0); 
	//VECTOR3 DockDir = _V(0,0,1);
	//VECTOR3 DockRot = _V(0,1,0);
	//VECTOR3 DockRef = _V(0.0,0,3.149);
	//vessel->SetDockParams(DockRef,DockDir,DockRot)
// ==============================================================
	vessel->ClearMeshes();
	vessel->ClearPropellantResources();
	vessel->ClearThrusterDefinitions();
	vessel->AddMesh (oapiLoadMeshGlobal("agena"),&AGENA_OFS);
	vessel->AddMesh (oapiLoadMeshGlobal("agenaAntenna"),&AGENA_OFS_ANT);

	fuel_agena = vessel->CreatePropellantResource(AGENA_MASS_FUEL, AGENA_MASS_FUEL);
	fuel_sps   = vessel->CreatePropellantResource(AGENA_MASS_FUEL_SPS, AGENA_MASS_FUEL_SPS);
	fuel_rcs   = vessel->CreatePropellantResource(AGENA_MASS_FUEL_RCS, AGENA_MASS_FUEL_RCS);
	thruster_main[0] = vessel->CreateThruster(_V(0, 0, AGENA_OFS_FLAME.z), _V(0,0,1), AGENA_THRUST_VAC, fuel_agena, AGENA_ISP_VAC, 0.0);
	vessel->AddExhaust(thruster_main[0], 5.0, .3);
	thruster_main[1] = vessel->CreateThruster(_V(.425, .35, AGENA_OFS_FLAME.z+.7), _V(0,0,1), AGENA_THRUST_SPS_1, fuel_sps, AGENA_ISP_VAC, 0.0);
	vessel->AddExhaust(thruster_main[1], 0.5, .03);
	thruster_main[2] = vessel->CreateThruster(_V(-.425, -.35, AGENA_OFS_FLAME.z+.7), _V(0,0,1), AGENA_THRUST_SPS_1, fuel_sps, AGENA_ISP_VAC, 0.0);
	vessel->AddExhaust(thruster_main[2], 0.5, .03);
	thruster_main[3] = vessel->CreateThruster(_V(.425, -.35, AGENA_OFS_FLAME.z+.7), _V(0,0,1), AGENA_THRUST_SPS_2, fuel_sps, AGENA_ISP_VAC, 0.0);
	vessel->AddExhaust(thruster_main[3], 0.5, .015);
	thruster_main[4] = vessel->CreateThruster(_V(-.425, .35, AGENA_OFS_FLAME.z+.7), _V(0,0,1), AGENA_THRUST_SPS_2, fuel_sps, AGENA_ISP_VAC, 0.0);
	vessel->AddExhaust(thruster_main[4], 0.5, .015);


	th_group[0] = thruster_main[0];
	vessel->CreateThrusterGroup(th_group,1,THGROUP_MAIN);

	thruster_rcs[0] = vessel->CreateThruster(_V(0, .6, AGENA_OFS_FLAME.z+.9), _V(0,-1,0), AGENA_THRUST_RSC, fuel_rcs, AGENA_ISP_RCS, 0.0);
	vessel->AddExhaust(thruster_rcs[0], 0.5, .03);
	vessel->CreateThrusterGroup(&thruster_rcs[0],1,THGROUP_ATT_PITCHUP);

	thruster_rcs[1] = vessel->CreateThruster(_V(0, -.6, AGENA_OFS_FLAME.z+.9), _V(0,1,0), AGENA_THRUST_RSC, fuel_rcs, AGENA_ISP_RCS, 0.0);
	vessel->AddExhaust(thruster_rcs[1], 0.5, .03);
	vessel->CreateThrusterGroup(&thruster_rcs[1],1,THGROUP_ATT_PITCHDOWN);

	thruster_rcs[2] = vessel->CreateThruster(_V(.05, .6, AGENA_OFS_FLAME.z+.9), _V(-1,0,0), AGENA_THRUST_RSC  / 2, fuel_rcs, AGENA_ISP_RCS, 0.0);
	vessel->AddExhaust(thruster_rcs[2], 0.5, .03);
	
	thruster_rcs[3] = vessel->CreateThruster(_V(.05, -.6, AGENA_OFS_FLAME.z+.9), _V(-1,0,0), AGENA_THRUST_RSC / 2, fuel_rcs, AGENA_ISP_RCS, 0.0);
	vessel->AddExhaust(thruster_rcs[3], 0.5, .03);

	vessel->CreateThrusterGroup(&thruster_rcs[2],2,THGROUP_ATT_YAWRIGHT);

	thruster_rcs[4] = vessel->CreateThruster(_V(-.05, .6, AGENA_OFS_FLAME.z+.9), _V(1,0,0), AGENA_THRUST_RSC / 2, fuel_rcs, AGENA_ISP_RCS, 0.0);
	vessel->AddExhaust(thruster_rcs[4], 0.5, .03);
	
	thruster_rcs[5] = vessel->CreateThruster(_V(-.05, -.6, AGENA_OFS_FLAME.z+.9), _V(1,0,0), AGENA_THRUST_RSC  / 2, fuel_rcs, AGENA_ISP_RCS, 0.0);
	vessel->AddExhaust(thruster_rcs[5], 0.5, .03);

	vessel->CreateThrusterGroup(&thruster_rcs[4],2,THGROUP_ATT_YAWLEFT);
	
	
	thruster_rcs[6] = vessel->CreateThruster(_V(-.05, .6, AGENA_OFS_FLAME.z+.9), _V(1,0,0), AGENA_THRUST_RSC  / 2, fuel_rcs, AGENA_ISP_RCS, 0.0);
	vessel->AddExhaust(thruster_rcs[6], 0.5, .03);

	thruster_rcs[7] = vessel->CreateThruster(_V(.05, -.6, AGENA_OFS_FLAME.z+.9), _V(-1,0,0), AGENA_THRUST_RSC  / 2, fuel_rcs, AGENA_ISP_RCS, 0.0);
	vessel->AddExhaust(thruster_rcs[7], 0.5, .03);

	vessel->CreateThrusterGroup(&thruster_rcs[6],2,THGROUP_ATT_BANKRIGHT);

	thruster_rcs[8] = vessel->CreateThruster(_V(.05, .6, AGENA_OFS_FLAME.z+.9), _V(-1,0,0), AGENA_THRUST_RSC  / 2, fuel_rcs, AGENA_ISP_RCS, 0.0);
	vessel->AddExhaust(thruster_rcs[8], 0.5, .03);
	
	thruster_rcs[9] = vessel->CreateThruster(_V(-.05, -.6, AGENA_OFS_FLAME.z+.9), _V(1,0,0), AGENA_THRUST_RSC / 2, fuel_rcs, AGENA_ISP_RCS, 0.0);
	vessel->AddExhaust(thruster_rcs[9], 0.5, .03);

	vessel->CreateThrusterGroup(&thruster_rcs[8],2,THGROUP_ATT_BANKLEFT);
}

#endif