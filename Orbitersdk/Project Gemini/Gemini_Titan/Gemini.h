#ifndef __GEMINI_H
#define __GEMINI_H

#include "..\..\include\orbitersdk.h"
#include "..\..\include\multi_stage4.h"
#include "panel_graphics.h"
#include "gemini_panel.h"
#include <cstdio>
#include <fstream>
#include <cmath>

const double GBOTTOM = -3.09+.75;
const VECTOR3 OFS_GEMINI         = { 0.0, 0.0, 0.75};
const VECTOR3 OFS_GEM_EQUIPMENT  = { 0.0, 0.0, GBOTTOM+.75};
const VECTOR3 OFS_GEM_ADAPTER    = { 0.0, 0.0, OFS_GEM_EQUIPMENT.z-.285+.85+.6};
const VECTOR3 OFS_GEM_CAPSULE    = { 0.0, 0.0, OFS_GEM_ADAPTER.z-.385 +.6+1.43};
const VECTOR3 OFS_GEM_DOOR_LEFT  = { -0.49, 0.647, OFS_GEM_CAPSULE.z-.631};
const VECTOR3 OFS_GEM_DOOR_RIGHT = { 0.479, 0.657, OFS_GEM_CAPSULE.z-.631};
const VECTOR3 OFS_GEM_NOSE       = { 0.0, 0.0, OFS_GEM_CAPSULE.z-.00 + 1.035 +.55};
const VECTOR3 OFS_GEM_CDR        = { -0.5, -0.049, OFS_GEM_CAPSULE.z-.72};
const VECTOR3 OFS_GEM_PLT        = { 0.4 , -0.049, OFS_GEM_CAPSULE.z-.72};
const VECTOR3 OFS_GEM_PLT_EVA    = { 0.4 , -0.049, OFS_GEM_CAPSULE.z-1.72};
const VECTOR3 OFS_SEAT           = _V(OFS_GEM_PLT.x+.075,OFS_GEM_PLT.y+.125,OFS_GEM_PLT.z-0.05);
const VECTOR3 OFS_GEM_EVA        = {OFS_SEAT.x,OFS_SEAT.y+.15,OFS_SEAT.z+.2};
const VECTOR3 OFS_MAINCHUTE      = { 0.0, 0.0, OFS_GEM_CAPSULE.z+1.18};
const VECTOR3 OFS_AGENA          = { 0.0, 0.0, 3.149+3.149+.09};
const VECTOR3 OFS_BEACON         = { 0.0, 0.0, GBOTTOM +.5 - .1};
const VECTOR3 OFS_GEM_HEATSHIELD = { 0.0, 0.0, OFS_GEM_CAPSULE.z-1.3};
const VECTOR3 OFS_CAMERA_CDR     = {-0.49,0.62,0.4};
//const VECTOR3 OFS_CAMERA_CDR     = {0,0,0};
const double GEM_DRAG=0.65;
	

double GEMINI_FUEL_MASS_OAMS      = 420*KG;
double GEMINI_FUEL_MASS_RCS       =  35*KG;
double GEMINI_FUEL_MASS_RETRO     =  25*KG;

const double RCMD_MAXRATE = 10*RAD; //10 DEG/SEC

const double  GEMINI_LENGTH       = 6.18;
const double  GEMINI_MASS         = 3851*KG;
const double  GEMINI_EMPTY_MASS   = GEMINI_MASS - GEMINI_FUEL_MASS_OAMS - GEMINI_FUEL_MASS_RCS - GEMINI_FUEL_MASS_RETRO * 4;
const double  GEMINI_THRUST_MAN   = 45.35*KGF;
const double  GEMINI_THRUST_RETRO = 38.55*KGF; 
const double  GEMINI_THRUST_RETRO_ROCKET = 1159*KGF;
const double  GEMINI_THRUST_ATT   = 11.34*KGF;
const double  GEMINI_ISP          = 273*SEC;
const double  GEMINI_ISP_RETRO    = 255*SEC;
const double  GEMINI_CHUTE_MAIN   = 16200;
const double  GEMINI_CHUTE_DROGUE = 390.0;

const VECTOR3 GEMINI_OFS          = {0, 0, .75};

const double CHUTE_OPERATING_SPEED = 0.4;
const double LINE_OPERATING_SPEED = 1.25;
const double LINE1_MAX_ANGLE = RAD*17.0;
const double LINE2_MAX_ANGLE = RAD*39.0;
const double CAPSULE_MAX_ANGLE = RAD*55.0;

const double DOOR_OPERATING_SPEED = 0.128;
const double DOOR_MAX_ANGLE = RAD*80.0;

void DoPulseThruster(VESSEL2 *vessel, bool &flag, double &time, THGROUP_HANDLE th_group, double simdt, THGROUP_TYPE group, DWORD mode) {
	if (flag) {
		time += simdt;
		if (time < .2)
			vessel->SetThrusterGroupLevel(th_group, 1.0);
		else if (vessel->GetManualControlLevel(group, MANCTRL_ANYMODE, mode) <= .3) 
			flag = false;

	}

	if (vessel->GetManualControlLevel(group, MANCTRL_ANYMODE, mode) > .3 && !flag) {
		flag = true;
		time = 0.0;
		vessel->SetThrusterGroupLevel(th_group, 1.0);
	}

}

void DoPulseThruster2(VESSEL2 *vessel, bool &flag, double &time, THGROUP_HANDLE th_group1, THGROUP_HANDLE th_group2, double simdt, THGROUP_TYPE group, DWORD mode) {
	if (flag) {
		time += simdt;
		if (time < .2) {
			vessel->SetThrusterGroupLevel(th_group1, 1.0);
			vessel->SetThrusterGroupLevel(th_group2, 1.0);
		} else if (vessel->GetManualControlLevel(group, MANCTRL_ANYMODE, mode) <= .3) {
			flag = false;
		}

	}

	if (vessel->GetManualControlLevel(group, MANCTRL_ANYMODE, mode) > .3 && !flag) {
		flag = true;
		time = 0.0;
		vessel->SetThrusterGroupLevel(th_group1, 1.0);
		vessel->SetThrusterGroupLevel(th_group2, 1.0);
	}

}

void DoRateThruster(VESSEL2 *vessel, double rate, double ang_vel, double deadband, THGROUP_HANDLE th_group_pos, THGROUP_HANDLE th_group_neg, THGROUP_TYPE group_pos, THGROUP_TYPE group_neg, DWORD mode ) {
	double t_pos;
	double t_rate;


	t_pos = vessel->GetManualControlLevel(group_pos, MANCTRL_ANYMODE, mode);
	if (t_pos == 0)
		t_pos = -vessel->GetManualControlLevel(group_neg, MANCTRL_ANYMODE, mode);

	if (fabs(t_pos) <= .1) 
		t_rate = 0;
	else

		t_rate = rate * t_pos;

	if (ang_vel > t_rate + deadband) {
		vessel->SetThrusterGroupLevel(th_group_neg, 1.0);
	} else {
		vessel->SetThrusterGroupLevel(th_group_neg, 0.0);
	}
	if (ang_vel < t_rate - deadband) {
		vessel->SetThrusterGroupLevel(th_group_pos, 1.0);
	} else {
		vessel->SetThrusterGroupLevel(th_group_pos, 0.0);
	}

}

void DoCmdRateThruster(VESSEL2 *vessel, double rate, double ang_vel, double deadband, THGROUP_HANDLE th_group_pos, THGROUP_HANDLE th_group_neg) {
	double t_pos;
	double t_rate;

	t_rate = rate;

	if (ang_vel > t_rate + deadband) {
		vessel->SetThrusterGroupLevel(th_group_neg, 1.0);
	} else {
		vessel->SetThrusterGroupLevel(th_group_neg, 0.0);
	}
	if (ang_vel < t_rate - deadband) {
		vessel->SetThrusterGroupLevel(th_group_pos, 1.0);
	} else {
		vessel->SetThrusterGroupLevel(th_group_pos, 0.0);
	}

}



void DoAttHoldThruster(VESSEL2 *vessel, double heading, double target, double ang_vel, double deadband, THGROUP_HANDLE th_group_pos, THGROUP_HANDLE th_group_neg) {
	if (fabs(target-heading) > deadband) {
		if ( heading < target ) {
			if (target-heading < (deadband + 5*RAD)) {
				DoCmdRateThruster(vessel, 1*RAD, ang_vel , 2*RAD, th_group_pos, th_group_neg);
			} else if (target-heading < (deadband + 10*RAD)) {
				DoCmdRateThruster(vessel, 5*RAD, ang_vel , 2*RAD, th_group_pos, th_group_neg);				
			}
		} else if (heading > target) {
			if (target-heading < (deadband + 5*RAD)) {
				DoCmdRateThruster(vessel, -1*RAD, ang_vel , 2*RAD, th_group_pos, th_group_neg);
			} else if (target-heading < (deadband + 10*RAD)) {
				DoCmdRateThruster(vessel, -5*RAD, ang_vel , 2*RAD, th_group_pos, th_group_neg);				
			}
		}
		
	} else {
		DoCmdRateThruster(vessel, 0*RAD, ang_vel, 2*RAD, th_group_pos, th_group_neg);
	}
}

void AddCapsuleVC(VESSEL2 *vessel)
{
	UINT nMesh;
	MESHHANDLE vMesh;

	vMesh = oapiLoadMeshGlobal("gem_capsule_dom");
	((Gemini_ControlPanel*)((Multi_Stage*)vessel)->GetPanel())->vcmesh_tpl = vMesh;

	nMesh = vessel->AddMesh(vMesh,&OFS_GEM_CAPSULE);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_VC);

	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_heatshield_dom"),&OFS_GEM_HEATSHIELD);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_VC);

}

void AddCommanderVC(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem-seat_dom"),&OFS_GEM_CDR);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_VC);
}

void AddPilotVC(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem-seatp_dom"),&OFS_GEM_PLT);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_VC);
}


void AddPilotEVAVC(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem-seatp_dom"),&OFS_GEM_PLT_EVA);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_VC);
}

void AddPilotSeatVC(VESSEL*vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh (oapiLoadMeshGlobal("gem-ejseat_dom"), &OFS_SEAT);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_VC);
}

void AddDoorLeftVC(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_door_l_dom"),&OFS_GEM_DOOR_LEFT);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_VC);
}

void AddDoorRightVC(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_door_r_dom"),&OFS_GEM_DOOR_RIGHT);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_VC);
}

void AddNoseVC(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_nose_dom"),&OFS_GEM_NOSE);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_VC);
}

void AddEquipment(VESSEL2 *vessel)
{
	vessel->AddMesh(oapiLoadMeshGlobal("gem_equip"),&OFS_GEM_EQUIPMENT);
}

void AddAdapter(VESSEL2 *vessel)
{
	vessel->AddMesh(oapiLoadMeshGlobal("gem_adapt"),&OFS_GEM_ADAPTER);
}

void AddCapsule(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_capsule"),&OFS_GEM_CAPSULE);
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_heatshield1"),&OFS_GEM_HEATSHIELD);
	AddCapsuleVC(vessel);
}

void AddCommander(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem-seat"),&OFS_GEM_CDR);
	AddCommanderVC(vessel);
}

void AddPilot(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem-seatp"),&OFS_GEM_PLT);
	AddPilotVC(vessel);
}


void AddPilotEVA(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem-seatp"),&OFS_GEM_PLT_EVA);
	AddPilotEVAVC(vessel);
}

void AddPilotSeat(VESSEL*vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh (oapiLoadMeshGlobal("gem-ejseat"), &OFS_SEAT);
	AddPilotSeatVC(vessel);
}

void AddDoorLeft(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_door_l"),&OFS_GEM_DOOR_LEFT);
	AddDoorLeftVC(vessel);
}

void AddDoorRight(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_door_r"),&OFS_GEM_DOOR_RIGHT);
	AddDoorRightVC(vessel);
}

void AddNose(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_nose"),&OFS_GEM_NOSE);
	AddNoseVC(vessel);
}


void AddChuteLine1(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_paraline"),&OFS_GEM_CAPSULE);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_ALWAYS);
}

void AddChuteLine2(VESSEL2 *vessel)
{
	UINT nMesh;
	nMesh = vessel->AddMesh(oapiLoadMeshGlobal("gem_paraline"),&OFS_GEM_CAPSULE);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_ALWAYS);
}

void AddDrogue(VESSEL2 *vessel)
{
	UINT nMesh;
	VECTOR3 mesh_dir=_V(0,0,OFS_GEM_NOSE.z+.55);
	nMesh = vessel->AddMesh ("gem_drogue", &mesh_dir);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_ALWAYS);
}

void AddChute(VESSEL2 *vessel)
{
	UINT nMesh;
	VECTOR3 mesh_dir=OFS_MAINCHUTE;
	nMesh = vessel->AddMesh ("gem_chutez", &mesh_dir);
	vessel->SetMeshVisibilityMode (nMesh, MESHVIS_ALWAYS);
}


void AddAgena(VESSEL2 *vessel)
{
	vessel->AddMesh(oapiLoadMeshGlobal("agena_dock"),&OFS_AGENA);
}

void AddBeacon(VESSEL2 *vessel)
{
	vessel->AddMesh(oapiLoadMeshGlobal("beacon"),&OFS_BEACON);
}

void AddGemini(VESSEL2 *vessel)
{
	AddCapsule(vessel);
	AddCommander(vessel);
	AddDoorLeft(vessel);
	AddDoorRight(vessel);
	AddPilot(vessel);
	AddPilotSeat(vessel);
	AddNose(vessel);
	AddAdapter(vessel);
	AddEquipment(vessel);
};

void AddGeminiEVA(VESSEL2 *vessel)
{
	AddCapsule(vessel);
	AddCommander(vessel);
	AddDoorLeft(vessel);
	AddDoorRight(vessel);
	AddPilotEVA(vessel);
	AddPilotSeat(vessel);
	AddNose(vessel);
	AddAdapter(vessel);
	AddEquipment(vessel);
};

void AddGeminiRetro(VESSEL2 *vessel)
{
	AddCapsule(vessel);   //0,1.2.3
	AddCommander(vessel); //4.5
	AddDoorLeft(vessel);  //6.7
	AddDoorRight(vessel); //8.9
	AddPilot(vessel);     //10,11
	AddPilotSeat(vessel); //12,13
	AddNose(vessel);      //14.15
	AddAdapter(vessel);
};

void AddGeminiCapsule(VESSEL2 *vessel, bool Nose)
{
	AddCapsule(vessel);    //0,1,2,3  0,1
	AddCommander(vessel);  //4.5      2
	AddDoorLeft(vessel);   //6.7      3
	AddDoorRight(vessel);  //8.9      4
	AddPilot(vessel);      //10,11    5
	AddPilotSeat(vessel);  //12,13    6
	AddChuteLine1(vessel); //14       7
	AddChuteLine2(vessel); //15       8
	if (Nose) 
		AddNose(vessel);   //16,17
};

