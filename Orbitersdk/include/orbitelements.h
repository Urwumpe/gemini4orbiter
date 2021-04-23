// ======================================================================
//                              ORBIT ELEMENTS
//                            Authored by DuncanS
// ======================================================================
#ifndef __ORBITELEMENTS_H
#define __ORBITELEMENTS_H

#include <math.h>
#include "orbitersdk.h"

//bool SelectVariableBody(void *id, char *str, void *usrdata);

double dotproduct(const VECTOR3 &vector1, const VECTOR3 &vector2);
VECTOR3 crossproduct(const VECTOR3 &vector1, const VECTOR3 &vector2);
VECTOR3 unitise(const VECTOR3 &vector);
double vectorsize(const VECTOR3 &vector);
double cosangle(const VECTOR3 &veca,const VECTOR3 &vecb);
double vectorsize2(const VECTOR3 &vector);
void matrixmultiply(const MATRIX3 &first, const MATRIX3 &second, MATRIX3 *result);
void matrixmultiply(const MATRIX3 &first, const VECTOR3 &second, VECTOR3 *result);
void getinvrotmatrix(VECTOR3 arot, MATRIX3 *invrotmatrix);


// Standard formatting function
//void TextShow(HDC hDC, const char *label, int wpos, int hpos, double value);
//void TextShow(HDC hDC, const char *label, int wpos, int hpos, OBJHANDLE handle);


const double GRAVITY=6.67259e-11; // Gravitational constant


typedef class orbitelements{
private:
	VECTOR3 planevector; // Vector perpendicular to plane - defines orbital plane
	double angularmomentum2; // Angular momentum squared
	double semimajor; // Length of semi-major axis - NEGATIVE if elliptical, POSITIVE if hyperbolic
	VECTOR3 eccvector; // Eccentricity vector
	double eccentricity; // eccentricity of orbit
	double currcosthi; // cosine of angle of craft from periapsis
	double currsinthi; //sin of angle of craft from periapsis
	double timestamp; //Simtime when orbit snapshot taken
	VECTOR3 majoraxis; // Unit vector along major axis towards periapsis
	VECTOR3 minoraxis; // Unit vector along minor axis towards outward side of orbit
	VECTOR3 currposition; // Current position on orbit
	VECTOR3 currvelocity; // Current velocity on orbit
	double planet; // GM for planet
	double deltatime;  // Time from current position to Periapsis
	double orbitconstant; //Time for one radian of mean anomaly
	bool valid; //Whether the orbit class is valid or not
	double simpletimetoradius(double radius) const;
	double simpletimetothi(double costhi, double sinthi) const;//private time calculation function
	void improvebyradius(double timetarget,double topthi,double timeattopthi,class orbittime *posvel) const;
	void improvebysubdivision(double timetarget,double topthi,double timeattopthi,class orbittime *posvel) const;
	bool improve(double timetarget,class orbittime *posvel) const;
public:
	orbitelements(); // Default constructor
	void gettimeorbit(int *orbitnumber,double *orbittime, double timefromnow) const;
	orbitelements(OBJHANDLE hmajor, OBJHANDLE hminor); //Constructor
	orbitelements(VECTOR3 rposition, VECTOR3 rvelocity, double gmplanet); //Constructor using pos and vel vectors
	void init(OBJHANDLE hmajor, OBJHANDLE hminor);//Initialiser
	void init(const VECTOR3 &rposition, const VECTOR3 &rvelocity, double gmplanet); //Initialiser using pos and vel vectors
	void init(const VECTOR3 &rposition, const VECTOR3 &rvelocity, double ttimestamp, double gmplanet); //Initialiser that pushes out a timestamp as well
	void minortomajorinit(const class orbitelements &craftinrmin, const class orbitelements &rmininrmaj, double soisize);
	//void majortominorinit(OBJHANDLE target, OBJHANDLE object, const class intercept &closestapproach, double soisize);//Going from major to minor
	//void draworbit(HDC hDC, const class graphset *graph, bool drawradius) const; //Draws an orbit in window and projection described in graph
	//Can be called even if orbit.isvalid() is false
	void setinvalid(){valid=false;};//Forces an existing orbit structure into invalidity
	//The routines below generally ASSUME that orbit has been initialised
	double GetTimeToThi(double costhi, double sinthi) const; //Gets time from present position to the angle thi.
	double GetTimeToRadius(double radius, bool outward) const; //Gets time from present position to the radius given. Better for hyperbolics.
	void vectortothi(const VECTOR3 &vector,double *costhi,double *sinthi) const; //Gets closest costhi and sinthi from vector angle (NOT from length)
	double thitoradius(double costhi) const; //cos thi to radius
	double radiustothi(double radius) const; //radius to cos thi
	void GetTimesToThi(double costhi, double *time1, double *time2) const; //Gets time from present position to a given costhi (both + and - solutions)
	void thitovectors(double costhi, double sinthi, VECTOR3 *position, VECTOR3 *velocity) const; //Angle thi from periapsis to pos and vel
	void radiustovectors(double radius, bool outward, VECTOR3 *position, VECTOR3 *velocity) const; // Now works from radius length too - should always go from known information
	void timetovectors(double timefromnow, VECTOR3 *pos, VECTOR3 *vel) const;//Time from present to pos and vel vectors
	void timetovectors(double timefromnow,class orbittime *posvel) const;
	void minortomajor(const class orbitelements &rmininrmaj, class orbitelements *rmajorbit, double soisize) const; //Finds path from hyperbolic orbit around present body to orbit around rmaj
	double geteccentricity() const; //Parameter retrieving routines
	double getangmomentum2() const;
	double getcurrcosthi() const;
	double getcurrsinthi() const;
	void getaxes(VECTOR3 *tmajoraxis, VECTOR3 *tminoraxis) const; //Major and minor axes
	void getcurrentvectors(VECTOR3 *tpos, VECTOR3 *tvel) const; //vectors when orbit defined
	double getsemimajor() const;
	VECTOR3 geteccentricityvector() const;
	VECTOR3 getplanevector() const;
	double gettimestamp() const;
	double getpedeltatime() const;
	double getinfinityvelocity() const;
	void getinfinityvelvector(bool outward,VECTOR3 *velocity) const;
	bool isvalid() const;//Says whether this structure has been initialised properly
	double getgmplanet() const;//mass of central body * GRAVITY
	double getcurrradius() const;
	double getpedistance() const;
	double getapodistance() const;
	VECTOR3 getintersectvector(const class orbitelements &torbit) const;//vector along line of intersection of two orbits (like align MFD)
} ORBIT;

