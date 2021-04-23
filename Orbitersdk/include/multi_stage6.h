#ifndef __MULTI_STAGE_H
#define __MULTI_STAGE_H

#include "orbitersdk.h"
#include "common_addon2.h"
#include "matrix.h"
#include "vectors.h"
#include <math.h>
#include <stdio.h>
#include <fstream.h>
#include <direct.h>

//const double G   = 9.80665;
const double N   = 1.0;
const double kN  = 1000.0;
const double KGF = N*G;
const double SEC = 1.0*G;
const double KG  = 1.0;
const double GM  = 3.99020882e14;
typedef void *TEST_HANDLE;

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))



#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79

void errhandler(LPTSTR pszFile, HWND hwnd) {
   sprintf(oapiDebugString(), "Error: %c120", pszFile);
}



char szReturn[256];

LPTSTR RetrieveTempFilename(LPCTSTR szDirectory, LPCTSTR szPrefix)
{
     char szBuffer[256];
	 char *szReplace;
     if(GetTempFileName(szDirectory,szPrefix,0,szBuffer) == 0)
     {
          // It failed - return the string "ERROR"
          lstrcpy(szReturn,"ERROR");
     } else {
          // Success!
          lstrcpy(szReturn,szBuffer);
     }
	 szReplace = strstr(szReturn, "tmp");
	 strncpy (szReplace,"bmp",3);
	 //puts(szReturn);
     return szReturn;
}

PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
        errhandler("GetObject", hwnd); 

    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 

     if (cClrBits != 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (1<< cClrBits)); 

     // There is no RGBQUAD array for the 24-bit-per-pixel format. 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure. 

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // For Windows NT, the width must be DWORD aligned unless 
    // the bitmap is RLE compressed. This example shows this. 
    // For Windows 95/98/Me, the width must be WORD aligned unless the 
    // bitmap is RLE compressed.
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
     pbmi->bmiHeader.biClrImportant = 0; 
     return pbmi; 
 } 

void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, 
                  HBITMAP hBMP, HDC hDC) 
 { 
     HANDLE hf;                 // file handle 
    BITMAPFILEHEADER hdr;       // bitmap file-header 
    PBITMAPINFOHEADER pbih;     // bitmap info-header 
    LPBYTE lpBits;              // memory pointer 
    DWORD dwTotal;              // total count of bytes 
    DWORD cb;                   // incremental count of bytes 
    BYTE *hp;                   // byte pointer 
    DWORD dwTmp; 

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits) 
         errhandler("GlobalAlloc", hwnd); 
	
    // Retrieve the color table (RGBQUAD array) and the bits 
    // (array of palette indices) from the DIB. 
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
        DIB_RGB_COLORS)) 
    {
        errhandler("GetDIBits", hwnd); 
    }

    // Create the .BMP file. 
    hf = CreateFile(pszFile, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                    NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 
    if (hf == INVALID_HANDLE_VALUE) 
        errhandler("CreateFile", hwnd); 
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file. 
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
                 pbih->biSize + pbih->biClrUsed 
                 * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices. 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    pbih->biSize + pbih->biClrUsed 
                    * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file. 
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL)) 
    {
       errhandler("WriteFile", hwnd); 
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
                  + pbih->biClrUsed * sizeof (RGBQUAD), 
                  (LPDWORD) &dwTmp, ( NULL))) 
        errhandler("WriteFile", hwnd); 

    // Copy the array of color indices into the .BMP file. 
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
           errhandler("WriteFile", hwnd); 

    // Close the .BMP file. 
     if (!CloseHandle(hf)) 
           errhandler("CloseHandle", hwnd); 

    // Free memory. 
    GlobalFree((HGLOBAL)lpBits);
}


void GetScreen(HWND hwnd) {
		HDC hdc = GetDC(NULL);
		HDC hDest = CreateCompatibleDC(hdc) ;

		// get the height and width of the screen
		// (multiple Monitors? see notes)
		int height = GetSystemMetrics(SM_CYVIRTUALSCREEN ) ;
		int width = GetSystemMetrics(SM_CXVIRTUALSCREEN) ;

		// create a bitmap,
		HBITMAP hbDesktop = CreateCompatibleBitmap( hdc, width, height ) ;

		// associate our own Device Context to the bitmap
		SelectObject(hDest, hbDesktop );

		// copy from one DC to the other
		BitBlt( hDest, 0,0, width, height, hdc, 0, 0, SRCCOPY) ;
		_mkdir("images");
		char fname[256] = "images\\";
		char *tName;
		tName = tmpnam(NULL);

		
		CreateBMPFile(hwnd,RetrieveTempFilename("images\\","merc") ,CreateBitmapInfoStruct(hwnd,hbDesktop),hbDesktop,hDest);
}

