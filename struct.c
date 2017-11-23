#include "CSCIx229.h"
extern int showAABB;
extern object_t *list;
void drawlist()
{
	if(showAABB)
	{
		object_t *end=list;
		while(end != NULL)
		{
			AABBbox(end->coords[0],end->coords[1],end->coords[2],end->coords[3],end->coords[4],end->coords[5],end->coords[6],end->coords[7]);
			end=end->next;
		}
	}
}

bool inoneobject(double x, double y, double z)
{
	object_t *end=list;
	while(end != NULL)
	{
		if(AABB(x,y,z,end->coords[0],end->coords[1],end->coords[2],end->coords[3],end->coords[4],end->coords[5],end->coords[6],end->coords[7]))
		{
			return True;
		}
		end=end->next;
		
	}
	return False;
}
void initobject(struct objects *box)
{
   box->next=NULL;
   box->coords[0]=0;
   box->coords[1]=0;
   box->coords[2]=0;
   box->coords[3]=0;
   box->coords[4]=0;
   box->coords[5]=0;
   box->coords[6]=0;
   box->coords[7]=0;
} 
void appendlist(double x, double y ,double z, double dx, double dy,double dz, double th, double ph)
{
	object_t *data=malloc(sizeof(struct objects));
	data->next=NULL;
	data->coords[0]=x;
	data->coords[1]=y;
	data->coords[2]=z;
	data->coords[3]=dx;
	data->coords[4]=dy;
	data->coords[5]=dz;
	data->coords[6]=th;
	data->coords[7]=ph;
	if(list==NULL)
	{
		list=data;
	}
	else
	{
		data->next=list;
		list=data;
	}
}
void deleteobjects()
{
	object_t *end=list;
	while(end != NULL)
	{
		end=list->next;
		free(list);
		list=end;
	}
	//list=NULL;
}
