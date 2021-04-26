#ifndef __MULTI_STAGE_H
#define __MULTI_STAGE_H

#include "orbitersdk.h"
#include "common_addon.h"
#include "matrix.h"
#include "vectors.h"
#include <cmath>
#include <cstdio>
#include <fstream>

//const double G   = 9.80665;
const double N   = 1.0;
const double kN  = 1000.0;
const double KGF = N*G;
const double SEC = 1.0*G;
const double KG  = 1.0;
const double GM  = 3.99020882e14;
typedef void *TEST_HANDLE;

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

//Courtesy of Orbiter Math Chris Knestrick cknestri@vt.edu
inline double Mag(const VECTOR3 &a)
{
	return (sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2)));
}


//inline VECTOR3 &operator/= (VECTOR3 &a, double f)
//{
//	a.x /= f;
//	a.y /= f;
//	a.z /= f;
//	return a;
//}
//
//inline VECTOR3 operator/ (VECTOR3 &a, double f)
//{
//	VECTOR3 c;
//	c.x = a.x / f;
//	c.y = a.y / f;
//	c.z = a.z / f;
//	return c;
//}


vector3 GetVesselHeading(VESSEL *vessel)//this is quite heavy. API could use a way to get orientation w.r.t. orbit
{
	OBJHANDLE planet=vessel->GetGravityRef();
	VECTOR3 gpos,pos,vel,vel2;
	vector3 Vpos,Vvel,Vnorm;
	vector3 target;
	vessel->GetGlobalPos(gpos);
	vessel->GetRelativePos(planet,pos);
	vessel->GetRelativeVel(planet,vel);
	Vpos=_vector3(pos.x,pos.y,pos.z);
	Vnorm=_vector3(vel.x,vel.y,vel.z);
	Vnorm=Vpos*Vnorm;//this is normal on orbital plane;
	Vnorm.selfnormalize();
 
	vel.x+=gpos.x;vel.y+=gpos.y;vel.z+=gpos.z;
	vessel->Global2Local(vel,vel2);
	Vvel=_vector3(vel2.x,vel2.y,vel2.z);//this is V vector in local frame
	Vvel.selfnormalize();
	Vnorm.x+=gpos.x;Vnorm.y+=gpos.y;Vnorm.z+=gpos.z;
	vessel->Global2Local(_V(Vnorm.x,Vnorm.y,Vnorm.z),vel2);
	Vnorm=_vector3(vel2.x,vel2.y,vel2.z);//and this is N vector in local frame
	Vnorm.selfnormalize();
	vector3 local_up=Vnorm;
	vector3 local_front=Vnorm*_vector3(0,0,1);
 
	Vnorm.z=0;Vnorm.selfnormalize();//all we need from here is roll now
	double Pi=acos(-1);
 // ***********************
	double roll=Pi-atan2(Vnorm.x,Vnorm.y);
 // ************************
	double pitch=-(Pi/2-local_up.angle(_vector3(0,0,1)));
 //*************************
 
	double heading=Vvel.angle(local_front);
	vector3 local_head=local_front*Vvel;
	local_head.selfnormalize();//need to bring heading to quadtrant. ugly...
	if (local_head.z*local_up.z>0) heading=-heading;
	target.x=heading+Pi;
	target.y=pitch;
	target.z=roll;
	return target;
};



void TetherCalc(VECTOR3 &dist, VECTOR3 &vel, VECTOR3 &newVel) 
{
	VECTOR3 R = dist;
	VECTOR3 V = vel;
	VECTOR3 Ru = _V(0,0,0);
	VECTOR3 Vu = _V(0,0,0);
	VECTOR3 negVu = _V(0,0,0);
	VECTOR3 Vk = _V(0,0,0);
	VECTOR3 U = _V(0,0,0);

	Ru = R / Mag(R);
	Vu = V / Mag(V);
	negVu = (V * -1) / Mag(V);
	Vk = Ru * (dotp(negVu,Ru) * Mag(V));
	U = (V * -1) - Vk;
	newVel = (U * -1) + Vk;
}