//Courtesy of Orbiter Math Chris Knestrick cknestri@vt.edu
inline double Mag(const VECTOR3 &a)
{
	return (sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2)));
}


inline VECTOR3 operator/ (VECTOR3 &a, double f)
{
	VECTOR3 c;
	c.x = a.x / f;
	c.y = a.y / f;
	c.z = a.z / f;
	return c;
}


vector3 GetVesselHeading(VESSEL2 *vessel)//this is quite heavy. API could use a way to get orientation w.r.t. orbit
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
void TetherCalc2(VESSEL2 *Anchor, VESSEL2 *Tether, double TetherLength) 
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

void AddImpulse(VESSEL2 *vessel, VECTOR3 &Impulse_Pt, VECTOR3 &Impulse_Force)
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
void TetherCalc3(VESSEL2 *Anchor, VESSEL2 *Tether, double TetherLength) 
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



void StageTransform(VESSEL2 *vessel, VESSELSTATUS *vs, VECTOR3 ofs, VECTOR3 vel)
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
	virtual bool LoadPanel          (VESSEL2 *vessel, int id) {return false;};
	virtual bool PanelRedrawEvent   (VESSEL2 *vessel, int id, int event, SURFHANDLE surf){return false;};
	virtual bool PanelMouseEvent    (VESSEL2 *vessel, int id, int event, int mx, int my){return false;};
	virtual void PreStep            (VESSEL2 *vessel,  double simt, double simdt, double mjd){};
	virtual void PostStep           (VESSEL2 *vessel,  double simt, double simdt, double mjd){};
	virtual void ParseSystem        (VESSEL2 *vessel, FILEHANDLE scn){};
	virtual void ParseLine          (VESSEL2 *vessel, char *line, void *vs){};
	virtual void SaveState          (VESSEL2 *vessel, FILEHANDLE scn){};
	virtual bool LoadGenericCockpit (VESSEL2 *vessel){ return false;};
	virtual bool LoadVC             (VESSEL2 *vessel, int id){return false;};
	virtual bool VCMouseEvent       (VESSEL2 *vessel, int id, int event, VECTOR3 &p){return false;};
	virtual bool VCRedrawEvent      (VESSEL2 *vessel, int id, int event, SURFHANDLE surf){ return false;};
};



// ==========================================================
// Interface for IStage
// ==========================================================

class MS_MSG{
public:
	MS_MSG ();
	MS_MSG (int aMsg, double aValue);
	int    msg;
	double value;
};

MS_MSG::MS_MSG() {
	msg	  = 0;
	value = 0.0;
}

MS_MSG::MS_MSG(int aMsg, double aValue) {
	msg	  = aMsg;
	value = aValue;
}

typedef void (*SetConfigurationFunc)(int status, VESSEL2 *vessel);

class IStage
{
public:

	virtual void Init               (){ };
	virtual void Exit               (VESSEL2 *vessel)                                         { };
	virtual void Setup              (VESSEL2 *vessel, double stage_sep_time)                  { };
	virtual void ParseLine          (VESSEL2 *vessel, char *line, void *vs)                   { };
	virtual void SaveState          (VESSEL2 *vessel, FILEHANDLE scn)                         { };
	virtual void PreStep            (VESSEL2 *vessel,  double simt, double simdt, double mjd) { };
	virtual void PostStep           (VESSEL2 *vessel, double simt, double simdt, double mjd)  { };
	virtual void Animate            (VESSEL2 *vessel, double simt)                            { };
	virtual void VisualCreated      (VESSEL2 *vessel, VISHANDLE vis, int refcount)            { };
	virtual void VisualDestroyed    (VESSEL2 *vessel, VISHANDLE vis, int refcount)            { };
	virtual int  ConsumeDirectKey   (VESSEL2 *vessel, char *keystate)                         { return 0; };
	virtual int  ConsumeBufferedKey (VESSEL2 *vessel, DWORD key, bool down, char *keystate)   { return 0;};
	virtual void Message            (VESSEL2 *vessel, int msg, double value)                  { };
	virtual void FocusChanged       (VESSEL2 *vessel, bool getfocus)                          { };
};

