

#ifndef _VECTORS_
#define _VECTORS_


#include "matrix.h"
#include <math.h>
class matrix;
class vector3
{ public:
  double x;
  double y;
  double z;

  //constructors
      vector3(double _x,double _y,double _z) {set(_x,_y,_z);}
	  vector3()	{set(0.0,0.0,0.0);}
	  vector3(vector3 &v) {set(v.x,v.y,v.z);}
  //operators
	//vector to vector
  vector3 operator+ (vector3);
  void	  operator+= (vector3);
  vector3 operator- (vector3);
  void    operator-=(vector3);
  vector3 operator* (vector3);
  vector3 operator/ (vector3);
  double operator% (vector3);   //dot product
  vector3 operator! ();
	//vector to double
  vector3 operator+ (double);
  void    operator+=(double);
  vector3 operator- (double);
  void    operator-=(double);
  vector3 operator* (double);
  void    operator*=(double);
  vector3 operator/ (double);
  void    operator/=(double);
    //vector to matrix
  //vector3 operator* (matrix &m);
  //void operator*=(matrix &m);
  //functions
  void set(double _x,double _y,double _z);
  double selfdot();
  vector3 normalize();
  void selfnormalize();
  double length();
  double mod();
  double distance(vector3 &v);
  double angle(vector3 &v); 
  vector3 inplane(vector3 &v1,vector3 &v2); //return the projection of this onto plane v1,v2

};

vector3 _vector3(double _x, double _y, double _z);

//const double PI   = 3.14159265358979;
vector3 vector3::operator + (vector3 p1)
{ vector3 p2;
  p2.x=x+p1.x;p2.y=y+p1.y;p2.z=z+p1.z;
  return p2;
}
void vector3::operator += (vector3 p1)
{ x+=p1.x;y+=p1.y;z+=p1.z;}

vector3 vector3::operator - (vector3 p1)
{ vector3 p2;
  p2.x=x-p1.x;p2.y=y-p1.y;p2.z=z-p1.z;
  return p2;
}
void vector3::operator -= (vector3 p1)
{ x-=p1.x;y-=p1.y;z-=p1.z;}
 
vector3 vector3::operator * (vector3 p1)
{ vector3 p2;
  p2.x=y*p1.z-z*p1.y;
  p2.y=z*p1.x-x*p1.z;
  p2.z=x*p1.y-y*p1.x;
  return p2;
}

vector3 vector3::operator / (vector3 p1)
{ vector3 p2;
  p2.x=x/p1.x;p2.y=y/p1.y;p2.z=z/p1.z;
  return p2;
}

double vector3::operator % (vector3 p1)
{ return x*p1.x+y*p1.y+z*p1.z;
}

vector3 vector3::operator + (double add)
{ vector3 p2;
  p2.x=x+add;
  p2.y=y+add;
  p2.z=z+add;
  return p2;
}
vector3 vector3::operator - (double add)
{ vector3 p2;
  p2.x=x-add;
  p2.y=y-add;
  p2.z=z-add;
  return p2;
}

vector3 vector3::operator *(double add)
{ vector3 p2;
  p2.x=x*add;
  p2.y=y*add;
  p2.z=z*add;
  return p2;
}

vector3 vector3::operator /(double add)
{ vector3 p2;
  p2.x=x/add;
  p2.y=y/add;
  p2.z=z/add;
  return p2;
}

void vector3::operator += (double add)
{ 
  x+=add;
  y+=add;
  z+=add;  
}

void vector3::operator -= (double add)
{ 
  x-=add;
  y-=add;
  z-=add;  
}

void vector3::operator *= (double add)
{ 
  x*=add;
  y*=add;
  z*=add;  
}

void vector3::operator /= (double add)
{ 
  x/=add;
  y/=add;
  z/=add;  
}

//vector3 vector3::operator * (matrix &m)
//{ 
//	return (m*(*this));
//}

//void vector3::operator *= (matrix &m)
//{ 
//	*this=*this*m; 
//}

vector3 vector3::operator ! ()
{vector3 temp(-x,-y,-z);
 return temp;
};
void vector3::set(double _x,double _y,double _z)
{ x=_x;y=_y;z=_z;};

double vector3::selfdot()
{ return x*x+y*y+z*z;
}
vector3 vector3::normalize()
{ 
  vector3 temp;
  temp=*this * (double)(1.0/sqrt(selfdot()));
  return temp;
}

void vector3::selfnormalize()
{ 
	*this*=(double)1.0/sqrt(selfdot());
}
double vector3::length()
{ return sqrt(x*x+y*y+z*z);
}
double vector3::mod()
{ return sqrt(x*x+y*y+z*z);
}

double vector3::distance(vector3 &v)
{return (*this-v).length();
}

double vector3::angle(vector3 &v)
{ return (double) acos((*this % v)/((*this).mod()*v.mod())); //acos( (p1%p2) / ([p1]*[p2]) )
}

vector3 vector3::inplane(vector3 &v1,vector3 &v2)
{
	vector3 norm,n2,n3;
	
	norm=(v1 * v2); //normal to the plane
	double ang=PI/2-norm.angle(*this);//angle of us to that plane
	norm.selfnormalize();
	n2=(norm * (*this)); //and the normal to second plane
	n2.selfnormalize();
	n3=(n2* norm);
	n3.selfnormalize(); //find the intersection line
    n3*=(*this).mod()*cos(ang); //then the magnitude of the vector
	return n3;//and voila

}

vector3 _vector3(double _x, double _y, double _z)
{ vector3 temp(_x,_y,_z);
 return temp;
};

#endif