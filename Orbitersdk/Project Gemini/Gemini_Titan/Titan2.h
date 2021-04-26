#ifndef __TITAN2_H
#define __TITAN2_H
#include "..\..\include\orbitersdk.h"
#include "..\..\include\multi_stage4.h"
#include "gemini_panel.h"
#include "gemini.h"



const double STAGE2_LENGTH       = 9.54;//9.3;
const double STAGE2_LEN_ENGINE   = 1.41;
const VECTOR3 STAGE2_OFS         = { 0.0     , 0.0     ,  GEMINI_OFS.z - GEMINI_LENGTH / 2 - STAGE2_LENGTH / 2 +.2};
const VECTOR3 STAGE2_OFS_FLAME   = { 0.0     , 0.0     ,  STAGE2_OFS.z-STAGE2_LENGTH/2};
const VECTOR3 STAGE2_OFS_ROLL    = { -0.75     , 0.0     ,  STAGE2_OFS.z-STAGE2_LENGTH/2 + 1.15};
const VECTOR3 STAGE2_FLAME_DIR   = { 0.0, 0.0, 1.0};
const double STAGE2_MASS         = 28939*KG;
const double STAGE2_MASS_EMPTY   = 2404*KG;
const double STAGE2_MASS_FUEL    = STAGE2_MASS-STAGE2_MASS_EMPTY;
const double STAGE2_ISP_SL       = 160*SEC;
const double STAGE2_ISP_VAC      = 316*SEC;
const double STAGE2_THRUST_VAC   = 45359*KGF;
const double STAGE2_THRUST_ROLL  = 215*KGF;

const double MID_LENGTH          = 3.17;
const VECTOR3 MID_OFS            = { 0.     , 0.0     , STAGE2_OFS.z - STAGE2_LENGTH / 2 + STAGE2_LEN_ENGINE - MID_LENGTH / 2};

const double  STAGE1_LENGTH      = 22.00;
const VECTOR3 STAGE1_OFS         = { 0.0     , 0.0     , MID_OFS.z - MID_LENGTH / 2 - STAGE1_LENGTH / 2 +.1};
const VECTOR3 STAGE1_OFS_FLAME   = { 0.0     , 0.0      , STAGE1_OFS.z-STAGE1_LENGTH/2};
const VECTOR3 STAGE1_OFS1_FLAME   = { 0.7     , 0.0     , STAGE1_OFS.z-STAGE1_LENGTH/2 + .6};
const VECTOR3 STAGE1_OFS2_FLAME   = { -0.7     , 0.0     , STAGE1_OFS.z-STAGE1_LENGTH/2 + .6};
const VECTOR3 STAGE1_FLAME_DIR   = { 0.0, 0.0, 1.0};
const double  STAGE1_MASS        = 122470*KG;//117866*KG;
const double  STAGE1_MASS_EMPTY  = 4241*KG;//6736*KG;
const double  STAGE1_MASS_FUEL   = STAGE1_MASS-STAGE1_MASS_EMPTY;
//const double  STAGE1_ISP_SL      = 274*SEC;
const double  STAGE1_ISP_SL      = 258*SEC;//284*SEC;
const double  STAGE1_ISP_VAC     = 296*SEC;//311*SEC;
const double  STAGE1_THRUST_VAC  = 110753*KGF;

bool bAutoPilot = true;

void AddFirstStage(VESSEL2 *vessel)
{
	vessel->AddMesh (oapiLoadMeshGlobal("titan21"), &STAGE1_OFS);
};

void AddMidSection(VESSEL2 *vessel)
{
	vessel->AddMesh (oapiLoadMeshGlobal("titan2i"), &MID_OFS);

};

void AddSecondStage(VESSEL2 *vessel)
{
	vessel->AddMesh (oapiLoadMeshGlobal("titan22"), &STAGE2_OFS);
};

void JettisonFirstStage(VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,-2.5);
	ofs = STAGE1_OFS;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	vessel->SetEngineLevel(ENGINE_MAIN, 0);
	StageTransform(vessel,&vs,ofs,vel);
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-Stage_First");
	VESSEL::Create (name, "Titan_Stage_First", vs);
};

void JettisonMidStage(VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,-0.5);
	ofs = MID_OFS;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	vessel->SetEngineLevel(ENGINE_MAIN, 0);
	StageTransform(vessel,&vs,ofs,vel);
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-Mid_Section");
	VESSEL::Create (name, "Titan_Mid_Section", vs);
}

