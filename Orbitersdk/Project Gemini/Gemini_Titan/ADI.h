#ifndef __ADI_H
#define __ADI_H

#include < GL\gl.h >                                
#include < GL\glu.h >
#include <math.h>
#include "panel_graphics.h"


int LoadOGLBitmap(char *filename)
{
   unsigned char *l_texture;
   int l_index, l_index2=0;
   FILE *file;
   BITMAPFILEHEADER fileheader; 
   BITMAPINFOHEADER infoheader;
   RGBTRIPLE rgb; 
   int num_texture=1; //we only use one OGL texture ,so...
   

   if( (file = fopen(filename, "rb"))==NULL) return (-1); 
   fread(&fileheader, sizeof(fileheader), 1, file); 
   fseek(file, sizeof(fileheader), SEEK_SET);
   fread(&infoheader, sizeof(infoheader), 1, file);

   l_texture = (byte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
   memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);

   for (l_index=0; l_index < infoheader.biWidth*infoheader.biHeight; l_index++)
   { 
      fread(&rgb, sizeof(rgb), 1, file); 

      l_texture[l_index2+0] = rgb.rgbtRed; // Red component
      l_texture[l_index2+1] = rgb.rgbtGreen; // Green component
      l_texture[l_index2+2] = rgb.rgbtBlue; // Blue component
      l_texture[l_index2+3] = 255; // Alpha value
      l_index2 += 4; // Go to the next position
   }

   fclose(file); // Closes the file stream

   glBindTexture(GL_TEXTURE_2D, num_texture);
   
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 
	                               0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);
   


   free(l_texture); 

return (num_texture);
};


class ADI{
public:
   void *void_hpbuffer;
   int type;							//instrument registration number for run-time ID
   int ScrX;
   int ScrY;			//coords on screen
   int idx;			//index on the panel list 
   int init;
   float sphere_vert[256][3];
   float sphere_norm[256][3];
   float sphere_tex[256][2];
   unsigned int sphere_index[600];
   void *p_tex;
   int tri; //number of triangle strip
   int list_name; //we store the rendering into a display list
   double radius;
   int function_mode;	//reference / GDC / Horizon
   int orbital_ecliptic;//orbital GDC or ecliptic
   vector3 reference;
   vector3 now;
   vector3 target;
   double over_rate;
   //some stuff for OpenGL
   HDC		   hDC2;
   HGLRC       hRC;
   HBITMAP	   hBMP;
   HBITMAP hBMP_old;
   ADI(int x,int y);
   virtual ~ADI();
   void InitGL();
   void MoveBall();
   void SetOrbital(VESSEL *vessel);
   void SetLocation(VESSEL *vessel);
   void SetEcliptic(VESSEL *vessel);
   void SetEquatorial(VESSEL *vessel);
   void SetReference(VESSEL *vessel);
   void GetReference(VESSEL *vessel);
   void RegisterMe(int index);
   void RegisterVC(int index, SURFHANDLE tgt);
   void PaintMe(VESSEL *vessel, SURFHANDLE surf, SURFHANDLE *hBorder);
   void RefreshMe(VESSEL *vessel, SURFHANDLE surf, SURFHANDLE *hBorder);
};

ADI::ADI(int x,int y)
{
	type= 44; //ADI ball
	init=0;
	ScrX=x;
	ScrY=y;
	radius=11;
	int i;
	double trad;
	//double Pi=acos(-1);
	int indx;
	function_mode=0;//GDC;
	orbital_ecliptic=-1; //orbital GDC;
	reference.x=0.0;
	reference.y=0.0;
	reference.z=0.0;//ecliptic is also first reference
	now.x=0.37;now.y=-1.2;now.z=0.5;
	
	double norm;
	//we define the ADI sphere to be rendered by OpenGL
	//we use a 6 by 12 sphere, with 84 vertices and 72 triangles
	//bottom point
	tri=0;//number of triangle strips
	for (i=0;i<16;i++)
		for (int j=0;j<16;j++) { 
			trad=cos(-PI/2+PI/15*i)*radius;
			indx=i*16+j;
//			sphere_vert[indx][0]=cos(2*acos(-1)/16*j)*trad;
//			sphere_vert[indx][1]=sin(2*acos(-1)/16*j)*trad;
//			sphere_vert[indx][2]=sin(-PI/2+PI/15*i)*radius;
//			sphere_tex[indx][0]=j/16.0;
//			sphere_tex[indx][1]=i/15.0;

			sphere_vert[indx][0]=cos(2*PI/15*j)*trad;
			sphere_vert[indx][1]=sin(2*PI/15*j)*trad;
		 	sphere_vert[indx][2]=sin(-PI/2+PI/15*i)*radius;
		 	sphere_tex[indx][0]=j/15.0;
		 	sphere_tex[indx][1]=i/15.0;			
			norm=sqrt(pow(sphere_vert[indx][0],2)+
				   pow(sphere_vert[indx][1],2)+
				   pow(sphere_vert[indx][2],2));
			sphere_norm[indx][0]=sphere_vert[indx][0]/norm;
			sphere_norm[indx][1]=sphere_vert[indx][1]/norm;
			sphere_norm[indx][2]=sphere_vert[indx][2]/norm;
			if (i>0) {
				sphere_index[tri++]=indx;
				sphere_index[tri++]=indx-16;
			};
		};

};