typedef class orbittime{
public:
	void getposvel(VECTOR3 *tpos,VECTOR3 *tvel);
	orbittime::orbittime(){processed=false;};
private:
	VECTOR3 pos,vel;
	double currangle,icosthi;
	bool processed;
	friend void orbitelements::timetovectors(double timefromnow,class orbittime *posvel) const;
	friend bool orbitelements::improve(double timetarget,class orbittime *posvel) const;
	friend void orbitelements::improvebysubdivision(double timetarget,double topthi,double timeattopthi,class orbittime *posvel) const;
	friend void orbitelements::improvebyradius(double timetarget,double topthi,double timeattopthi,class orbittime *posvel) const;

} ORBITTIME;

// Standard vector functions
double dotproduct(const VECTOR3 &vector1, const VECTOR3 &vector2)
{
	
	double temp=vector1.x*vector2.x+vector1.y*vector2.y+vector1.z*vector2.z;
	return temp;
}

VECTOR3 crossproduct (const VECTOR3 &vector1, const VECTOR3 &vector2)
{
	VECTOR3 out;
	out.x=vector1.y*vector2.z-vector1.z*vector2.y;
	out.y=vector1.z*vector2.x-vector1.x*vector2.z;
	out.z=vector1.x*vector2.y-vector1.y*vector2.x;
	return out;
}

// Create vector of length 1
VECTOR3 unitise (const VECTOR3 &vector)
{
	double scalar= 1/sqrt(vector.x*vector.x+vector.y*vector.y+vector.z*vector.z);
	return vector*scalar;
}

// Find length of vector
double vectorsize(const VECTOR3 &vector)
{
	return sqrt(vector.x*vector.x+vector.y*vector.y+vector.z*vector.z);
}

double vectorsize2(const VECTOR3 &vector)
{
	return vector.x*vector.x+vector.y*vector.y+vector.z*vector.z;
}

double cosangle(const VECTOR3 &veca,const VECTOR3 &vecb)
{
	return dotproduct(veca,vecb)/sqrt(vectorsize2(veca)*vectorsize2(vecb));
}

void matrixmultiply(const MATRIX3 &first, const MATRIX3 &second, MATRIX3 *result)
{
	int a,b,c;
	for (a=0;a<3;a++)
	{
		c=3*a;
		for (b=0;b<3;b++)
		{
			result->data[3*a+b]=first.data[c]*second.data[b]+first.data[c+1]*second.data[b+3]+first.data[c+2]*second.data[b+6];
		}
	}
}

void matrixmultiply(const MATRIX3 &first, const VECTOR3 &second, VECTOR3 *result)
{
	int a,c;
	for (a=0;a<3;a++)
	{
		c=3*a;
		result->data[a]=first.data[c]*second.data[0]+first.data[c+1]*second.data[1]+first.data[c+2]*second.data[2];
	}
}


void getinvrotmatrix(VECTOR3 arot, MATRIX3 *invrotmatrix)//arot not really a vector - see arot defn from vessel struct
{
	
	double tcos=cos(arot.z);
	double tsin=sin(arot.z);
	MATRIX3 z={0,0,0,0,0,0,0,0,1};
	z.m11=z.m22=tcos;
	z.m12=-tsin;
	z.m21=tsin;
	tcos=cos(arot.y);
	tsin=sin(arot.y);
	MATRIX3 y={0,0,0,0,1,0,0,0,0};
	y.m11=y.m33=tcos;
	y.m13=tsin;
	y.m31=-tsin;
	tcos=cos(arot.x);
	tsin=sin(arot.x);
	MATRIX3 x={1,0,0,0,0,0,0,0,0};
	x.m22=x.m33=tcos;
	x.m32=tsin;
	x.m23=-tsin;
	MATRIX3 temp;
	matrixmultiply(z,y,&temp);
	matrixmultiply(temp,x,invrotmatrix);
}


