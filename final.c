/*
 *
 *  Demonstrates how to draw a cockpit view for a 3D scene.
 *
 *  Key bindings:
 *  m          Toggle cockpit mode
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  l		   Enable lighting	
 *  ESC        Exit
 */
#include "CSCIx229.h"
// object data
object_t *list;


//world parameters
int axes=0;       //  Display axes
int mode=0;       //  perspective or first person
int th=0;         //  Azimuth of view angle
int ph=-90;         //  Elevation of view angle
int fov=25;       //  Field of view
double asp=1;     //  Aspect ratio
double dim=6;   //  Size of world
//window size
int size=800;


//lighting parameters
int movelight =1; // toggle for idlily moving light
int diffuse=50;
int specular=20;
int ambient=20;
int emission=30;
int light=1; //enable or disable light
double zh=0; //lighting angle of rotation
double distance  =   2;  // Light distance
float ylight  =   8;  // Elevation of light
int local     =   0;  // Local Viewer Model
float shiny   =   1;  // Shininess (value)
int shininess =   0;  // Shininess (power of two)

//terrain parameter
float z[65][65];
float zmag=.1;
float zmin=+1e8;       //  DEM lowest location
float zmax=-1e8;       //  DEM highest location

//debug info show
int showAABB=0;
int showpath=0;
//texture parameters
int stone;
int land;
int wood;
int wood2;
int metal;
int water;
int bark;
int roof;
int tree1;
int    sky[2];
//movement parameters
double dirx=0;
double diry=0;
double dirz=0;
double movex=5;
double movey=1;
double movez=0;
double moveth=0;// angle for the planar movement
double moveph=0;// angle for the spherical cords
int x=300;
int y=300;

//trebuchet movement
double angle=0;
double amp=1;
int firelock=0;
double deltaangle;
int loaded=0;

