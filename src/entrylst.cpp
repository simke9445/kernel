#include <iostream.h>
#include "entry.h"
#include "kernel.h"
#include "entrylst.h"
#include "DEF.h"



EntryListElem::EntryListElem(IVTEntry *entry) 
{ 
	lock();
	this->entry = entry;
	this->next = NULL;
	unlock();
}



EntryList::EntryList()
{
	lock();
	first = NULL;
	last = NULL;
	size = 0;
	unlock();
}

EntryList::~EntryList()
{
	lock();
	doAllMissedInterrupts();
	first = NULL;
	last = NULL;
	size = 0;
	unlock();
}
	
int EntryList::getSize() const { return size;}
void EntryList::push(IVTEntry *entry)
{
	lock();
	if ( first == NULL )
	{
		first = last = new EntryListElem(entry);
	}
	else 
	{
		last = last->next = new EntryListElem(entry);
	}
	size++;
	unlock();
}
	
void EntryList::doAllMissedInterrupts()
{
	lock();
	static EntryListElem *temp;
	temp = first;
	while(temp != NULL)
	{
		first = first->next;
		lock();
		
		// allowing the interrupts here
		asm sti;
		*(temp->entry->oldRoutine);
		unlock();
		
		delete temp;
		temp = first;
		size--;
	}
	
	first = temp;
	size = 0;
	unlock();
}