void AddGeminiNoDoors(VESSEL2 *vessel)
{
	AddNose(vessel);
	AddCapsule(vessel);
	AddCommander(vessel);
	AddPilot(vessel);
	AddPilotSeat(vessel);
	AddAdapter(vessel);
	AddEquipment(vessel);
};

void AddGeminiNoCrew(VESSEL2 *vessel)
{
	AddEquipment(vessel);
	AddAdapter(vessel);
	AddCapsule(vessel);
	AddNose(vessel);
};

void JettisonCommander (VESSEL2 *vessel)
{
	// Create Tank as individual object
	VESSELSTATUS vs;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	VECTOR3 ofs = OFS_GEM_CDR;
	VECTOR3 rofs, rvel = {vs.rvel.x, vs.rvel.y, vs.rvel.z};
	VECTOR3 vel = {-5,100,60};
	vs.status=0;
	vessel->Local2Rel (ofs, vs.rpos);
	vessel->GlobalRot (vel, rofs);
	vs.rvel.x = rvel.x+rofs.x;
	vs.rvel.y = rvel.y+rofs.y;
	vs.rvel.z = rvel.z+rofs.z;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-CDR");
	VESSEL::Create (name, "Gemini_CDR", vs);
}

void JettisonPilot (VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	vessel->GetStatus (vs);
	vs.status=0;
	vs.eng_main = vs.eng_hovr = 0.0;
	VECTOR3 ofs = OFS_GEM_PLT;
	VECTOR3 rofs, rvel = {vs.rvel.x, vs.rvel.y, vs.rvel.z};
	VECTOR3 vel = {5,100,60};
	vessel->Local2Rel (ofs, vs.rpos);
	vessel->GlobalRot (vel, rofs);
	vs.rvel.x = rvel.x+rofs.x;
	vs.rvel.y = rvel.y+rofs.y;
	vs.rvel.z = rvel.z+rofs.z;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-PLT");
	VESSEL::Create (name, "Gemini_PLT", vs);
}

void JettisonDoorLeft (VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	VECTOR3 ofs = OFS_GEM_DOOR_LEFT;
	VECTOR3 rofs, rvel = {vs.rvel.x, vs.rvel.y, vs.rvel.z};
	VECTOR3 vel = {-5,100,50};
	vessel->Local2Rel (ofs, vs.rpos);
	vessel->GlobalRot (vel, rofs);
	vs.status=0;
	vs.rvel.x = rvel.x+rofs.x;
	vs.rvel.y = rvel.y+rofs.y;
	vs.rvel.z = rvel.z+rofs.z;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-LDOOR");
	VESSEL::Create (name, "Gemini_Door_Left", vs);
}

void JettisonDoorRight (VESSEL2 *vessel)
{
	// Create Tank as individual object
	VESSELSTATUS vs;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	VECTOR3 ofs = OFS_GEM_DOOR_RIGHT;
	VECTOR3 rofs, rvel = {vs.rvel.x, vs.rvel.y, vs.rvel.z};
	VECTOR3 vel = {5,100,50};
	VECTOR3 rot = {0,-90*RAD,0};
	vessel->Local2Rel (ofs, vs.rpos);
	vessel->GlobalRot (vel, rofs);
	vs.status=0;
	vs.rvel.x = rvel.x+rofs.x;
	vs.rvel.y = rvel.y+rofs.y;
	vs.rvel.z = rvel.z+rofs.z;
	vessel->GlobalRot(rot,vs.arot);
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-RDOOR");
	VESSEL::Create (name, "Gemini_Door_Right", vs);
}

void JettisonBeacon(VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,-0.1);
	ofs = OFS_BEACON;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	vessel->SetEngineLevel(ENGINE_MAIN, 0);
	StageTransform(vessel,&vs,ofs,vel);
	vs.status=0;
	vs.vrot.x = 0.0;
	vs.vrot.y = 0.0;
	vs.vrot.z = 0.0;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-BEACON");
	VESSEL::Create (name, "beacon", vs);
}



class Gemini:public IStage
{
public:
	Gemini(Gemini_ControlPanel *Panel);
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
	bool doAbort;
private:
	void CreateGeminiThrusters(VESSEL2 *vessel);
	void SetStage(VESSEL2 *vessel);
	void SetRetroStage(VESSEL2 *vessel);
	void SetCapsuleStage(VESSEL2 *vessel);
	void JettisonEquipment(VESSEL2 *vessel);
	void JettisonAdapter(VESSEL2 *vessel);
	void JettisonNose(VESSEL2 *vessel);
	void JettisonDrogue(VESSEL2 *vessel);
	void JettisonMainChute(VESSEL2 *vessel);
	void DoCapsuleThrusters(VESSEL2 *vessel, double simdt);
	void DoPilotDoor(VESSEL2 *vessel);
	void SetRCS(VESSEL2 *vessel);
	void SetOAMS(VESSEL2 *vessel);
	void SeparateEVA(VESSEL2 *vessel);
	void RevertPilotDoor(VESSEL2 *vessel);
	void OperatePilotDoor(VESSEL2 *vessel);
	void UpdateMesh(VESSEL2 *vessel);
	void SetMainChuteStage(VESSEL2 *vessel);
	void DeployDrogue(VESSEL2 *vessel);
	void DeployMain(VESSEL2 *vessel);
	void SepParachute(VESSEL2 *vessel);
	void RevertLine(VESSEL2 *vessel);
	void OperateLine(VESSEL2 *vessel); 
	void OpenChute (VESSEL2 *vessel);
	void OpenDrogue (VESSEL2 *vessel);
	bool bEquipmentJettisoned;
	bool bAdapterJettisoned;
	bool bManualSeparate;
	bool bRCSJets;
	bool bAbort;
	bool bJettDoors;
	double DoorJettTime;

	double stage_sep;
	enum DoorStatus { DOOR_CLOSED, DOOR_OPEN, DOOR_CLOSING, DOOR_OPENING } door_status;
	double door_proc, vdoor_proc; 
	bool door_moving;             
	bool door_endanim;            
	enum LineStatus { LINE_CLOSED, LINE_OPEN, LINE_CLOSING, LINE_OPENING } line1_status;
	double line1_proc, vline1_proc; 
	bool line1_moving;             
	bool line1_endanim;            
	enum DrogueStatus { DROGUE_CLOSED, DROGUE_DEPLOYED, DROGUE_OPENING, DROGUE_REEFED, DROGUE_UNREEFING, DROGUE_OPENED } drogue_status;
	double drogue_proc, vdrogue_proc; 
	bool drogue_moving;             
	bool drogue_endanim;            
	double drogue_reefed;
	enum ChuteStatus { CHUTE_CLOSED, CHUTE_DEPLOYED, CHUTE_OPENING, CHUTE_REEFED, CHUTE_UNREEFING, CHUTE_OPENED } chute_status;
	double chute_proc, vchute_proc; 
	bool chute_moving;             
	bool chute_endanim;            
	double chute_reefed;
	bool bBeaconJettisoned;
	bool bRetro;
	int retro_seq;
	double retro_timer;
	bool bDrogueDeployed;
	bool bMainDeployed;
	bool bParachute;
	bool bParachuteJettisoned;
	bool bDrogueJettisoned;
	bool bMainJettisoned;
	double PressDelta;
	double LastPress;
	double ThrustTime;
	bool bDoorsJettisoned;
	bool bGemini_EVA;
	VESSEL *hAgena;

	PROPELLANT_HANDLE fuel_oams;
	PROPELLANT_HANDLE fuel_rcs;
	PROPELLANT_HANDLE fuel_retro[4];

	THRUSTER_HANDLE thruster_oams[20];
	THRUSTER_HANDLE thruster_rcs[32];
	THRUSTER_HANDLE thruster_retro[4];
	
	THGROUP_HANDLE oams_pitch_up;
	THGROUP_HANDLE oams_pitch_down;
	THGROUP_HANDLE oams_yaw_left;
	THGROUP_HANDLE oams_yaw_right;
	THGROUP_HANDLE oams_roll_left;
	THGROUP_HANDLE oams_roll_right;
	THGROUP_HANDLE oams_back;
	THGROUP_HANDLE oams_forward;
	THGROUP_HANDLE oams_left;
	THGROUP_HANDLE oams_right;
	THGROUP_HANDLE oams_up;
	THGROUP_HANDLE oams_down;

	bool do_pitch_up;
	bool do_pitch_down;
	bool do_yaw_left;
	bool do_yaw_right;
	bool do_roll_left;
	bool do_roll_right;
	bool do_back;
	bool do_forward;
	bool do_left;
	bool do_right;
	bool do_up;
	bool do_down;
	
	bool hasFocus;

	double time_pitch_up;
	double time_pitch_down;
	double time_yaw_left;
	double time_yaw_right;
	double time_roll_left;
	double time_roll_right;
	double time_back;
	double time_forward;
	double time_left;
	double time_right;
	double time_up;
	double time_down;

	THGROUP_HANDLE rcs_pitch_up1;
	THGROUP_HANDLE rcs_pitch_down1;
	THGROUP_HANDLE rcs_yaw_left1;
	THGROUP_HANDLE rcs_yaw_right1;
	THGROUP_HANDLE rcs_roll_left1;
	THGROUP_HANDLE rcs_roll_right1;

	THGROUP_HANDLE rcs_pitch_up2;
	THGROUP_HANDLE rcs_pitch_down2;
	THGROUP_HANDLE rcs_yaw_left2;
	THGROUP_HANDLE rcs_yaw_right2;
	THGROUP_HANDLE rcs_roll_left2;
	THGROUP_HANDLE rcs_roll_right2;
	
	bool bAgenaCommand;
	int  nAgenaCommand;
	int  AgenaCommand[3];
	Gemini_ControlPanel *ControlPanel;
};

Gemini::Gemini(Gemini_ControlPanel *Panel)
{
	ControlPanel = Panel;
	Init();
}

void Gemini::Init ()
{
	hasFocus = false;
	doAbort = false;
	bManualSeparate = false;
	bRCSJets=false;
	stage_sep=0;
	door_status = DOOR_CLOSED;
	door_moving=false;
	door_endanim=false;
	door_proc=0;
	vdoor_proc=0;
	bBeaconJettisoned =false;
	bEquipmentJettisoned = false;
	bAdapterJettisoned = false;
	bRetro = false;
	retro_seq = 0;
	retro_timer = 0.0;
	bDrogueDeployed = false;
	bMainDeployed = false;
	bParachute = false;
	bDrogueJettisoned=false;
	bMainJettisoned=false;
	bParachuteJettisoned=false;
	PressDelta=0.0;
	LastPress=0.0;
	line1_status = LINE_CLOSED;
	line1_moving=false;
	line1_endanim=false;
	line1_proc=0;
	vline1_proc=0;
	chute_status = CHUTE_CLOSED;
	chute_moving=false;
	chute_endanim=false;
	chute_proc=0;
	chute_reefed=0;
	vchute_proc=0;
	drogue_status = DROGUE_CLOSED;
	drogue_moving=false;
	drogue_endanim=false;
	drogue_proc=0;
	drogue_reefed=0;
	vdrogue_proc=0;
	bJettDoors=false;
	DoorJettTime=0.0;
	bAbort = false;
	bDoorsJettisoned = false;
	fuel_oams     = NULL;
	fuel_rcs      = NULL;
	fuel_retro[0] = NULL;
	fuel_retro[1] = NULL;
	fuel_retro[2] = NULL;
	fuel_retro[3] = NULL;
	hAgena        = NULL;
	bAgenaCommand = false;
	nAgenaCommand = 0;
	AgenaCommand[0] = 0;
	AgenaCommand[1] = 0;
	AgenaCommand[2] = 0;
	bGemini_EVA = false;
	ThrustTime = 0.0;

	do_pitch_up = false;
	do_pitch_down = false;
	do_yaw_left = false;
	do_yaw_right = false;
	do_roll_left = false;
	do_roll_right = false;
	do_back = false;
	do_forward = false;
	do_left = false;
	do_right = false;
	do_up = false;
	do_down = false;
	
	time_pitch_up = 0.0;
	time_pitch_down = 0.0;
	time_yaw_left = 0.0;
	time_yaw_right = 0.0;
	time_roll_left = 0.0;
	time_roll_right = 0.0;
	time_back = 0.0;
	time_forward = 0.0;
	time_left = 0.0;
	time_right = 0.0;
	time_up = 0.0;
	time_down = 0.0;
};

void Gemini::Exit (VESSEL2 *vessel)
{
};

void Gemini::FocusChanged(VESSEL2 *vessel, bool getfocus) {
	hasFocus = getfocus;
}

void Gemini::Setup (VESSEL2 *vessel, double stage_sep_time)
{
	stage_sep=stage_sep_time;
	SetStage(vessel);

};

void Gemini::ParseLine (VESSEL2 *vessel, char *line, void *vs)
{
	double tempD = 0.0;
	int    tempI = 0;
    if (!strnicmp (line, "RCS_JETS", 8)) {
        sscanf (line+8, "%lf", &tempD);
		if (tempD == 1.0)
			bRCSJets=true;
		else
			bRCSJets=false;
    } else if (!strnicmp (line, "DOOR_STATUS", 11)) 	{
        sscanf (line+11, "%d", &door_status);
    } else if (!strnicmp (line, "DOOR_PROC", 9)) 	{
        sscanf (line+9, "%lf", &door_proc);
    } else if (!strnicmp (line, "BEACON_JETTISONED", 17)) 	{
        sscanf (line+17, "%lf", &tempD);
		if (tempD == 1.0)
			bBeaconJettisoned=true;
		else
			bBeaconJettisoned=false;
    } else if (!strnicmp (line, "EQUIP_JETTISONED", 16)) 	{
        sscanf (line+16, "%lf", &tempD);
		if (tempD == 1.0)
			bEquipmentJettisoned = true;
		else
			bEquipmentJettisoned = false;
    } else if (!strnicmp (line, "ADAPT_JETTISONED", 16)) 	{
        sscanf (line+16, "%lf", &tempD);
		if (tempD == 1.0)
			bAdapterJettisoned=true;
		else
			bAdapterJettisoned=false;
    } else if (!strnicmp (line, "RETRO", 5)) 	{
        sscanf (line+5, "%lf", &tempD);
		if (tempD == 1.0)
			bRetro=true;
		else
			bRetro=false;
    } else if (!strnicmp (line, "RETRO_SEQ", 9)) 	{
        sscanf (line+9, "%d", &retro_seq);
    } else if (!strnicmp (line, "RETRO_TIMER", 11)) 	{
        sscanf (line+11, "%lf", &tempD);
		retro_timer = -tempD;
    } else if (!strnicmp (line, "DROGUE_DEPLOYED", 15)) 	{
        sscanf (line+15, "%lf", &tempD);
		if (tempD == 1.0)
			bDrogueDeployed=true;
		else
			bDrogueDeployed=false;
    } else if (!strnicmp (line, "MAIN_DEPLOYED", 13)) 	{
        sscanf (line+13, "%lf", &tempD);
		if (tempD == 1.0)
			bMainDeployed=true;
		else
			bMainDeployed=false;
    } else if (!strnicmp (line, "DROGUE_JETTISONED", 17)) 	{
        sscanf (line+17, "%lf", &tempD);
		if (tempD == 1.0)
			bDrogueJettisoned=true;
		else
			bDrogueJettisoned=false;
    } else if (!strnicmp (line, "MAIN_JETTISONED", 15)) 	{
        sscanf (line+15, "%lf", &tempD);
		if (tempD == 1.0)
			bMainJettisoned=true;
		else
			bMainJettisoned=false;
    } else if (!strnicmp (line, "LINE1_STATUS", 12)) 	{
        sscanf (line+12, "%d", &line1_status);
    } else if (!strnicmp (line, "LINE1_PROC", 10)) 	{
        sscanf (line+10, "%lf", &line1_proc);
    } else if (!strnicmp (line, "GEMINI_EVA", 10)) 	{
        sscanf (line+10, "%d", &tempI);
		if (tempI == 1)
			bGemini_EVA=true;
		else
			bGemini_EVA=false;
    } else if (!strnicmp (line, "LINE1_PROC", 10)) 	{
        sscanf (line+10, "%lf", &line1_proc);
    } else if (!strnicmp (line, "DOORS_JETTISONED", 16)) 	{
        sscanf (line+16, "%lf", &tempD);
		if (tempD == 1.0)
			bDoorsJettisoned=true;
		else
			bDoorsJettisoned=false;
    } else {
        vessel->ParseScenarioLineEx (line, vs);
    }
};

