#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "kernel.h"

typedef unsigned char IVTNo;

class Event;
class KernelEvent;
class Kernel;
class EntryList;

// IVTNo - Interrupt Vector Table Number
// IVTEntries is the Interrupt Vector Table with 256 entries

class IVTEntry
{
private:
	KernelEvent *event;
	InterruptRoutine oldRoutine;
	IVTNo num;
public:
	friend class Event;
	friend class KernelEvent;
	friend class EntryList;
	friend class Kernel;
	
	static IVTEntry* IVTEntries[256];
	
	IVTEntry(IVTNo num, InterruptRoutine newRoutine);
	~IVTEntry();
	
	static IVTEntry* getEntry(IVTNo num);
	static void initializeEntries();
	static void restoreEnties();
	void signal();
	
	static volatile EntryList* myEntryList;
	
	void callOldRoutine();
	
};

// PREPAREENTRY macro generates a interrupt routine for num-th entry,
// substitutes the old one, and preserves it inside num-th entry var.
// While the system is in kernel mode (e.g systemCallFlag == 1),
// all event interrupts are kept pending, and will be proceded when it's possible.
// Interrupt routine itself contains an option of calling the old one,
// or signaling the attached event instead, followed by preemption(dispatch)


#define PREPAREENTRY(num,callOld)\
void interrupt interruptEntry##num(...);\
IVTEntry entry##num(num, interruptEntry##num);\
void interrupt interruptEntry##num(...)\
{\
	if(Kernel::systemCallFlag != 0)\
	{\
		((EntryList*)IVTEntry::myEntryList)->push(&entry##num);\
	}\
	else\
	{\
		if( callOld != 0 ) entry##num.callOldRoutine();\
		entry##num.signal();\
		Kernel::dispatch();\
	}\
}


#endif