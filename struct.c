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
object_t* fetchtrebuchetdata()
{
	object_t *end=list;
	while(end != NULL)
	{
		if(end->type==0)
		{
			return end;
		}
		end=end->next;
	}
	return NULL;
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
bool inworldobject(double x, double y, double z)
{
	object_t *end=list;
	while(end != NULL)
	{

		if(end->type==1 && AABB(x,y,z,end->coords[0],end->coords[1],end->coords[2],end->coords[3],end->coords[4],end->coords[5],end->coords[6],end->coords[7]))
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
   box->coords[0]=0; //x
   box->coords[1]=0; //y
   box->coords[2]=0; //z
   box->coords[3]=0; //dx
   box->coords[4]=0; //dy
   box->coords[5]=0; //dz
   box->coords[6]=0; //th
   box->coords[7]=0; //ph
   box->type=0; //0=trebuchet, 1= everything else
} 
void appendlist(double x, double y ,double z, double dx, double dy,double dz, double th, double ph,int type)
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
	data->type=type;
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