//void TetherCalc2(VECTOR3 &dist, VECTOR3 &vel, double massAnchor, double massTether, VECTOR3 &newAnchor, VECTOR3 &newTether) 
void TetherCalc2(VESSEL *Anchor, VESSEL *Tether, double TetherLength) 
{
	
	VESSELSTATUS sAnchor;
	VESSELSTATUS sTether;
	VECTOR3 RelDist      = _V(0,0,0);
	VECTOR3 RelVel       = _V(0,0,0);
	VECTOR3 DistVelNorm  = _V(0,0,0);
	VECTOR3 TangentVect  = _V(0,0,0);
	VECTOR3 nRelDist     = _V(0,0,0);
	VECTOR3 nTangentVect = _V(0,0,0);
	VECTOR3 DistVel      = _V(0,0,0);
	VECTOR3 TangentVel   = _V(0,0,0);
	VECTOR3 nDistVel      = _V(0,0,0);
	VECTOR3 newDistVel      = _V(0,0,0);
	double magRelDist    = 0;
	double magRelVel     = 0;
	double DistRelAngle  = 0;
	double Momentum      = 0;
	double newVel        = 0;

	Anchor->GetStatus(sAnchor);
	Tether->GetStatus(sTether);
	
	RelDist    = sTether.rpos-sAnchor.rpos;
	RelVel     = sTether.rvel-sAnchor.rvel;
	magRelDist = Mag(RelDist);
	magRelVel  = Mag(RelVel);
	if (magRelDist >= TetherLength)
	{

		DistVelNorm  = crossp(RelDist, RelVel);
		TangentVect  = crossp(DistVelNorm, RelDist);
		DistRelAngle = acos(Mag(DistVelNorm) / (magRelDist * magRelVel));
		nRelDist     = RelDist / magRelDist;
		nTangentVect = TangentVect / Mag(TangentVect);
		DistVel      = nRelDist * magRelVel * sin(DistRelAngle);
		TangentVel   = nTangentVect * magRelVel * cos(DistRelAngle);
		nDistVel     = DistVel / Mag(DistVel);
		Momentum     = magRelVel * Tether->GetMass();
		newVel       = Momentum / (Anchor->GetMass() + Tether->GetMass());
		newDistVel   = nDistVel * newVel;
		sTether.rvel = sAnchor.rvel + (newDistVel + TangentVel);
		sAnchor.rvel = sAnchor.rvel + newDistVel;
		sTether.rpos = sAnchor.rpos + nRelDist * TetherLength;
		Anchor->DefSetState(&sAnchor);
		Tether->DefSetState(&sTether);
	} 
	//Anchor->Local2Global(_V(0,0,0), tetherpt1);
	//Anchor->Local2Global(_V(0,0,1), tetherpt2);

	//RelDist = sTether.rpos - (sAnchor.rpos + (tetherpt2 - tetherpt1));
	//magRelDist = Mag(RelDist);
	//sprintf(oapiDebugString(), "Tether: %f, %f, %f, %f", magRelDist, magRelVel, Anchor->GetMass(), Tether->GetMass());

}

void AddImpulse(VESSEL *vessel, VECTOR3 &Impulse_Pt, VECTOR3 &Impulse_Force)
{
	VESSELSTATUS sVessel;
	VECTOR3 PMI   = _V(0,0,0);
	VECTOR3 vTmp1 = _V(0,0,0);
	VECTOR3 vTmp2 = _V(0,0,0);
	VECTOR3 RadialForce  = _V(0,0,0);
	VECTOR3 TangentForce = _V(0,0,0);


	TangentForce = crossp(crossp(Impulse_Pt, Impulse_Force), Impulse_Pt);
	RadialForce  = Impulse_Force - TangentForce;

	

	vessel->GetStatus(sVessel);
	vessel->GetPMI(PMI);
	
	vTmp1.x = PMI.x * sVessel.vrot.x;
	vTmp1.y = PMI.x * sVessel.vrot.y;
	vTmp1.z = PMI.z * sVessel.vrot.z;

	vTmp2 = crossp(Impulse_Pt, TangentForce) - crossp(sVessel.vrot,vTmp1);

	sVessel.vrot.x += (1/ PMI.x) * vTmp2.x;
	sVessel.vrot.y += (1/ PMI.y) * vTmp2.y;
	sVessel.vrot.z += (1/ PMI.z) * vTmp2.z;

	vessel->Local2Global(RadialForce / Mag(RadialForce), vTmp1);
	oapiGetGlobalPos(sVessel.rbody, &vTmp2);
	vTmp1 = vTmp1 - vTmp2 - sVessel.rpos;
	
	sVessel.rvel.x += vTmp1.x * (RadialForce.x / vessel->GetMass());
	sVessel.rvel.y += vTmp1.y * (RadialForce.y / vessel->GetMass());
	sVessel.rvel.z += vTmp1.z * (RadialForce.z / vessel->GetMass());
	vessel->DefSetState(&sVessel);

//	sprintf(oapiDebugString(),"T: %f, %f, %f R: %f, %f, %f", TangentForce.x, TangentForce.y, TangentForce.z, RadialForce.x, RadialForce.y, RadialForce.z);
}