void Gemini::SaveState (VESSEL2 *vessel, FILEHANDLE scn)
{
	oapiWriteScenario_int (scn, "CONFIGURATION", 2);
	oapiWriteScenario_float (scn, "RCS_JETS",          bRCSJets);
	oapiWriteScenario_int   (scn, "DOOR_STATUS",       door_status);
	oapiWriteScenario_float (scn, "DOOR_PROC",         door_proc);
	oapiWriteScenario_float (scn, "BEACON_JETTISONED", bBeaconJettisoned);
	oapiWriteScenario_float (scn, "EQUIP_JETTISONED",  bEquipmentJettisoned);
	oapiWriteScenario_float (scn, "ADAPT_JETTISONED",  bAdapterJettisoned);
	oapiWriteScenario_float (scn, "RETRO",             bRetro);
	oapiWriteScenario_int   (scn, "RETRO_SEQ",         retro_seq);
	oapiWriteScenario_float (scn, "RETRO_TIMER",       retro_timer);
	oapiWriteScenario_float (scn, "DROGUE_DEPLOYED",   bDrogueDeployed);
	oapiWriteScenario_float (scn, "MAIN_DEPLOYED",     bMainDeployed);
	oapiWriteScenario_float (scn, "DROGUE_JETTISONED", bDrogueJettisoned);
	oapiWriteScenario_float (scn, "MAIN_JETTISONED",   bMainJettisoned);
	oapiWriteScenario_int   (scn, "LINE1_STATUS",      line1_status);
	oapiWriteScenario_float (scn, "LINE1_PROC",        line1_proc);
	oapiWriteScenario_float (scn, "DOORS_JETTISONED",  bDoorsJettisoned );

	char name[256];
	OBJHANDLE hvessel;
	strcpy (name, vessel->GetName()); strcat (name, "-PLT");
	hvessel=oapiGetVesselByName(name);
	if (hvessel != 0) 
		oapiWriteScenario_int (scn, "GEMINI_EVA",  1);
	else
		oapiWriteScenario_int (scn, "GEMINI_EVA",  0);
};

void Gemini::DoPilotDoor(VESSEL2 *vessel)
{
	if (door_moving) { 
		double dt = oapiGetSimStep();
		double da = dt * DOOR_OPERATING_SPEED;
		if (door_status == DOOR_CLOSING) {
			if (door_endanim) {
				door_status = DOOR_CLOSED;
				door_moving = door_endanim = false;
				vessel->UnregisterAnimation();
			} else if ((door_proc -= da) <= 0.0) {
				door_proc   = 0.0;
				door_endanim = true;
			}
		} else if (door_status = DOOR_OPENING) {
			if (door_endanim) {
				door_status = DOOR_OPEN;
				door_moving = door_endanim = false;
				vessel->UnregisterAnimation();
			} else if ((door_proc += da) >= 1.0) {
				door_proc   = 1.0;
				door_endanim = true;
			}
		}
	}
}

void Gemini::PreStep (VESSEL2 *vessel,  double simt, double simdt, double mjd)
{
	PressDelta = LastPress-vessel->GetAtmPressure();
	LastPress = vessel->GetAtmPressure();
	DoPilotDoor(vessel);
	DoCapsuleThrusters(vessel, simdt);
	if (doAbort)
	{
		if (!bEquipmentJettisoned)
		{
			JettisonEquipment(vessel);
			bEquipmentJettisoned = true;
			SetRetroStage(vessel);
			vessel->SetThrusterLevel(thruster_retro[0], 1.0);
			vessel->SetThrusterLevel(thruster_retro[1], 1.0);
			vessel->SetThrusterLevel(thruster_retro[2], 1.0);
			vessel->SetThrusterLevel(thruster_retro[3], 1.0);
		} else if  (vessel->GetPropellantMass(fuel_retro[3])==0) {
			doAbort = false;
			retro_seq = 3;
		}
		return;
	}
	if (door_status == DOOR_CLOSED && bManualSeparate && !bEquipmentJettisoned)
	{
		bManualSeparate=false;
		JettisonEquipment(vessel);
		bEquipmentJettisoned = true;
		SetRetroStage(vessel);
		return;
	}
	if (door_status == DOOR_CLOSED && bManualSeparate && bEquipmentJettisoned && !bAdapterJettisoned)
	{
		bManualSeparate=false;
		JettisonAdapter(vessel);
		bAdapterJettisoned = true;
		SetCapsuleStage(vessel);
		return;
	}
	if (bEquipmentJettisoned)
	{
		if (bRetro)
		{
			if (retro_seq == 0)
			{
				retro_seq = 1;
				bRetro = false;
			}
			else if (retro_seq == 1)
			{
				retro_timer = simt;
				retro_seq = 2;
				vessel->SetThrusterLevel(thruster_retro[0], 1.0);
			}
			else if (retro_seq == 2)
			{
				if (simt-retro_timer >= 16.5)
				{
					vessel->SetThrusterLevel(thruster_retro[3], 1.0);
				}
				else if (simt-retro_timer >= 11)
				{
					vessel->SetThrusterLevel(thruster_retro[2], 1.0);
				}
				else if (simt-retro_timer >= 5.5)
				{
					vessel->SetThrusterLevel(thruster_retro[1], 1.0);
				}
				if (vessel->GetPropellantMass(fuel_retro[3])==0)
				{
					bRetro=false;
					retro_seq = 3;
				}
			}
		}

	}

	if (bAdapterJettisoned) {
	//vessel->SetCrossSections (_V(2.8,2.8,52.1));
		
		if (drogue_moving) { 
			double ct = oapiGetSimStep();
			double ca = ct * CHUTE_OPERATING_SPEED;
			//sprintf(oapiDebugString(),"Reef Time %f %f", ct, chute_reefed);
			if (drogue_status == DROGUE_OPENING) {
				if (drogue_endanim) {
					drogue_status = DROGUE_REEFED;
					drogue_reefed = oapiGetSimTime();
					//chute_moving = true;
					drogue_endanim = false;
					vessel->UnregisterAnimation();
					vessel->SetCrossSections (_V(2.8,2.8,GEMINI_CHUTE_DROGUE * 0.12));
					vessel->SetCW (0.1, 1.5, .3,.3);
				} else if ((drogue_proc += ca) >= 1.0) {
					drogue_proc   = 1.0;
					drogue_endanim = true;
				} else {
					vessel->SetCrossSections (_V(2.8,2.8,GEMINI_CHUTE_DROGUE * 0.12 * drogue_proc));
				}
			} else if(drogue_status == DROGUE_REEFED && oapiGetSimTime() - drogue_reefed > 16.0) {
				drogue_status = DROGUE_UNREEFING;
				vdrogue_proc = 0.12;
				drogue_proc = 0.12;
				vessel->RegisterAnimation();
			} else if (drogue_status == DROGUE_UNREEFING) {
				if (drogue_endanim) {
					drogue_status = DROGUE_OPENED;
					drogue_moving = false;
					drogue_endanim = false;
					vessel->UnregisterAnimation();
					vessel->SetCrossSections (_V(2.8,2.8,GEMINI_CHUTE_DROGUE));
					vessel->SetCW (0.1, 1.5, .3,.3);
				} else if ((drogue_proc += ca) >= 1.0) {
					drogue_proc   = 1.0;
					drogue_endanim = true;
				} else {
					vessel->SetCrossSections (_V(2.8,2.8,(GEMINI_CHUTE_DROGUE- (GEMINI_CHUTE_DROGUE * 0.12)) * drogue_proc + (GEMINI_CHUTE_DROGUE * 0.12)));
				}
			}
		}

		if (chute_moving) { 
			double ct = oapiGetSimStep();
			double ca = ct * CHUTE_OPERATING_SPEED;
			//sprintf(oapiDebugString(),"Reef Time %f %f", ct, chute_reefed);
			if (chute_status == CHUTE_OPENING) {
				if (chute_endanim) {
					chute_status = CHUTE_REEFED;
					chute_reefed = oapiGetSimTime();
					//chute_moving = true;
					chute_endanim = false;
					vessel->UnregisterAnimation();
					vessel->SetCrossSections (_V(2.8,2.8,GEMINI_CHUTE_MAIN * 0.12));
					vessel->SetCW (0.1, 1.5, .3,.3);
				} else if ((chute_proc += ca) >= 1.0) {
					chute_proc   = 1.0;
					chute_endanim = true;
				} else {
					vessel->SetCrossSections (_V(2.8,2.8,GEMINI_CHUTE_MAIN * 0.12 * chute_proc));
				}
			} else if(chute_status == CHUTE_REEFED && oapiGetSimTime() - chute_reefed > 10.0) {
				chute_status = CHUTE_UNREEFING;
				vchute_proc = 0.12;
				chute_proc = 0.12;
				vessel->RegisterAnimation();
			} else if (chute_status == CHUTE_UNREEFING) {
				if (chute_endanim) {
					chute_status = CHUTE_OPENED;
					chute_moving = false;
					chute_endanim = false;
					vessel->UnregisterAnimation();
					vessel->SetCrossSections (_V(2.8,2.8,GEMINI_CHUTE_MAIN));
					vessel->SetCW (0.1, 1.5, .3,.3);
				} else if ((chute_proc += ca) >= 1.0) {
					chute_proc   = 1.0;
					chute_endanim = true;
				} else {
					vessel->SetCrossSections (_V(2.8,2.8,(GEMINI_CHUTE_MAIN- (GEMINI_CHUTE_MAIN * 0.12)) * chute_proc + (GEMINI_CHUTE_MAIN * 0.12)));
				}
			}
		}
		
		if (chute_status == CHUTE_DEPLOYED) {
			OpenChute(vessel);
		};

		if (drogue_status == DROGUE_DEPLOYED) {
			OpenDrogue(vessel);
		};

		if (vessel->GetAtmPressure() > 15160 && !bDrogueDeployed && PressDelta < 0)
		{
			bDrogueDeployed = true;
			DeployDrogue(vessel);
		};

		if (bDrogueJettisoned && !bMainDeployed && PressDelta < 0)
		{
			bMainDeployed=true;
			DeployMain(vessel);
		};			

		if (vessel->GetAtmPressure() > 67750 && !bDrogueJettisoned && PressDelta < 0)
		{
			bDrogueJettisoned=true;
			JettisonDrogue(vessel);
			JettisonNose(vessel);
			SetMainChuteStage(vessel);
		};			

		if (bParachuteJettisoned && (bMainDeployed || bDrogueDeployed)) 
		{
			bParachuteJettisoned=false;
			SepParachute(vessel);
		};
		double altitude=0;
		oapiGetAltitude(vessel->GetHandle(),&altitude);
		if (altitude < 3 && !bMainJettisoned) 
		{
			SepParachute(vessel);
			line1_proc =0;
			UpdateMesh(vessel);
			vessel->SetTouchdownPoints(_V(0,-.1,1),_V(-.1,0,-1),_V(.1,0,-1));
		}

		if (bMainDeployed && !bMainJettisoned)
		{
			if (line1_moving) { 
				double dt = oapiGetSimStep();
				double da = dt * LINE_OPERATING_SPEED;
				if (line1_status == LINE_CLOSING) {
					if (line1_endanim) {
						line1_status = LINE_CLOSED;
						line1_moving = line1_endanim = false;
						vessel->UnregisterAnimation();
					} else if ((line1_proc -= da) <= 0.0) {
						line1_proc   = 0.0;
						line1_endanim = true;
					}
				} else if (line1_status == LINE_OPENING) {
					if (line1_endanim) {
						line1_status = LINE_OPEN;
						line1_moving = line1_endanim = false;
						vessel->UnregisterAnimation();
					} else if ((line1_proc += da) >= 1.0) {
						line1_proc   = 1.0;
						line1_endanim = true;
					}
				}
			}
		}
	}
	if (!bDoorsJettisoned) {
		if (bAbort) {
			bAbort = false;
			if (!bJettDoors)
			{
				JettisonDoorLeft(vessel);
				JettisonDoorRight(vessel);
				vessel->ClearMeshes();
				if (!bDrogueJettisoned)
					AddNose(vessel);
				AddCapsule(vessel);
				AddCommander(vessel);
				AddPilot(vessel);
				AddPilotSeat(vessel);
				bJettDoors=true;
				DoorJettTime=simt;
			}
		}
		if (bJettDoors && (simt-DoorJettTime) > 1)
		{
			OBJHANDLE hvessel;
			char name[256];
			JettisonCommander(vessel);
			JettisonPilot(vessel);
			strcpy (name, vessel->GetName()); strcat (name, "-CDR");
			hvessel=oapiGetVesselByName(name);
			if (hvessel != 0)
			{
				oapiSetFocusObject(hvessel);
			}
			bDoorsJettisoned = true;
			bJettDoors = false;
			vessel->ClearMeshes();
			if (!bDrogueJettisoned)
				AddNose(vessel);
			AddCapsule(vessel);
		}
	} 
};

void Gemini::PostStep (VESSEL2 *vessel, double simt, double simdt, double mjd)
{
};

