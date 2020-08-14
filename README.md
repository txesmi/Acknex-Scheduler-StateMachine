A simple object oriented function scheduler and state machine manager all in one.

The basis is the SCHEDULER_ITEM data structure which saves each scheduled object data. 

	typedef struct SCHEDULER_ITEM
	{
		void *object;
		void remover(void *item);
		void *state(void *item);
	} SCHEDULER_ITEM;
	
	SCHEDULER_ITEM schItems[SCHEDULER_MAX_ITEMS];
	int schItemCount = 0;

The scheduler works over a static list of SCHEDULER_ITEM and loops through it calling every scheduled state function each frame. The list starts empty. The objects are added to the scheduler through 'schItemAdd' which adds a new SCHEDULER_ITEM to the list. The list items are deleted when their state function call returns NULL. The object can also be removed at the same time.

You would only need to include 'scheduler.h', set 'scheduler' function as 'on_frame' event and set 'schItemRemoveAll' as 'on_exit' event. You might, of course, call these functions differently. f.e: as a part of larger looping or closing functions.

	#include <acknex.h>
	#include "scheduler.h"
	
	void main()
	{
	   on_frame = scheduler;
	   on_exit = schItemRemoveAll;
	}

At this point, you can start adding objects to the scheduler. Lets imagine we want to make an entity on WED be managed by the scheduler. It will need assign an action to it, as it is commonly done, so the engine would call the action on level load.

An action is usually composed by three blocks that can be built as three functions:
- initialization
- loop
- deletion

The initialization remains in the action, but the loop content and the deletion are passed to the scheduler as parameters of 'schItemAdd' in the form of functions. Something like the following:

	void *crazyLoop(ENTITY *ent)
	{
		ent->x = random(100) - 50;
		ent->y = random(100) - 50;
		ent->z = random(100) - 50;
		
		if (key_space)
			return NULL;
		
		return crazyLoop;
	}
	
	void crazyRemove(ENTITY *ent)
	{
		str_remove((STRING*)ent->skill1);
		ent_remove(ent);
	}
	
	action actCrazy()
	{ 
		my->skill1 = str_create("my crazy name");
		
		schItemAdd(me, crazyRemove, crazyLoop);
	}

This setup will start by the action asociated to an entity in WED which creates a string, saves it into a skill, and includes the entity into the scheduler list with its removing and looping functions.

The removing function removes the string created on initialization and the entity itself.

The looping function needs to be of a particular signature:

	void *loopingFunction(TypeOfTheObject *object)
	{
		...
		return loopingFunction;
	}

The return of the function must be a function (void* or NULL) and the parameter should be of the type of the object, an 'ENTITY*' in the example. As long as a function returns its function name, it will act as a loop with a 'wait'. When NULL is returned, the scheduler calls the object deletion function and also deletes the scheduler item from its internal list.

You will need to call 'schItemRemoveAll' before level changes.

All this function returning stuff might sound strange but it is mandatory because the scheduler function is running over the static array and it should not be modified by other functions while its execution, which executes every scheduled object function. That is why the looping function must return something in order to make the scheduler know that the list have to be shortened when removing an an object and modify the list localy inside the scheduler. Once it needs to take care of the return of the looping functions, it can return pointers to functions by the same prize, which results in an object oriented state machine manager. In the other hand, calling 'schItemAdd' from inside an scheduled function is totally safe.

The returning function pointer is the function that will be called next frame. This way you can build each state as a separate function and control the execution flux by the return of the functions.

Notice that you can include items in the scheduler with no object or remover function. If there is no object, the scheduler will call the looping function same, but with a null parameter. It might serve to manage global variable changes or whatever. If there is no remover function, the scheduler will not remove the object associated.

Also notice that the object can be of any kind, entities, panels, bitmaps, custom structs, whatever you need. It is like having engine actions for everything.