/*
void TetherCalc3(VESSEL *Anchor, VESSEL *Tether, double TetherLength) 
{
	
	VESSELSTATUS sAnchor;
	VESSELSTATUS sTether;
	VECTOR3 RelDist      = _V(0,0,0);
	VECTOR3 RelVel       = _V(0,0,0);
	VECTOR3 nRelDist     = _V(0,0,0);
	double magRelDist    = 0;

	VECTOR3 TangentVel   = _V(0,0,0);
	VECTOR3 RadialVel    = _V(0,0,0);
	VECTOR3 nForce       = _V(0,0,0);
	VECTOR3 locForce     = _V(0,0,0);

	VECTOR3 vTmp1 = _V(0,0,0);
	VECTOR3 vTmp2 = _V(0,0,0);

	Anchor->GetStatus(sAnchor);
	Tether->GetStatus(sTether);

	
	RelDist    = sTether.rpos-sAnchor.rpos;
	RelVel     = sTether.rvel-sAnchor.rvel;
	magRelDist = Mag(RelDist);
	if (magRelDist >= TetherLength)
	{


		nRelDist     = RelDist / magRelDist;

		TangentVel   = crossp(crossp(RelDist, RelVel), RelDist);
		RadialVel    = RelVel - TangentVel;
		RadialVel    = RadialVel;
		
		nForce = nRelDist * ((50 * (magRelDist - TetherLength) + 600 * (dotp(RadialVel,RelDist) / magRelDist)));

		nForce = nForce / Tether->GetMass();

		nForce = nForce * oapiGetSimStep();

		sTether.rvel.x -= nForce.x;
		sTether.rvel.y -= nForce.y;
		sTether.rvel.z -= nForce.z;


		sprintf(oapiDebugString(), "Tether: %f,D: %f, %f, %f, F: %f, %f %f", magRelDist, RelDist.x, RelDist.y, RelDist.z, nForce.x, nForce.y, nForce.z);
	} else
		sprintf(oapiDebugString(), "Tether: %f,D: %f, %f, %f, %f, %f", magRelDist, RelDist.x, RelDist.y, RelDist.z, Anchor->GetMass(), Tether->GetMass());

}
*/
void DrawNeedle (HDC hDC, int x, int y, double rad, double angle, HPEN pen0, HPEN pen1)
{
	double dx = rad * cos(angle), dy = rad * sin(angle);
	HGDIOBJ oldObj;
	oldObj = SelectObject (hDC, pen1);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(0.85*dx+0.5), y - (int)(0.85*dy+0.5));
	SelectObject (hDC, oldObj);
	oldObj = SelectObject (hDC, pen0);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(dx+0.5), y - (int)(dy+0.5));
	SelectObject (hDC, oldObj);
}



void DrawNumbers(SURFHANDLE surf, SURFHANDLE numbers, double number, int x2, int y2, int x1, int y1, int w, int h) {
	int num1 = 0;
	int num2 = 0;

	num2 = ((int) number) / 10;
	num1 = ((int) number) % 10;
	oapiBlt(surf, numbers, x2, y2, w * num2, 0, w, h);
	oapiBlt(surf, numbers, x1, y1, w * num1, 0, w, h);
}



void StageTransform(VESSEL *vessel, VESSELSTATUS *vs, VECTOR3 ofs, VECTOR3 vel)
{
	VECTOR3 rofs, rvel = {vs->rvel.x, vs->rvel.y, vs->rvel.z};
	rofs.x=0;
	rofs.y=0;
	rofs.z=0;
// Staging Velocity represents 
// Need to do some transforms to get the correct vector to eject the stage
	vessel->Local2Rel (ofs, vs->rpos);
	vessel->GlobalRot (vel, rofs);
	vs->rvel.x = rvel.x+rofs.x;
	vs->rvel.y = rvel.y+rofs.y;
	vs->rvel.z = rvel.z+rofs.z;
}