typedef void (*SetInitialCapsFunc)(VESSEL2 *vessel);


class Multi_Stage: public COMMON_VESSEL2 {
public:
	Multi_Stage                 (OBJHANDLE hObj, int fmodel);
	~Multi_Stage                ();
	void Init                   ();
	void Exit                   ();
	void SetPanel               (IControlPanel *Panel);
	IControlPanel* GetPanel     ();
	void SetConfiguration       (SetConfigurationFunc scf);
	void SetInitialCaps         (SetInitialCapsFunc icaps);
	void ParseLine              (char *line, void *vs);
	void ParseSystem            (FILEHANDLE scn);
	void Setup                  (double stage_sep_time);
	void SetStage               (IStage *Stage, double simt);
	int SendMessage             (const char* pszMessage, void* aMessage);

	void clbkSetClassCaps       (FILEHANDLE cfg);
	void clbkLoadStateEx        (FILEHANDLE scn, void *status);
	int  clbkConsumeDirectKey   (char *keystate);
	int  clbkConsumeBufferedKey (DWORD key, bool down, char *keystate);
	void clbkSaveState          (FILEHANDLE scn);
	void clbkPreStep            (double simt, double simdt, double mjd);
	void clbkPostStep           (double simt, double simdt, double mjd);
	void clbkAnimate            (double simt);
	void clbkVisualCreated      (VISHANDLE vis, int refcount);
	void clbkVisualDestroyed    (VISHANDLE vis, int refcount);
	bool clbkLoadGenericCockpit ();
	bool clbkLoadPanel          (int id);
	bool clbkPanelMouseEvent    (int id, int event, int mx, int my);
	bool clbkPanelRedrawEvent   (int id, int event, SURFHANDLE surf);
	bool clbkLoadVC             (int id);
	bool clbkVCMouseEvent       (int id, int event, VECTOR3 &p);
	bool clbkVCRedrawEvent      (int id, int event, SURFHANDLE surf);
	void clbkSetStateEx         (const void *status);
	void clbkFocusChanged       (bool getfocus, OBJHANDLE hNewVessel, OBJHANDLE hOldVessel);

	VISHANDLE hVis;
private:
	IStage *cStage;
	IControlPanel *cPanel;
	SetConfigurationFunc scf;
	SetInitialCapsFunc icaps;
};

Multi_Stage::Multi_Stage(OBJHANDLE hObj, int fmodel)
: COMMON_VESSEL2(hObj, fmodel)
{
	hVis   = 0;
	cStage = 0;
	scf    = 0;
	cPanel = 0;
	icaps  = 0;
	this->GetClassName();
}

