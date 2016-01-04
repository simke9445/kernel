#ifndef KERNELEVENT_H_
#define KERNELEVENT_H_

#include "kernelse.h"

typedef unsigned char IVTNo;

class PCB;
class KernelSem;

class KernelEvent
{
private:
	IVTNo num;
	PCB *myPCB;
	KernelSem eventSem;
public:
	KernelEvent(IVTNo num);
	~KernelEvent();
	
	void wait();
	void signal();
	
	friend class Event;
	friend class Kernel;
	friend class IVTEntry;
};


#endif