void orbitelements::gettimeorbit(int *orbitnumber,double *orbittime, double timefromnow) const
{
	*orbittime=orbitconstant*2*PI;
	*orbitnumber=int(floor(timefromnow/ (*orbittime)));
}


void orbitelements::minortomajorinit(const ORBIT &craftinrmin, const ORBIT &rmininrmaj, double soisize)
//Now aims close to transfer coordinates very close to the minor planet - should be very accurate!
{
	if (craftinrmin.eccentricity<1 || !craftinrmin.valid || !rmininrmaj.valid)
	{
		setinvalid();
		return;
	}
	double costhi=craftinrmin.angularmomentum2/(craftinrmin.planet*3*soisize*craftinrmin.eccentricity)-1/craftinrmin.eccentricity;//take vectors at 3x SOI radius
	double sinthi=1-costhi*costhi;
	if (sinthi>0)
	{
		sinthi=sqrt(sinthi); //Positive thi = outward motion
	}
	else
	{
		sinthi=0;
	}
	VECTOR3 pos, vel, plpos, plvel;
	pos=(craftinrmin.majoraxis*costhi+craftinrmin.minoraxis*sinthi)*10*soisize; //Position vector
	double time=craftinrmin.simpletimetoradius(10*soisize); //elapsed time from periapsis to soi

	// Using the velocity for INFINITY rather than current position as it gives better results for onward orbit in most cases
	// Note that this may be non-standard for some applications
	costhi=-1/craftinrmin.eccentricity;
	sinthi=sqrt(1-costhi*costhi); //Positive thi again
	
	//At infinity position and velocity vectors are aligned
	vel=(craftinrmin.majoraxis*costhi+craftinrmin.minoraxis*sinthi)*sqrt(craftinrmin.planet/craftinrmin.semimajor);

	//Calculate position at time zero - without the planet!
	pos=pos-vel*time;
	
	//Get the planet's position at the time of ejection
	time=craftinrmin.gettimestamp()-craftinrmin.deltatime;
	rmininrmaj.timetovectors(time-rmininrmaj.gettimestamp(), &plpos, &plvel);
	pos=pos+plpos;// Convert to rmaj coordinates using future position of planet at time 'time'
	vel=vel+plvel;

	// Get new orbital elements
	init(pos,vel,time, rmininrmaj.planet);
}

double orbitelements::getinfinityvelocity() const
{
	if (semimajor>1) return sqrt(planet/semimajor);
	return -1;
}

/*void orbitelements::majortominorinit(OBJHANDLE target, OBJHANDLE object, const class intercept &closestapproach, double soisize)
{
	if (target==NULL)
	{
		setinvalid();
		return;
	}
	//Get position and velocity of object relative to target
	VECTOR3 minorpos,minorvel;
	closestapproach.getrelpos(&minorpos);
	double pos2=dotproduct(minorpos,minorpos);
	if (pos2>soisize*soisize)//outside SOI, no intercept
	{
		setinvalid();
		return;
	}

	closestapproach.getrelvel(&minorvel);

	//First use dotproduct to get to closest approach exactly
	double minorvelsize=vectorsize(minorvel);
	double firsttime=dotproduct(minorpos,minorvel)/minorvelsize;
	minorpos=minorpos-minorvel*(dotproduct(minorpos,minorvel)/minorvelsize);
	//Now precisely at closest approach
	//Now use pythagoras to find required length back to 10xSOI
	double backradius=100*soisize*soisize-dotproduct(minorpos,minorpos);
	//Now get actual distance from minor body - compensates for any speedup as planet is approached
	VECTOR3 currvector;
	if (object!=NULL)//can be a legal call
	{
		oapiGetRelativePos(object, target,&currvector);
		double actdistance2=vectorsize2(currvector);
		if (actdistance2<backradius) backradius=actdistance2;
	}
	backradius=sqrt(backradius);
	
	double timeoffset=backradius/minorvelsize-firsttime;
	minorpos=minorpos-minorvel*(1/minorvelsize)*backradius;
	//Generate new orbit
	init(minorpos,minorvel,closestapproach.gettimeintercept()-timeoffset,oapiGetMass(target)*GRAVITY);
}
*/


void orbitelements::getinfinityvelvector(bool outward,VECTOR3 *velocity) const
{
	if (!valid || eccentricity<1) return;
	double costhi=-1/eccentricity;
	double sinthi=sqrt(1-costhi*costhi);
	if (outward)
	{
		*velocity=(majoraxis*costhi+minoraxis*sinthi)*sqrt(planet/semimajor);
	}
	else
	{
		*velocity=(minoraxis*sinthi-majoraxis*costhi)*sqrt(planet/semimajor);
	}
}



void orbitelements::radiustovectors(double radius, bool outward, VECTOR3 *position, VECTOR3 *velocity) const
{
	if (!valid) return;
	double costhi=(angularmomentum2/(radius*planet)-1)/eccentricity;
	double sinthi=sqrt(1-costhi*costhi);
	if (!outward) sinthi=-sinthi;
	*position=majoraxis*(radius*costhi)+minoraxis*(radius*sinthi);
	VECTOR3 outvector=unitise(*position);
	VECTOR3 roundvector=unitise(crossproduct(planevector, outvector));
	double rndvel2=angularmomentum2/(radius*radius);
	double outvel=eccentricity*planet*sinthi/sqrt(angularmomentum2);
	*velocity=outvector*outvel+roundvector*sqrt(rndvel2);
}