void Gemini::DoCapsuleThrusters(VESSEL2 *vessel, double simdt)
{
	VECTOR3 ang_vel = {0.0, 0.0, 0.0};
	DWORD mode;
	if (!hasFocus) return;
	vessel->SetThrusterGroupLevel(oams_pitch_up, 0.0);
	vessel->SetThrusterGroupLevel(oams_pitch_down, 0.0);
	vessel->SetThrusterGroupLevel(oams_yaw_left, 0.0);
	vessel->SetThrusterGroupLevel(oams_yaw_right, 0.0);
	vessel->SetThrusterGroupLevel(oams_roll_left, 0.0);
	vessel->SetThrusterGroupLevel(oams_roll_right, 0.0);
	vessel->SetThrusterGroupLevel(oams_forward, 0.0);
	vessel->SetThrusterGroupLevel(oams_back, 0.0);
	vessel->SetThrusterGroupLevel(oams_left, 0.0);
	vessel->SetThrusterGroupLevel(oams_right, 0.0);
	vessel->SetThrusterGroupLevel(oams_up, 0.0);
	vessel->SetThrusterGroupLevel(oams_down, 0.0);

	vessel->SetThrusterGroupLevel(rcs_pitch_up1, 0.0);
	vessel->SetThrusterGroupLevel(rcs_pitch_down1, 0.0);
	vessel->SetThrusterGroupLevel(rcs_yaw_left1, 0.0);
	vessel->SetThrusterGroupLevel(rcs_yaw_right1, 0.0);
	vessel->SetThrusterGroupLevel(rcs_roll_left1, 0.0);
	vessel->SetThrusterGroupLevel(rcs_roll_right1, 0.0);

	vessel->SetThrusterGroupLevel(rcs_pitch_up2, 0.0);
	vessel->SetThrusterGroupLevel(rcs_pitch_down2, 0.0);
	vessel->SetThrusterGroupLevel(rcs_yaw_left2, 0.0);
	vessel->SetThrusterGroupLevel(rcs_yaw_right2, 0.0);
	vessel->SetThrusterGroupLevel(rcs_roll_left2, 0.0);
	vessel->SetThrusterGroupLevel(rcs_roll_right2, 0.0);
	if (vessel->GetAttitudeMode() == RCS_LIN || vessel->GetAttitudeMode() == RCS_NONE) {
		mode = MANCTRL_JOYSTICK;
	} else {
		mode = MANCTRL_ANYDEVICE;
	}
	//sprintf(oapiDebugString(), "PU: %f PD: %f", vessel->GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYMODE, MANCTRL_JOYSTICK), vessel->GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYMODE, MANCTRL_JOYSTICK));
	if (bRCSJets) {
		if (ControlPanel->attitude_mode == ATTMODE_DIRECT) {
			if (ControlPanel->ring_mode == 1 || ControlPanel->ring_mode == 3) {
				if (vessel->GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_pitch_up1, 1.0);
				
				if (vessel->GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_pitch_down1, 1.0);
				
				if (vessel->GetManualControlLevel(THGROUP_ATT_YAWLEFT, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_yaw_left1, 1.0);
				
				if (vessel->GetManualControlLevel(THGROUP_ATT_YAWRIGHT, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_yaw_right1, 1.0 );
				
				if (vessel->GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_roll_left1, 1.0);
				
				if (vessel->GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_roll_right1, 1.0);
			} 

			if (ControlPanel->ring_mode == 2 || ControlPanel->ring_mode == 3) {
				if (vessel->GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_pitch_up2, 1.0);
				
				if (vessel->GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_pitch_down2, 1.0);
				
				if (vessel->GetManualControlLevel(THGROUP_ATT_YAWLEFT, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_yaw_left2, 1.0);
				
				if (vessel->GetManualControlLevel(THGROUP_ATT_YAWRIGHT, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_yaw_right2, 1.0 );
				
				if (vessel->GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_roll_left2, 1.0);
				
				if (vessel->GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ANYMODE, mode) > .3) 
					vessel->SetThrusterGroupLevel(rcs_roll_right2, 1.0);
			}
		
		} else if (ControlPanel->attitude_mode == ATTMODE_RATECMD) {
			vessel->GetAngularVel(ang_vel);
			if (ControlPanel->ring_mode == 1 || ControlPanel->ring_mode == 3) {
				DoRateThruster(vessel, 10*RAD, ang_vel.x, .2*RAD, rcs_pitch_up1,   rcs_pitch_down1,  THGROUP_ATT_PITCHUP,  THGROUP_ATT_PITCHDOWN,  mode);
				DoRateThruster(vessel, 10*RAD, ang_vel.y, .2*RAD, rcs_yaw_left1,   rcs_yaw_right1,   THGROUP_ATT_YAWLEFT,  THGROUP_ATT_YAWRIGHT,   mode);
				DoRateThruster(vessel, 15*RAD, ang_vel.z, .2*RAD, rcs_roll_right1, rcs_roll_left1,   THGROUP_ATT_BANKRIGHT, THGROUP_ATT_BANKLEFT,  mode);
			}
			if (ControlPanel->ring_mode == 2 || ControlPanel->ring_mode == 3) {
				DoRateThruster(vessel, 10*RAD, ang_vel.x, .2*RAD, rcs_pitch_up2,   rcs_pitch_down2,  THGROUP_ATT_PITCHUP,  THGROUP_ATT_PITCHDOWN,  mode);
				DoRateThruster(vessel, 10*RAD, ang_vel.y, .2*RAD, rcs_yaw_left2,   rcs_yaw_right2,   THGROUP_ATT_YAWLEFT,  THGROUP_ATT_YAWRIGHT,   mode);
				DoRateThruster(vessel, 15*RAD, ang_vel.z, .2*RAD, rcs_roll_right2, rcs_roll_left2,   THGROUP_ATT_BANKRIGHT, THGROUP_ATT_BANKLEFT,  mode);
			}

		} else if (ControlPanel->attitude_mode == ATTMODE_REENT) {
			vessel->GetAngularVel(ang_vel);
			if (ControlPanel->ring_mode == 1 || ControlPanel->ring_mode == 3) {
				DoCmdRateThruster(vessel,  0*RAD, ang_vel.x, 4*RAD, rcs_pitch_up1,   rcs_pitch_down1);
				DoCmdRateThruster(vessel,  0*RAD, ang_vel.y, 4*RAD, rcs_yaw_left1,   rcs_yaw_right1);
				DoCmdRateThruster(vessel, 15*RAD, ang_vel.z, 2*RAD, rcs_roll_right1, rcs_roll_left1);
			}
			if (ControlPanel->ring_mode == 2 || ControlPanel->ring_mode == 3) {
				DoCmdRateThruster(vessel,  0*RAD, ang_vel.x, 4*RAD, rcs_pitch_up2,   rcs_pitch_down2);
				DoCmdRateThruster(vessel,  0*RAD, ang_vel.y, 4*RAD, rcs_yaw_left2,   rcs_yaw_right2);
				DoCmdRateThruster(vessel, 15*RAD, ang_vel.z, 2*RAD, rcs_roll_right2, rcs_roll_left2);
			}
		} else if (ControlPanel->attitude_mode == ATTMODE_CMDREENT) {
			vessel->GetAngularVel(ang_vel);
			if (ControlPanel->ring_mode == 1 || ControlPanel->ring_mode == 3) {
				DoRateThruster(vessel, 10*RAD, ang_vel.x, 4*RAD, rcs_pitch_up1,   rcs_pitch_down1,  THGROUP_ATT_PITCHUP,  THGROUP_ATT_PITCHDOWN,  mode);
				DoCmdRateThruster(vessel,  0*RAD, ang_vel.y, 4*RAD, rcs_yaw_left1,   rcs_yaw_right1);
				DoRateThruster(vessel, 15*RAD, ang_vel.z, 2*RAD, rcs_roll_right1, rcs_roll_left1,   THGROUP_ATT_BANKRIGHT, THGROUP_ATT_BANKLEFT,  mode);
			}
			if (ControlPanel->ring_mode == 2 || ControlPanel->ring_mode == 3) {
				DoRateThruster(vessel, 10*RAD, ang_vel.x, 4*RAD, rcs_pitch_up2,   rcs_pitch_down2,  THGROUP_ATT_PITCHUP,  THGROUP_ATT_PITCHDOWN,  mode);
				DoCmdRateThruster(vessel,  0*RAD, ang_vel.y, 4*RAD, rcs_yaw_left2,   rcs_yaw_right2);
				DoRateThruster(vessel, 15*RAD, ang_vel.z, 2*RAD, rcs_roll_right2, rcs_roll_left2,   THGROUP_ATT_BANKRIGHT, THGROUP_ATT_BANKLEFT,  mode);
			}

		} else if (ControlPanel->attitude_mode == ATTMODE_PULSE) {

			if (ControlPanel->ring_mode == 1) {
				DoPulseThruster(vessel, do_pitch_up,   time_pitch_up,   rcs_pitch_up1,   simdt, THGROUP_ATT_PITCHUP,   mode);
				DoPulseThruster(vessel, do_pitch_down, time_pitch_down, rcs_pitch_down1, simdt, THGROUP_ATT_PITCHDOWN, mode);
				DoPulseThruster(vessel, do_yaw_left,   time_yaw_left,   rcs_yaw_left1,   simdt, THGROUP_ATT_YAWLEFT,   mode);
				DoPulseThruster(vessel, do_yaw_right,  time_yaw_right,  rcs_yaw_right1,  simdt, THGROUP_ATT_YAWRIGHT,  mode);
				DoPulseThruster(vessel, do_roll_left,  time_roll_left,  rcs_roll_left1,  simdt, THGROUP_ATT_BANKLEFT,  mode);
				DoPulseThruster(vessel, do_roll_right, time_roll_right, rcs_roll_right1, simdt, THGROUP_ATT_BANKRIGHT, mode);
			}
			if (ControlPanel->ring_mode == 2) {
				DoPulseThruster(vessel, do_pitch_up,   time_pitch_up,   rcs_pitch_up2,   simdt, THGROUP_ATT_PITCHUP,   mode);
				DoPulseThruster(vessel, do_pitch_down, time_pitch_down, rcs_pitch_down2, simdt, THGROUP_ATT_PITCHDOWN, mode);
				DoPulseThruster(vessel, do_yaw_left,   time_yaw_left,   rcs_yaw_left2,   simdt, THGROUP_ATT_YAWLEFT,   mode);
				DoPulseThruster(vessel, do_yaw_right,  time_yaw_right,  rcs_yaw_right2,  simdt, THGROUP_ATT_YAWRIGHT,  mode);
				DoPulseThruster(vessel, do_roll_left,  time_roll_left,  rcs_roll_left2,  simdt, THGROUP_ATT_BANKLEFT,  mode);
				DoPulseThruster(vessel, do_roll_right, time_roll_right, rcs_roll_right2, simdt, THGROUP_ATT_BANKRIGHT, mode);
			}
			if (ControlPanel->ring_mode == 3) {
				DoPulseThruster2(vessel, do_pitch_up,   time_pitch_up,   rcs_pitch_up1,   rcs_pitch_up2,   simdt, THGROUP_ATT_PITCHUP,   mode);
				DoPulseThruster2(vessel, do_pitch_down, time_pitch_down, rcs_pitch_down1, rcs_pitch_down2, simdt, THGROUP_ATT_PITCHDOWN, mode);
				DoPulseThruster2(vessel, do_yaw_left,   time_yaw_left,   rcs_yaw_left1,   rcs_yaw_left2,   simdt, THGROUP_ATT_YAWLEFT,   mode);
				DoPulseThruster2(vessel, do_yaw_right,  time_yaw_right,  rcs_yaw_right1,  rcs_yaw_right2,  simdt, THGROUP_ATT_YAWRIGHT,  mode);
				DoPulseThruster2(vessel, do_roll_left,  time_roll_left,  rcs_roll_left1,  rcs_roll_left2,  simdt, THGROUP_ATT_BANKLEFT,  mode);
				DoPulseThruster2(vessel, do_roll_right, time_roll_right, rcs_roll_right1, rcs_roll_right2, simdt, THGROUP_ATT_BANKRIGHT, mode);
			}
		}
	} else {
		if (ControlPanel->attitude_mode == ATTMODE_DIRECT) {
			if (vessel->GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYMODE, mode) > .3) 
				vessel->SetThrusterGroupLevel(oams_pitch_up, 1.0);
			
			if (vessel->GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYMODE, mode) > .3) 
				vessel->SetThrusterGroupLevel(oams_pitch_down, 1.0);
			
			if (vessel->GetManualControlLevel(THGROUP_ATT_YAWLEFT, MANCTRL_ANYMODE, mode) > .3) 
				vessel->SetThrusterGroupLevel(oams_yaw_left, 1.0);
			
			if (vessel->GetManualControlLevel(THGROUP_ATT_YAWRIGHT, MANCTRL_ANYMODE, mode) > .3) 
				vessel->SetThrusterGroupLevel(oams_yaw_right, 1.0 );
			
			if (vessel->GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ANYMODE, mode) > .3) 
				vessel->SetThrusterGroupLevel(oams_roll_left, 1.0);
			
			if (vessel->GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ANYMODE, mode) > .3) 
				vessel->SetThrusterGroupLevel(oams_roll_right, 1.0);
		
		
		} else if (ControlPanel->attitude_mode == ATTMODE_RATECMD) {
			vessel->GetAngularVel(ang_vel);
			
			DoRateThruster(vessel, 10*RAD, ang_vel.x, .2*RAD, oams_pitch_up,   oams_pitch_down,  THGROUP_ATT_PITCHUP,  THGROUP_ATT_PITCHDOWN,  mode);
			DoRateThruster(vessel, 10*RAD, ang_vel.y, .2*RAD, oams_yaw_left,   oams_yaw_right,   THGROUP_ATT_YAWLEFT,  THGROUP_ATT_YAWRIGHT,   mode);
			DoRateThruster(vessel, 15*RAD, ang_vel.z, .2*RAD, oams_roll_right, oams_roll_left,   THGROUP_ATT_BANKRIGHT, THGROUP_ATT_BANKLEFT,  mode);


		} else if (ControlPanel->attitude_mode == ATTMODE_REENT) {
			vessel->GetAngularVel(ang_vel);
			DoCmdRateThruster(vessel,  0*RAD, ang_vel.x, 4*RAD, oams_pitch_up,   oams_pitch_down);
			DoCmdRateThruster(vessel,  0*RAD, ang_vel.y, 4*RAD, oams_yaw_left,   oams_yaw_right);
			DoCmdRateThruster(vessel,  0*RAD, ang_vel.z, 2*RAD, oams_roll_right, oams_roll_left);
		} else if (ControlPanel->attitude_mode == ATTMODE_CMDREENT) {
			vessel->GetAngularVel(ang_vel);
			DoRateThruster(vessel, 10*RAD, ang_vel.x, 4*RAD, oams_pitch_up,   oams_pitch_down,  THGROUP_ATT_PITCHUP,  THGROUP_ATT_PITCHDOWN,  mode);
			DoCmdRateThruster(vessel,  0*RAD, ang_vel.y, 4*RAD, oams_yaw_left,   oams_yaw_right);
			DoRateThruster(vessel, 15*RAD, ang_vel.z, 2*RAD, oams_roll_right, oams_roll_left,   THGROUP_ATT_BANKRIGHT, THGROUP_ATT_BANKLEFT,  mode);

		} else if (ControlPanel->attitude_mode == ATTMODE_PULSE) {

			DoPulseThruster(vessel, do_pitch_up,   time_pitch_up,   oams_pitch_up,   simdt, THGROUP_ATT_PITCHUP,   mode);
			DoPulseThruster(vessel, do_pitch_down, time_pitch_down, oams_pitch_down, simdt, THGROUP_ATT_PITCHDOWN, mode);
			DoPulseThruster(vessel, do_yaw_left,   time_yaw_left,   oams_yaw_left,   simdt, THGROUP_ATT_YAWLEFT,   mode);
			DoPulseThruster(vessel, do_yaw_right,  time_yaw_right,  oams_yaw_right,  simdt, THGROUP_ATT_YAWRIGHT,  mode);
			DoPulseThruster(vessel, do_roll_left,  time_roll_left,  oams_roll_left,  simdt, THGROUP_ATT_BANKLEFT,  mode);
			DoPulseThruster(vessel, do_roll_right, time_roll_right, oams_roll_right, simdt, THGROUP_ATT_BANKRIGHT, mode);

		} else if (ControlPanel->attitude_mode == ATTMODE_HORSCAN) {
		} else if (ControlPanel->attitude_mode == ATTMODE_PARA) {
			vessel->GetAngularVel(ang_vel);
			DoCmdRateThruster(vessel,  0*RAD, ang_vel.x, 4*RAD, oams_pitch_up,   oams_pitch_down);
			DoCmdRateThruster(vessel,  0*RAD, ang_vel.y, 4*RAD, oams_yaw_left,   oams_yaw_right);
			DoCmdRateThruster(vessel,  0*RAD, ang_vel.z, 2*RAD, oams_roll_right, oams_roll_left);
		}


		if (vessel->GetAttitudeMode() == RCS_LIN || vessel->GetAttitudeMode() == RCS_NONE) {
			if (vessel->GetManualControlLevel(THGROUP_ATT_FORWARD, MANCTRL_ANYMODE, MANCTRL_KEYBOARD) > .3) 
				vessel->SetThrusterGroupLevel(oams_forward, 1.0);

			if (vessel->GetManualControlLevel(THGROUP_ATT_BACK, MANCTRL_ANYMODE, MANCTRL_KEYBOARD) > .3) 
				vessel->SetThrusterGroupLevel(oams_back, 1.0);
			
			if (vessel->GetManualControlLevel(THGROUP_ATT_UP, MANCTRL_ANYMODE, MANCTRL_KEYBOARD) > .3) 
				vessel->SetThrusterGroupLevel(oams_up, 1.0);

			if (vessel->GetManualControlLevel(THGROUP_ATT_DOWN, MANCTRL_ANYMODE, MANCTRL_KEYBOARD) > .3) 
				vessel->SetThrusterGroupLevel(oams_down, 1.0);

			if (vessel->GetManualControlLevel(THGROUP_ATT_LEFT, MANCTRL_ANYMODE, MANCTRL_KEYBOARD) > .3) 
				vessel->SetThrusterGroupLevel(oams_left, 1.0);

			if (vessel->GetManualControlLevel(THGROUP_ATT_RIGHT, MANCTRL_ANYMODE, MANCTRL_KEYBOARD) > .3) 
				vessel->SetThrusterGroupLevel(oams_right, 1.0);
		}
	}


}

