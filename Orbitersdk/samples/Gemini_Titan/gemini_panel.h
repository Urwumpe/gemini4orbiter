#ifndef __GEMINI_PANEL_H
#define __GEMINI_PANEL_H

#include "resource.h"
#include <OrbiterAPI.h>
#include <VesselAPi.h>
#include "multi_stage4.h"
#include "panel_graphics.h"
#include "ADI.h"

const int nsurf = 5; // number of bitmap handles

#define AID_ADI 0
#define AID_KNOB_ATTMODE 1

const int ATTMODE_PARA = 1;
const int ATTMODE_REENT = 2;
const int ATTMODE_CMDREENT = 3;
const int ATTMODE_PULSE = 4;
const int ATTMODE_DIRECT = 5;
const int ATTMODE_RATECMD = 6;
const int ATTMODE_HORSCAN = 7;

class Gemini_ControlPanel: public IControlPanel
{
public:
	SURFHANDLE srf[nsurf];  // handles for panel bitmaps
	Gemini_ControlPanel();
	~Gemini_ControlPanel();
	bool LoadPanel          (VESSEL2 *vessel, int id);
	bool PanelRedrawEvent   (VESSEL2 *vessel, int id, int event, SURFHANDLE surf);
	bool PanelMouseEvent    (VESSEL2 *vessel, int id, int event, int mx, int my);
	void PreStep            (VESSEL2 *vessel,  double simt, double simdt, double mjd);
	void PostStep           (VESSEL2 *vessel,  double simt, double simdt, double mjd);
	void ParseSystem        (VESSEL2 *vessel, FILEHANDLE scn);
	void ParseLine          (VESSEL2 *vessel, char *line, void *vs);
	void SaveState          (VESSEL2 *vessel, FILEHANDLE scn);
	bool LoadGenericCockpit (VESSEL2 *vessel);
	bool LoadVC             (VESSEL2 *vessel, int id);
	bool VCMouseEvent       (VESSEL2 *vessel, int id, int event, VECTOR3 &p);
	bool VCRedrawEvent      (VESSEL2 *vessel, int id, int event, SURFHANDLE surf);
	void ReleaseSurfaces ();
	MESHHANDLE vcmesh, vcmesh_tpl;

	int attitude_mode;
	int ring_mode;

	double x_pitch_heading;
	double y_yaw_heading;
	double z_roll_heading;

	double hor_pitch_heading;
	double hor_roll_heading;

private:
	void Redraw_KnobSquare(int value, SURFHANDLE surf);
	ADI* adi;
};

Gemini_ControlPanel::Gemini_ControlPanel(){
	int i = 0;
	for (i = 0; i < nsurf; i++) srf[i] = 0;
	adi = new ADI(820, 540);
	adi = new ADI(338, 1288);
	vcmesh_tpl = NULL;
	attitude_mode = ATTMODE_RATECMD;
	ring_mode = 3;
	x_pitch_heading = 90*RAD;
	y_yaw_heading = 180*RAD;
	z_roll_heading = 0.0;
}

Gemini_ControlPanel::~Gemini_ControlPanel(){
	ReleaseSurfaces();
	delete adi;
}

void Gemini_ControlPanel::ParseSystem(VESSEL2 *vessel, FILEHANDLE scn){

}

void Gemini_ControlPanel::ParseLine (VESSEL2 *vessel, char *line, void *vs)
{
	if (!strnicmp (line, "ATTITUDE_MODE", 13)) 	{
        sscanf (line+13, "%d", &attitude_mode);
    } else if (!strnicmp (line, "HEADING_PITCH", 13)) {
        sscanf (line+13, "%lf", &x_pitch_heading);
    } else if (!strnicmp (line, "HEADING_YAW", 11)) {
        sscanf (line+11, "%lf", &y_yaw_heading);
    } else if (!strnicmp (line, "HEADING_ROLL", 12)) {
        sscanf (line+12, "%lf", &z_roll_heading);
	} else {
		vessel->ParseScenarioLineEx (line, vs);
	}
}

void Gemini_ControlPanel::SaveState (VESSEL2 *vessel, FILEHANDLE scn)
{
	oapiWriteScenario_int   (scn, "ATTITUDE_MODE",       attitude_mode);
	oapiWriteScenario_float (scn, "HEADING_PITCH", x_pitch_heading);
	oapiWriteScenario_float (scn, "HEADING_YAW",   y_yaw_heading);
	oapiWriteScenario_float (scn, "HEADING_ROLL",  z_roll_heading);
}

void Gemini_ControlPanel::PreStep(VESSEL2 *vessel,  double simt, double simdt, double mjd)
{
	VECTOR3 new_vel;
	double tempD;

	vessel->GetAngularVel(new_vel);
	tempD = vessel->GetPitch();
	x_pitch_heading = tempD;

	tempD = vessel->GetSlipAngle();
	y_yaw_heading = tempD;

	tempD = vessel->GetBank();
	z_roll_heading = tempD;

}

void Gemini_ControlPanel::PostStep(VESSEL2 *vessel,  double simt, double simdt, double mjd)
{
}

bool Gemini_ControlPanel::PanelMouseEvent (VESSEL2 *vessel, int id, int event, int mx, int my)
{
	switch (id) {
	case AID_KNOB_ATTMODE:

		if (event & PANEL_MOUSE_LBDOWN) 
		{
			attitude_mode++;
			if (attitude_mode > 7)
				attitude_mode = 7;
		}

		if (event & PANEL_MOUSE_RBDOWN) 
		{
			attitude_mode--;
			if (attitude_mode < 1)
				attitude_mode = 1;
		}		
		
		return true;
	}
	return false;
}

