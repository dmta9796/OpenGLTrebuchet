#ifndef CSCIx229
#define CSCIx229

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define Cos(th) (cos(3.1415926/180*(th)))
#define Sin(th) (sin(3.1415926/180*(th)))

#ifdef __cplusplus
extern "C" {
#endif
typedef enum 
{
	False,True
}bool;
typedef struct objects
{
	double coords[8]; //{x,y,z,dx,dy,dz,th,ph}
	int type; // ids: 0=trebuchet, 1= everything else
	struct objects *next;
}object_t;
typedef struct projectiles
{
	double coords[8];
	int collision;
	struct projectiles *next;
}proj_t;
 
//struct operations
void appendlist(double x, double y ,double z, double dx, double dy,double dz, double th, double ph,int type);
bool inoneobject(double x, double y, double z);
bool inworldobject(double x, double y, double z);
void drawlist();
void deleteobjects();
void AABBbox(double x, double y, double z, double dx, double dy, double dz,double th, double ph);
object_t* fetchtrebuchetdata();

//other operations
void Print(const char* format , ...);
void Fatal(const char* format , ...);
unsigned int LoadTexBMP(const char* file);
void Project(double fov,double asp,double dim, int mode);
void ErrCheck(const char* where);
int  LoadOBJ(const char* file);
int AABB(double movex, double movey, double movez, double x, double y ,double z, double dx, double dy,double dz, double th, double ph);
void projectile(double x, double y, double z, double dx, double dy, double dz, double th, double ph);
#ifdef __cplusplus
}
#endif

#endif
