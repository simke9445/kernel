#include "kernelev.h"
#include "PCB.h"
#include "DEF.h"
#include "kernel.h"
#include "SCHEDULE.H"
#include <iostream.h>
#include "entry.h"

void KernelEvent::wait()
{
	if ( this->myPCB == (PCB*)PCB::running) eventSem.wait();
}

void KernelEvent::signal()
{
	eventSem.signal();
}

KernelEvent::KernelEvent(IVTNo num) : eventSem(0)
{
	this->num = num;
	this->myPCB = (PCB*)PCB::running;
	
	// setting the event to the it's IV Table entry spot
	if (IVTEntry::IVTEntries[this->num] != NULL)
		IVTEntry::IVTEntries[this->num]->event = this;
}

KernelEvent::~KernelEvent()
{
	if (IVTEntry::IVTEntries[this->num] != NULL)
		IVTEntry::IVTEntries[this->num]->event = NULL;
}