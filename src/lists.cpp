#include "lists.h"
#include "PCB.h"
#include "kernelev.h"
#include "kernelse.h"
#include "DEF.h"
#include <iostream.h>


ElemPCB::ElemPCB(PCB *pcb, ID id)
{
	this->id = id;
	this->pcb = pcb;
	this->next = NULL;
}
ElemPCB::~ElemPCB()
{
	delete this->pcb;
}

ElemEv::ElemEv(KernelEvent *kev, ID id)
{
	this->id = id;
	this->kev = kev;
	this->next = NULL;
}
ElemEv::~ElemEv()
{
	delete this->kev;
}


ElemSem::ElemSem(KernelSem *sem, ID id)
{
	this->id = id;
	this->sem = sem;
	this->next = NULL;
}
ElemSem::~ElemSem()
{
	delete this->sem;
}


PCBList::PCBList()
{
	first = NULL;
	last = NULL;
}

void PCBList::add(PCB *pcb, ID id)
{
	lock();
	if( first == NULL ) first = last = new ElemPCB(pcb, id);
	else last = last->next = new ElemPCB(pcb, id);
	unlock();
}

void PCBList::remove(ID id)
{
	static ElemPCB *temp, *pred;
	temp = first;
	pred = NULL;
	while( temp != NULL && temp->id != id ) 
	{
		pred = temp;
		temp = temp->next;
	}
	
	if( pred == NULL ) first = first->next;
	else 
	{
		pred->next = temp->next;
	}
	if( temp == last ) last = pred;
	if( first == NULL ) last = first;
	
	delete temp;
}

PCBList::~PCBList()
{
	static ElemPCB *temp;
	while( first != NULL )
	{
		temp = first->next;
		delete first;
		first = temp;
	}
	first = last = NULL;
}

PCB* PCBList::operator[](ID id)
{
	static ElemPCB *temp;
	temp = first;
	while( temp != NULL && temp->id != id )
	{
		temp = temp->next;
	}
	
	if( temp != NULL ) return temp->pcb;
	return NULL;
}




EventList::EventList()
{
	first = NULL;
	last = NULL;
}

void EventList::add(KernelEvent *ev, ID id)
{
	lock();
	if( first == NULL ) first = last = new ElemEv(ev, id);
	else last = last->next = new ElemEv(ev, id);
	unlock();
}

void EventList::remove(ID id)
{
	static ElemEv *temp, *pred;
	temp = first;
	pred = NULL;
	while( temp != NULL && temp->id != id ) 
	{
		pred = temp;
		temp = temp->next;
	}
	
	if( pred == NULL ) first = first->next;
	else 
	{
		pred->next = temp->next;
	}
	if( temp == last ) last = pred;
	if( first == NULL ) last = first;
	
	delete temp;
}

EventList::~EventList()
{
	static ElemEv *temp;
	while( first != NULL )
	{
		temp = first->next;
		delete first;
		first = temp;
	}
	first = last = NULL;
}

KernelEvent* EventList::operator[](ID id)
{
	static ElemEv *temp;
	temp = first;
	while( temp != NULL && temp->id != id )
	{
		temp = temp->next;
	}
	
	if( temp != NULL ) return temp->kev;
	return NULL;
}




SemList::SemList()
{
	first = NULL;
	last = NULL;
}

void SemList::add(KernelSem *sem, ID id)
{
	lock();
	if( first == NULL ) first = last = new ElemSem(sem, id);
	else last = last->next = new ElemSem(sem, id);
	unlock();
}

void SemList::remove(ID id)
{
	static ElemSem *temp, *pred;
	temp = first;
	pred = NULL;
	while( temp != NULL && temp->id != id ) 
	{
		pred = temp;
		temp = temp->next;
	}
	
	if( pred == NULL ) first = first->next;
	else 
	{
		pred->next = temp->next;
	}
	if( temp == last ) last = pred;
	if( first == NULL ) last = first;
	
	delete temp;
}

SemList::~SemList()
{
	static ElemSem *temp;
	while( first != NULL )
	{
		temp = first->next;
		delete first;
		first = temp;
	}
	first = last = NULL;
}

KernelSem* SemList::operator[](ID id)
{
	static ElemSem *temp;
	temp = first;
	while( temp != NULL && temp->id != id )
	{
		temp = temp->next;
	}
	
	if( temp != NULL ) return temp->sem;
	return NULL;
}