void orbitelements::thitovectors(double costhi, double sinthi,VECTOR3 *position, VECTOR3 *velocity) const
// Finds position and velocity vectors at a given angle thi
// thi is the angle from periapsis, as measured from the centre of the body orbited
{
	if (!valid) return;
	double radius=angularmomentum2/(planet*(eccentricity*costhi+1));
	*position=majoraxis*(radius*costhi)+minoraxis*(radius*sinthi);
	VECTOR3 outward=majoraxis*costhi+minoraxis*sinthi;
	VECTOR3 roundward=minoraxis*costhi-majoraxis*sinthi;
	double angmom=sqrt(angularmomentum2);
	double outvel=eccentricity*planet*sinthi/angmom;
	*velocity=outward*outvel+roundward*(angmom/radius);
}



orbitelements::orbitelements()
{
	valid=false; //Not a valid orbit yet
}

orbitelements::orbitelements(OBJHANDLE hmajor, OBJHANDLE hminor)
{
	init(hmajor,hminor);
}



orbitelements::orbitelements(VECTOR3 rposition, VECTOR3 rvelocity, double gmplanet)
{
	init(rposition,rvelocity, gmplanet);
}

void orbitelements::init(OBJHANDLE hmajor, OBJHANDLE hminor)
{
	valid=false;
	if (hmajor !=NULL && hminor !=NULL && hmajor!=hminor)
	{
		VECTOR3 craftpos, craftvel;
		oapiGetRelativePos(hminor, hmajor, &craftpos);
		oapiGetRelativeVel(hminor, hmajor, &craftvel);
		double gmmajor=GRAVITY*oapiGetMass(hmajor);
		double timestamp=oapiGetSimTime();
		init(craftpos, craftvel, timestamp, gmmajor);
	}
}

double orbitelements::gettimestamp() const
{
	return timestamp;
}

void orbitelements::init(const VECTOR3 &rposition, const VECTOR3 &rvelocity, double gmplanet)
{
	timestamp=oapiGetSimTime();
	init(rposition, rvelocity, timestamp, gmplanet);
}

void orbitelements::init(const VECTOR3 &rposition, const VECTOR3 &rvelocity, double ttimestamp, double gmplanet)
{
	valid=true;
	planevector=crossproduct(rposition,rvelocity);
	angularmomentum2=dotproduct(planevector, planevector); //Angular momentum squared (don't usually need it non-squared)

	double rvel2=dotproduct(rvelocity, rvelocity);
	double radius=vectorsize(rposition);
	eccvector=(rposition*(rvel2-gmplanet/radius)-rvelocity*dotproduct(rposition, rvelocity))*(1/gmplanet); //Eccentricity vector
	semimajor=gmplanet/(rvel2-2*gmplanet/radius); //Length of semimajor axis  - is NEGATIVE if orbit is elliptical, POSITIVE if hyperbolic
	eccentricity=vectorsize(eccvector); //Eccentricity of orbit
	majoraxis=unitise(eccvector); //Vector towards Periapsis
	minoraxis=unitise(crossproduct(planevector, majoraxis)); // Vector parallel to Minor axis of orbit - important for extracting vectors later
	currcosthi=dotproduct(rposition, majoraxis)/radius; //cos thi is angle from periapsis, as measured from planet centre
	currsinthi=dotproduct(rposition, minoraxis)/radius;
	currposition=rposition;
	currvelocity=rvelocity;
	planet=gmplanet;
	if (eccentricity>1)
	{
		orbitconstant=-sqrt(semimajor*semimajor*semimajor/planet);
	}
	else
	{
		orbitconstant=sqrt(-semimajor*semimajor*semimajor/planet);
	}
	deltatime=0;
	deltatime=GetTimeToThi(currcosthi, currsinthi);
	timestamp=ttimestamp;
}

double orbitelements::simpletimetoradius(double radius) const
{
	double costhi,coscosh, sinsinh, e_angle, loc_deltatime;
	//Calculate costhi from radius
	costhi=(angularmomentum2/(radius*planet)-1)/eccentricity;
	//Can use radius in expression for coscosh, reducing error in hyperbolics dramatically
	coscosh=(eccentricity+costhi)*radius*planet/angularmomentum2;
	if (eccentricity<=1)
	{
		//Elliptical case
		double temp=1-coscosh*coscosh;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=acos(coscosh);
		}
		else
		{
			sinsinh=0;
			if (coscosh>0)
			{
				e_angle=0;
			}
			else
			{
				e_angle=PI;
			}
		}
	}
	else
	{
		//Hyperbolic case
		double temp=coscosh*coscosh-1;
		if (temp>0)
		{
			sinsinh=sqrt(coscosh*coscosh-1);
			e_angle=log(coscosh+sinsinh);
		}
		else
		{
			sinsinh=0;
			e_angle=0;
		}
	}
	loc_deltatime=orbitconstant*(e_angle-eccentricity*sinsinh); 
	return loc_deltatime;
}


