#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

//-------------------------------------------------------------------------------------------

#define SCHEDULER_MAX_ITEMS    1000 // Maximum amount of simultaneous objects into the scheduler

//-------------------------------------------------------------------------------------------

typedef struct SCHEDULER_ITEM
{
	void *object;               // A generic pointer to an object of an undetermined type
	void remover(void *item);   // A function pointer that removes the pointed object
	void *state(void *item);    // A function pointer to the current state function
} SCHEDULER_ITEM;

// Static scheduler items
SCHEDULER_ITEM schItems[SCHEDULER_MAX_ITEMS];
int schItemCount = 0;

//-------------------------------------------------------------------------------------------

/* Add an object to the scheduler
 *  object:  a pointer to an object of any kind. It can be NULL.
 *  remover: function that removes the object. It can be NULL, but the object will not be removed.
 *  state:   function to be called each frame with the object as parameter. Mandatory!
 *           void *myAction(TypeOfTheObject *object) 
 *           {
 *	             ...
 *	             return myAction; // Return NULL in order to remove the object and the scheduler item
 *           }
 */
void schItemAdd(void *object, void *remover, void *state)
{
	if(schItemCount == SCHEDULER_MAX_ITEMS) // No space enough?
		return;
	
	SCHEDULER_ITEM *item = schItems + schItemCount; // Get a pointer to the next last scheduler item
	schItemCount += 1; // Increase the item counter;
	
	item->object = object; // Init the scheduler item 
	item->state = state;
	item->remover = remover;
}

//-------------------------------------------------------------------------------------------

/* Remove all the objects and items in the scheduler
 *  
 */
void schItemRemoveAll()
{
	SCHEDULER_ITEM *item = schItems;
	SCHEDULER_ITEM *itemLast = item + schItemCount;
	for(; item < itemLast; ++item)
		if(item->remover != NULL)
			item->remover(item->object);
	schItemCount = 0;
}

//-------------------------------------------------------------------------------------------

/* Scheduler
 *  
 */
void scheduler()
{
	int index = 0;
	SCHEDULER_ITEM *item = schItems;
	for(; index < schItemCount; ++index, ++item) // Loop through the whole list of scheduled items
	{
		// Call the state function with the object as parameter and save the return on itself, so next time will call the current return
		item->state = item->state(item->object); 
		
		// Continue through the list when the return is not NULL
		if(item->state != NULL)
			continue;
		
		// Otherway, proceed to delete the object
		if(item->remover != NULL) // Call the object remover function stated at itemAdd function call
			item->remover(item->object); 
		
		// Decrease the item counter
		schItemCount -= 1; 
		
		// Copy the last scheduled item to the place where the deleted one is
		memcpy(item, schItems + schItemCount, sizeof(SCHEDULER_ITEM)); 
		// This is the fastest deletion but sorting gets messed. 
		// In case the sorting is important, it will need to copy the whole tail down instead.
		// memcpy(item, item + 1, sizeof(SCHEDULER_ITEM) * (schItemCount - index))
		
		// Decrease the for loop variables, so it computes the same item again, the item overwritten by the memcpy instruction
		index -= 1;
		item -= 1;
	}
}

#endif