// ==========================================================
// Interface for Panel
// ==========================================================

class IControlPanel
{
public:
	virtual bool LoadPanel (VESSEL *vessel, int id);
	virtual bool PanelRedrawEvent (VESSEL *vessel, int id, int event, SURFHANDLE surf);
	virtual bool PanelMouseEvent (VESSEL *vessel, int id, int event, int mx, int my);
	virtual void Timestep (VESSEL *vessel, double simt);
	virtual void ParseSystem(VESSEL *vessel, FILEHANDLE scn);
	virtual void ParseLine (VESSEL *vessel, char *line, void *vs);
	virtual void SaveState (VESSEL *vessel, FILEHANDLE scn);
};

class Instrument;

class InstrumentList	//list used by panels to keep instruments 
{
public:
   Instrument *instance;	
   InstrumentList *next;
   InstrumentList(){
	   instance=NULL;
	   next=NULL;
   };
};



// ==========================================================
// Interface for IStage
// ==========================================================

class MS_MSG
{
public:
	MS_MSG();
	MS_MSG(int aMsg, double aValue);
	int msg;
	double value;
};

MS_MSG::MS_MSG() 
{
	msg	  = 0;
	value = 0.0;
}

MS_MSG::MS_MSG(int aMsg, double aValue)
{
	msg	  = aMsg;
	value = aValue;
}



class IStage
{
public:
	virtual void Init ();
	virtual void Exit (VESSEL *vessel);
	virtual void Setup (VESSEL *vessel, double stage_sep_time);
	virtual void ParseLine (VESSEL *vessel, char *line, void *vs);
	virtual void SaveState (VESSEL *vessel, FILEHANDLE scn);
	virtual void Timestep (VESSEL *vessel, double simt);
	virtual void Animate (VESSEL *vessel, double simt);
	virtual void VisualCreated (VESSEL *vessel, VISHANDLE vis, int refcount);
	virtual void VisualDestroyed (VESSEL *vessel, VISHANDLE vis, int refcount);
	virtual int ConsumeKey (VESSEL *vessel, const char *keystate);
	virtual int ConsumeBufferedKey (VESSEL *vessel, DWORD key, bool down, char *kstate);
	virtual void Message(VESSEL *vessel, int msg, double value);
};

void IStage::Init ()
{
};
void IStage::Exit (VESSEL *vessel)
{
};
void IStage::Setup (VESSEL *vessel, double stage_sep_time)
{
};
void IStage::ParseLine (VESSEL *vessel, char *line, void *vs)
{
};
void IStage::SaveState (VESSEL *vessel, FILEHANDLE scn)
{
};
void IStage::Timestep (VESSEL *vessel, double simt)
{
};
void IStage::Animate (VESSEL *vessel, double simt)
{
};
void IStage::VisualCreated (VESSEL *vessel, VISHANDLE vis, int refcount)
{
};
void IStage::VisualDestroyed (VESSEL *vessel, VISHANDLE vis, int refcount)
{
};
int IStage::ConsumeKey (VESSEL *vessel, const char *keystate)
{
	return 0;
};

int IStage::ConsumeBufferedKey (VESSEL *vessel, DWORD key, bool down, char *kstate)
{
	return 0;
};

void IStage::Message(VESSEL *vessel, int msg, double value)
{

};


class Multi_Stage: public COMMON_VESSEL {
public:
	Multi_Stage (OBJHANDLE hObj);
	~Multi_Stage ();
	void Init ();
	void Exit ();
	void SetClassCaps (FILEHANDLE cfg);
	void ParseLine (char *line, void *vs);
	void ParseSystem (FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);
	void Timestep (double simt);
	void Animate (double simt);
	void VisualCreated (VISHANDLE vis, int refcount);
	void VisualDestroyed (VISHANDLE vis, int refcount);
	void SetPanel(IControlPanel *Panel);
	IControlPanel* GetPanel();
	int ConsumeKey (const char *keystate);
	int ConsumeBufferedKey (DWORD key, bool down, char *kstate);
	void SetStage (IStage *Stage, double simt);
	void SetCurrentStage(IStage *Stage);
	void Setup (double stage_sep_time);
	int SendMessage(const char* pszMessage, void* aMessage);
	bool LoadPanel (int id);
	bool PanelRedrawEvent (int id, int event, SURFHANDLE surf);
	bool PanelMouseEvent (int id, int event, int mx, int my);