void ADI::InitGL() {
	GLuint      PixelFormat;  
	BITMAPINFOHEADER BIH;
	int iSize=sizeof(BITMAPINFOHEADER);
	BIH.biSize=iSize;
	BIH.biWidth=160;				//size of the sphere is 160x160
	BIH.biHeight=160;
	BIH.biPlanes=1;
	BIH.biBitCount=16;//default is 16.
	BIH.biCompression=BI_RGB;
	BIH.biSizeImage=0;
	void* m_pBits;
	hDC2=CreateCompatibleDC(NULL);//we make a new DC and DIbitmap for OpenGL to draw onto
	static  PIXELFORMATDESCRIPTOR pfd2;
	DescribePixelFormat(hDC2,1,sizeof(PIXELFORMATDESCRIPTOR),&pfd2);//just get a random pixel format.. 
	BIH.biBitCount=pfd2.cColorBits;//to get the current bit depth.. !?
	hBMP=CreateDIBSection(hDC2,(BITMAPINFO*)&BIH,DIB_RGB_COLORS,&m_pBits,NULL,0);
	hBMP_old=(HBITMAP)SelectObject(hDC2,hBMP);
	static  PIXELFORMATDESCRIPTOR pfd={                             // pfd Tells Windows How We Want Things To Be
        sizeof(PIXELFORMATDESCRIPTOR),                              // Size Of This Pixel Format Descriptor
        1,                                                          // Version Number
		PFD_DRAW_TO_BITMAP |                                        // Format Must Support Bitmap Rendering
        PFD_SUPPORT_OPENGL |									
		PFD_SUPPORT_GDI,											// Format Must Support OpenGL,                                           
		0,//        PFD_TYPE_RGBA,                                              // Request An RGBA Format
        16,															// Select Our Color Depth
        0, 0, 0, 0, 0, 0,                                           // Color Bits Ignored
        0,//1,                                                          // No Alpha Buffer
        0,                                                          // Shift Bit Ignored
        0,                                                          // No Accumulation Buffer
        0, 0, 0, 0,                                                 // Accumulation Bits Ignored
        0,//16,                                                         // 16Bit Z-Buffer (Depth Buffer)  
        0,                                                          // No Stencil Buffer
        0,                                                          // No Auxiliary Buffer
        0,//PFD_MAIN_PLANE,                                             // Main Drawing Layer
        0,                                                          // Reserved
        0, 0, 0                                                     // Layer Masks Ignored
    };
	pfd.cColorBits=pfd2.cColorBits;//same color depth needed.
	DWORD code;
	code=GetLastError();
	PixelFormat=ChoosePixelFormat(hDC2,&pfd);// now pretend we want a new format
	int ret;
	ret=SetPixelFormat(hDC2,PixelFormat,&pfd);
	code=GetLastError();
	hRC=wglCreateContext(hDC2);
	ret=wglMakeCurrent(hDC2,hRC);					//all standard OpenGL init so far

//We load the texture
	int texture_index=LoadOGLBitmap("Textures\\adi.dds");
	if (texture_index>0) glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_SMOOTH);                        // Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);           // Panel Background color
	glClearDepth(1.0f);                             // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);                        // Enables Depth Testing
	glDepthFunc(GL_LESS);                           // The Type Of Depth Testing To Do
	glViewport(0,0,160,160);                       // Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
	glLoadIdentity();                                   // Reset The Projection Matrix
	gluPerspective(45.0f,1.0,1.0f,1000.0f);      
	glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix          
	glLoadIdentity();                                   // Reset The Projection Matrix

	//some ambiental setup
	GLfloat light_position[]={10.0,20.0,10.0,0.0};
	GLfloat light_diffuse[]={1.0,0.2,0.2,1.0};
	GLfloat light_ambient[]={0.8,0.6,0.6,1.0};
	GLfloat mat_specular[]={0.3,0.3,0.3,1.0};
	GLfloat mat_shin[]={5.0};
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	//glEnable(GL_LIGHTING);
	//glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	//glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	//glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	//glLightfv(GL_LIGHT0,GL_SPECULAR,mat_specular);
	//glEnable(GL_LIGHT0);


	//defining our geometry and composing a display list;
	glEnableClientState(GL_NORMAL_ARRAY);
	if (texture_index>0) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,sphere_vert);
	glNormalPointer(GL_FLOAT,0,sphere_norm);
	if (texture_index>0) glTexCoordPointer(2,GL_FLOAT,0,sphere_tex);
	list_name=glGenLists(1);
	glNewList(list_name,GL_COMPILE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);             // Clear The Screen And The Depth Buffer        
		glColor3f(1.0,1.0,1.0);   
		glDrawElements(GL_TRIANGLE_STRIP,tri,GL_UNSIGNED_INT,sphere_index);
	glEndList();

	init=1;		//that's it. If we made it so far, we can use OpenGL
};