void JettisonSecondStage(VESSEL2 *vessel)
{
	VESSELSTATUS vs;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,-0.5);
	ofs = STAGE2_OFS;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	vessel->SetEngineLevel(ENGINE_MAIN, 0);
	StageTransform(vessel,&vs,ofs,vel);
	vs.vrot.x = 0.0;
	vs.vrot.y = 0.0;
	vs.vrot.z = 0.0;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-Stage_Second");
	VESSEL::Create (name, "Titan_Stage_Second", vs);
}

class Titan2SecondStage:public IStage
{
public:
	Titan2SecondStage(Gemini_ControlPanel *Panel);
	void Init ();
	void Exit (VESSEL2 *vessel);
	void Setup (VESSEL2 *vessel, double stage_sep_time);
	void ParseLine (VESSEL2 *vessel, char *line, void *vs);
	void SaveState (VESSEL2 *vessel, FILEHANDLE scn);
	void PreStep (VESSEL2 *vessel,  double simt, double simdt, double mjd);
	void PostStep (VESSEL2 *vessel, double simt, double simdt, double mjd);
	void Animate (VESSEL2 *vessel, double simt);
	void VisualCreated (VESSEL2 *vessel, VISHANDLE vis, int refcount);
	void VisualDestroyed (VESSEL2 *vessel, VISHANDLE vis, int refcount);
	int  ConsumeDirectKey (VESSEL2 *vessel, char *keystate);
	int  ConsumeBufferedKey (VESSEL2 *vessel, DWORD key, bool down, char *keystate);
	void Message(VESSEL2 *vessel, int msg, double value);		
	double ignition_time;
private:
	void SetInitialStage(VESSEL2 *vessel);
	void SetFinalStage(VESSEL2 *vessel);
	void AutoPilot(VESSEL2 *vessel, double t);
	bool bManualSeparate;
	bool bAbort;
	bool doAbort;
	bool bMidSectionJettisoned;
	bool doModeIIsep;
	double abort_time;
	double first_stage_sep;
	PROPELLANT_HANDLE main_tank;
	THRUSTER_HANDLE main_engine[1];
	THRUSTER_HANDLE roll_thruster[2];
	Gemini_ControlPanel *ControlPanel;
};

Titan2SecondStage::Titan2SecondStage(Gemini_ControlPanel *Panel) {
	ControlPanel = Panel;
	Init();
}

void Titan2SecondStage::Init ()
{
	bManualSeparate       = false;
	bAbort                = false;
	doAbort               = false;
	first_stage_sep       = 0;
	bMidSectionJettisoned = false;
	doModeIIsep           = false;
	abort_time            = 0.0;
	ignition_time         = 0.0;
};

void Titan2SecondStage::Exit (VESSEL2 *vessel)
{
};

void Titan2SecondStage::Setup (VESSEL2 *vessel, double stage_sep_time)
{
	first_stage_sep=stage_sep_time;
	if(bMidSectionJettisoned)
		SetFinalStage(vessel);
	else
		SetInitialStage(vessel);
};
void Titan2SecondStage::ParseLine (VESSEL2 *vessel, char *line, void *vs)
{
	double readdouble = 0.0;
    if (!strnicmp (line, "MIDJETT", 7)) 
	{
        sscanf (line+7, "%lf", &readdouble);
		if (readdouble == 1)
			bMidSectionJettisoned = true;
		else
			bMidSectionJettisoned = false;
		if(bMidSectionJettisoned)
			SetFinalStage(vessel);
	} else if (!strnicmp (line, "IGNITION_TIME", 13)) {
		sscanf (line+13, "%lf", &readdouble);
		ignition_time=-readdouble;
    } else if (!strnicmp (line, "AUTOPILOT", 9)) 	{
        sscanf (line+9, "%lf", &readdouble);
		if (readdouble == 1)
			bAutoPilot= true;
		else
			bAutoPilot = false;
    } else {
        vessel->ParseScenarioLineEx (line, vs);
    }
};
void Titan2SecondStage::SaveState (VESSEL2 *vessel, FILEHANDLE scn)
{
	oapiWriteScenario_int (scn, "CONFIGURATION", 1);
	oapiWriteScenario_float (scn, "MIDJETT", bMidSectionJettisoned);
	oapiWriteScenario_float (scn, "IGNITION_TIME", oapiGetSimTime() - ignition_time);
	oapiWriteScenario_float (scn, "AUTOPILOT", bAutoPilot);
};