//generic projectile positions
double projx;
double projy;
double projz;
double t; //time parameter for parabolic trajectory
double fireangle=0;
int hit=0;
int theta;

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
 static void Sky(double x, double y, double z, double D)
{
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   glPushMatrix();
   glTranslated(x,y,z);
   //  Sides
   glBindTexture(GL_TEXTURE_2D,sky[0]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.00,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);

   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);

   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(1.00,1); glVertex3f(-D,+D,-D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glEnd();
   glPopMatrix();

   //  Top and bottom
   glBindTexture(GL_TEXTURE_2D,sky[1]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(1.0,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(1.0,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.0,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(1.0,1); glVertex3f(-D,-D,+D);
   glTexCoord2f(1.0,1); glVertex3f(+D,-D,+D);
   glTexCoord2f(1.0,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(1.0,0); glVertex3f(-D,-D,-D);
   glEnd();

   glDisable(GL_TEXTURE_2D);
}
static void Vertex(double th,double ph)
{
	double x=Sin(th)*Cos(ph);
	double y= Sin(ph);
	double z= Cos(th)*Cos(ph);
	glNormal3d(x,y,z);
	glVertex3d(x , y , z);
}
double *normalplane(double *p1,double *p2, double *p3,double *norm)
{
	double a1[3]={0,0,0};
	double a2[3]={0,0,0};
	a1[0]=p2[0]-p1[0];
	a1[1]=p2[1]-p1[1];
	a1[2]=p2[2]-p1[2];
	a2[0]=p3[0]-p1[0];
	a2[1]=p3[1]-p1[1];
	a2[2]=p3[2]-p1[2];
	//printf("x:%.2f, y:%.2f, z:%.2f",a1[0],a1[1],a1[2]);
	norm[0]=(a1[1]*a2[2])-(a2[1]*a1[2]);
	norm[1]=-(a1[0]*a2[2])+(a2[0]*a1[2]);
	norm[2]=(a1[0]*a2[1])-(a2[0]*a1[1]);
	return norm;
}
static void cone(double x, double y, double z, double dx, double dy,double dz,double th, double ph)
{
	int i = 15;
	int k;
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(th,0,1,0);
	glRotated(ph,0,0,1);
	glScaled(dx,dy,dz);
	//base
    //glColor3f(1,1,1);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(1.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    for (k=0;k<=360;k+=i)
    {
	   glNormal3f(0,0,-1);
       glTexCoord2f(0.5*Cos(k)+0.5,0.5*Sin(k)+0.5);
       glVertex3f(Cos(k),0,Sin(k));
    }
    glEnd();
    
    //sides
    glBegin(GL_QUAD_STRIP);
    for (k=0;k<=360;k+=i)
    {
	   glNormal3f(Cos(45)*Cos(k),Sin(45),Cos(45)*Sin(k));
       glTexCoord2f(0.01*k,0); glVertex3f(0,+1,0);
       glTexCoord2f(0.01*k,1); glVertex3f(Cos(k),-0,Sin(k));
    }
    glEnd();
	glPopMatrix();
}
static void sphere(double x,double y,double z,double dx, double dy, double dz, double th, double ph)
{
   const int d=5;
   //int th,ph;
   float yellow[] = {0,1.0,0.0,1.0};
   float Emission[]  = {0.01*emission,0.005*emission,0.0,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(ph,0,0,1);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  yellow ball
   glColor3f(1,1,0);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,-90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,d-90);
   }
   glEnd();

   //  Latitude bands
   for (ph=d-90;ph<=90-2*d;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,90-d);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
} 
static void cylinder(double x,double y,double z, double r,double d, double th, double ph)
{
   int i,k;
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glRotated(ph,0,0,1);
   glRotated(th,0,1,0);
   glScaled(r,r,d);
   //  Head & Tail
   glColor3f(1,1,1);
   for (i=1;i>=-1;i-=2)
   {
      glNormal3f(0,0,i);
      glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.5,0.5);
      glVertex3f(0.0,0.0,i);
      for (k=0;k<=360;k+=10)
      {
         glTexCoord2f(0.5*Cos(k)+0.5,0.5*Sin(k)+0.5);
         glVertex3f(Cos(k),Sin(k),i);
      }
      glEnd();
   }
   //  Edge
   glBegin(GL_QUAD_STRIP);
   for (k=0;k<=360;k+=10)
   {
      glNormal3f(Cos(k),Sin(k),0);
      glTexCoord2f(0.01*k,0); glVertex3f(Cos(k),Sin(k),+1);
      glTexCoord2f(0.01*k,1); glVertex3f(Cos(k),Sin(k),-1);
   }
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, double ph)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(ph,0,0,1);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   glTexCoord2f(1.0,1.0);glVertex3f(-1,-1, 1);
   glTexCoord2f(1.0,-1.0);glVertex3f(+1,-1, 1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(+1,+1, 1);
   glTexCoord2f(-1.0,1.0);glVertex3f(-1,+1, 1);
   //  Back
   glNormal3f( 0, 0,-1);
   glTexCoord2f(1.0,1.0);glVertex3f(+1,-1,-1);
   glTexCoord2f(1.0,-1.0);glVertex3f(-1,-1,-1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(-1,+1,-1);
   glTexCoord2f(-1.0,1.0);glVertex3f(+1,+1,-1);
   //  Right
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0.5,0.5);
   glTexCoord2f(1.0,1.0);glVertex3f(+1,-1,+1);
   glTexCoord2f(1.0,-1.0);glVertex3f(+1,-1,-1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(+1,+1,-1);
   glTexCoord2f(-1.0,1.0);glVertex3f(+1,+1,+1);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(1.0,1.0);glVertex3f(-1,-1,-1);
   glTexCoord2f(1.0,-1.0);glVertex3f(-1,-1,+1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(-1,+1,+1);
   glTexCoord2f(-1.0,1.0);glVertex3f(-1,+1,-1);
   //  Top
   glNormal3f( 0,+1, 0);
   glTexCoord2f(1.0,1.0);glVertex3f(-1,+1,+1);
   glTexCoord2f(1.0,-1.0);glVertex3f(+1,+1,+1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(+1,+1,-1);
   glTexCoord2f(-1.0,1.0);glVertex3f(-1,+1,-1);
   //  Bottom
   glNormal3f( 0,-1, 0);
   glTexCoord2f(1.0,1.0);glVertex3f(-1,-1,-1);
   glTexCoord2f(1.0,-1.0);glVertex3f(+1,-1,-1);
   glTexCoord2f(-1.0,-1.0);glVertex3f(+1,-1,+1);
   glTexCoord2f(-1.0,1.0);glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}
void weight(double x, double y, double z, double dx, double dy, double dz, double th, double ph)
{
	glPushMatrix();
	//  Offset, scale and rotate
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glRotated(ph,0,0,1);
    glScaled(dx,dy,dz);
	glBegin(GL_QUADS);
	//bottom of weight
	glColor3f(1,1,1);
	glNormal3f(0,-1,0);
	glTexCoord2f(1.0,1.0);glVertex3f(0.1,0.0,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(-0.1,0.0,0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(-0.1,0.0,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(0.1,0.0,-0.1);
	
	// angled bottoms
	glColor3f(1,1,1);
	double n1[3]={0,0,0};
	double p1[3]={0.1,0.0,0.1};
	double p2[3]={0.3,0.1,0.1};
	double p3[3]={0.3,0.1,-0.1};
	normalplane(p1,p2,p3,n1);
	glNormal3f(-n1[0],-n1[1],-n1[2]);
	glTexCoord2f(1.0,1.0);glVertex3f(0.1,0.0,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(0.3,0.1,0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(0.3,0.1,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(0.1,0.0,-0.1);
	
	glColor3f(1,1,1);
	double n2[3]={0,0,0};
	double p4[3]={-0.1,0.0,0.1};
	double p5[3]={-0.3,0.1,0.1};
	double p6[3]={-0.3,0.1,-0.1};
	normalplane(p4,p5,p6,n2);
	glNormal3f(n2[0],n2[1],n2[2]);
	glTexCoord2f(1.0,1.0);glVertex3f(-0.1,0.0,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(-0.3,0.1,0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(-0.3,0.1,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(-0.1,0.0,-0.1);
	
	//top
	glColor3f(1,1,1);
	glNormal3f(0,1,0);
	glTexCoord2f(1.0,1.0);glVertex3f(0.3,0.25,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(0.3,0.25,-0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(-0.3,0.25,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(-0.3,0.25,0.1);
	//high sides
	glColor3f(1,1,1);
	glNormal3f(-1,0,0);
	glTexCoord2f(1.0,1.0);glVertex3f(-0.3,0.1,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(-0.3,0.1,-0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(-0.3,0.25,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(-0.3,0.25,0.1);
	
	glColor3f(1,1,1);
	glNormal3f(1,0,0);
	glTexCoord2f(1.0,1.0);glVertex3f(0.3,0.1,0.1);
	glTexCoord2f(1.0,-1.0);glVertex3f(0.3,0.1,-0.1);
	glTexCoord2f(-1.0,-1.0);glVertex3f(0.3,0.25,-0.1);
	glTexCoord2f(-1.0,1.0);glVertex3f(0.3,0.25,0.1);
	glEnd();
	//code to check normal vector correctness
	//glBegin(GL_LINES);
	//glColor3f(1,0,0);
	//glVertex3d(p4[0],p4[1],p4[2]);
	//glVertex3d(p4[0]+10*n2[0],p4[1]+10*n2[1],p4[2]+10*n2[2]);
	//glEnd();
	
	
	//odd sides
	glBegin(GL_POLYGON);
	glColor3f(1,1,1);
	glNormal3f(0,0,1);
	glTexCoord2f(.3,.25);glVertex3f( .3, .25, .1);
	glTexCoord2f(-.3,.25);glVertex3f(-.3, .25, .1);
	glTexCoord2f(-.3,.1);glVertex3f(-.3, .1 , .1);
	glTexCoord2f(-1.0,0);glVertex3f(-.1,  0 , .1);
	glTexCoord2f(.1,0);glVertex3f( .1,  0 , .1);
	glTexCoord2f(.3,.1);glVertex3f( .3, .1 , .1);
	glTexCoord2f(.3,.25);glVertex3f( .3, .25, .1);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(1,1,1);
	glNormal3f(0,0,-1);
	glTexCoord2f(.3,.25);glVertex3f( .3, .25, -.1);
	glTexCoord2f(-.3,.25);glVertex3f(-.3, .25, -.1);
	glTexCoord2f(-.3,.1);glVertex3f(-.3, .1 , -.1);
	glTexCoord2f(-.1,0);glVertex3f(-.1,  0 , -.1);
	glTexCoord2f(.1,0);glVertex3f( .1,  0 , -.1);
	glTexCoord2f(.3,.1);glVertex3f( .3, .1 , -.1);
	glTexCoord2f(.3,.25);glVertex3f( .3, .25, -.1);
	glEnd();
	glPopMatrix();
}
void terrain(float zmag,int texture)
{
	  glPushMatrix();
	  int i,j;
	  double z0 = (zmin+zmax)/2;
	  glColor3f(1,1,1);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,texture);
      for (i=0;i<64;i++)
      {
         for (j=0;j<64;j++)
         {
            float x=16*i-512;
            float y=16*j-512;
            glBegin(GL_QUADS);
            glTexCoord2f((i+0)/64.,(j+0)/64.); glVertex3d(x+ 0,zmag*(z[i+0][j+0]-z0)-2.8,y+0);
            glTexCoord2f((i+1)/64.,(j+0)/64.); glVertex3d(x+16,zmag*(z[i+1][j+0]-z0)-2.8,y+0);
            glTexCoord2f((i+1)/64.,(j+1)/64.); glVertex3d(x+16,zmag*(z[i+1][j+1]-z0)-2.8,y+16);
            glTexCoord2f((i+0)/64.,(j+1)/64.); glVertex3d(x+ 0,zmag*(z[i+0][j+1]-z0)-2.8,y+16);
            glEnd();
         }
	  }
      glDisable(GL_TEXTURE_2D);
      glPopMatrix();
}
static void treecube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, double ph)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(ph,0,0,1);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0);glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0);glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1);glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1);glVertex3f(-1,+1, 1);
   //  Back
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0);glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0);glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1);glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1);glVertex3f(+1,+1,-1);
   //  Right
   //glNormal3f(+1, 0, 0);
   //glTexCoord2f(0.5,0.5);
   //glTexCoord2f(0,0);glVertex3f(+1,-1,+1);
   //glTexCoord2f(1,0);glVertex3f(+1,-1,-1);
   //glTexCoord2f(1,1);glVertex3f(+1,+1,-1);
   //glTexCoord2f(0,1);glVertex3f(+1,+1,+1);
   //  Left
   //glNormal3f(-1, 0, 0);
   //glTexCoord2f(0,0);glVertex3f(-1,-1,-1);
   //glTexCoord2f(1,0);glVertex3f(-1,-1,+1);
   //glTexCoord2f(1,1);glVertex3f(-1,+1,+1);
   //glTexCoord2f(0,1);glVertex3f(-1,+1,-1);
   //  Top
   //glNormal3f( 0,+1, 0);
   //glTexCoord2f(0,0);glVertex3f(-1,+1,+1);
   //glTexCoord2f(1,0);glVertex3f(+1,+1,+1);
   //glTexCoord2f(1,1);glVertex3f(+1,+1,-1);
   //glTexCoord2f(0,1);glVertex3f(-1,+1,-1);
   //  Bottom
   //glNormal3f( 0,-1, 0);
   //glTexCoord2f(1.0,1.0);glVertex3f(-1,-1,-1);
   //glTexCoord2f(-1.0,1.0);glVertex3f(+1,-1,-1);
   //glTexCoord2f(-1.0,-1.0);glVertex3f(+1,-1,+1);
   //glTexCoord2f(1.0,-1.0);glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}
void tree(double x, double y, double z, double dx, double dy, double dz, double th,double ph)
{
	//generate object last due to transperancy property of foilage
	double rbark=.1;
	glColor4f(1,1,1,1);
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc (GL_ONE,GL_ONE_MINUS_SRC_COLOR);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
	glBindTexture(GL_TEXTURE_2D,tree1);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTranslated(x,y,z);
    glRotated(th,0,0,1);
    glRotated(ph,0,1,0);
    glScaled(dx,dy,dz);
	treecube(0,0,0,1.5,2,.01,0,0);
	treecube(0,0,0,1.5,2,.01,90,0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
    appendlist(x,y,z, 1.1*rbark*dx , 1.75*dy , 1.1*rbark*dz , th , ph,1);
    //appendlist(x,y+.5,z,1.1*.4*dx,1.1*dy,1.1*.4*dz,th,ph,1);
	
} 
void keep(double x, double y,double z, double dx, double dy, double dz,double th, double ph)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,stone);
	glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glRotated(ph,0,0,1);
    glScaled(dx,dy,dz);
    glColor3f(1,1,1);
	cube(0,1,0,1,1,1,0,90);
	cylinder(1,1.5,1,.3,1.5,90,90);
	cylinder(-1,1.5,1,.3,1.5,270,90);
	cylinder(-1,1.5,-1,.3,1.5,90,90);
	cylinder(1,2.0,-1,.5,2.0,270,90);
	//tower tops
	glBindTexture(GL_TEXTURE_2D,roof);
	cone(1,3.0,1,.3,1,.3,90,0);
	cone(1,3.0,1,.4,.5,.4,90,0);
	cone(-1,3.0,1,.3,1,.3,90,0);
	cone(-1,3.0,1,.4,.5,.4,90,0);
	cone(-1,3.0,-1,.3,1,.3,90,0);
	cone(-1,3.0,-1,.4,.5,.4,90,0);
	//large tower
	cone(1,4.0,-1,.5,1.5,.5,90,0);
	cone(1,4.0,-1,.6,.75,.6,90,0);
	
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
    appendlist(x,y+1.5,z,1.6*dx,3.5*dy,1.6*dz,th,ph,1);
}
void trebuchetarm(double x, double y,double z,double dx, double dy, double dz, double th, double ph)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,wood2);
	//  Offset, scale and rotate
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glRotated(ph,0,0,1);
    glScaled(dx,dy,dz);
    glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D,wood);
	//trebuchet arm
	cylinder(0,0,0.0,.08,.8,0,0);
	cylinder(0,0,0.0,.08,1.5,90,90);
	//holder of projectile
	cube(0.0,1.51,0.0,0.1,0.01,0.2,0,0);
	cube(-0.1,1.81,0.0,0.01,0.30,0.2,0,0);
	cube(0.0,1.81,0.2,0.01,0.30,0.1,270,0);
	cube(0.0,1.81,-0.2,0.01,0.30,0.1,90,0);
	// counterweightw
	glBindTexture(GL_TEXTURE_2D,wood2);
	weight(0,-2,0,3,3,3,0,0);
		
	//projectile	
	if(loaded==1)
	{
		sphere(.2,1.71,0,0.2,0.2,0.2,0,0);
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
void roundhouse(double x, double y,double z,double dx, double dy, double dz, double th, double ph)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,wood2);
	//  Offset, scale and rotate
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    glColor3f(1,1,1);
	cylinder(0,0,0,1,1,90,90);
	glBindTexture(GL_TEXTURE_2D,land);
	glColor3f(.8,.8,.2);
	cone(0,1,0,1,1,1,90,0);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	appendlist(x,y,z,1.2*dx,2.3*dy,1.2*dz,th,ph,1);
}
void Trebuchet(double x, double y,double z,double dx, double dy, double dz, double th, double ph)
{ 
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,wood2);
	//  Offset, scale and rotate
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    glColor3f(1,1,1);
    //base
	cube(0.5,0.0,0.5,2.0,0.1,0.1,0,0);
	cube(0.5,0.0,-0.5,  2.0,0.1,0.1,  0,0);
	//arm support
	cube(1.0,1.1, 0.5,  0.1,1.1,0.1,  0,0);
	cube(1.0,1.1,-0.5,  0.1,1.1,0.1,  0,0);
	
	cube(1.4,0.75, 0.5,  0.09,0.80,0.1,  0,30);
	cube(0.6,0.75, 0.5,  0.09,0.80,0.1,  0,-30);
	cube(1.4,0.75, -0.5,  0.09,0.80,0.1,  0,30);
	cube(0.6,0.75, -0.5,  0.09,0.80,0.1,  0,-30);
	//gear support
	cube(-1.0,0.25, 0.5,  0.1,0.25,0.1,0,0);
	cube(-1.0,0.25,-0.5,  0.1,0.25,0.1,0,0);
	//gears
	cylinder(-1.0,0.40,-0.7,.2,.05,0,amp*angle);
	cylinder(-1.0,0.40, 0.7,.2,.05,0,amp*angle);
	//rotation arm
	cylinder(-1.0,0.40, 0.0,.05,0.7,0,amp*angle);
	//arms for gears
	glBindTexture(GL_TEXTURE_2D,metal);
	cylinder(-1.0,0.40,-0.7,.05,.30,90,amp*angle);
	cylinder(-1.0,0.40, 0.7,.05,.30,90,amp*angle);
	cylinder(-1.0,0.40,-0.7,.05,.30,90,90+amp*angle);
	cylinder(-1.0,0.40, 0.7,.05,.30,90,90+amp*angle);
	trebuchetarm(1.0,2.05,0,1,1,1,0,amp*angle);
	projectile(projx,projy,projz,0.2,0.2,0.2,th,ph);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
    appendlist(x+.50,y+1.60,z,2.3*dx,2.2*dy,.8*dz,th,ph,0);
}
void showprojpath(double x, double y, double z, double dx, double dy, double dz, double th, double ph)
{
	int i;
	double xcoord=x;
	double ycoord=y;
	double zcoord=z;
	glPushMatrix();
	glTranslated(x,y,z);
    glRotated(th,1,0,0);
    glRotated(ph,0,1,0);
    glScaled(dx,dy,dz);
    glColor3f(1,.5,1);
    glPointSize(1);
	glBegin(GL_LINE_STRIP);
	for(i=0;i<50000;i++)
	{
		xcoord+=Cos(th);
		ycoord+=ycoord-ycoord*ycoord;
		zcoord+=Sin(th);
		glVertex3d(xcoord,ycoord,zcoord);
	}
	glEnd();
	glPopMatrix();
}
void projectile(double x, double y, double z, double dx, double dy, double dz, double th, double ph)
{
	if(showpath)
	{
		showprojpath(x,y,z,dx,dy,dz,th,ph);
	}
	if(firelock==1 && loaded==0)
	{
		
		sphere(x,y,z,.2,.2,.2,0,0);
	}
}
/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   deleteobjects(list);
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective
   if (mode==0)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  first person view 
   else if(mode==1)
   {
      dirx = +Sin(th)*Cos(ph);
      diry = fmod(+Sin(ph),89);
      dirz = -Cos(th)*Cos(ph);
      gluLookAt(movex,movey,movez , dirx+movex,diry+movey,dirz+movez , 0,Cos(ph),0);
   }
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   if (light)
   {
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance*Sin(zh),ylight,distance*Cos(zh),1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        sphere(Position[0],Position[1],Position[2] , 0.1, 0.1,0.1,0,0);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else 
      glDisable(GL_LIGHTING);
		
   //code for rendering scene
   terrain(0.3,land);
   terrain(0.0,water);
   keep(4,5.2,-6,2,3,2,0,0);
   roundhouse(6,6.3,5,2,.5,.5,0,0);
   roundhouse(4,6.3,3,2,.5,.5,10,0);
   roundhouse(8.5,6.3,3,1,.5,.5,30,0);
   roundhouse(3,6.3,7,.5,.5,.5,0,0);
   //keep(4,6.3,0,1,1,1,0,0);
   //Trebuchet(3.15,0.15,3.05,.75,.75,.75,0,0);
   Trebuchet(-3.15,5.55,-3.05,.75,.75,.75,theta,0);
   tree(-3,6.9,-.5,1,1,1,0,0);
   drawlist();                         // draw bounding boxes
   glDisable(GL_LIGHTING);
   Sky(movex,movey,movez,100);
   //  Draw axes
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Disable Z-buffering in OpenGL
   glDisable(GL_DEPTH_TEST);
   //  Draw cockpit
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f showAABB=%d, move cords=(%.2f,%.2f,%.2f), angle=%.2f,amp=%.2f",th,ph,dim,showAABB,movex, movey, movez,angle,amp);
   glWindowPos2i(5,25);
   Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f, loaded=%d",ambient,diffuse,specular,emission,shiny,loaded);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(fov,asp,dim,mode);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}
/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
   {
      movex=3;
      movey=14;
      movez=15;
      th = ph = 0;
   }
   //  Toggle axes
   else if (ch == '1')
      light = fmod(light+1,2);
   else if (ch == '2')
      axes = fmod(axes+1,2);
   //  Switch display mode
  else if (ch == '3')
      mode = fmod(mode+1,2);
  else if (ch == '4')
      movelight = fmod(movelight+1,2);
  else if (ch == '5')
      showAABB = fmod(showAABB+1,2);
  else if (ch == '6')
      showpath = fmod(showpath+1,2);
  else if (ch == 'q')
      zh -= 5;
  else if (ch == 'Q')
      zh += 5;
  else if (ch == '8' && fov>0)
      fov -= 1;
  else if (ch == '9' && fov<180)
      fov += 1;
  //  Light elevation
  else if (ch=='[')
	  ylight -= 0.1;
  else if (ch==']')
      ylight += 0.1;
  //  Light distance
  else if (ch=='l' && distance >0)
      distance -= 0.1;
  else if (ch=='L')
		distance += 0.1;
  else if (ch=='o')
      theta = fmod(theta+1,360);
  else if (ch=='O')
      theta = fmod(theta-1,360);
  else if(ch=='K')
  {
	  firelock=1;
	  fireangle=angle;
  }
  else if(ch=='k' && angle <110)
  {
	  angle+=1;;
  }
  else if (ch == 'Z' && ambient>0)
	ambient-=1;
  else if (ch == 'z' && ambient<100)
	ambient+=1;
  else if (ch == 'X' && diffuse>0)
	diffuse-=1;
  else if (ch == 'x' && diffuse<100)
	diffuse+=1;
  else if (ch == 'C' && specular>0)
	specular-=1;
  else if (ch == 'c' && specular<100)
	specular+=1;
  else if (ch == 'V' && emission>0)
	emission-=1;
  else if (ch == 'v' && emission<100)
	emission+=1;		
  else if (ch=='B' && shininess>-1)
	shininess -= 1;
  else if (ch=='b' && shininess<7)
	shininess += 1;
  else if (ch == 'w')
  {      

	  double xnew=movex+.1*dirx ;
	  double ynew=movey+.1*diry ;
	  double znew=movez+.1*dirz ;
	  //double z0 = (zmin+zmax)/2;
	  //int i=ceil((xnew+512)/16);
      //int j=ceil((znew+512)/16); 
      //double plane=z[i-1][j-1],z[i-1][j]z[i+1][j+1];
	  if(inoneobject(xnew,ynew,znew)== False)
	  {
		movex= xnew ;
		movey=ynew;//zmag*(z[i][j]-z0)+.5;
		movez= znew;
	  }
  }
  else if (ch == 's')
  {
	  double xnew=movex-.1*dirx ;
	  double ynew=movey-.1*diry ;
	  double znew=movez-.1*dirz ;
	  if(inoneobject(xnew,ynew,znew)== False)
	  {
		movex= xnew;
		movey= ynew;
		movez= znew;
	  }
  }
  else if(ch=='f' && angle!=0 && firelock==0)
  {
	  loaded=1;
  }
  //angle=fmod(angle,360);
  //  Translate shininess power to value (-1 => 0)
  shiny = shininess<0 ? 0 : pow(2.0,shininess);
  //  Reproject
  Project(fov,asp,dim,mode);
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(fov,asp,dim,mode);
}
void mousex(int x0, int y0)
{
	if(x>x0)
	{
		th=th+1;
	}
	else if(x<x0)
	{
		th=th-1;
	}
}
void timer(int time)
{
	//object_t* data=fetchtrebuchetdata();
	//double th=0;
	//double x=0;
	//double y=0;
	//double z=0;
	//if(data!=NULL)
	//{
		//x=data->coords[0];
		//y=data->coords[1];
		//z=data->coords[2];
		//double dx=data->coords[3];
		//double dy=data->coords[4];
		//double dz=data->coords[5];
		//th=data->coords[6];
		//double ph=data->coords[7];
	//}
	if(firelock==1)
	{
		double dt = .075;
		if(angle>0)
		{
			deltaangle-=dt;
		}
		else
		{
			deltaangle+=dt;
		}
		angle=(angle+deltaangle);
		angle=fmod(angle,360);
		amp-=.001;
		if(amp<0)
		{
			amp=1;
			angle=0;
			deltaangle=0;
			firelock=0;
			fireangle=0;
			hit=0;
    	}
    	if(angle<0 && loaded==1)
    	{
			t=0;
			projx=0;
			projy=0;
			projz=0;
			loaded=0;
		}
    	if(loaded==0)
    	{
			if(hit==0)
			{
				t+=dt;
				projx=(fireangle/27)*t;
				projy=3.76+1*t-.1*t*t;
				projz=0;
			}
		}
   }
   
   //  Request display update
   glutPostRedisplay();
   glutTimerFunc(10,timer,-1);
}
void idle()
{
   //  Get elapsed (wall) time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/3000.0;
   //  Calculate spin angle 90 degrees/second
   moveth = fmod(90*t,360);
   //moveph = fmod(90*t,360);
   if(movelight==1)
		zh = fmod(90*t,360.0);
   //  Request display update
   glutPostRedisplay();
}
/*
 *  Start up GLUT and tell it what to do
 */
void ReadDEM(char* file)
{
   int i,j;
   FILE* f = fopen(file,"r");
   if (!f) Fatal("Cannot open file %s\n",file);
   for (j=0;j<=64;j++)
      for (i=0;i<=64;i++)
      {
         if (fscanf(f,"%f",&z[i][j])!=1) Fatal("Error reading saddleback.dem\n");
         if (z[i][j] < zmin) zmin = z[i][j];
         if (z[i][j] > zmax) zmax = z[i][j];
      }
   fclose(f);
}

int main(int argc,char* argv[])
{  
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(size,size);
   glutCreateWindow("Project by Dmitriy Tarasov");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   //glutPassiveMotionFunc(mousex);
   glutTimerFunc(10,timer,-1);
   glutIdleFunc(idle);
   //  Load textures
   land=LoadTexBMP("grass.bmp");
   wood=LoadTexBMP("wood.bmp");
   wood2=LoadTexBMP("wood2.bmp");
   metal=LoadTexBMP("metal.bmp");
   water=LoadTexBMP("water.bmp");
   bark=LoadTexBMP("bark.bmp");
   stone=LoadTexBMP("stone.bmp");
   roof=LoadTexBMP("roof.bmp");
   tree1=LoadTexBMP("tree.bmp");
   sky[0] = LoadTexBMP("sky0.bmp");
   sky[1] = LoadTexBMP("sky1.bmp");
   ReadDEM("saddleback.dem");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