void Gemini::Animate (VESSEL2 *vessel, double simt)
{
	UpdateMesh(vessel);
};

void Gemini::VisualCreated (VESSEL2 *vessel, VISHANDLE vis, int refcount)
{
	vdoor_proc=0.0;
	UpdateMesh(vessel);
};

void Gemini::VisualDestroyed (VESSEL2 *vessel, VISHANDLE vis, int refcount)
{

};

int Gemini::ConsumeDirectKey (VESSEL2 *vessel, char *keystate)
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

	}
	return 0;
};

int Gemini::ConsumeBufferedKey (VESSEL2 *vessel, DWORD key, bool down, char *kstate)
{
	int Cmd = 0;
	if (down == 0) return 0;
	if (KEYMOD_SHIFT (kstate)) 
	{

	}
	else if (KEYMOD_CONTROL (kstate)) 
	{
		// insert ctrl key combinations here
	}
	else 
	{ // unmodified keys
		if (bAdapterJettisoned) {
			if (key == OAPI_KEY_J) { // "Jettison Stage"
				bParachuteJettisoned = true;
				return 1;
			}
			if (key == OAPI_KEY_P) { // "Deploy Parachute"
				bParachute = true;
				return 1;
			}
			if (key == OAPI_KEY_D) // "Pilot Door" 
			{
				RevertLine(vessel);
				return 1;			
			}
			if (key == OAPI_KEY_E) { 
				bAbort = true;
				return 1;
			}
		} else {
			if (key == OAPI_KEY_S) { // "Select OAMS/RCS"
				if (bEquipmentJettisoned)
					bRCSJets = true;
				else
					bRCSJets = !bRCSJets;
				//if (bRCSJets)
				//	SetRCS(vessel, 3);
				//else
				//	SetOAMS(vessel);
				return 1;
			}
			if (key == OAPI_KEY_E) { 
				if (door_status == DOOR_OPEN)
				{
					char name[256];
					OBJHANDLE hvessel;
					strcpy (name, vessel->GetName()); strcat (name, "-PLT");
					hvessel=oapiGetVesselByName(name);
					if (hvessel == 0)
					{
						SeparateEVA(vessel);
						vessel->ClearMeshes();
						AddGeminiEVA(vessel);
						hvessel=oapiGetVesselByName(name);
						if (hvessel != 0)
							oapiSetFocusObject(hvessel);
					}
					else
					{
						oapiDeleteVessel(hvessel);
						vessel->ClearMeshes();
						AddGemini(vessel);
					};
				}
				return 1;
			}
			if (key == OAPI_KEY_P) // "Pilot Door" 
			{
				RevertPilotDoor(vessel);
				return 1;			
			}
			if (hAgena != NULL) {
				if (bAgenaCommand) {
					if (nAgenaCommand < 3) {
						if (key == OAPI_KEY_1) 
						{
							AgenaCommand[nAgenaCommand] = 1;
							nAgenaCommand += 1;
						}
						if (key == OAPI_KEY_2) 
						{
							AgenaCommand[nAgenaCommand] = 2;
							nAgenaCommand += 1;
						}
						if (key == OAPI_KEY_3) 
						{
							AgenaCommand[nAgenaCommand] = 3;
							nAgenaCommand += 1;
						}
						if (key == OAPI_KEY_4) 
						{
							AgenaCommand[nAgenaCommand] = 4;
							nAgenaCommand += 1;
						}
						if (key == OAPI_KEY_5) 
						{
							AgenaCommand[nAgenaCommand] = 5;
							nAgenaCommand += 1;
						}
						if (key == OAPI_KEY_6) 
						{
							AgenaCommand[nAgenaCommand] = 6;
							nAgenaCommand += 1;
						}
						if (key == OAPI_KEY_7) 
						{
							AgenaCommand[nAgenaCommand] = 7;
							nAgenaCommand += 1;
						}
						if (key == OAPI_KEY_8) 
						{
							AgenaCommand[nAgenaCommand] = 8;
							nAgenaCommand += 1;
						}
						if (key == OAPI_KEY_9) 
						{
							AgenaCommand[nAgenaCommand] = 9;
							nAgenaCommand += 1;
						}
						if (key == OAPI_KEY_0) 
						{
							AgenaCommand[nAgenaCommand] = 0;
							nAgenaCommand += 1;
						}
						if (nAgenaCommand == 3) 
							sprintf(oapiDebugString(),"Agena Command %d%d%d Ready To Transmit", AgenaCommand[0], AgenaCommand[1], AgenaCommand[2]);
						else if (nAgenaCommand == 2) 
							sprintf(oapiDebugString(),"Agena Command %d%dX", AgenaCommand[0], AgenaCommand[1]);
						else if (nAgenaCommand == 1) 
							sprintf(oapiDebugString(),"Agena Command %dXX", AgenaCommand[0]);
						return 1;
					} 
					sprintf(oapiDebugString(),"Agena Command %d Ready To Transmit", AgenaCommand);
					
					if (key == OAPI_KEY_T && nAgenaCommand == 3) // "Transmit Agena Command" 
					{
						Cmd = 0;
						for (int i = 0; i < 3; i++) {
							Cmd += (int) pow(10,(2 - i)) * AgenaCommand[i];
						}
						((COMMON_VESSEL2*)hAgena)->SendMessage("MS_MSG", new MS_MSG(Cmd, 0.0));
						bAgenaCommand = false;
						return 1;
					}
					if (key == OAPI_KEY_C ) // "Clear Command" 
					{
						nAgenaCommand = 0;
						AgenaCommand[0] = 0;
						AgenaCommand[1] = 0;
						AgenaCommand[2] = 0;
						sprintf(oapiDebugString(),"Agena Command Cleared");
						return 1;
					}
				}
				if (key == OAPI_KEY_B) // "Agena Command" 
				{
					bAgenaCommand = true;
					nAgenaCommand = 0;
					AgenaCommand[0] = 0;
					AgenaCommand[1] = 0;
					AgenaCommand[2] = 0;
					sprintf(oapiDebugString(),"Agena Command Begin");
					return 1;
				}
				if (key == OAPI_KEY_J) { // "Jettison Agena"
					vessel->Undock(0);
					hAgena = 0;
					return 1;
				}
			} else {
				if (key == OAPI_KEY_J) { // "Jettison Stage"
					bManualSeparate = true;
					return 1;
				}
				if (key == OAPI_KEY_B) // "Jettison Beacon" 
				{
					if (!bBeaconJettisoned)
					{
						JettisonBeacon(vessel);
						bBeaconJettisoned=true;
					}
					return 1;			
				}
			}
			if (key == OAPI_KEY_L) // "Lock on Agena" 
			{
				OBJHANDLE hVessel;
				hVessel = vessel->GetDockStatus(vessel->GetDockHandle(0));
				if (hVessel != 0) {
					hAgena = oapiGetVesselInterface(hVessel);
					if (!strnicmp (hAgena->GetClassName(), "CV_", 3) != 0)
						if (((COMMON_VESSEL2*)hAgena)->SendMessage("MS_MSG", new MS_MSG(0,0.0))== 1) 
							sprintf(oapiDebugString(),"Agena Comm Estabilshed");
						else
							hAgena = 0;	
					else {
						sprintf(oapiDebugString(), hAgena->GetClassName());
						hAgena = 0;	
					}
				}
				return 1;			
			}
			if (bEquipmentJettisoned) {
				if (key == OAPI_KEY_D) { // "Start Retro Fire"
					bRetro = true;
					return 1;
				}
			}
		}			
/*		if (key == OAPI_KEY_1) // "Pilot Door" 
		{
			VESSELSTATUS vs;
			OBJHANDLE hVessel;
			IStage *newStage;
			hVessel = oapiGetVesselByName("GATV");
			vessel->GetStatus(vs);
			if (hVessel != 0 && vs.status == 3)
			{
				oapiDeleteVessel(hVessel);
				newStage = new GeminiAgena();
				((Multi_Stage*)vessel)->SetStage(newStage,vessel,0);
			}
		}*/
	}	
	return 0;
}

void Gemini::SeparateEVA (VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	vs.fuel=1.0;
	vs.status=0;
	VECTOR3 ofs = OFS_GEM_EVA;
	vessel->Local2Rel (ofs, vs.rpos);
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-PLT");
	VESSEL::Create (name, "gemini_eva", vs);
}

void Gemini::SepParachute(VESSEL2 *vessel)
{
	if (!bMainDeployed && bDrogueDeployed && !bDrogueJettisoned)
	{
		JettisonDrogue(vessel);
		JettisonNose(vessel);
		bDrogueJettisoned=true;
		SetMainChuteStage(vessel);
	}
	else if (bMainDeployed && !bMainJettisoned)
	{
		JettisonMainChute(vessel);
		SetMainChuteStage(vessel);
		bMainJettisoned=true;
	}
}

void Gemini::DeployDrogue(VESSEL2 *vessel)
{
	//vessel->SetCrossSections (_V(2.8,2.8,52.1));
	vessel->SetCW (0.1, 1.5, 0.3, 0.3);
	vessel->SetSize (9);
	vessel->SetLiftCoeffFunc(0);
	AddDrogue(vessel);
	vessel->SetMeshVisibleInternal(8,true);
	drogue_status = DROGUE_DEPLOYED;
}

void Gemini::DeployMain(VESSEL2 *vessel)
{
	vessel->SetSize (12);
	vessel->ClearMeshes();
	AddGeminiCapsule(vessel, false);
	AddChute(vessel);
	chute_status = CHUTE_DEPLOYED;
}	

void Gemini::RevertLine (VESSEL2 *vessel)
{
	line1_status = (line1_status == LINE_CLOSED || line1_status == LINE_CLOSING ?
		LINE_OPENING : LINE_CLOSING);
	 OperateLine(vessel);
}

void Gemini::OperateLine (VESSEL2 *vessel)
{
	if ((line1_status == LINE_OPENING || line1_status == LINE_CLOSING) && !line1_moving) {
		line1_moving = true;
		vessel->RegisterAnimation();
	}
}

void Gemini::OpenChute (VESSEL2 *vessel)
{
	MESHGROUP_TRANSFORM mt;
	mt.nmesh = 16; //9;
	mt.ngrp = -1;
	mt.transform = MESHGROUP_TRANSFORM::SCALE;
	mt.P.scaleparam.scale.x	=.12;
	mt.P.scaleparam.scale.y	=.12;
	mt.P.scaleparam.scale.z	=0.01;
	vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
	vchute_proc = 0.01;
	chute_proc = 0.01;
	chute_moving = true;
	chute_status = CHUTE_OPENING;
	vessel->RegisterAnimation();
}

void Gemini::OpenDrogue (VESSEL2 *vessel)
{
	MESHGROUP_TRANSFORM mt;
	mt.nmesh = 16;//8;
	mt.ngrp = -1;
	mt.transform = MESHGROUP_TRANSFORM::SCALE;
	mt.P.scaleparam.scale.x	=.12;
	mt.P.scaleparam.scale.y	=.12;
	mt.P.scaleparam.scale.z	=0.01;
	vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
	vdrogue_proc = 0.01;
	drogue_proc = 0.01;
	drogue_moving = true;
	drogue_status = DROGUE_OPENING;
	vessel->RegisterAnimation();
}