void Titan2SecondStage::PreStep (VESSEL2 *vessel,  double simt, double simdt, double mjd)
{
	Gemini *newStage;

	if (!bMidSectionJettisoned && simt-first_stage_sep > 2.0)
	{
		JettisonMidStage(vessel);
		SetFinalStage(vessel);
		bMidSectionJettisoned=true;
		vessel->SetEngineLevel(ENGINE_MAIN,1.0);
	}

	if (doModeIIsep && simt-abort_time > 5.0)
	{
		JettisonMidStage(vessel);
		JettisonSecondStage(vessel);
		newStage = new Gemini(ControlPanel);
		newStage->doAbort = true;
		((Multi_Stage*)vessel)->SetStage(newStage, simt);
		doModeIIsep=false;
	}

	if (bAbort)
	{
		bAbort = false;
		doAbort = true;
	}
	else if (doAbort)
	{
		double velocity;
		oapiGetFocusAirspeed(&velocity);
		if (velocity > (6330) && vessel->GetAtmPressure() < 5000)
		{
			if (!bMidSectionJettisoned)
				JettisonMidStage(vessel);
			JettisonSecondStage(vessel);
			newStage = new Gemini(ControlPanel);
			((Multi_Stage*)vessel)->SetStage(newStage,simt);
			vessel->SetThrusterGroupLevel(THGROUP_MAIN,1.0);
			doAbort=false;
		}
		else if (vessel->GetAtmPressure()<5000)
		{
			if (!bMidSectionJettisoned)
				JettisonMidStage(vessel);
			JettisonSecondStage(vessel);
			newStage = new Gemini(ControlPanel);
			newStage->doAbort = true;
			((Multi_Stage*)vessel)->SetStage(newStage,simt);
			doAbort=false;
		}
		else if (vessel->GetAtmPressure()<56000)
		{
			vessel->SetThrusterGroupLevel(THGROUP_MAIN,0.0);
			abort_time = simt;
			doAbort=false;
			doModeIIsep=true;
		}
		return;
	}
	else 
	{
		if (vessel->GetFuelMass() == 0 || bManualSeparate)
		{
			vessel->ActivateNavmode(NAVMODE_KILLROT);
			vessel->SetAttitudeRotLevel(0, 0);
			bManualSeparate=false;
			JettisonSecondStage(vessel);
			newStage = new Gemini(ControlPanel);
			((Multi_Stage*)vessel)->SetStage(newStage,simt);
			return;
		}
	}

	if (vessel->GetFuelMass() == 0 || bManualSeparate)
	{
		bManualSeparate=false;
		JettisonSecondStage(vessel);
		newStage = new Gemini(ControlPanel);
		((Multi_Stage*)vessel)->SetStage(newStage,simt);
		return;
	}
	if (bAutoPilot)
		AutoPilot(vessel, simt-ignition_time);
	else {
		double tempP = 0.0;
		double tempY = 0.0;
		double tempR = 0.0;
		tempP = vessel->GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - vessel->GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		tempY = vessel->GetManualControlLevel(THGROUP_ATT_YAWLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - vessel->GetManualControlLevel(THGROUP_ATT_YAWRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		tempR = vessel->GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - vessel->GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		vessel->SetThrusterDir(main_engine[0],_V(0.01*tempY,0.01*tempP,1));
		if (tempR < 0) {
			vessel->SetThrusterLevel(roll_thruster[0], 0.0);
			vessel->SetThrusterLevel(roll_thruster[1], fabs(tempR));
		} else if (tempR > 0) {
			vessel->SetThrusterLevel(roll_thruster[0], fabs(tempR));
			vessel->SetThrusterLevel(roll_thruster[1], 0.0);
		} else {
			vessel->SetThrusterLevel(roll_thruster[0], 0.0);
			vessel->SetThrusterLevel(roll_thruster[1], 0.0);
		}
	}
};

void Titan2SecondStage::PostStep (VESSEL2 *vessel, double simt, double simdt, double mjd)
{
};


void Titan2SecondStage::Animate (VESSEL2 *vessel, double simt)
{
};

void Titan2SecondStage::VisualCreated (VESSEL2 *vessel, VISHANDLE vis, int refcount)
{
};

void Titan2SecondStage::VisualDestroyed (VESSEL2 *vessel, VISHANDLE vis, int refcount)
{
};

int Titan2SecondStage::ConsumeDirectKey (VESSEL2 *vessel, char *keystate)
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
			return 0;
		}
	}
	return 0;
};

int Titan2SecondStage::ConsumeBufferedKey (VESSEL2 *vessel, DWORD key, bool down, char *kstate)
{
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
		if (key == OAPI_KEY_J) { // "Jettison Stage"
			bManualSeparate = true;
			return 1;
		}
		if (key == OAPI_KEY_E) { // "Abort Emergency"
			bAbort = true;
			return 1;
		}
		if (key == OAPI_KEY_P) // "autoPilot" 
		{
			if (bAutoPilot){
				bAutoPilot = false;
				vessel->SetAttitudeRotLevel(0, 0);
			} else
				bAutoPilot = true;
			return 1;			
		}
	}	
	return 0;
};

