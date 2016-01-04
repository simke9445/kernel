#include "kernelse.h"
#include "queue.h"
#include "PCB.h"
#include "SCHEDULE.H"
#include "kernel.h"

KernelSem::KernelSem(int initValue)
{
	this->value = initValue;
	lock();
	this->waitQueue = new Queue();
	unlock();
}

KernelSem::~KernelSem()
{
	static PCB *tempPCB;
	this->value = 0;
	
	// allow passage to all that were waiting for signal
	while( this->waitQueue->getSize() > 0 )
	{
		tempPCB = this->waitQueue->pop();
		tempPCB->status = PCB::READY;
		Scheduler::put(tempPCB);
	}
	delete waitQueue;
}

void KernelSem::wait()
{
	if ( --value < 0 )
	{
		PCB::running->status = PCB::BLOCKED;
		waitQueue->push((PCB*)PCB::running);
	}
}

void KernelSem::signal()
{
	static PCB* tempPCB;
	if( value++ < 0)
	{
		tempPCB = waitQueue->pop();
		tempPCB->status = PCB::READY;
		Scheduler::put(tempPCB);
	}
}

int KernelSem::val() const
{
	return value;
}