void Gemini::SetRetroStage(VESSEL2 *vessel)
{
	vessel->SetSize (2.0);
	vessel->SetEmptyMass (2573 *KG - GEMINI_FUEL_MASS_RCS - GEMINI_FUEL_MASS_RETRO * 4); 
	vessel->SetCOG_elev (4.0);
	vessel->SetCameraOffset(OFS_CAMERA_CDR);
	vessel->SetDockParams(_V(0,0,3.149+.75),_V(0,0,1),_V(0,1,0));
	vessel->SetPMI (_V(3.5,3.5,0.85));
	vessel->SetCrossSections (_V(12.5,12.5,8.5));
	vessel->SetCW (.1, 1, .3, .3);
	vessel->SetRotDrag (_V(0.5,0.5,0.2));
	vessel->SetPitchMomentScale (0);
	vessel->SetBankMomentScale (0);
	vessel->SetLiftCoeffFunc (0); 
	vessel->ClearMeshes();
	AddCapsule(vessel);
	AddCommander(vessel);
	AddDoorLeft(vessel);
	AddDoorRight(vessel);
	AddPilot(vessel);
	AddPilotSeat(vessel);
	AddNose(vessel);
	AddAdapter(vessel);
	vessel->DelPropellantResource(fuel_oams);
	bRCSJets = true;
//	SetRCS(vessel, 3);
	for (int i = 0; i < 20 ; i++)
		vessel->DelThruster(thruster_oams[i]);
};

void CoeffFunc (double aoa, double M, double Re, double *cl, double *cm, double *cd){
	const int nlift      = 11;
	const double factor  = 0.0;
	static const double AOA[nlift] = {-180*RAD,-160*RAD,-150*RAD,-120*RAD,-90*RAD,0*RAD, 90*RAD, 120*RAD,150*RAD,160*RAD,180*RAD};
	static const double CL[nlift]  = {     0.0,  -0.137,  -0.194,  -0.098,    0.0,  0.0,    0.0,   0.098,  0.194,  0.137,    0.0};
//	static const double CL[nlift]  = {     0.0,    -0.3,  -0.425,  -0.215,    0.0,  0.0,    0.0,   0.215,  0.425,    0.3,    0.0};
	static const double CM[nlift]  = {     0.0,   0.004,   0.006,   0.012,  0.015,  0.0, -0.015,  -0.012, -0.006, -0.004,    0.0};
	static const double CD[nlift]  = {     1.6,     1.4,     1.0,     0.6,   0.75,  0.0,   0.75,     0.6,    1.0,    1.4,    1.6};
	static double SCL[nlift-1];
	static double SCM[nlift-1];
	static double SCD[nlift-1];
	
	for(int j = 0; j < nlift-1; j++){
		SCL[j]= (CL[j+1]-CL[j])/(AOA[j+1]-AOA[j]);
		SCM[j]= (CM[j+1]-CM[j])/(AOA[j+1]-AOA[j]);
		SCD[j]= (CD[j+1]-CD[j])/(AOA[j+1]-AOA[j]);
	}

	int i;
	//Iterative search for table entry by AOA
	for (i = 0; i < nlift-1 && AOA[i+1] < aoa; i++);

	*cl = (CL[i] + (aoa-AOA[i])*SCL[i]);
	*cm = factor*(CM[i] + (aoa-AOA[i])*SCM[i]);
	*cd = (CD[i] + (aoa-AOA[i])*SCD[i]);
} 

double LiftCoeff (double aoa)
{
	int i;
	const int nlift = 9;
	static const double AOA[nlift] = {-180*RAD,-165*RAD,-164*RAD,-45*RAD,0*RAD,45*RAD,164*RAD,165*RAD,180*RAD};
	static const double CL[nlift]  = {0.06375,  0.1275,       0,      0,    0,     0,      0,      0,0.06375};
	//	static const double CL[nlift]  = {       0,  0.1275,       0,      0,    0,     0,      0,-0.1275,      0};
	static const double SCL[nlift] = {(CL[1]-CL[0])/(AOA[1]-AOA[0]), (CL[2]-CL[1])/(AOA[2]-AOA[1]),
		                              (CL[3]-CL[2])/(AOA[3]-AOA[2]), (CL[4]-CL[3])/(AOA[4]-AOA[3]),
									  (CL[5]-CL[4])/(AOA[5]-AOA[4]), (CL[6]-CL[5])/(AOA[6]-AOA[5]),
									  (CL[7]-CL[6])/(AOA[7]-AOA[6]), (CL[8]-CL[7])/(AOA[8]-AOA[7])};
	for (i = 0; i < nlift-1 && AOA[i+1] < aoa; i++);
	return CL[i] + (aoa-AOA[i])*SCL[i];
}

void NewLiftCoeff (double aoa, double M, double Re, double *cl, double *cm, double *cd)
{
	int i;
	const int nabsc = 9;
	static const double AOA[nabsc] = {-180*RAD,-150*RAD,-130*RAD,-15*RAD, 0*RAD, 15*RAD,130*RAD,150*RAD,180*RAD};
	static const double CM[nabsc]  = {    .0002,    .0002,     0.0, 0.0002,  0.00,  -0.0002,      0,  -.0002,  -.0002};

	for (i = 0; i < nabsc-1 && AOA[i+1] < aoa; i++);
	double f = (aoa-AOA[i]) / (AOA[i+1]-AOA[i]);

	*cl = 0;
	*cm = CM[i] + (CM[i+1]-CM[i]) * f;  // aoa-dependent moment coefficient
	//sprintf(oapiDebugString(),"CD %f ", MERC_DRAG + oapiGetWaveDrag (M, 1.1, 1.0, 1.1, 10.0));
	*cd = GEM_DRAG + oapiGetWaveDrag (M, 1.1, 15.0, 30.0, 2.5);
}

void Gemini::SetCapsuleStage(VESSEL2 *vessel)
{
	vessel->SetSize (2.0);
	vessel->SetCOG_elev (2.0);
	vessel->SetCameraOffset(OFS_CAMERA_CDR);
	vessel->SetEmptyMass  (1893 *KG - GEMINI_FUEL_MASS_RCS);
	vessel->SetPMI (_V(3.5,3.5,0.85));
	vessel->SetCrossSections (_V(4.5,4.5,8.5));
	vessel->SetCW (0.1, .75, 0.3, .3);
	vessel->SetRotDrag (_V(0.7,0.7,0.1));
	vessel->SetPitchMomentScale (-1e-3);
	vessel->SetBankMomentScale (-1e-3);
	vessel->SetLiftCoeffFunc (LiftCoeff); 
	vessel->SetTouchdownPoints (_V(0,-2,2), _V(.5,-2,-2), _V(-.5,-2,-2));
	
	
	//vessel->ClearAirfoilDefinitions();
	//vessel->ClearVariableDragElements();
	//vessel->CreateAirfoil(LIFT_VERTICAL, _V(-0.014,0.107,0.75), NewLiftCoeff, 3.5 ,11.95, 1.0);
	
	//vessel->SetPitchMomentScale (-1e-3);
	//vessel->SetBankMomentScale (-1e-3);
	//vessel->SetLiftCoeffFunc (LiftCoeff); 
	//vessel->CreateVariableDragElement(&drogue_proc,.7,_V(0,0,(OFS_GEM_NOSE.z+.55)+13));
	//vessel->CreateVariableDragElement(&chute_proc,.7,_V(0,0,OFS_MAINCHUTE.z+18));

	PARTICLESTREAMSPEC rps = {
		0, 5, 100, 0, 0.03, 5, 100, 3, PARTICLESTREAMSPEC::DIFFUSE,
		PARTICLESTREAMSPEC::LVL_FLAT, 1, 1, PARTICLESTREAMSPEC::ATM_PLIN, 6e7, 12e7
	};
	vessel->AddReentryStream (&rps);	
	if (bMainDeployed && bMainJettisoned) {
		vessel->ClearMeshes();
		AddCapsule(vessel);
		if (!bDoorsJettisoned) {
			AddCommander(vessel);
			AddDoorLeft(vessel);
			AddDoorRight(vessel);
			AddPilot(vessel);
			AddPilotSeat(vessel);
		}
		AddNose(vessel);
	} else if (bMainDeployed && !bMainJettisoned) {
		SetMainChuteStage(vessel);
	}else if (bDrogueDeployed && bDrogueJettisoned) {
		vessel->ClearMeshes();
		AddCapsule(vessel);
		if (!bDoorsJettisoned) {
			AddCommander(vessel);
			AddDoorLeft(vessel);
			AddDoorRight(vessel);
			AddPilot(vessel);
			AddPilotSeat(vessel);
		}
	}else if (bDrogueDeployed && !bDrogueJettisoned) {
		vessel->ClearMeshes();
		AddCapsule(vessel);
		if (!bDoorsJettisoned) {
			AddCommander(vessel);
			AddDoorLeft(vessel);
			AddDoorRight(vessel);
			AddPilot(vessel);
			AddPilotSeat(vessel);
		}
		AddNose(vessel);
		AddDrogue(vessel);
		vessel->SetCrossSections (_V(2.8,2.8,GEMINI_CHUTE_DROGUE));
		vessel->SetCW (0.1, 1.5, .3,.3);
	} else {
		vessel->ClearMeshes();
		AddCapsule(vessel);
		if (!bDoorsJettisoned) {
			AddCommander(vessel);
			AddDoorLeft(vessel);
			AddDoorRight(vessel);
			AddPilot(vessel);
			AddPilotSeat(vessel);
		}
		AddNose(vessel);
	}
};

void Gemini::SetMainChuteStage(VESSEL2 *vessel)
{
//	vessel->ClearAirfoilDefinitions();
	vessel->SetSize (2.0);
	vessel->SetCOG_elev (2.0);
	vessel->SetCameraOffset(OFS_CAMERA_CDR);
	vessel->SetEmptyMass  (1893 *KG - GEMINI_FUEL_MASS_RCS);
	vessel->SetPMI (_V(3.5,3.5,0.85));
	vessel->SetCrossSections (_V(4.5,4.5,8.5));
	vessel->SetCW (0.1, .75, 0.3, .3);
	vessel->SetRotDrag (_V(0.7,0.7,1.2));
	vessel->SetPitchMomentScale (-1e-3);
	vessel->SetBankMomentScale (-1e-3);
	vessel->SetLiftCoeffFunc (LiftCoeff); 
	vessel->SetTouchdownPoints (_V(0,-2,2), _V(.5,-2,-2), _V(-.5,-2,-2));
	vessel->ClearMeshes();
	AddCapsule(vessel);
	if (!bDoorsJettisoned) {
		AddCommander(vessel);
		AddDoorLeft(vessel);
		AddDoorRight(vessel);
		AddPilot(vessel);
		AddPilotSeat(vessel);
	}
};

void Gemini::Message(VESSEL2 *vessel, int msg, double value)
{
	if (msg == 100) {
		vessel->ClearMeshes();
		AddGemini(vessel);
	}
}
void Gemini::SetStage(VESSEL2 *vessel)
{
	vessel->SetSize (2.0);
	vessel->SetEmptyMass (GEMINI_EMPTY_MASS);
	vessel->SetCOG_elev (4.0);
	vessel->SetCOG_elev (-10.0);
	vessel->SetDockParams(_V(0,0,3.149+.85),_V(0,0,1),_V(0,1,0));
	vessel->SetPMI (_V(3.5,3.5,0.85));
	vessel->SetCrossSections (_V(12.5,12.5,8.5));
	vessel->SetCW (.1, 1, .3, .3);
	vessel->SetRotDrag (_V(0.5,0.5,0.2));
	vessel->SetPitchMomentScale (0);
	vessel->SetBankMomentScale (0);
	vessel->SetLiftCoeffFunc (0); 
	vessel->SetCameraOffset(OFS_CAMERA_CDR);
	CreateGeminiThrusters(vessel);
	if (bAdapterJettisoned) {
		SetRetroStage(vessel);
		SetCapsuleStage(vessel);
	} else if (bEquipmentJettisoned) {
		SetRetroStage(vessel);
	} else {
		vessel->ClearMeshes();
		AddCapsule(vessel);
		AddCommander(vessel);
		AddDoorLeft(vessel);
		AddDoorRight(vessel);
		if (bGemini_EVA)
			AddPilotEVA(vessel);
		else
			AddPilot(vessel);
		AddPilotSeat(vessel);
		AddNose(vessel);
		AddAdapter(vessel);
		AddEquipment(vessel);
	}
};