double orbitelements::GetTimeToRadius(double radius, bool outward) const
{
	if (!valid) return 0;
	double costhi,coscosh, sinsinh, e_angle, loc_deltatime;
	//Calculate costhi from radius
	costhi=(angularmomentum2/(radius*planet)-1)/eccentricity;
	//Can use radius in expression for coscosh, reducing error in hyperbolics dramatically
	coscosh=(eccentricity+costhi)*radius*planet/angularmomentum2;
	if (eccentricity<=1)
	{
		//Elliptical case
		double temp=1-coscosh*coscosh;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=acos(coscosh);
		}
		else
		{
			sinsinh=0;
			if (coscosh>0)
			{
				e_angle=0;
			}
			else
			{
				e_angle=PI;
			}
		}
	}
	else
	{
		//Hyperbolic case
		double temp=coscosh*coscosh-1;
		if (temp>0)
		{
			sinsinh=sqrt(coscosh*coscosh-1);
			e_angle=log(coscosh+sinsinh);
		}
		else
		{
			sinsinh=0;
			e_angle=0;
		}
	}
	loc_deltatime=orbitconstant*(e_angle-eccentricity*sinsinh); 
	if (!outward) loc_deltatime=-loc_deltatime;
	loc_deltatime-=deltatime;
	if (loc_deltatime<0 && eccentricity<1)
	{
		loc_deltatime+=orbitconstant*(PI+PI); //If calculated time is in the past, add another orbit on_
		if (loc_deltatime<0) loc_deltatime+=orbitconstant*(PI+PI);
		if (loc_deltatime<0) loc_deltatime+=orbitconstant*(PI+PI);
	}
	return loc_deltatime;
}


double orbitelements::simpletimetothi(double costhi,double sinthi) const

{
	double coscosh;
	if (1+eccentricity*costhi<0)
	{
		//This is an error condition - substituting partially sensible result
		//This should never occur
		coscosh=1e8; //Very, very large figure
	}
	else
	{
		coscosh=(eccentricity+costhi)/(1+eccentricity*costhi);
	}
	double sinsinh, e_angle, loc_deltatime;
	if (eccentricity<=1)
	{
		//Elliptical case
		double temp=1-coscosh*coscosh;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=acos(coscosh);
		}
		else
		{
			sinsinh=0;
			if (coscosh>0)
			{
				e_angle=0;
			}
			else
			{
				e_angle=PI;
			}
		}
	}
	else
	{
		//Hyperbolic case
		double temp=coscosh*coscosh-1;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=log(coscosh+sinsinh);
		}
		else
		{
			sinsinh=0;
			e_angle=0;
		}
	}
	loc_deltatime=orbitconstant*(e_angle-eccentricity*sinsinh); 
	if (sinthi<0) return -loc_deltatime;
	return loc_deltatime;
}




double orbitelements::GetTimeToThi(double costhi, double sinthi) const

//Gets time from present position to the position pointed to by cos and sin thi.

{
	if (!valid) return 0;
	double coscosh;
	if (1+eccentricity*costhi<0)
	{
		//This is an error condition - substituting partially sensible result
		//This should never occur
		coscosh=1e8; //Very, very large figure
	}
	else
	{
		coscosh=(eccentricity+costhi)/(1+eccentricity*costhi);
	}
	double sinsinh, e_angle, loc_deltatime;
	if (eccentricity<=1)
	{
		//Elliptical case
		double temp=1-coscosh*coscosh;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=acos(coscosh);
		}
		else
		{
			sinsinh=0;
			if (coscosh>0)
			{
				e_angle=0;
			}
			else
			{
				e_angle=PI;
			}
		}
	}
	else
	{
		//Hyperbolic case
		double temp=coscosh*coscosh-1;
		if (temp>0)
		{
			sinsinh=sqrt(temp);
			e_angle=log(coscosh+sinsinh);
		}
		else
		{
			sinsinh=0;
			e_angle=0;
		}
	}
	loc_deltatime=orbitconstant*(e_angle-eccentricity*sinsinh); 
	if (sinthi<0) loc_deltatime=-loc_deltatime;
	loc_deltatime-=deltatime;
	if (loc_deltatime<0 && eccentricity<1)
	{
		loc_deltatime+=orbitconstant*(PI+PI); //If calculated time is in the past, add another orbit on_
		if (loc_deltatime<0) loc_deltatime+=orbitconstant*(PI+PI);
		if (loc_deltatime<0) loc_deltatime+=orbitconstant*(PI+PI);
	}
	return loc_deltatime;
}

void orbitelements::vectortothi(const VECTOR3 &vector,double *costhi,double *sinthi) const
{
	double majorsize=dotproduct(vector,majoraxis);
	double minorsize=dotproduct(vector,minoraxis);
	double scaling=sqrt(majorsize*majorsize+minorsize*minorsize);
	*costhi=majorsize/scaling;
	*sinthi=minorsize/scaling;
}

double orbitelements::thitoradius(double costhi) const
{
	return angularmomentum2/(planet*(eccentricity*costhi+1));
}

double orbitelements::radiustothi(double radius) const
{
	return (angularmomentum2/(planet*radius)-1)/eccentricity;
}

double orbitelements::getpedistance() const
{
	return semimajor*(eccentricity-1);
}

double orbitelements::getapodistance() const
{
	return -semimajor*(eccentricity+1);
}

double orbitelements::getcurrradius() const
{
	return vectorsize(currposition);
}

