#include "CSCIx229.h"
extern int showAABB;
void AABBbox(double x, double y, double z, double dx, double dy, double dz,double th, double ph)
{
	glPushMatrix();
	glTranslated(x,y,z);
    glRotated(ph,0,0,1);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    glColor3f(1,.3,.3);
	glBegin(GL_LINES);
	//top
	glVertex3f(1,1,1);
	glVertex3f(-1,1,1);
	glVertex3f(-1,1,1);
	glVertex3f(-1,1,-1);
	glVertex3f(-1,1,-1);
	glVertex3f(1,1,-1);
	glVertex3f(1,1,-1);
	glVertex3f(1,1,1);
	//bottom
	glVertex3f(1,-1,1);
	glVertex3f(-1,-1,1);
	glVertex3f(-1,-1,1);
	glVertex3f(-1,-1,-1);
	glVertex3f(-1,-1,-1);
	glVertex3f(1,-1,-1);
	glVertex3f(1,-1,-1);
	glVertex3f(1,-1,1);
	//sides
	glVertex3f(1,1,1);
	glVertex3f(1,-1,1);
	glVertex3f(-1,1,1);
	glVertex3f(-1,-1,1);
	glVertex3f(-1,1,-1);
	glVertex3f(-1,-1,-1);
	glVertex3f(1,1,-1);
	glVertex3f(1,-1,-1);
	glEnd();
	glPopMatrix();
	
}
int AABB(double movex, double movey, double movez, double x, double y ,double z, double dx, double dy,double dz, double th, double ph)
{
	int inobject=0;
	double boxminx=x-1.2*dx;//*Sin(ph)*Cos(th);
	double boxmaxx=x+1.2*dx;//*Sin(ph)*Cos(th);
	double boxminy=y-1.2*dy;//*Sin(th)*Sin(ph);
	double boxmaxy=y+1.2*dy;//*Sin(th)*Sin(ph);
	double boxminz=z-1.2*dz;//*Cos(ph);
	double boxmaxz=z+1.2*dz;//*Cos(ph);
	if(
	(movex >= boxminx && movex <= boxmaxx) &&
	(movey >= boxminy && movey <= boxmaxy) &&
    (movez >= boxminz && movez <= boxmaxz))
	{
		inobject=1;
	}
	return inobject;
}