void Gemini::CreateGeminiThrusters(VESSEL2 *vessel)
{

	VECTOR3 att_ref;
	VECTOR3 att_dir;

	
	
	const double R_ATTCOOR = .25;     //RCS Exhaust lateral distance from centerline
	const double R_ATTCOOR_PORT = .4; //RCS distance from centerline
	const double R_ATTZ = 2.21;//RCS Z position
	const double R_ATTWIDTH=.03;//Flame width
	const double R_ATTHEIGHT=.7;//Flame length
	const double R_RCSOFFSET=.13;//Distance between thrusters

	const double O_ATTCOOR = 1.04;//OAMS distance from centerline
	const double O_ATTZ = -2.1;//OAMS Z position
	const double O_TRANCOOR = 1.15;//Translation thruster distance from centerline
	const double O_TRANZ= 0;//Translation thruster Z position
	const double O_ATTWIDTH= .04;//Flame width
	const double O_ATTHEIGHT= .7;//Flame length
	const double O_TRANWIDTH=.05;
	const double O_TRANHEIGHT=1;
	

	vessel->ClearPropellantResources();
	vessel->ClearThrusterDefinitions();

	fuel_oams = vessel->CreatePropellantResource(GEMINI_FUEL_MASS_OAMS, GEMINI_FUEL_MASS_OAMS);
	fuel_rcs = vessel->CreatePropellantResource(GEMINI_FUEL_MASS_OAMS, GEMINI_FUEL_MASS_OAMS);
	for (int i = 0; i < 4; i++)
		fuel_retro[i] = vessel->CreatePropellantResource(GEMINI_FUEL_MASS_RETRO, GEMINI_FUEL_MASS_RETRO);

	att_ref= _V(-O_ATTCOOR,O_ATTCOOR,O_ATTZ);
	att_dir = _V(0,-1,0);
	thruster_oams[0] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[0], O_ATTHEIGHT,O_ATTWIDTH);

	thruster_oams[16] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[16], O_ATTHEIGHT,O_ATTWIDTH);


	att_ref= _V(O_ATTCOOR,O_ATTCOOR,O_ATTZ);
	att_dir = _V(0,-1,0);
	thruster_oams[1] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[1], O_ATTHEIGHT,O_ATTWIDTH);
	thruster_oams[17] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[17], O_ATTHEIGHT,O_ATTWIDTH);

	att_ref= _V(-O_ATTCOOR,O_ATTCOOR,O_ATTZ);
	att_dir = _V(1,0,0);
	thruster_oams[2] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[2], O_ATTHEIGHT,O_ATTWIDTH);

	att_ref= _V(-O_ATTCOOR,-O_ATTCOOR,O_ATTZ);
	att_dir = _V(1,0,0);
	thruster_oams[3] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[3], O_ATTHEIGHT,O_ATTWIDTH);

	att_ref= _V(-O_ATTCOOR,-O_ATTCOOR,O_ATTZ);
	att_dir = _V(0,1,0);
	thruster_oams[4] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[4], O_ATTHEIGHT,O_ATTWIDTH);
	thruster_oams[18] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[18], O_ATTHEIGHT,O_ATTWIDTH);

	att_ref= _V(O_ATTCOOR,-O_ATTCOOR,O_ATTZ);
	att_dir = _V(0,1,0);
	thruster_oams[5] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[5], O_ATTHEIGHT,O_ATTWIDTH);
	thruster_oams[19] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[19], O_ATTHEIGHT,O_ATTWIDTH);

	att_ref= _V(O_ATTCOOR,O_ATTCOOR,O_ATTZ);
	att_dir = _V(-1,0,0);
	thruster_oams[6] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[6], O_ATTHEIGHT,O_ATTWIDTH);

	att_ref= _V(O_ATTCOOR,-O_ATTCOOR,O_ATTZ);
	att_dir = _V(-1,0,0);
	thruster_oams[7] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[7], O_ATTHEIGHT,O_ATTWIDTH);

	//Mains
	att_dir = _V(0,0,1);
	att_ref= _V(0,1.42,-2.32);
	thruster_oams[8] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_MAN, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[8], O_TRANHEIGHT,O_TRANWIDTH);

	att_ref= _V(0,-1.42,-2.32);
	thruster_oams[9] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_MAN, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[9], O_TRANHEIGHT,O_TRANWIDTH);
	
	//Retros
	att_dir = _V(0,0,-1);
	att_ref= _V(1.25,-.15,-.15);
	thruster_oams[10] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_RETRO, fuel_oams, GEMINI_ISP, 0.0);
	att_dir = _V(0,0,1);
	att_ref= _V(1.25,-.10,-.15);
	vessel->AddExhaust(thruster_oams[10], O_TRANHEIGHT,O_TRANWIDTH, att_ref, att_dir);

	att_dir = _V(0,0,-1);
	att_ref= _V(-1.25,.15,-.15);
	thruster_oams[11] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_RETRO, fuel_oams, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_oams[11], O_TRANHEIGHT,O_TRANWIDTH);

	//tran up
	att_ref= _V(0,-O_TRANCOOR,O_TRANZ);
	att_dir = _V(0,1,0);
	thruster_oams[12] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_MAN, fuel_oams, GEMINI_ISP, 0.0);
	att_ref= _V(0,-1.15,-0.37);
	att_dir = _V(0,-1,0);
	vessel->AddExhaust(thruster_oams[12], O_TRANHEIGHT,O_TRANWIDTH, att_ref, att_dir);

	//tran down
	att_ref= _V(0,O_TRANCOOR,O_TRANZ);
	att_dir = _V(0,-1,0);
	thruster_oams[13] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_MAN, fuel_oams, GEMINI_ISP, 0.0);
	att_ref= _V(0,1.15,-0.37);
	att_dir = _V(0,1,0);
	vessel->AddExhaust(thruster_oams[13], O_TRANHEIGHT,O_TRANWIDTH, att_ref, att_dir);

	//tran left
	att_ref= _V(O_TRANCOOR,0,O_TRANZ);
	att_dir = _V(-1,0,0);
	thruster_oams[14] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_MAN, fuel_oams, GEMINI_ISP, 0.0);
	att_ref= _V(1.15,0.035,-0.37);
	att_dir = _V(1,0,0);
	vessel->AddExhaust(thruster_oams[14], O_TRANHEIGHT,O_TRANWIDTH, att_ref, att_dir);

	//tran right
	att_ref= _V(-O_TRANCOOR,0,O_TRANZ);
	att_dir = _V(1,0,0);
	thruster_oams[15] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_MAN, fuel_oams, GEMINI_ISP, 0.0);
	att_ref= _V(-1.15,0,-0.37);
	att_dir = _V(-1,0,0);
	vessel->AddExhaust(thruster_oams[15], O_TRANHEIGHT,O_TRANWIDTH, att_ref, att_dir);


//RCS
	//up
	att_ref= _V(-R_ATTCOOR,-R_ATTCOOR_PORT,R_ATTZ);
	att_dir = _V(0,1,0);
	thruster_rcs[0] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[0], R_ATTHEIGHT,R_ATTWIDTH);
	thruster_rcs[24] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[24], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(R_ATTCOOR,-R_ATTCOOR_PORT,R_ATTZ);
	att_dir = _V(0,1,0);
	thruster_rcs[1] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[1], R_ATTHEIGHT,R_ATTWIDTH);
	thruster_rcs[28] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[28], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(-R_ATTCOOR,-R_ATTCOOR_PORT,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(0,1,0);
	thruster_rcs[2] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[2], R_ATTHEIGHT,R_ATTWIDTH);
	thruster_rcs[26] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[26], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(R_ATTCOOR,-R_ATTCOOR_PORT,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(0,1,0);
	thruster_rcs[3] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[3], R_ATTHEIGHT,R_ATTWIDTH);
	thruster_rcs[30] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[30], R_ATTHEIGHT,R_ATTWIDTH);


	//dn
	att_ref= _V(-R_ATTCOOR,R_ATTCOOR_PORT,R_ATTZ);
	att_dir = _V(0,-1,0);
	thruster_rcs[4] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[4], R_ATTHEIGHT,R_ATTWIDTH);
	thruster_rcs[29] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[29], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(R_ATTCOOR,R_ATTCOOR_PORT,R_ATTZ);
	att_dir = _V(0,-1,0);
	thruster_rcs[5] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[5], R_ATTHEIGHT,R_ATTWIDTH);
	thruster_rcs[25] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[25], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(-R_ATTCOOR, R_ATTCOOR_PORT,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(0,-1,0);
	thruster_rcs[6] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[6], R_ATTHEIGHT,R_ATTWIDTH);
	thruster_rcs[31] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[31], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(R_ATTCOOR, R_ATTCOOR_PORT,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(0,-1,0);
	thruster_rcs[7] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[7], R_ATTHEIGHT,R_ATTWIDTH);
	thruster_rcs[27] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[27], R_ATTHEIGHT,R_ATTWIDTH);


	//right
	att_ref= _V(-R_ATTCOOR_PORT,R_ATTCOOR,R_ATTZ);
	att_dir = _V(1,0,0);
	thruster_rcs[8] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[8], R_ATTHEIGHT,R_ATTWIDTH);
	
	att_ref= _V(-R_ATTCOOR_PORT,-R_ATTCOOR,R_ATTZ);
	att_dir = _V(1,0,0);
	thruster_rcs[9] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[9], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(-R_ATTCOOR_PORT,R_ATTCOOR,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(1,0,0);
	thruster_rcs[10] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[10], R_ATTHEIGHT,R_ATTWIDTH);
	
	att_ref= _V(-R_ATTCOOR_PORT,-R_ATTCOOR,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(1,0,0);
	thruster_rcs[11] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[11], R_ATTHEIGHT,R_ATTWIDTH);

	//left
	att_ref= _V(R_ATTCOOR_PORT,R_ATTCOOR,R_ATTZ);
	att_dir = _V(-1,0,0);
	thruster_rcs[12] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[12], R_ATTHEIGHT,R_ATTWIDTH);
	
	att_ref= _V(R_ATTCOOR_PORT,-R_ATTCOOR,R_ATTZ);
	att_dir = _V(-1,0,0);
	thruster_rcs[13] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[13], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(R_ATTCOOR_PORT,R_ATTCOOR,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(-1,0,0);
	thruster_rcs[14] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[14], R_ATTHEIGHT,R_ATTWIDTH);
	
	att_ref= _V(R_ATTCOOR_PORT,-R_ATTCOOR,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(-1,0,0);
	thruster_rcs[15] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[15], R_ATTHEIGHT,R_ATTWIDTH);
	
	//rot right
	att_ref= _V(R_ATTCOOR,R_ATTCOOR_PORT,R_ATTZ);
	att_dir = _V(0,-1,0);
	thruster_rcs[16] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[16], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(-R_ATTCOOR,-R_ATTCOOR_PORT,R_ATTZ);
	att_dir = _V(0,1,0);
	thruster_rcs[17] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[17], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(R_ATTCOOR,R_ATTCOOR_PORT,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(0,-1,0);
	thruster_rcs[18] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[18], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(-R_ATTCOOR,-R_ATTCOOR_PORT,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(0,1,0);
	thruster_rcs[19] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[19], R_ATTHEIGHT,R_ATTWIDTH);

	//rot left
	att_ref= _V(-R_ATTCOOR,R_ATTCOOR,R_ATTZ);
	att_dir = _V(0,-1,0);
	thruster_rcs[20] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[20], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(R_ATTCOOR,-R_ATTCOOR,R_ATTZ);
	att_dir = _V(0,1,0);
	thruster_rcs[21] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[21], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(-R_ATTCOOR,R_ATTCOOR,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(0,-1,0);
	thruster_rcs[22] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[22], R_ATTHEIGHT,R_ATTWIDTH);

	att_ref= _V(R_ATTCOOR,-R_ATTCOOR,R_ATTZ-R_RCSOFFSET);
	att_dir = _V(0,1,0);
	thruster_rcs[23] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_ATT, fuel_rcs, GEMINI_ISP, 0.0);
	vessel->AddExhaust(thruster_rcs[23], R_ATTHEIGHT,R_ATTWIDTH);

	//Retro 

	att_ref= _V(-0.33,0.33,OFS_GEM_ADAPTER.z -.42);
	att_dir = _V(0.703,-0.703,1.8);
	thruster_retro[0] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_RETRO_ROCKET, fuel_retro[0], GEMINI_ISP_RETRO, 0.0);
	vessel->AddExhaust(thruster_retro[0], 1.0,.075);

	att_ref= _V(0.33,0.33,OFS_GEM_ADAPTER.z -.42);
	att_dir = _V(-0.703,-0.703,1.8);
	thruster_retro[1] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_RETRO_ROCKET, fuel_retro[1], GEMINI_ISP_RETRO, 0.0);
	vessel->AddExhaust(thruster_retro[1], 1.0,.075);

	att_ref= _V(0.33,-0.33,OFS_GEM_ADAPTER.z -.42);
	att_dir = _V(-0.703,0.703,1.8);
	thruster_retro[2] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_RETRO_ROCKET, fuel_retro[2], GEMINI_ISP_RETRO, 0.0);
	vessel->AddExhaust(thruster_retro[2], 1.0,.075);

	att_ref= _V(-0.33,-0.33,OFS_GEM_ADAPTER.z -.42);
	att_dir = _V(0.703,0.703,1.8);
	thruster_retro[3] = vessel->CreateThruster( att_ref, att_dir, GEMINI_THRUST_RETRO_ROCKET, fuel_retro[3], GEMINI_ISP_RETRO, 0.0);
	vessel->AddExhaust(thruster_retro[3], 1.0,.075);

	SetRCS(vessel);
	SetOAMS(vessel);

}

void Gemini::SetRCS(VESSEL2 *vessel) {


	THRUSTER_HANDLE th_group[4];
	int nThrust = 0;


	th_group[0] = thruster_rcs[0];
	th_group[1] = thruster_rcs[1];
	nThrust = 2;
	rcs_pitch_up1 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);
	th_group[0] = thruster_rcs[2];
	th_group[1] = thruster_rcs[3];
	nThrust = 2;
	rcs_pitch_up2 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);
	
	th_group[0] = thruster_rcs[4];
	th_group[1] = thruster_rcs[5];
	nThrust = 2;
	rcs_pitch_down1 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);

	th_group[0] = thruster_rcs[6];
	th_group[1] = thruster_rcs[7];
	nThrust = 2;
	rcs_pitch_down2 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);

	th_group[0] = thruster_rcs[8];
	th_group[1] = thruster_rcs[9];
	nThrust = 2;
	rcs_yaw_right1 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);

	th_group[0] = thruster_rcs[10];
	th_group[1] = thruster_rcs[11];
	nThrust = 2;
	rcs_yaw_right2 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);
	
	th_group[0] = thruster_rcs[12];
	th_group[1] = thruster_rcs[13];
	nThrust = 2;
	rcs_yaw_left1 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);

	th_group[0] = thruster_rcs[14];
	th_group[1] = thruster_rcs[15];
	nThrust = 2;
	rcs_yaw_left2 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);
	
	th_group[0] = thruster_rcs[24];
	th_group[1] = thruster_rcs[25];
	nThrust = 2;
	rcs_roll_right1 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);

	th_group[0] = thruster_rcs[26];
	th_group[1] = thruster_rcs[27];
	nThrust = 2;
	rcs_roll_right2 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);
	
	th_group[0] = thruster_rcs[28];
	th_group[1] = thruster_rcs[29];
	nThrust = 2;
	rcs_roll_left1 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);

	th_group[0] = thruster_rcs[30];
	th_group[1] = thruster_rcs[31];
	nThrust = 2;
	rcs_roll_left2 = vessel->CreateThrusterGroup(th_group,nThrust,THGROUP_USER);

}

void Gemini::SetOAMS(VESSEL2 *vessel)
{
	THRUSTER_HANDLE th_group[4];
	

	th_group[0] = thruster_oams[2];
	th_group[1] = thruster_oams[3];
	oams_yaw_left = vessel->CreateThrusterGroup(th_group,2,THGROUP_USER);
	
	th_group[0] = thruster_oams[6];
	th_group[1] = thruster_oams[7];
	oams_yaw_right = vessel->CreateThrusterGroup(th_group,2,THGROUP_USER);
	
	th_group[0] = thruster_oams[0];
	th_group[1] = thruster_oams[5];
	oams_roll_left = vessel->CreateThrusterGroup(th_group,2,THGROUP_USER);
	
	th_group[0] = thruster_oams[4];
	th_group[1] = thruster_oams[1];
	oams_roll_right = vessel->CreateThrusterGroup(th_group,2,THGROUP_USER);

	
	th_group[0] = thruster_oams[16];
	th_group[1] = thruster_oams[17];
	oams_pitch_up = vessel->CreateThrusterGroup(th_group,2,THGROUP_USER);
	
	th_group[0] = thruster_oams[18];
	th_group[1] = thruster_oams[19];
	oams_pitch_down = vessel->CreateThrusterGroup(th_group,2,THGROUP_USER);


	th_group[0] = thruster_oams[10];
	th_group[1] = thruster_oams[11];
	oams_back = vessel->CreateThrusterGroup(th_group,2,THGROUP_USER);
	
	th_group[0] = thruster_oams[8];
	th_group[1] = thruster_oams[9];
	oams_forward  = vessel->CreateThrusterGroup(th_group,2,THGROUP_USER);
	
	th_group[0] = thruster_oams[14];
	oams_left = vessel->CreateThrusterGroup(th_group,1,THGROUP_USER);
	
	th_group[0] = thruster_oams[15];
	oams_right = vessel->CreateThrusterGroup(th_group,1,THGROUP_USER);
	
	th_group[0] = thruster_oams[12];
	oams_up = vessel->CreateThrusterGroup(th_group,1,THGROUP_USER);
	 
	th_group[0] = thruster_oams[13];
	oams_down = vessel->CreateThrusterGroup(th_group,1,THGROUP_USER);

}