VECTOR3 orbitelements::getintersectvector(const ORBIT &torbit) const
{
	return crossproduct(planevector, torbit.planevector);
}

double orbitelements::geteccentricity() const 
{
	return eccentricity;
}

double orbitelements::getangmomentum2() const
{
	return angularmomentum2;
}

double orbitelements::getcurrcosthi() const
{
	return currcosthi;
}

double orbitelements::getcurrsinthi() const 
{
	return currsinthi;
}

void orbitelements::getaxes(VECTOR3 *tmajoraxis, VECTOR3 *tminoraxis) const
{
	*tmajoraxis=majoraxis;
	*tminoraxis=minoraxis;
}
	
void orbitelements::getcurrentvectors(VECTOR3 *tpos, VECTOR3 *tvel) const
{
	*tpos=currposition;
	*tvel=currvelocity;
}

double orbitelements::getsemimajor() const
{
	return semimajor;
}

VECTOR3 orbitelements::geteccentricityvector() const
{
	return eccvector;
}

VECTOR3 orbitelements::getplanevector() const
{
	return planevector;
}

double orbitelements::getpedeltatime() const
{
	//Gets time to next Pe passage
	//Deltatime is always time from the last one (unless ecc>1, in which case there is only one)
	double temp;
	if (eccentricity<1)
	{
		
		temp=orbitconstant*(PI+PI)-deltatime;//Time to next passage
	}
	else
	{
		temp=-deltatime; //Only one passage
	}
	return temp;
}

bool orbitelements::isvalid() const
{
	return valid;
}

double orbitelements::getgmplanet() const
{
	return planet;
}





void orbitelements::GetTimesToThi(double costhi, double *time1, double *time2) const

// There are two solutions for any value of costhi
// Useful as costhi can be derived from a radius from a body

{
	if (!valid) return;
	double coscosh;
	if (1+eccentricity*costhi<0)
	{
		//hyperbolic orbit - costhi in range which orbit never reaches
		coscosh=1e8; //Very, very large figure
	}
	else
	{
		coscosh=(eccentricity+costhi)/(1+eccentricity*costhi);
	}
	double sinsinh, e_angle, loc_deltatime;
	if (eccentricity<=1)
	{
		//Elliptical case
		sinsinh=1-coscosh*coscosh;
		if (sinsinh>0)
		{
			sinsinh=sqrt(sinsinh);
			e_angle=acos(coscosh);
		}
		else
		{
			sinsinh=0;
			if (coscosh>0)
			{
				e_angle=0;
			}
			else
			{
				e_angle=PI;
			}
		}
	}
	else
	{
		//Hyperbolic case
		sinsinh=coscosh*coscosh-1;
		if (sinsinh>0)
		{
			sinsinh=sqrt(coscosh*coscosh-1);
			e_angle=log(coscosh+sinsinh);
		}
		else
		{
			sinsinh=0;
			e_angle=0;
		}
	}
	loc_deltatime=orbitconstant*(e_angle-eccentricity*sinsinh); //Gives deltatime from periapsis
	*time1=-deltatime+loc_deltatime;
	*time2=-deltatime-loc_deltatime;//Can be positive if deltatime sufficiently negative
	if (eccentricity<1)
	{
		if (*time1<0) *time1+=orbitconstant*(PI+PI);
		if (*time2<0) *time2+=orbitconstant*(PI+PI);
		if (*time1<0) *time1+=orbitconstant*(PI+PI);
		if (*time2<0) *time2+=orbitconstant*(PI+PI);
	}
	if (*time1>*time2)
	{
		double temp=*time1;
		*time1=*time2;
		*time2=temp;
	}
}


