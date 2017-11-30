#include "CSCIx229.h"
extern int showAABB;
extern proj_t *projlist;

void initobject(proj_t *box)
{
   box->next=NULL;
   box->coords[0]=0; //x
   box->coords[1]=0; //y
   box->coords[2]=0; //z
   box->coords[3]=0; //dx
   box->coords[4]=0; //dy
   box->coords[5]=0; //dz
   box->coords[6]=0; //th
   box->coords[7]=0; //ph
   box->collision=0; //0=no collision, 1= collided
} 
void appendprojlist(double x, double y ,double z, double dx, double dy,double dz, double th, double ph,int hit)
{
	proj_t *data=malloc(sizeof(struct projectiles));
	data->next=NULL;
	data->coords[0]=x;
	data->coords[1]=y;
	data->coords[2]=z;
	data->coords[3]=dx;
	data->coords[4]=dy;
	data->coords[5]=dz;
	data->coords[6]=th;
	data->coords[7]=ph;
	data->collision=hit;
	if(projlist==NULL)
	{
		projlist=data;
	}
	else
	{
		data->next=projlist;
		projlist=data;
	}
}
void deleteobjects()
{
	proj_t *end=projlist;
	while(end != NULL)
	{
		end=projlist->next;
		free(projlist);
		projlist=end;
	}
	//list=NULL;
}
