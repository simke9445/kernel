#include "entry.h"
#include "DEF.h"
#include <iostream.h>
#include "event.h"
#include <dos.h>
#include "kernelev.h"
#include "kernel.h"

IVTEntry* IVTEntry::IVTEntries[256];
volatile EntryList* IVTEntry::myEntryList = new EntryList();

IVTEntry::IVTEntry(IVTNo num, InterruptRoutine newRoutine)
{
	lock();
	this->event = NULL;
	this->num = num;
	this->oldRoutine = Kernel::replaceRoutine(num, newRoutine);
	IVTEntry::IVTEntries[num] = this;
	unlock();
}

IVTEntry::~IVTEntry()
{
	lock();
	Kernel::replaceRoutine(this->num, this->oldRoutine);
	this->event = NULL;
	
	// if the entry num is the keyboard interrupt entry(9)
	if (this->num == 9) 
	{
		asm mov al, 20h;//send End-Of-Interrupt signal
		asm out 20h, al;//to the Interrupt controller
	}
	unlock();
}


IVTEntry* IVTEntry::getEntry(IVTNo num)
{
	return IVTEntry::IVTEntries[num];
}

void IVTEntry::signal()
{
	if ( event != NULL ) 
	{
		event->signal();
	}
}

void IVTEntry::callOldRoutine()
{
	(*this->oldRoutine);
}