Multi_Stage::~Multi_Stage() {
	if (cStage)
		delete cStage;
	if (cPanel)
		delete cPanel;
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

void Multi_Stage::SetPanel(IControlPanel *Panel)
{
	cPanel = Panel;
}

IControlPanel* Multi_Stage::GetPanel(){
	return cPanel;
}

void Multi_Stage::SetConfiguration(SetConfigurationFunc aFunc) {
	scf = aFunc;
}

void Multi_Stage::SetInitialCaps(SetInitialCapsFunc aCaps){
	icaps = aCaps;
}

void Multi_Stage::clbkSetClassCaps (FILEHANDLE cfg){
	if (icaps) {
		icaps(this);
	}
				
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

void Multi_Stage::clbkLoadStateEx (FILEHANDLE scn, void *vs) {
    char *line;
	int status = 0;
	double test=0.0;
	bool configurationset = false;
	while (oapiReadScenario_nextline (scn, line)) {
		if (configurationset) {
			if (!strnicmp (line, "<SYSTEMS>", 9)) {
				ParseSystem(scn);
			} else {
				ParseLine(line, vs);
			}
		} 
		else 
		{
			if (!strnicmp (line, "CONFIGURATION", 13)) {
				if (scf != 0) {
					sscanf (line+13, "%d", &status);
					scf(status, this);
				}
				configurationset=true;
			} else {
				ParseScenarioLineEx (line, vs);
			}
		}
    }
	Setup(oapiGetSimTime());
}

void Multi_Stage::Setup (double stage_sep_time)
{
	if (cStage)
		cStage->Setup(this, stage_sep_time);
}

void Multi_Stage::SetStage (IStage *Stage, double simt)
{
	cStage=Stage;
	if (cStage)
	{
		cStage->Setup(this,simt);
	};
};

int Multi_Stage::SendMessage(const char* pszMessage, void* aMessage)
{
	MS_MSG* aMsg;
	if (!strnicmp (pszMessage, "MS_MSG", 6) != 0) {
		aMsg = (MS_MSG*)aMessage;
		cStage->Message(this,aMsg->msg,aMsg->value);
		return 1;
	} else
		return 0;
}

int  Multi_Stage::clbkConsumeDirectKey (char *keystate){
	if (cStage)
		return cStage->ConsumeDirectKey(this,keystate);
	else
		return 0;
};


int  Multi_Stage::clbkConsumeBufferedKey (DWORD key, bool down, char *keystate){
	if (cStage)
		return cStage->ConsumeBufferedKey(this, key, down, keystate);
	else
		return 0;
};

void Multi_Stage::clbkSaveState (FILEHANDLE scn){
	this->SaveDefaultState(scn);
	if (cStage)
		cStage->SaveState(this,scn);
	if (cPanel)
		cPanel->SaveState(this,scn);
}

void Multi_Stage::clbkPreStep (double simt, double simdt, double mjd){
	if (cStage)
		cStage->PreStep(this,simt, simdt, mjd);
	if (cPanel)
		cPanel->PreStep(this,simt, simdt, mjd);
};

void Multi_Stage::clbkPostStep (double simt, double simdt, double mjd){
	if (cStage)
		cStage->PostStep(this,simt, simdt, mjd);
	if (cPanel)
		cPanel->PostStep(this,simt, simdt, mjd);
};

void Multi_Stage::clbkAnimate (double simt)
{
	if (cStage)
		cStage->Animate(this,simt);
};
void Multi_Stage::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	if (refcount > 1) return; 
	hVis=vis;
	if (cStage)
		cStage->VisualCreated(this,vis,refcount);
	clbkAnimate(0);
};

void Multi_Stage::clbkVisualDestroyed (VISHANDLE vis, int refcount)
{
	hVis=NULL;
	if (cStage)
		cStage->VisualDestroyed(this,vis,refcount);
};

bool Multi_Stage::clbkLoadGenericCockpit (){
	if (cPanel)
		return cPanel->LoadGenericCockpit(this);
	else
		return FALSE;
};

bool Multi_Stage::clbkLoadPanel (int id){
	if (cPanel)
		return cPanel->LoadPanel(this, id);
	else
		return FALSE;
};

bool Multi_Stage::clbkPanelMouseEvent (int id, int event, int mx, int my){
	if (cPanel)
		return cPanel->PanelMouseEvent(this, id, event, mx, my);
	else
		return FALSE;
};

bool Multi_Stage::clbkPanelRedrawEvent (int id, int event, SURFHANDLE surf){
	if (cPanel)
		return cPanel->PanelRedrawEvent (this, id, event, surf);
	else
		return FALSE;
};

bool Multi_Stage::clbkLoadVC (int id){
	if (cPanel)
		return cPanel->LoadVC(this, id);
	else
		return FALSE;
};

bool Multi_Stage::clbkVCMouseEvent (int id, int event, VECTOR3 &p){
	if (cPanel)
		return cPanel->VCMouseEvent(this, id, event, p);
	else
		return FALSE;
};

bool Multi_Stage::clbkVCRedrawEvent (int id, int event, SURFHANDLE surf){
	if (cPanel)
		return cPanel->VCRedrawEvent (this, id, event, surf);
	else
		return FALSE;
};

void Multi_Stage::clbkSetStateEx (const void *status){
	DefSetStateEx(status);
};

void Multi_Stage::clbkFocusChanged(bool getfocus, OBJHANDLE hNewVessel, OBJHANDLE hOldVessel){
	if (cStage)
		cStage->FocusChanged(this, getfocus);
}

#endif // !__MULTI_STAGE_H