void Titan2SecondStage::Message(VESSEL2 *vessel, int msg, double value) 
{
};

void Titan2SecondStage::SetInitialStage(VESSEL2 *vessel)
{
	PARTICLESTREAMSPEC titan_exhaust = {
		0, .65, 120, 25.0, 0, 0.2, -.5, 0.5, PARTICLESTREAMSPEC::EMISSIVE,
			PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
		PARTICLESTREAMSPEC::ATM_FLAT, 1, 1
	};

	PARTICLESTREAMSPEC srb_exhaust = {
		0, 0.5, 30, 100.0, 0, 0.1, 30, 0.5, PARTICLESTREAMSPEC::EMISSIVE,
		PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
		PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
	};
	srb_exhaust.tex = oapiRegisterParticleTexture ("contrail_blue");
	titan_exhaust.tex = oapiRegisterParticleTexture ("contrail_blue");

	vessel->SetSize             ((GEMINI_LENGTH + STAGE2_LENGTH) / 2);
	vessel->SetCOG_elev         ((GEMINI_LENGTH + STAGE2_LENGTH) / 2);
	vessel->SetEmptyMass        (GEMINI_MASS + STAGE2_MASS_EMPTY);
	vessel->SetPMI              (_V(6+3.5,6+3.5,1.25+.85));
	vessel->SetCrossSections    (_V(25+12.5,25+12.5,8.5));
	vessel->SetCW               (1.4, 0.1, 0.3, 0.3);
	vessel->SetRotDrag          (_V(0.7,0.7,0.7));
	vessel->SetPitchMomentScale (0);
	vessel->SetBankMomentScale  (0);
	vessel->SetLiftCoeffFunc    (0); 
	vessel->ClearMeshes();
	vessel->ClearPropellantResources();
	vessel->ClearThrusterDefinitions();
	AddGemini(vessel);
	AddSecondStage(vessel);
	AddMidSection(vessel);

	main_tank = vessel->CreatePropellantResource(STAGE2_MASS_FUEL);
	main_engine[0] = vessel->CreateThruster(STAGE2_OFS_FLAME, STAGE2_FLAME_DIR, STAGE2_THRUST_VAC, main_tank, STAGE2_ISP_VAC, STAGE2_ISP_SL);
	roll_thruster[0] = vessel->CreateThruster(_V(-.75,0,0), _V(0,-1,0), STAGE2_THRUST_ROLL, main_tank, STAGE2_ISP_VAC, STAGE2_ISP_SL);
	roll_thruster[1] = vessel->CreateThruster(_V(-.75,0,0), _V(0,1,0), STAGE2_THRUST_ROLL, main_tank, STAGE2_ISP_VAC, STAGE2_ISP_SL);
	//vessel->AddExhaust(main_engine[0],10.0,1.0);
	vessel->AddExhaust(roll_thruster[0],0.5,0.05,STAGE2_OFS_ROLL,_V(0,1,0));
	vessel->AddExhaust(roll_thruster[1],0.5,0.05,STAGE2_OFS_ROLL,_V(0,-1,0));

	vessel->CreateThrusterGroup(main_engine,1,THGROUP_MAIN);
	vessel->AddExhaustStream(main_engine[0], &srb_exhaust);
	vessel->AddExhaustStream(main_engine[0], &titan_exhaust);
};

void Titan2SecondStage::SetFinalStage(VESSEL2 *vessel)
{
	vessel->ClearMeshes();
	AddGemini(vessel);
	AddSecondStage(vessel);
};