	VISHANDLE hVis;
private:
	IStage *cStage;
	IControlPanel *cPanel;
};

Multi_Stage::Multi_Stage(OBJHANDLE hObj)
: COMMON_VESSEL(hObj)
{
	this->GetClassName();
}

Multi_Stage::~Multi_Stage() {
	if (cStage)
		delete cStage;
	if (cPanel)
		delete cPanel;
}

void Multi_Stage::SetPanel(IControlPanel *Panel)
{
	cPanel = Panel;
}

IControlPanel* Multi_Stage::GetPanel(){
	return cPanel;
}

void Multi_Stage::Init ()
{
	if (cStage)
		cStage->Init();
};
void Multi_Stage::Exit ()
{
	if (cStage)
		cStage->Exit(this);
};
void Multi_Stage::SetClassCaps (FILEHANDLE cfg)
{
	if (cStage)
		cStage->Init();
};

void Multi_Stage::ParseSystem(FILEHANDLE scn){
	if (cPanel)
		cPanel->ParseSystem(this, scn);
}

void Multi_Stage::ParseLine (char *line, void *vs)
{
	if (cStage)
		cStage->ParseLine(this, line, vs);

	if (cPanel)
		cPanel->ParseLine(this, line, vs);
};

void Multi_Stage::SaveState (FILEHANDLE scn)
{
	if (cStage)
		cStage->SaveState(this,scn);
	if (cPanel)
		cPanel->SaveState(this,scn);
};
void Multi_Stage::Timestep (double simt)
{
	if (cStage)
		cStage->Timestep(this,simt);
	if (cPanel)
		cPanel->Timestep(this,simt);
};
void Multi_Stage::Animate (double simt)
{
	if (cStage)
		cStage->Animate(this,simt);
};
void Multi_Stage::VisualCreated (VISHANDLE vis, int refcount)
{
	if (refcount > 1) return; 
	hVis=vis;
	if (cStage)
		cStage->VisualCreated(this,vis,refcount);
	Animate(0);
};
void Multi_Stage::VisualDestroyed (VISHANDLE vis, int refcount)
{
	hVis=NULL;
	if (cStage)
		cStage->VisualDestroyed(this,vis,refcount);
};
int Multi_Stage::ConsumeKey (const char *keystate)
{
	if (cStage)
		return cStage->ConsumeKey(this,keystate);
	else
		return 0;
};

int Multi_Stage::ConsumeBufferedKey (DWORD key, bool down, char *kstate)
{
	if (cStage)
		return cStage->ConsumeBufferedKey(this, key, down, kstate);
	else
		return 0;
};

void Multi_Stage::SetStage (IStage *Stage, double simt)
{
	cStage=Stage;
	if (cStage)
	{
		cStage->Setup(this,simt);
	};
};
void Multi_Stage::SetCurrentStage (IStage *Stage)
{
	cStage=Stage;
};

void Multi_Stage::Setup (double stage_sep_time)
{
	if (cStage)
		cStage->Setup(this, stage_sep_time);
}


int Multi_Stage::SendMessage(const char* pszMessage, void* aMessage)
{
	MS_MSG* aMsg;
	if (_strnicmp (pszMessage, "MS_MSG", 6) != 0) {
		aMsg = (MS_MSG*)aMessage;
		cStage->Message(this,aMsg->msg,aMsg->value);
		return 1;
	} else
		return 0;
}


bool Multi_Stage::LoadPanel (int id) {
	if (cPanel)
		return cPanel->LoadPanel(this, id);
	else
		return FALSE;
}

bool Multi_Stage::PanelRedrawEvent (int id, int event, SURFHANDLE surf) {
	if (cPanel)
		return cPanel->PanelRedrawEvent (this, id, event, surf);
	else
		return FALSE;
}

bool Multi_Stage::PanelMouseEvent (int id, int event, int mx, int my) {
	if (cPanel)
		return cPanel->PanelMouseEvent(this, id, event, mx, my);
	else
		return FALSE;
}


#endif // !__MULTI_STAGE_H