void Gemini::JettisonEquipment(VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,-0.5);
	ofs = OFS_GEM_EQUIPMENT;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	vessel->SetEngineLevel(ENGINE_MAIN, 0);
	StageTransform(vessel,&vs,ofs,vel);
	vs.status=0;
	vs.vrot.x = 0.0;
	vs.vrot.y = 0.0;
	vs.vrot.z = 0.0;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-equip");
	VESSEL::Create (name, "gemini_equip", vs);
}


void Gemini::JettisonAdapter(VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,-0.5);
	ofs = OFS_GEM_ADAPTER;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	vessel->SetEngineLevel(ENGINE_MAIN, 0);
	StageTransform(vessel,&vs,ofs,vel);
	vs.status=0;
	vs.vrot.x = 0.0;
	vs.vrot.y = 0.0;
	vs.vrot.z = 0.0;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-adapter");
	VESSEL::Create (name, "gemini_adapt", vs);
};

void Gemini::JettisonDrogue(VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,0);
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	ofs = OFS_GEM_NOSE;		
	vel = _V(0,1,0);	
	StageTransform(vessel,&vs,ofs,vel);
	vs.status=0;
	vs.vrot.x = 0.0;	
	vs.vrot.y = 0.0;	
	vs.vrot.z = 0.0;	
	vs.eng_main=1.0;
	vs.fuel=1.0;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-Drogue");
	VESSEL::Create (name, "Gemini_Drogue", vs);
};

void Gemini::JettisonNose(VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,0);
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	ofs = OFS_GEM_NOSE;		
	vel = _V(0,1,0);	
	StageTransform(vessel,&vs,ofs,vel);
	vs.status=0;
	vs.vrot.x = 0.0;	
	vs.vrot.y = 0.0;	
	vs.vrot.z = 0.0;	
	vs.eng_main=1.0;
	vs.fuel=1.0;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-Nose");
	VESSEL::Create (name, "Gemini_Nose", vs);
};

void Gemini::JettisonMainChute(VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,0);
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	ofs = OFS_MAINCHUTE;		
	vel = _V(0,1,0);	
	StageTransform(vessel,&vs,ofs,vel);
	vs.status=0;
	vs.vrot.x = 0.0;	
	vs.vrot.y = 0.0;	
	vs.vrot.z = 0.0;	
	vs.eng_main=1.0;
	vs.fuel=1.0;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-Main");
	VESSEL::Create (name, "Gemini_Chute", vs);
};


void Gemini::OperatePilotDoor (VESSEL2 *vessel)
{
	if ((door_status == DOOR_OPENING || door_status == DOOR_CLOSING) && !door_moving) {
		door_moving = true;
		vessel->RegisterAnimation();
	}
}

void Gemini::RevertPilotDoor (VESSEL2 *vessel)
{
	door_status = (door_status == DOOR_CLOSED || door_status == DOOR_CLOSING ?
		DOOR_OPENING : DOOR_CLOSING);
	OperatePilotDoor(vessel);
}

void Gemini::UpdateMesh (VESSEL2 *vessel)
{
	int grp_ofs;
	double delta = 0;
	float da = 0;
	double ang = 0;
	int i = 0;
	VECTOR3 newoffset = _V(0,0,0);
	VECTOR3 offoffset = OFS_CAMERA_CDR;
	MESHGROUP_TRANSFORM mt;

	if (delta = door_proc-vdoor_proc) { // need to bring pilot door up to date
		grp_ofs = 0;
		float da = (float)(-delta*DOOR_MAX_ANGLE);
		
		mt.nmesh = 8;//4;
		mt.transform = MESHGROUP_TRANSFORM::ROTATE;
		mt.P.rotparam.axis = _V(-0.249,0.278,-1.081);//_V(-.299,-.135,.945);
		mt.P.rotparam.angle = da;
		mt.P.rotparam.ref.x = 0.602185;
		mt.P.rotparam.ref.y = -0.00364;
		mt.P.rotparam.ref.z = -0.849945;

		mt.ngrp = grp_ofs; 

		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);


		mt.ngrp = grp_ofs+1; 
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		mt.ngrp = grp_ofs+2; 
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		mt.ngrp = grp_ofs+3; 
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		mt.ngrp = grp_ofs+4; 
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		mt.ngrp = grp_ofs+5; 
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

//VC===========================================================
		mt.nmesh = 9;//4;
		mt.transform = MESHGROUP_TRANSFORM::ROTATE;
		mt.P.rotparam.axis = _V(-0.249,0.278,-1.081);//_V(-.299,-.135,.945);
		mt.P.rotparam.angle = da;
		mt.P.rotparam.ref.x = 0.602185;
		mt.P.rotparam.ref.y = -0.00364;
		mt.P.rotparam.ref.z = -0.849945;

		mt.ngrp = grp_ofs; 

		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);


		mt.ngrp = grp_ofs+1; 
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		mt.ngrp = grp_ofs+2; 
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		mt.ngrp = grp_ofs+3; 
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		mt.ngrp = grp_ofs+4; 
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		mt.ngrp = grp_ofs+5; 
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float)-.320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float)-.280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,0,1);
		mt.P.rotparam.angle = da;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = (float).280;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		mt.P.rotparam.axis = _V(0,1,0);
		mt.P.rotparam.angle = (float).320;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);


//VC===========================================================
		vdoor_proc = door_proc;
	}
	if (delta = line1_proc-vline1_proc) { 
		

		da = (float)(delta*LINE1_MAX_ANGLE);
		mt.nmesh = 14;//7;
		mt.ngrp  = 0;
		mt.transform = MESHGROUP_TRANSFORM::ROTATE;
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = da;
		mt.P.rotparam.ref.x = 0;
		mt.P.rotparam.ref.y = 0;
		mt.P.rotparam.ref.z = 1.18;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		da = (float)(-delta*LINE2_MAX_ANGLE);
		mt.nmesh = 15;//8;
		mt.ngrp  = 0;
		mt.transform = MESHGROUP_TRANSFORM::ROTATE;
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = da;
		mt.P.rotparam.ref.x = 0;
		mt.P.rotparam.ref.y = 0;
		mt.P.rotparam.ref.z = 1.18;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		ang = line1_proc * CAPSULE_MAX_ANGLE;
		vessel->SetCameraDefaultDirection(_V(0,sin(-ang),cos(-ang)));
		newoffset.x = OFS_CAMERA_CDR.x;
		newoffset.y = (1.0-OFS_CAMERA_CDR.y) * cos(-ang) - (OFS_CAMERA_CDR.z+0.09) * sin(-ang);
		newoffset.z = (1.0-OFS_CAMERA_CDR.y) * sin(-ang) + (OFS_CAMERA_CDR.z+0.09) * cos(-ang);
		newoffset.y = 1 - newoffset.y ;
		newoffset.z = -0.09+newoffset.z;
		vessel->SetCameraOffset(newoffset);

		da = (float)(delta*CAPSULE_MAX_ANGLE);
		mt.nmesh = 0;
		mt.transform = MESHGROUP_TRANSFORM::ROTATE;
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = da;
		mt.P.rotparam.ref.x = 0;
		mt.P.rotparam.ref.y = 1.0;
		mt.P.rotparam.ref.z = -1.31;
		for (i=0; i < 24; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}

		da = (float)(delta*CAPSULE_MAX_ANGLE);
		mt.nmesh = 2;
		mt.transform = MESHGROUP_TRANSFORM::ROTATE;
		mt.P.rotparam.axis = _V(1,0,0);
		mt.P.rotparam.angle = da;
		mt.P.rotparam.ref.x = 0;
		mt.P.rotparam.ref.y = 1.0;
		mt.P.rotparam.ref.z = -1.31;
		for (i=0; i < 46; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}

		mt.nmesh = 1;
		mt.ngrp = 0;
		mt.P.rotparam.ref.x = 0-OFS_GEM_HEATSHIELD.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_GEM_HEATSHIELD.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_GEM_HEATSHIELD.z;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);

		mt.nmesh = 3;
		mt.ngrp = 0;
		mt.P.rotparam.ref.x = 0-OFS_GEM_HEATSHIELD.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_GEM_HEATSHIELD.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_GEM_HEATSHIELD.z;
		vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);


		mt.nmesh = 12;//6;
		mt.P.rotparam.ref.x = 0-OFS_SEAT.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_SEAT.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_SEAT.z;
		for (i=0; i < 1; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}

		mt.nmesh = 13;//6;
		mt.P.rotparam.ref.x = 0-OFS_SEAT.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_SEAT.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_SEAT.z;
		for (i=0; i < 1; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}


		mt.nmesh = 6;//3;
		mt.P.rotparam.ref.x = 0-OFS_GEM_DOOR_LEFT.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_GEM_DOOR_LEFT.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_GEM_DOOR_LEFT.z;
		for (i=0; i < 6; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}
		
		mt.nmesh = 8;//4;
		mt.P.rotparam.ref.x = 0-OFS_GEM_DOOR_RIGHT.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_GEM_DOOR_RIGHT.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_GEM_DOOR_RIGHT.z;
		for (i=0; i < 6; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}

		mt.nmesh = 7;//3;
		mt.P.rotparam.ref.x = 0-OFS_GEM_DOOR_LEFT.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_GEM_DOOR_LEFT.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_GEM_DOOR_LEFT.z;
		for (i=0; i < 6; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}
		
		mt.nmesh = 9;//4;
		mt.P.rotparam.ref.x = 0-OFS_GEM_DOOR_RIGHT.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_GEM_DOOR_RIGHT.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_GEM_DOOR_RIGHT.z;
		for (i=0; i < 6; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}


		mt.nmesh = 4;//2;
		mt.P.rotparam.ref.x = 0-OFS_GEM_CDR.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_GEM_CDR.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_GEM_CDR.z;
		for (i=0; i < 26; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}

		mt.nmesh = 10;//5;
		mt.P.rotparam.ref.x = 0-OFS_GEM_PLT.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_GEM_PLT.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_GEM_PLT.z;
		for (i=0; i < 26; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}

		mt.nmesh = 5;//2;
		mt.P.rotparam.ref.x = 0-OFS_GEM_CDR.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_GEM_CDR.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_GEM_CDR.z;
		for (i=0; i < 2; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}

		mt.nmesh = 11;//5;
		mt.P.rotparam.ref.x = 0-OFS_GEM_PLT.x;
		mt.P.rotparam.ref.y = 1.0 - OFS_GEM_PLT.y;
		mt.P.rotparam.ref.z = (OFS_GEM_CAPSULE.z + -1.31) - OFS_GEM_PLT.z;
		for (i=0; i < 2; i++)
		{
			mt.ngrp=i;
			vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
		}

		vline1_proc = line1_proc;

	}
	if (delta = chute_proc-vchute_proc) { 
		if (chute_status == CHUTE_OPENING) {
			if (vchute_proc > 0) {
				MESHGROUP_TRANSFORM mt;
				mt.nmesh = 16;//9;
				mt.ngrp = -1;
				mt.transform = MESHGROUP_TRANSFORM::SCALE;
				mt.P.scaleparam.scale.x	=1;
				mt.P.scaleparam.scale.y	=1;
				mt.P.scaleparam.scale.z	=(1 / vchute_proc);
				vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
			}
			if (chute_proc > 0) {
				MESHGROUP_TRANSFORM mt;
				mt.nmesh = 16;//9;
				mt.ngrp = -1;
				mt.transform = MESHGROUP_TRANSFORM::SCALE;
				mt.P.scaleparam.scale.x	=1;
				mt.P.scaleparam.scale.y	=1;
				mt.P.scaleparam.scale.z	=chute_proc;
				vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
			}
		} else if (chute_status == CHUTE_UNREEFING) {
			if (vchute_proc > 0) {
				MESHGROUP_TRANSFORM mt;
				mt.nmesh = 16;//9;
				mt.ngrp = -1;
				mt.transform = MESHGROUP_TRANSFORM::SCALE;
				mt.P.scaleparam.scale.x	=(1 / vchute_proc);
				mt.P.scaleparam.scale.y	=(1 / vchute_proc);
				mt.P.scaleparam.scale.z	=1;
				vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
			}
			if (chute_proc > 0) {
				MESHGROUP_TRANSFORM mt;
				mt.nmesh = 16;//9;
				mt.ngrp = -1;
				mt.transform = MESHGROUP_TRANSFORM::SCALE;
				mt.P.scaleparam.scale.x	=chute_proc;
				mt.P.scaleparam.scale.y	=chute_proc;
				mt.P.scaleparam.scale.z	=1;
				vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
			}
		}
		vchute_proc = chute_proc;
	}

	if (delta = drogue_proc-vdrogue_proc) { 
		if (drogue_status == DROGUE_OPENING) {
			if (vdrogue_proc > 0) {
				MESHGROUP_TRANSFORM mt;
				mt.nmesh = 16;//8;
				mt.ngrp = -1;
				mt.transform = MESHGROUP_TRANSFORM::SCALE;
				mt.P.scaleparam.scale.x	=1;
				mt.P.scaleparam.scale.y	=1;
				mt.P.scaleparam.scale.z	=(1 / vdrogue_proc);
				vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
			}
			if (drogue_proc > 0) {
				MESHGROUP_TRANSFORM mt;
				mt.nmesh = 16;//8;
				mt.ngrp = -1;
				mt.transform = MESHGROUP_TRANSFORM::SCALE;
				mt.P.scaleparam.scale.x	=1;
				mt.P.scaleparam.scale.y	=1;
				mt.P.scaleparam.scale.z	=drogue_proc;
				vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
			}
		} else if (drogue_status == DROGUE_UNREEFING) {
			if (vdrogue_proc > 0) {
				MESHGROUP_TRANSFORM mt;
				mt.nmesh = 16;//8;
				mt.ngrp = -1;
				mt.transform = MESHGROUP_TRANSFORM::SCALE;
				mt.P.scaleparam.scale.x	=(1 / vdrogue_proc);
				mt.P.scaleparam.scale.y	=(1 / vdrogue_proc);
				mt.P.scaleparam.scale.z	=1;
				vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
			}
			if (drogue_proc > 0) {
				MESHGROUP_TRANSFORM mt;
				mt.nmesh = 16;//8;
				mt.ngrp = -1;
				mt.transform = MESHGROUP_TRANSFORM::SCALE;
				mt.P.scaleparam.scale.x	=drogue_proc;
				mt.P.scaleparam.scale.y	=drogue_proc;
				mt.P.scaleparam.scale.z	=1;
				vessel->MeshgroupTransform (((Multi_Stage*)vessel)->hVis, mt);
			}
		}
		vdrogue_proc = drogue_proc;
	}}


#endif