void Titan2SecondStage::AutoPilot(VESSEL2 *vessel, double t)
{
	double pitch = 0.0;
	double drop_vel = 0.0;
	double tempD = 0.0;
	double target_att;
	double target_thrust = 0.0;
	char current_key=0;
	double ap_dist = 0.0;
	VECTOR3 ang_vel;

// navigation
	oapiGetFocusPitch(&pitch);
	pitch=pitch*DEG;

 // guidance
	vessel->GetAngularVel(ang_vel);
 // control
	if (t>166.04) {

	


 // guidance
		vessel->GetAngularVel(ang_vel);
	//sprintf(oapiDebugString(), "Time: %f, Pitch: %f x: %f, y: %f, z: %f",t, pitch, ang_vel.x, ang_vel.y, ang_vel.z);
//		if (t > 305.0) {
		if (t > 250.0) {
			drop_vel = -0.125;
			target_att = 0.0;
			target_thrust = 1.0;
		} else {
			drop_vel = -0.0125;
			target_att = 0.0;
			target_thrust = .4;
			//drop_vel = -0.0125;
			//target_att = -10.0;
		}
		if (t > 390.0) {
			vessel->GetApDist(ap_dist);
			if (ap_dist >= 6682000.0) {
				bManualSeparate = true;
				target_thrust = 0;
			}
		}
 // control
		if (pitch > (target_att - 0.5) && pitch < (target_att + 0.5)) {
			if (ang_vel.x > .001) 
				current_key = OAPI_KEY_NUMPAD2;
			else if (ang_vel.x < -.001)
				current_key = OAPI_KEY_NUMPAD8;
			else
				current_key = OAPI_KEY_NUMPAD5;
		} else if (pitch < (target_att - 0.5)) {
			if (ang_vel.x > -drop_vel +.0005) 
				current_key = OAPI_KEY_NUMPAD2;
			else if (ang_vel.x < -drop_vel-.0005)
				current_key = OAPI_KEY_NUMPAD8;
			else
				current_key = OAPI_KEY_NUMPAD5;
		} else {
			if (ang_vel.x > drop_vel +.0005) 
				current_key = OAPI_KEY_NUMPAD2;
			else if (ang_vel.x < drop_vel - .0005)
				current_key = OAPI_KEY_NUMPAD8;
			else
				current_key = OAPI_KEY_NUMPAD5;
		}
		if (current_key == OAPI_KEY_NUMPAD2) {
			vessel->SetThrusterDir(main_engine[0],_V(0,0.005,1));
		} else if (current_key == OAPI_KEY_NUMPAD5) {
			vessel->SetThrusterDir(main_engine[0],_V(0,0,1));
		} else if (current_key == OAPI_KEY_NUMPAD8) {
			vessel->SetThrusterDir(main_engine[0],_V(0,-0.005,1));
		} else {
			vessel->SetThrusterDir(main_engine[0],_V(0,0,1));
		}
		vessel->SetThrusterLevel(main_engine[0],target_thrust);
	}
	//sprintf(oapiDebugString(), "Time: %f, Pitch: %f x: %f, y: %f, z: %f dv: %f ap: %f",t, pitch, ang_vel.x*DEG, ang_vel.y*DEG, ang_vel.z*DEG, drop_vel*DEG, ap_dist);
}


class Titan2FirstStage:public IStage
{
public:
	Titan2FirstStage(Gemini_ControlPanel *Panel);
	void Init ();
	void Exit (VESSEL2 *vessel);
	void Setup (VESSEL2 *vessel, double stage_sep_time);
	void ParseLine (VESSEL2 *vessel, char *line, void *vs);
	void SaveState (VESSEL2 *vessel, FILEHANDLE scn);
	void PreStep (VESSEL2 *vessel,  double simt, double simdt, double mjd);
	void PostStep (VESSEL2 *vessel, double simt, double simdt, double mjd);
	void Animate (VESSEL2 *vessel, double simt);
	void VisualCreated (VESSEL2 *vessel, VISHANDLE vis, int refcount);
	void VisualDestroyed (VESSEL2 *vessel, VISHANDLE vis, int refcount);
	int  ConsumeDirectKey (VESSEL2 *vessel, char *keystate);
	int  ConsumeBufferedKey (VESSEL2 *vessel, DWORD key, bool down, char *keystate);
	void Message(VESSEL2 *vessel, int msg, double value);
private:
	void SetStage(VESSEL2 *vessel);
	void AutoPilot(VESSEL2 *vessel, double t);
	bool bManualSeparate;
	bool bAbort;
	bool doAbort;
	bool bJettDoors;
	double DoorJettTime;
	bool doModeIIsep;
	double abort_time;
	bool bEngineIgnited;
	double ignition_time;
	PROPELLANT_HANDLE main_tank;
	THRUSTER_HANDLE main_engine[2];
	Gemini_ControlPanel *ControlPanel;
};