/*void orbitelements::draworbit(HDC hDC, const GRAPH *graph, bool drawradius) const
{
	// Create projection vectors
	if (!valid) return;
	const double xoffset = (graph->ixstart+graph->ixend)/2;
	const double yoffset = (graph->iystart+graph->iyend)/2;
	VECTOR3 xaxis=graph->xaxis;
	VECTOR3 yaxis=graph->yaxis;
	DWORD xstart=graph->ixstart;
	DWORD xend=graph->ixend;
	DWORD ystart=graph->iystart;
	DWORD yend=graph->iyend;
	double scale=graph->scale;
	POINT pointarray[50];
	DWORD numpoints;
	int xpos, ypos;
	double xposd,yposd;
	double step=PI/50;
	double cosstep=cos(step);
	double sinstep=sqrt(1-cosstep*cosstep);
	double currcos=1;
	double currsin=0;
	double currdivisor, currentradius, ratio, difference;
	double tempsin;
	VECTOR3 currvector;
	int ct;
	double topconstant=semimajor*(eccentricity*eccentricity-1);
	do //This loop runs twice to draw both orbit halves
	{
		ct=0;
		currvector=majoraxis*(topconstant/(eccentricity+1));
		xposd= dotproduct(xaxis, currvector)*scale +xoffset;
		yposd= dotproduct(yaxis, currvector)*scale +yoffset;
		if (xposd<xstart || xposd>xend || yposd<xstart || yposd>xend) return; // Makes safe if orbit too large to fit screen!
		xpos=int(xposd);
		ypos=int(yposd);
		MoveToEx (hDC, xpos, ypos, NULL);
		bool exit=false;
		do //This inner loop draws half an orbit from periapsis outwards
		{
			tempsin=currsin;
			currsin=tempsin*cosstep+currcos*sinstep;
			currcos=currcos*cosstep-tempsin*sinstep;
			currdivisor=eccentricity*currcos+1;
			if (currdivisor<=0)  //Puts in special final line in hyperbolic orbits
			{
				currcos=-1/eccentricity+0.0001;
				currsin=sqrt(1-currcos*currcos);
				if (sinstep<0) currsin=-currsin;
				currdivisor=eccentricity*currcos+1;
				exit=true;
			}
			currentradius=topconstant/currdivisor;
			currvector=majoraxis*(currcos*currentradius)+minoraxis*(currentradius*currsin);
			xposd=dotproduct(xaxis,currvector)*scale+xoffset;
			yposd=dotproduct(yaxis,currvector)*scale+yoffset;
			// Following four if statements ensure proper truncation of line segments attempting to draw
			// beyond the boundaries of the MFD
			if (xposd<xstart)
			{
				ratio=(pointarray[ct-1].x-xstart)/(pointarray[ct-1].x-xposd);
				difference=yposd-pointarray[ct-1].y;
				yposd=difference*ratio+pointarray[ct-1].y;
				xposd=xstart;
				exit=true;
			}
			if (xposd>xend)
			{
				ratio=(xend-pointarray[ct-1].x)/(xposd-pointarray[ct-1].x);
				difference=yposd-pointarray[ct-1].y;
				yposd=difference*ratio+pointarray[ct-1].y;
				xposd=xend;
				exit=true;
			}
			if (yposd<ystart)
			{
				ratio=(pointarray[ct-1].y-ystart)/(pointarray[ct-1].y-yposd);
				difference=xposd-pointarray[ct-1].x;
				xposd=difference*ratio+pointarray[ct-1].x;
				yposd=ystart;
				exit=true;
			}
			if (yposd>yend)
			{
				ratio=(yend-pointarray[ct-1].y)/(yposd-pointarray[ct-1].y);
				difference=xposd-pointarray[ct-1].x;
				xposd=difference*ratio+pointarray[ct-1].x;
				yposd=yend;
				exit=true;
			}
			pointarray[ct].x=int(xposd); //Thunks down
			pointarray[ct].y=int(yposd); //to integer
			if (++ct>49) exit=true;
		}
		while (!exit);
		numpoints=DWORD(ct);
		PolylineTo(hDC, pointarray, numpoints);
		sinstep=-sinstep;
		currcos=1;
		currsin=0;
		ct=0;
	}
	while (sinstep<0); // Only true after the first pass - false after the second
	if (drawradius)
	{
		xpos=(int) xoffset;
		ypos=(int) yoffset;
		MoveToEx (hDC, xpos, ypos, NULL);
		currvector=currposition;
		xpos= int(dotproduct(xaxis, currvector)*scale +xoffset);
		ypos= int(dotproduct(yaxis, currvector)*scale +yoffset);
		LineTo(hDC, xpos, ypos);
	}
}

*/




void orbitelements::timetovectors(double timefromnow,ORBITTIME *posvel) const
{
	if (!valid) return;
	double timetarget=timefromnow+deltatime;

	if (eccentricity<1)
	{//Adjust to nearest orbit - simple optimisation which has no effect on result
		double orbittime=orbitconstant*2*PI;
		timetarget-=int(timetarget/orbittime+0.5)*orbittime;
	}
	if (posvel->processed==true)
	{//We've scanned this structure already, go ahead and improve on it
		if (improve(timetarget,posvel)) return;
	}
	//At this point, we have a not very initialised structure to sort out
	double topthi;
	if (eccentricity>1)//split off the hyperbolic from the elliptical, and handle differently
	{
		topthi=-1/eccentricity+0.2;//Not at infinity - a bit closer
		double timeattopthi=simpletimetothi(topthi,timetarget);
		if (fabs(timeattopthi)<timetarget)//In asymptote region, work by radius at first
		{
			improvebyradius(timetarget,topthi,timeattopthi,posvel);
		}
		else
		{
			improvebysubdivision(timetarget,topthi,timeattopthi,posvel);
		}
	}
	else
	{
		topthi=-1;
		double timeattopthi=orbitconstant*PI;
		improvebysubdivision(timetarget,topthi,timeattopthi,posvel);
	}
	improve(timetarget,posvel);//improves, and converts to vectors
	posvel->processed=true;
}

void orbitelements::improvebyradius(double timetarget,double topthi,double timeattopthi,ORBITTIME *posvel) const
{
	double radius=thitoradius(topthi);
	bool reversed=false;
	if (timetarget<0)
	{
		timetarget=-timetarget;
		timeattopthi=-timeattopthi;
		reversed=true;
	}
	double outvel;
	do
	{
		outvel=eccentricity*planet*sqrt((1-topthi*topthi)/angularmomentum2);
		radius=(timetarget-timeattopthi)*outvel+radius;
		timeattopthi=GetTimeToRadius(radius,true);
	}
	while (fabs(timetarget-timeattopthi)*1e8>timetarget);
	topthi=radiustothi(radius);
	posvel->icosthi=topthi;
	posvel->currangle=acos(topthi);
	if (reversed) posvel->currangle=-posvel->currangle;
}

