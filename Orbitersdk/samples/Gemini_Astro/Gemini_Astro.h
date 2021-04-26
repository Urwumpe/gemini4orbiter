#ifndef __GEMINI_ASTRO_H
#define __GEMINI_ASTRO_H

#include "../../include/orbitersdk.h"
#include "../../include/multi_stage3.h"

const VECTOR3 OFS_SEAT = _V(.1,.1,-.1);
const VECTOR3 OFS_CHUTE = _V(0,.3,.25);

class Gemini_Astro:public IStage
{
public:
	Gemini_Astro();
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
private:
	void SetStage(VESSEL *vessel);
	void JettisonSeat(VESSEL *vessel);
	bool main_deployed;
	bool chute_deployed;
	bool drogue_deployed;
	bool chute_down;
	bool bManualSeparate;
	bool bChuteDeploy;
	double LastPress;
	bool   bDumpSeat;
	double start_simt;
	double dump_time;
};

Gemini_Astro::Gemini_Astro() 
{
	Init();
}

void Gemini_Astro::Init ()
{
	chute_down     =false;
	chute_deployed =false;
	main_deployed =false;
	bChuteDeploy   = false;
	drogue_deployed = false;
	bManualSeparate=false;
	bDumpSeat = false;
	LastPress=0.0;
	start_simt=0.0;
	dump_time=0.0;
}

void Gemini_Astro::Exit (VESSEL *vessel)
{
}

void Gemini_Astro::Setup (VESSEL *vessel, double stage_sep_time)
{
	start_simt=stage_sep_time;
	LastPress = vessel->GetAtmPressure();
	SetStage(vessel);
}

void Gemini_Astro::ParseLine (VESSEL *vessel, char *line, void *vs)
{
}

void Gemini_Astro::SaveState (VESSEL *vessel, FILEHANDLE scn)
{
}

void Gemini_Astro::Timestep (VESSEL *vessel, double simt)
{
	double altitude=0.0;

	if (vessel->GetAtmPressure() >= 80000) {
		if (simt-start_simt > 1.1 && !bDumpSeat) {
			JettisonSeat(vessel);
			vessel->ClearMeshes();
			vessel->AddMesh("gem-seatz");		
			dump_time = simt;
			bDumpSeat=true;

		}
		if (simt-dump_time > 1.2 && !chute_deployed && bDumpSeat) {
			chute_deployed=true;
			vessel->SetPMI (_V(3.5,3.5,1.85));
			vessel->SetCrossSections (_V(2.8,2.8,52.1));
			vessel->SetSize(10.0);
			vessel->ClearMeshes();
			vessel->AddMesh("gem-seatz");
			vessel->AddMesh ("gem_chutez", &OFS_CHUTE);
		}

		if (chute_deployed && vessel->GetPitch() > 70*RAD) {
			vessel->SetCrossSections (_V(1.5,1.5,500.0));
		}
		oapiGetAltitude(vessel->GetHandle(),&altitude);
		if (altitude < 2 && !chute_down) 
		{
			chute_down=true;
			vessel->SetCW(0.3,0.3,0.3,0.3);
			vessel->SetCrossSections (_V(1.5,1.5,1.5));
			vessel->ClearMeshes();
			vessel->AddMesh("gem-seatz");
			vessel->AddMesh ("gem_chute", &OFS_CHUTE);
		}
	} else if (vessel->GetAtmPressure() < 75000) {
		if (simt-start_simt > 1.1 && !drogue_deployed) {
			vessel->SetCrossSections (_V(2.8,2.8,52.1));
			drogue_deployed = true;	
		}
	}
}

void Gemini_Astro::Animate (VESSEL *vessel, double simt)
{
}

void Gemini_Astro::VisualCreated (VESSEL *vessel, VISHANDLE vis, int refcount)
{
}

void Gemini_Astro::VisualDestroyed (VESSEL *vessel, VISHANDLE vis, int refcount)
{
}

int Gemini_Astro::ConsumeKey (VESSEL *vessel, const char *keystate)
{
	return 0;
}

int Gemini_Astro::ConsumeBufferedKey (VESSEL *vessel, DWORD key, bool down, char *kstate)
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
		if (key == OAPI_KEY_J) { 
			bManualSeparate = true;
			return 1;
		}
		if (key == OAPI_KEY_P) { 
			bChuteDeploy = true;
			return 1;
		}
		if (key == OAPI_KEY_M) { 
			JettisonSeat(vessel);
			vessel->ClearMeshes();
			vessel->AddMesh(oapiLoadMeshGlobal("gem-seat2"));		
			return 1;
		}
	}	
	return 0;
}

void Gemini_Astro::SetStage(VESSEL *vessel)
{
// ==============================================================
// These numbers are based off of the MS Space Simulator MMU
	vessel->SetSize (1.25);
	vessel->SetCOG_elev (0.75);
	vessel->SetEmptyMass (200.0);
// ==============================================================
	vessel->SetPMI (_V(.35,.35,.185));
	vessel->SetCrossSections (_V(1.64,.77,1.64));
// ==============================================================
	vessel->SetCW (0.1, 1.5, 0.3, 0.3);
	vessel->SetRotDrag (_V(0.7,0.7,0.7));
	vessel->SetPitchMomentScale (-1e-1);
	vessel->SetBankMomentScale (-1e-1);
	vessel->SetLiftCoeffFunc (0); 
// ==============================================================
	vessel->ClearMeshes();
	vessel->ClearExhaustRefs();
	vessel->ClearAttExhaustRefs();
	vessel->AddMesh (oapiLoadMeshGlobal("gem-seat"));
	vessel->SetTouchdownPoints (_V(0,-1.0,-1.45), _V(-.7,.7,-1.5), _V(.7,.7,-1.5));
}

void Gemini_Astro::JettisonSeat(VESSEL *vessel)
{
	VESSELSTATUS vs;
	vessel->GetStatus (vs);
	vs.eng_main = vs.eng_hovr = 0.0;
	VECTOR3 ofs = _V(.1,.1,-.1);
	VECTOR3 rofs, rvel = {vs.rvel.x, vs.rvel.y, vs.rvel.z};
	VECTOR3 vel = _V(0,-.1,-.1);
	vs.status=0;
	vessel->Local2Rel (ofs, vs.rpos);
	vessel->GlobalRot (vel, rofs);
	vs.rvel.x = rvel.x+rofs.x;
	vs.rvel.y = rvel.y+rofs.y;
	vs.rvel.z = rvel.z+rofs.z;
	char name[256];
	strcpy (name, vessel->GetName()); strcat (name, "-Seat");
	VESSEL::Create (name, "ejectseat", vs);


}

#endif