// ==============================================================
// ORBITER vessel module: Mercury Redstone
// ==============================================================

#include "..\..\include\orbitersdk.h"
#include "..\..\include\multi_stage6.h"
#include "gemini.h"
#include "titan2.h"
#include "gemini_panel.h"
#include "panel_graphics.h"


BOOL WINAPI DllMain (HINSTANCE hModule,
					 DWORD ul_reason_for_call,
					 LPVOID lpReserved)
{
	int i;

	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		g_Param.hDLL = hModule;
		// allocate GDI resources
		g_Param.col[2] = oapiGetColour(80,80,224);
		g_Param.col[3] = oapiGetColour(160,120,64);
		g_Param.font[0]  = CreateFont (-8, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
		g_Param.font[1]  = CreateFont (-10, 0, 0, 0, 800, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
		g_Param.brush[0] = CreateSolidBrush (RGB(0,255,0));    // green
		g_Param.brush[1] = CreateSolidBrush (RGB(255,0,0));    // red
		g_Param.brush[2] = CreateSolidBrush (RGB(80,80,224));  // blue
		g_Param.brush[3] = CreateSolidBrush (RGB(160,120,64));  // brown
		g_Param.brush[4] = CreateSolidBrush (RGB(0,0,0));  // brown
		g_Param.pen[0] = CreatePen (PS_SOLID, 1, RGB(224,224,224));
		g_Param.pen[1] = CreatePen (PS_SOLID, 3, RGB(164,164,164)); 
		g_Param.pen[2] = CreatePen (PS_SOLID, 2, RGB(255,255,254)); 
		g_Param.pen[3] = CreatePen (PS_SOLID, 1, RGB(255,255,254)); 
		break;
	case DLL_PROCESS_DETACH:
		// deallocate GDI resources
		for (i = 0; i < 2; i++) DeleteObject (g_Param.font[i]);
		for (i = 0; i < 5; i++) DeleteObject (g_Param.brush[i]);
		for (i = 0; i < 4; i++) DeleteObject (g_Param.pen[i]);

		break;
	}
	return TRUE;
}

void InitialCaps(VESSEL2 *vessel) {
	IStage *newStage;
	newStage = new Titan2FirstStage((Gemini_ControlPanel*)((Multi_Stage*)vessel)->GetPanel());
	((Multi_Stage*)vessel)->SetStage(newStage,oapiGetSimTime());
}


void SetConfiguration(int status, VESSEL2 *vessel)
{
	IStage *newStage;
	switch (status) {
	case 0:
		newStage = new Titan2FirstStage((Gemini_ControlPanel*)((Multi_Stage*)vessel)->GetPanel());
		((Multi_Stage*)vessel)->SetStage(newStage,oapiGetSimTime());
		break;
	case 1:
		newStage = new Titan2SecondStage((Gemini_ControlPanel*)((Multi_Stage*)vessel)->GetPanel());
    	((Multi_Stage*)vessel)->SetStage(newStage,oapiGetSimTime());
		break;
	case 2:
		newStage = new Gemini((Gemini_ControlPanel*)((Multi_Stage*)vessel)->GetPanel());
		((Multi_Stage*)vessel)->SetStage(newStage,oapiGetSimTime());
		break;
	default:
		InitialCaps(vessel);
		break;
	}
	vessel->SetAttitudeMode(RCS_LIN);
};


// Initialisation
DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	Gemini_ControlPanel *ControlPanel;
	Multi_Stage *Vessel;
	ControlPanel  = new Gemini_ControlPanel();
	Vessel = new Multi_Stage(hvessel, flightmodel);
	Vessel->SetPanel(ControlPanel);
	Vessel->SetConfiguration(SetConfiguration);
	Vessel->SetInitialCaps(InitialCaps);
	//((VESSEL*)Vessel)->SetAttitudeMode(RCS_LIN);
	return Vessel;
}

// Cleanup
DLLCLBK void ovcExit (VESSEL *vessel)
{
	if (vessel) delete (Multi_Stage*) vessel;
}