Titan2FirstStage::Titan2FirstStage(Gemini_ControlPanel *Panel) {
	ControlPanel = Panel;
	Init();
}
void Titan2FirstStage::Init ()
{
	bManualSeparate = false;
	bAbort          = false;
	doAbort         = false;
	bJettDoors      = false;
	DoorJettTime    = 0.0;
	doModeIIsep     = false;
	abort_time      = 0.0;
	bManualSeparate = false;
	bEngineIgnited  = false;
	ignition_time   = 0.0;
};
void Titan2FirstStage::Exit (VESSEL2 *vessel)
{
};
void Titan2FirstStage::Setup (VESSEL2 *vessel, double stage_sep_time)
{
	SetStage(vessel);
};
void Titan2FirstStage::ParseLine (VESSEL2 *vessel, char *line, void *vs)
{
	double readdouble = 0.0;
	if (!strnicmp (line, "IGNITION_TIME", 13)) {
		sscanf (line+13, "%lf", &readdouble);
		ignition_time=-readdouble;
	} else if (!strnicmp (line, "ENGINE_IGNITED", 14)) {
		sscanf (line+14, "%lf", &readdouble);
		if (readdouble == 1.0)
			bEngineIgnited = true;
		else
			bEngineIgnited = false;
	} else if (!strnicmp (line, "AUTOPILOT", 9)) {
		sscanf (line+9, "%lf", &readdouble);
		if (readdouble == 1.0)
			bAutoPilot = true;
		else
			bAutoPilot = false;
	} else {
		vessel->ParseScenarioLineEx(line,vs);
	}
};
void Titan2FirstStage::SaveState (VESSEL2 *vessel, FILEHANDLE scn)
{
	oapiWriteScenario_int (scn, "CONFIGURATION", 0);
	oapiWriteScenario_float (scn, "IGNITION_TIME", oapiGetSimTime() - ignition_time);
	oapiWriteScenario_float (scn, "ENGINE_IGNITED", bEngineIgnited);
	oapiWriteScenario_float (scn, "AUTOPILOT", bAutoPilot);
};

void Titan2FirstStage::PreStep (VESSEL2 *vessel,  double simt, double simdt, double mjd)
{
	Titan2SecondStage *newStage;
	Gemini            *newGemini;
	
	
	if (doModeIIsep && simt-abort_time > 5.0)
	{
		JettisonFirstStage(vessel);
		JettisonMidStage(vessel);
		JettisonSecondStage(vessel);
		newGemini = new Gemini(ControlPanel);
		newGemini->doAbort= true;
		((Multi_Stage*)vessel)->SetStage(newGemini, simt);
 		doModeIIsep=false;
		return;
	}
	if (bAbort)
	{
		bAbort = false;
		doAbort = true;
	} else if (doAbort)	{
		double velocity;
		oapiGetFocusAirspeed(&velocity);
		if (velocity > (6330) && vessel->GetAtmPressure() < 5000)
		{
			JettisonFirstStage(vessel);
			JettisonMidStage(vessel);
			JettisonSecondStage(vessel);
			newGemini = new Gemini(ControlPanel);
			((Multi_Stage*)vessel)->SetStage(newGemini,simt);
			vessel->SetThrusterGroupLevel(THGROUP_MAIN,1.0);
			doAbort=false;
		}
		else if (vessel->GetAtmPressure()<5000)
		{
			JettisonFirstStage(vessel);
			JettisonMidStage(vessel);
			JettisonSecondStage(vessel);
			newGemini = new Gemini(ControlPanel);
			newGemini->doAbort = true;
			((Multi_Stage*)vessel)->SetStage(newGemini, simt);			
			doAbort=false;
		}
		else if (vessel->GetAtmPressure()<56000)
		{
			vessel->SetThrusterGroupLevel(THGROUP_MAIN,0.0);
			abort_time = simt;
			doAbort=false;
			doModeIIsep=true;
		}
		else if (vessel->GetAtmPressure()>=56000)
		{
			if (!bJettDoors)
			{
				JettisonDoorLeft(vessel);
				JettisonDoorRight(vessel);
				vessel->ClearMeshes();
				AddGeminiNoDoors(vessel);
				AddSecondStage(vessel);
				AddMidSection(vessel);
				AddFirstStage(vessel);
				bJettDoors=true;
				DoorJettTime=simt;
			}
		}
		if (bJettDoors && (simt-DoorJettTime) > 1)
		{
			OBJHANDLE hvessel;
			char name[256];
			//Mode I
			JettisonCommander(vessel);
			JettisonPilot(vessel);
			strcpy (name, vessel->GetName()); strcat (name, "-CDR");
			hvessel=oapiGetVesselByName(name);
			if (hvessel != 0)
			{
				oapiSetFocusObject(hvessel);
			}
			doAbort=false;
			vessel->ClearMeshes();
			AddGeminiNoCrew(vessel);
			AddSecondStage(vessel);
			AddMidSection(vessel);
			AddFirstStage(vessel);
			vessel->SetThrusterGroupLevel(THGROUP_MAIN,0.0);
		}
		return;
	}
	
	
	if (bManualSeparate)
	{
		bManualSeparate=false;
		JettisonFirstStage(vessel);
		newStage = new Titan2SecondStage(ControlPanel);
		((Multi_Stage*)vessel)->SetStage(newStage,simt);
		newStage->ignition_time = ignition_time;
		return;
	}

	if (vessel->GetPropellantMass(main_tank) < .01) {
		bManualSeparate = true;
		vessel->SetThrusterLevel(main_engine[0],0.0);
		vessel->SetThrusterLevel(main_engine[1],0.0);
	}


	if (bAutoPilot)
		AutoPilot(vessel, simt-ignition_time);
	else { 
		double tempP = 0.0;
		double tempY = 0.0;
		double tempR = 0.0;
		tempP = vessel->GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - vessel->GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		tempY = vessel->GetManualControlLevel(THGROUP_ATT_YAWLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - vessel->GetManualControlLevel(THGROUP_ATT_YAWRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		tempR = vessel->GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - vessel->GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		if (tempR != 0.0) {
			vessel->SetThrusterDir(main_engine[0],_V(0,0.05*tempR,1));
			vessel->SetThrusterDir(main_engine[1],_V(0,-0.05*tempR,1));
		} else {
			vessel->SetThrusterDir(main_engine[0],_V(0.01*tempY,0.01*tempP,1));
			vessel->SetThrusterDir(main_engine[1],_V(0.01*tempY,0.01*tempP,1));
		}
	}
	if (!bEngineIgnited)
	{
		if (vessel->GetEngineLevel(ENGINE_MAIN) > .95)
		{
			ignition_time=simt;
			bEngineIgnited=true;
		};
	};
};

void Titan2FirstStage::PostStep (VESSEL2 *vessel, double simt, double simdt, double mjd)
{
};

void Titan2FirstStage::Animate (VESSEL2 *vessel, double simt)
{
};
void Titan2FirstStage::VisualCreated (VESSEL2 *vessel, VISHANDLE vis, int refcount)
{
};
void Titan2FirstStage::VisualDestroyed (VESSEL2 *vessel, VISHANDLE vis, int refcount)
{
};

int Titan2FirstStage::ConsumeDirectKey (VESSEL2 *vessel, char *keystate)
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
			return 0;
		}
	}
	return 0;
};