ADI::~ADI()
{
	wglMakeCurrent(NULL,NULL);	//standard OpenGL release
	wglDeleteContext(hRC);
	hRC=NULL;
	SelectObject(hDC2,hBMP_old);//remember to delete DC and bitmap memory we created
	DeleteObject(hBMP);
	DeleteDC(hDC2);

};
void ADI::RegisterMe(int index) {
	oapiRegisterPanelArea(index,_R(ScrX,ScrY,ScrX+212,ScrY+212),PANEL_REDRAW_ALWAYS,PANEL_MOUSE_IGNORE,PANEL_MAP_CURRENT);
	idx=index;
};
void ADI::RegisterVC(int index, SURFHANDLE tgt) {
//	sprintf(oapiDebugString(), "Test VC: %d", index);
	oapiVCRegisterArea (index,_R(ScrX,ScrY,ScrX+212,ScrY+212), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_CURRENT, tgt);
	idx=index;
};
void ADI::SetOrbital(VESSEL *vessel)//this is quite heavy. API could use a way to get orientation w.r.t. orbit
{
	OBJHANDLE planet=vessel->GetGravityRef();
	VECTOR3 gpos,pos,vel,vel2;
	vector3 Vpos,Vvel,Vnorm;
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
 // ***********************
	double roll=PI-atan2(Vnorm.x,Vnorm.y);
 // ************************
	double pitch=-(PI/2-local_up.angle(_vector3(0,0,1)));
 //*************************
 
	double heading=Vvel.angle(local_front);
	vector3 local_head=local_front*Vvel;
	local_head.selfnormalize();//need to bring heading to quadtrant. ugly...
	if (local_head.z*local_up.z>0) heading=-heading;
	target.x=heading+PI;
	target.y=pitch;
	target.z=roll;
};
 
void ADI::SetLocation(VESSEL *vessel)//this is quite heavy. API could use a way to get orientation w.r.t. orbit
{

	double longitude = 0.0;
	double latitude = 0.0;
	double radius = 0.0;
	double heading = 0.0;
	double tempD = 0.0;
	 
	vessel->GetEquPos(longitude, latitude, radius);
	target.x=-longitude;//heading+PI;
	target.y=latitude;//pitch;
	target.z=heading;//roll;
	target.x+=PI/2;//long;
	target.y+=0;//lat
	target.z+=-PI/2;
};


void ADI::SetEcliptic(VESSEL *vessel) {	
	
    VESSELSTATUS vstat;vessel->GetStatus(vstat);
	vstat.arot.z+=PI/2; //roll is referenced to horizontal ?!!?
	vstat.arot.x+=PI/2;
	vstat.arot.x=2*PI-vstat.arot.x;
	target.x=vstat.arot.x;
	target.y=vstat.arot.y;
	target.z=vstat.arot.z;

};

void ADI::SetReference(VESSEL *vessel) {	
	VESSELSTATUS vstat;vessel->GetStatus(vstat);
	vstat.arot.z+=acos(-1)/2; //roll is referenced to horizontal ?!!?
	vstat.arot.x+=PI/2;
	vstat.arot.x=2*PI-vstat.arot.x;
	vstat.arot.x-=reference.x;vstat.arot.y-=reference.y;vstat.arot.z-=reference.z;
	target.x=vstat.arot.x;//*cos(vstat.arot.z)+sin(vstat.arot.z)*vstat.arot.y;
	target.y=vstat.arot.y;//*cos(vstat.arot.z)-sin(vstat.arot.z)*vstat.arot.x;
	target.z=vstat.arot.z;

};
void ADI::SetEquatorial(VESSEL *vessel){
	double pitch=vessel->GetPitch();
	double heading;
	oapiGetHeading(vessel->GetHandle(),&heading);
	heading+=PI/2;
	heading=2*PI-heading;
	//heading+=PI;
	double bank=-(vessel->GetBank());
	target.x=heading;
	target.y=pitch;
	target.z=bank;
}