bool Gemini_ControlPanel::LoadPanel (VESSEL2 *vessel, int id)
{
	ReleaseSurfaces();
	HBITMAP hBmp;
	switch (id) {	
	case 0:
		oapiCameraSetAperture(RAD*(60/2));
		//vessel->SetCameraOffset(_V(-.715,.865,0));
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_WINDOWCDR));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_ATTACH_RIGHT, 0xffffff);
		oapiSetPanelNeighbours (-1,1,-1,2);
		break;
	case 1:
		oapiCameraSetAperture(RAD*(60/2));
		//vessel->SetCameraOffset(_V(.715,.865,0));
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_WINDOWPLT));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_ATTACH_RIGHT, 0xffffff);
		oapiSetPanelNeighbours (0,-1,-1,-1);
		break;
	case 2:
		//oapiCameraSetAperture(RAD*(60/2));
		//vessel->SetCameraOffset(_V(0,0,0));
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CDRPANEL));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_ATTACH_RIGHT, 0xffffff);
		oapiSetPanelNeighbours (-1,3,0,-1);

		if (srf[0] == 0) srf[0]  = oapiCreateSurface (LOADBMP (IDB_ADIPANEL));
		if (srf[1] == 0) srf[1]  = oapiCreateSurface (LOADBMP (IDB_RATEHORZ));
		if (srf[2] == 0) srf[2]  = oapiCreateSurface (LOADBMP (IDB_RATEVERT));
		if (srf[3] == 0) srf[3]  = oapiCreateSurface (LOADBMP (IDB_RATEROLL));
		
		adi->ScrX = 424-35-35;
		adi->ScrY = 300;
		adi->RegisterMe(AID_ADI);
		break;
	case 3:
		//vessel->SetCameraOffset(_V(0,0,0));
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LOWCENTERPNL));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_ATTACH_RIGHT, 0xffffff);
		oapiSetPanelNeighbours (2,-1,-1,-1);
		if (srf[4] == 0) srf[4]  = oapiCreateSurface (LOADBMP (IDB_KNOBSQ));
		oapiRegisterPanelArea (AID_KNOB_ATTMODE,  _R(  227, 441, 277, 491), PANEL_REDRAW_MOUSE , PANEL_MOUSE_DOWN  , PANEL_MAP_BACKGROUND);
		break;
	}
	return hBmp != NULL;
}

bool Gemini_ControlPanel::PanelRedrawEvent (VESSEL2 *vessel, int id, int event, SURFHANDLE surf)
{
	switch (id) {
	case AID_ADI:
		adi->PaintMe(vessel,surf, srf);
		return true;
	case AID_KNOB_ATTMODE:
		//sprintf(oapiDebugString(), "Test Att: %d", attitude_mode);
		Redraw_KnobSquare(attitude_mode, surf);
		return true;
	}
	return false;
}


bool Gemini_ControlPanel::LoadGenericCockpit(VESSEL2 *vessel)
{
	//vessel->SetCameraOffset(_V(0.00,0.00,0.00));
	return true;
};

bool Gemini_ControlPanel::LoadVC(VESSEL2 *vessel, int id)
{
	if (this->vcmesh_tpl != NULL) {

		
		SURFHANDLE tex1 = oapiGetTextureHandle (vcmesh_tpl, 6);
		SURFHANDLE tex2 = oapiGetTextureHandle (vcmesh_tpl, 4);
		//vessel->SetCameraOffset(_V(-0.49,0.62,0.4));
		switch (id) {
		case 0:
			oapiCameraSetAperture(RAD*(40/2));
			if (srf[0] == 0) srf[0]  = oapiCreateSurface (LOADBMP (IDB_ADIPANEL));
			if (srf[1] == 0) srf[1]  = oapiCreateSurface (LOADBMP (IDB_RATEHORZ));
			if (srf[2] == 0) srf[2]  = oapiCreateSurface (LOADBMP (IDB_RATEVERT));
			if (srf[3] == 0) srf[3]  = oapiCreateSurface (LOADBMP (IDB_RATEROLL));
			if (srf[4] == 0) srf[4]  = oapiCreateSurface (LOADBMP (IDB_KNOBSQ));
			adi->ScrX = 380-12;
			adi->ScrY = 80;
			adi->RegisterVC(AID_ADI, tex1);
			oapiVCRegisterArea (AID_KNOB_ATTMODE,_R(160,230,210,305), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, tex2);
			return true;
		default:
			return false;
		}

		return true;
	} else {
		return false;
	}
};

bool Gemini_ControlPanel::VCMouseEvent(VESSEL2 *vessel, int id, int event, VECTOR3 &p)
{
	return false;
};

bool Gemini_ControlPanel::VCRedrawEvent(VESSEL2 *vessel, int id, int event, SURFHANDLE surf)
{
	switch (id) {
	case AID_ADI:
		adi->PaintMe(vessel,surf, srf);
		return true;
	case AID_KNOB_ATTMODE:
		Redraw_KnobSquare(attitude_mode,surf);
		return true;
	}
	return false;
};


void Gemini_ControlPanel::ReleaseSurfaces ()
{
	for (int i = 0; i < nsurf; i++)
		if (srf[i]) {
			oapiDestroySurface (srf[i]);
			srf[i] = 0;
		}
}



void Gemini_ControlPanel::Redraw_KnobSquare(int value, SURFHANDLE surf)
{
	oapiBlt(surf, srf[4], 0, 0, 0, value * 50, 50, 50, 0xFFFFFF);
}


#endif 
