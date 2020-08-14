#include <acknex.h>
#include <default.c>

#include "..\\source\\scheduler.h"

//-------------------------------------------------------------------------------------------
// States
//-------------------------------------------------------------------------------------------

// Prototypes
void *stMoveUp(ENTITY *ent);

// Skill macros
#define limit                skill1
#define limitCounter         skill2
#define loopsMax             skill3
#define loopCounter          skill4

// States
void *stWaitDown(ENTITY *ent)
{
	ent->limitCounter += time_step;
	
	if(ent->limitCounter > ent->limit)
	{
		ent->loopCounter += 1;
		if(ent->loopCounter > ent->loopsMax)
		{
			return NULL;
		}
		
		ent->limit = random(200);
		ent->limitCounter = 0;
		
		return stMoveUp;
	}
	
	return stWaitDown;
}

void *stMoveDown(ENTITY *ent)
{
	ent->z -= time_step * 10;
	
	if(ent->z < ent->limit)
	{
		ent->limit = 4 + random(16);
		ent->limitCounter = 0;
		return stWaitDown;
	}
	
	return stMoveDown;
}

void *stWaitOnTop(ENTITY *ent)
{
	ent->limitCounter += time_step;
	
	if(ent->limitCounter > ent->limit)
	{
		ent->limit = -random(200);
		ent->limitCounter = 0;
		
		return stMoveDown;
	}
	
	return stWaitOnTop;
}

void *stMoveUp(ENTITY *ent)
{
	ent->z += time_step * 10;
	
	if(ent->z > ent->limit)
	{
		ent->limit = 4 + random(16);
		ent->limitCounter = 0;
		
		return stWaitOnTop;
	}
	
	return stMoveUp;
}

//-------------------------------------------------------------------------------------------

void sphereCreate()
{
	if(schItemCount > SCHEDULER_MAX_ITEMS - 1)
		return;
	
	ENTITY *ent = ent_create(SPHERE_MDL, vector(0, random(600) - 300, -random(200)), NULL);
	ent->limit = random(200);
	ent->limitCounter = 0;
	ent->loopsMax = 1 + random(4);
	ent->loopCounter = 0;
	
	schItemAdd(ent, ent_remove, stMoveUp);
}

void createMultipleSpheres()
{
	int i = 0;
	for(; i<SCHEDULER_MAX_ITEMS; i+=1)
		sphereCreate();
}

//-------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------

void main()
{
	video_mode = 8;
	max_entities = SCHEDULER_MAX_ITEMS;
	
	level_load("");
	camera->x = -600;
	
	on_frame = scheduler;
	on_exit = schItemRemoveAll;
	
	createMultipleSpheres();
	
	on_space = sphereCreate;
}