void ADI::GetReference(VESSEL *vessel) {	
	VESSELSTATUS vstat;vessel->GetStatus(vstat);
	vstat.arot.z+=acos(-1)/2;
	//vstat.arot.x+=PI	/2;
	vstat.arot.x=2*PI-vstat.arot.x;
	reference.x=vstat.arot.x;
	reference.y=vstat.arot.y;
	reference.z=vstat.arot.z;

}
void ADI::MoveBall() {
	double delta;
	
    over_rate=0.0;
    delta=target.z-now.z;
	if (delta>0.05) {
		if (delta>PI) {
			now.z+=2*PI;
			MoveBall();	
			return;
		}
			
		now.z+=0.05;
		over_rate=1.;
	} else if (delta<-0.05){ if (delta<-PI) {now.z-=2*PI;MoveBall();
								return;}
						now.z-=0.05;over_rate=1.;}
	else now.z+=delta;

	delta=target.y-now.y;
	if (delta>0.05) {if (delta>PI) {now.y+=2*PI;MoveBall();
										return;}
					now.y+=0.05;over_rate=1.;}
	else if (delta<-0.05) {if (delta<-PI) {now.y-=2*PI;MoveBall();
								return;}
						now.y-=0.05;over_rate=1.;}
	else now.y+=delta;

	delta=target.x-now.x;
	if (delta>0.05) {if (delta>PI) {now.x+=2*PI;MoveBall();
									return;}
						now.x+=0.05;over_rate=1.;}
	else if (delta<-0.05) {if (delta<-PI) {now.x-=2*PI;MoveBall();
								return;}
						now.x-=0.05;over_rate=1.;}
	else now.x+=delta;

	now.x = target.x;
	now.y = target.y;
	now.z = target.z;

	glLoadIdentity(); 
	gluLookAt(0.0,-35.0,0.0, 0.0,0.0,0.0,0.0,0.0,1.0);
	glRotatef(-now.z/PI*180.0,0.0,1.0,0.0);
	glRotatef(now.y/PI*180.0,1.0,0.0,0.0);
	glRotatef(now.x/PI*180.0,0.0,0.0,1.0);

}

void ADI::PaintMe(VESSEL *vessel, SURFHANDLE surf, SURFHANDLE *hBorder)
{
	

	HGDIOBJ oldObj;
	if (!init) InitGL();

	
	VESSELSTATUS vs;
	VECTOR3 ang_vel;
	vessel->GetStatus(vs);
	vessel->GetAngularVel(ang_vel);

	double time_step = oapiGetSimStep();
	double pitch_rate = -ang_vel.x; 
	double roll_rate =  ang_vel.z; 
	double yaw_rate =   -ang_vel.y; 
	int rate = 0;

	pitch_rate = pitch_rate / (PI / 12);
	roll_rate = roll_rate / (PI / 12);
	yaw_rate = yaw_rate / (PI / 12);

	if (pitch_rate > 1)  pitch_rate = 1;
	if (pitch_rate < -1) pitch_rate = -1;
	if (roll_rate > 1)   roll_rate = 1;
	if (roll_rate < -1)  roll_rate = -1;
	if (yaw_rate > 1)    yaw_rate = 1;
	if (yaw_rate < -1)   yaw_rate = -1;

	SetOrbital(vessel);
	MoveBall();
	glCallList(list_name);	//render
	glFlush();
	glFinish();

	HDC hDC=oapiGetDC(surf);

	BitBlt(hDC,5+37,5+35,130,130,hDC2,15,15,SRCCOPY);//then we bitblt onto the panel. wish there

	oapiReleaseDC(surf,hDC);		   // was a faster way ...
	
	oapiBlt(surf,hBorder[0],0,0,0,0,212,212,0xFFFFFF);

	rate = int((pitch_rate * 60) + 69+35);
	oapiBlt(surf,hBorder[1],29+37,rate,0,0,80,3,0xFFFFFF);

	rate = int((yaw_rate * 60) + 69+36);
	oapiBlt(surf,hBorder[2],rate,29+35,0,0,3,80,0xFFFFFF);

	rate = int((roll_rate * 38) + 103);
	oapiBlt(surf,hBorder[3],rate,31,0,0,7,8,0xFFFFFF);
//496,321

}

void ADI::RefreshMe(VESSEL *vessel, SURFHANDLE surf, SURFHANDLE *hBorder)
{
	PaintMe(vessel, surf, hBorder);
}

#endif