void orbitelements::improvebysubdivision(double timetarget,double topthi,double timeattopthi,ORBITTIME *posvel) const
{
	double angmomentum=sqrt(angularmomentum2);
	bool reversed=false;
	double lowthi=1;
	double timeatlowthi=0;
	double ratio;
	double guess;
	double timeatguess;
	if (timetarget<0)
	{
		timetarget=-timetarget;
		reversed=true;
	}
	for (int a=0;a<10;a++)
	{
		if ((timeattopthi-timeatlowthi)*1e7<timeattopthi+timeatlowthi) break;
		ratio=(topthi-lowthi)/(timeattopthi-timeatlowthi);
		guess=(timetarget-timeattopthi)*ratio+topthi;
		timeatguess=simpletimetothi(guess,timetarget);
		if (timeatguess>timetarget)
		{
			timeattopthi=timeatguess;
			topthi=guess;
		}
		else
		{
			timeatlowthi=timeatguess;
			lowthi=guess;
		}
	}
	posvel->icosthi=guess;
	guess=acos(guess);
	if (reversed) guess=-guess;
	posvel->currangle=guess;
	return;
}






bool orbitelements::improve(double timetarget,ORBITTIME *posvel) const
{
	
	double angmomentum=sqrt(angularmomentum2);
	double timeerr,iradius,iangleerr,icosthi;
	VECTOR3 *tpos=&(posvel->pos);
	VECTOR3 *tvel=&(posvel->vel);
	double tempangle=posvel->currangle;
	icosthi=posvel->icosthi;
	do
	{
		timeerr=simpletimetothi(icosthi,timetarget)-timetarget;
		iradius=thitoradius(icosthi);
		iangleerr=timeerr*angmomentum/(iradius*iradius);
		if (fabs(iangleerr)>0.5) return false;
		tempangle-=iangleerr;
		icosthi=cos(tempangle);
	}
	while (fabs(iangleerr)>1e-6);
	posvel->currangle=tempangle;
	posvel->icosthi=icosthi;
	double isinthi=sin(posvel->currangle);
	thitovectors(icosthi,isinthi,tpos,tvel);
	return true;
}




		




void orbitelements::timetovectors(double timefromnow, VECTOR3 *pos, VECTOR3 *vel) const
{
	// Find the position of a planet at timefromnow in the future

	// This routine is written to be most efficient at low eccentricity typical of planet - to find planet positions
	if (!valid) return;
	if (eccentricity>1)
	{
		
		double e_angle;
		double loc_deltatime,delta,ex;
		double to=(timefromnow+deltatime)/orbitconstant; //Required time / orbitconstant
		if (to>PI)
		{
			e_angle=-to-eccentricity/2;
		}
		else
		{
			e_angle=-to+eccentricity/2;
		}
		for (int i=0;i<20;i++){// Should quickly converge to correct value of e_angle
			ex=exp(e_angle);
			delta=(eccentricity*(ex-1/ex)*0.5-e_angle+to)/(1-eccentricity*(ex+1/ex)*0.5); //(sinh and cosh are the ex expressions)
			e_angle+=delta;
		}
		ex=exp(e_angle);
		loc_deltatime=orbitconstant*(e_angle-eccentricity*(ex-1/ex)*0.5); //Probably still slightly different from required time
		double cosh_e=(ex+1/ex)*0.5;
		double costhi=(eccentricity-cosh_e)/(eccentricity*cosh_e-1);
		double sinthi=1-costhi*costhi;
		if (sinthi>0)
		{
			sinthi=sqrt(sinthi);
		}
		else
		{
			sinthi=0;
		}
		if (ex-1/ex>0) sinthi=-sinthi;
		thitovectors(costhi, sinthi, pos, vel);
		// Finally adjust the vectors using deltatime
		*pos=*pos+*vel*(timefromnow-loc_deltatime+deltatime); //Make final (small) correction to deltatime error
	}
	else
	{
		double e_angle;
		// Assume that the first initial e_angle is thi (present position)! 
		double loc_deltatime,delta;
		double to=(timefromnow+deltatime)/orbitconstant; //Required time / orbitconstant
		if (to>PI)
		{
			e_angle=to-eccentricity/2;
		}
		else
		{
			e_angle=to+eccentricity/2;
		}		
		for (int i=0;i<20;i++){// Should quickly converge to correct value of e_angle
			delta=(eccentricity*sin(e_angle)-e_angle+to)/(1-eccentricity*cos(e_angle));
			e_angle+=delta;
		}
		loc_deltatime=orbitconstant*(e_angle-eccentricity*sin(e_angle)); //Probably still slightly different from required time
		double cos_e=cos(e_angle);
		double costhi=(eccentricity-cos_e)/(eccentricity*cos_e-1);
		double sinthi=sin(e_angle)*sqrt(1-eccentricity*eccentricity)/(1-eccentricity*cos_e);
		thitovectors(costhi, sinthi, pos, vel);
		// Finally adjust the vectors using deltatime
		*pos=*pos+*vel*(timefromnow-loc_deltatime+deltatime); //Make final (small) correction to deltatime error
	}
}


void orbittime::getposvel(VECTOR3 *tpos,VECTOR3 *tvel)
{
	*tpos=pos;
	*tvel=vel;
}


#endif