int Titan2FirstStage::ConsumeBufferedKey (VESSEL2 *vessel, DWORD key, bool down, char *kstate)
{
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
		if (key == OAPI_KEY_J) { // "Jettison Stage"
			bManualSeparate = true;
			return 1;
		}
		if (key == OAPI_KEY_E) { // "Abort Emergency"
			bAbort = true;
			return 1;
		}
		if (key == OAPI_KEY_P) // "autoPilot" 
		{
			if (bAutoPilot){
				bAutoPilot = false;
			} else
				bAutoPilot = true;
			return 1;			
		}
	}	
	return 0;
};

void Titan2FirstStage::Message(VESSEL2 *vessel, int msg, double value)
{
};

void Titan2FirstStage::SetStage(VESSEL2 *vessel)
{

	PARTICLESTREAMSPEC srb_contrail = {
		0, 1, 1, 5.0, 1, 10, 50, 5, PARTICLESTREAMSPEC::DIFFUSE,
		PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
		PARTICLESTREAMSPEC::ATM_PLOG, 1.2, 1.25
	};

	PARTICLESTREAMSPEC srb_exhaust = {
		0, 0.5, 30, 100.0, 0, 0.1, 30, 0.5, PARTICLESTREAMSPEC::EMISSIVE,
		PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
		PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
	};
	
	PARTICLESTREAMSPEC titan_exhaust = {
		0, .65, 120, 25.0, 0, 0.2, -.5, 0.5, PARTICLESTREAMSPEC::EMISSIVE,
			PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
		PARTICLESTREAMSPEC::ATM_FLAT, 1, 1
	};

	titan_exhaust.tex = oapiRegisterParticleTexture ("contrail_blue");
	srb_exhaust.tex = oapiRegisterParticleTexture ("contrail_blue");
	srb_contrail.tex = oapiRegisterParticleTexture ("contrail_brown");
	
	vessel->SetSize            ((GEMINI_LENGTH + STAGE2_LENGTH - STAGE2_LEN_ENGINE + MID_LENGTH + STAGE1_LENGTH) / 2);
	vessel->SetCOG_elev        (vessel->GetSize());
	vessel->SetEmptyMass       (GEMINI_MASS + STAGE2_MASS + STAGE1_MASS_EMPTY);
	vessel->SetPMI             (_V(40+6+3.5,40+6+3.5,1.25+1.25+.85));
	vessel->SetCrossSections   (_V(70+25+12.5,70+25+12.5,8.5));
	vessel->SetTouchdownPoints (_V(0,-1.0,STAGE1_OFS1_FLAME.z), _V(-.7,.7,STAGE1_OFS1_FLAME.z), _V(.7,.7,STAGE1_OFS1_FLAME.z));
	//vessel->SetCW              (1.0, 0.1, 0.3, 0.3);
	vessel->SetCW              (0.1,0.3, .8, .8);
	vessel->SetRotDrag         (_V(0.7,0.7,0.7));
	vessel->SetPitchMomentScale (0);
	vessel->SetBankMomentScale (0);
	vessel->SetLiftCoeffFunc   (0); 

	vessel->ClearMeshes();
	vessel->ClearPropellantResources();
	vessel->ClearThrusterDefinitions();

	AddGemini(vessel);
	AddSecondStage(vessel);
	AddMidSection(vessel);
	AddFirstStage(vessel);

	main_tank = vessel->CreatePropellantResource(STAGE1_MASS_FUEL);
	main_engine[0] = vessel->CreateThruster(STAGE1_OFS_FLAME, STAGE1_FLAME_DIR, STAGE1_THRUST_VAC, main_tank, STAGE1_ISP_VAC, STAGE1_ISP_SL);
	//vessel->AddExhaust(main_engine[0],10.0,0.6, STAGE1_OFS1_FLAME, _V(0,0,-1));
	main_engine[1] = vessel->CreateThruster(STAGE1_OFS_FLAME, STAGE1_FLAME_DIR, STAGE1_THRUST_VAC, main_tank, STAGE1_ISP_VAC, STAGE1_ISP_SL);
	//vessel->AddExhaust(main_engine[1],10.0,0.6, STAGE1_OFS2_FLAME, _V(0,0,-1));
	vessel->CreateThrusterGroup(main_engine,2,THGROUP_MAIN);

	vessel->AddExhaustStream(main_engine[0], STAGE1_OFS1_FLAME, &titan_exhaust);
	vessel->AddExhaustStream(main_engine[0], STAGE1_OFS1_FLAME, &srb_exhaust);
	vessel->AddExhaustStream(main_engine[0], STAGE1_OFS1_FLAME, &srb_contrail);
	vessel->AddExhaustStream(main_engine[1], STAGE1_OFS2_FLAME, &titan_exhaust);
	vessel->AddExhaustStream(main_engine[1], STAGE1_OFS2_FLAME, &srb_exhaust);
	vessel->AddExhaustStream(main_engine[1], STAGE1_OFS2_FLAME, &srb_contrail);

};

