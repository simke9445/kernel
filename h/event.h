#ifndef EVENT_H_
#define EVENT_H_


#include "entrylst.h"
#include "entry.h"
#include "thread.h"

typedef unsigned char IVTNo;

class KernelEvent;
class IVTEntry;

class Event
{
private:
	ID myID;
public:
	Event(IVTNo num);
	~Event();
	
	void wait();
	
	friend class IVTEntry;
	friend class Kernel;
	
protected:
	friend class KernelEvent;
	void signal();
};

#endif