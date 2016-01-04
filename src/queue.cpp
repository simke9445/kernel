#include "queue.h"
#include <iostream.h>
#include "DEF.h"
#include "SCHEDULE.H"



Elem::Elem(PCB *pcb)
{
	this->pcb = pcb;
	this->next = NULL;
}

Queue::Queue()
{
	first = NULL;
	size = 0;
}

Queue::~Queue()
{
	static Elem *temp;
	temp = first;
	while( temp != NULL )
	{
		first = first->next;
		delete temp;
		temp = first;
		size--;
	}
	first = NULL;
	size = 0;
}

Queue::getSize() const 
{
	return size;
}

PCB* Queue::pop()
{
	if ( first == NULL ) return NULL;
	
	static Elem *temp;
	temp = first;
	static PCB *tempPCB;
	tempPCB = first->pcb;
	first = first->next;
	delete temp;
	size--;
	return tempPCB;
}

void Queue::push(PCB *pcb)
{
	if( first == NULL) 
	{
		lock();
		first = new Elem(pcb);
		unlock();
		size++;
		
	}
	else
	{
		static Elem *temp;
		temp = first;
		while( temp->next != NULL)
		{
			if( temp->pcb == pcb ) 
			{
				return;
			}
			temp = temp->next;
		}

		if( temp->pcb == pcb)
		{
			return;
		}
		lock();
		temp->next = new Elem(pcb);
		unlock();
		size++;
	}
	
}


SleepElem::SleepElem(PCB *pcb, Time sleepTime)
{
	this->pcb = pcb;
	this->sleepTime = sleepTime;
	this->next = NULL;
}


PriorityQueue::PriorityQueue()
{
	first = NULL;
	size = 0;
}

PriorityQueue::~PriorityQueue()
{
	static SleepElem *temp ;
	temp = first;
	while(first != NULL)
	{
		temp = temp->next;
		delete first;
		first = temp;
		size--;
	}
}

PCB* PriorityQueue::pop()
{
	if( first == NULL ) return NULL;
	
	static SleepElem *tempSleepElem;
	tempSleepElem = first;
	static PCB *tempPCB;
	tempPCB = first->pcb;
	first = first->next;
	delete tempSleepElem;
	size--;
	
	return tempPCB;
	
	
}

void PriorityQueue::push(PCB *pcb, Time sleepTime)
{
	static SleepElem *temp, *pred, *curr;
	temp = first;
	pred = NULL;
	
	if ( first == NULL ) 
	{
		lock();
		first = new SleepElem(pcb, sleepTime);
		unlock();
		size++;
		return;
	}
	
	while( temp != NULL && temp->sleepTime <= sleepTime ) 
	{
		pred = temp;
		temp = temp->next;
	}
	
	if( temp == NULL ) 
	{
		lock();
		pred->next = new SleepElem(pcb, sleepTime);
		unlock();
	}
	else 
	{
		if( pred == NULL ) 
		{
			lock();
			temp = new SleepElem(pcb, sleepTime);
			unlock();
			temp->next = first;
			first = temp;
		}
		else
		{
			lock();
			curr = new SleepElem(pcb, sleepTime);
			unlock();
			curr->next = temp;
			pred->next = curr;
		}
	}
	size++;
	
}

void PriorityQueue::update()
{
	static SleepElem *temp;
	static PCB *tempPCB;
	temp = first;
	while( temp != NULL ) 
	{
		if ( temp->sleepTime - 1 <= 0 )
		{
			tempPCB = this->pop();
			tempPCB->status = PCB::READY;
			Scheduler::put(tempPCB);
			temp = first;
		}
		else 
		{
			temp->sleepTime--;
			temp = temp->next;
		}
	}
}

int PriorityQueue::getSize() const { return size; }