void Titan2FirstStage::AutoPilot(VESSEL2 *vessel, double t)
{
	double pitch = 0.0;
	double drop_vel = 0.0;
	double tempD = 0.0;
	static double target_att = 0.0;
	char current_key=0;
	VECTOR3 ang_vel;

// navigation
	oapiGetFocusPitch(&pitch);
	pitch=pitch*DEG;

 // guidance
	vessel->GetAngularVel(ang_vel);
 // control
	if (t<23.04) {
		vessel->SetThrusterDir(main_engine[0],_V(0,0,1));
		vessel->SetThrusterDir(main_engine[1],_V(0,0,1));
	} else {
// guidance
		vessel->GetAngularVel(ang_vel);

		if (t < 87.414) {
			drop_vel = -.609*RAD;
			target_att = 90.0 + (drop_vel * DEG * (t-23.04));
		} else if (t < 118.134) {
			drop_vel = -.516*RAD;
			target_att = 90.0 + (drop_vel * DEG * (t-87.414)) - 0.609 * (87.414-23.04);
		} else {
			drop_vel = -.26*RAD;
			target_att = 90.0 + (drop_vel * DEG * (t-118.134)) - .516 * (118.134 -87.414) - 0.609 * (87.414-23.04);
		}
 // control
		if (ang_vel.x > drop_vel+.0005 ) 
			tempD = 0.0005;
		else if (ang_vel.x < drop_vel - .0005)
			tempD = -0.0005;

		
		vessel->SetThrusterDir(main_engine[0],_V(0,tempD,1));
		vessel->SetThrusterDir(main_engine[1],_V(0,tempD,1));
	}
	//sprintf(oapiDebugString(), "Time: %f, Pitch: %f x: %f, y: %f, z: %f dv: %f at: %f",t, pitch, ang_vel.x*DEG, ang_vel.y*DEG, ang_vel.z*DEG, drop_vel*DEG, target_att);
}


#endif