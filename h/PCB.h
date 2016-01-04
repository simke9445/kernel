#ifndef PCB_H_
#define PCB_H_

#include "thread.h"
#include "DEF.h"

// running - PCB of the current running thread in the system
// sleepQueue - Queue of sleeping processes
// waitQueue - Queue of processes that are waiting for the one to finish

class Thread;
class PriorityQueue;
class Queue;

class PCB {
private:
	StackSize myStackSize;
	Time myTimeSlice;
public:
	ID id;
	
	friend class Kernel;
	friend class Queue;

	static volatile PCB *running;
	static const int NEW, READY, BLOCKED, OVER;

	static PriorityQueue *sleepQueue;
	
	unsigned int sp, ss;

	volatile unsigned int counter;
	unsigned char *mySP;
	
	volatile int status;

	int getTest() const;
	
	Thread *myThread;
	Queue *waitQueue;
	
	PCB(Thread *myThread, ID id, StackSize stackSize, Time timeSlice);
	~PCB();


	void initStack();


	static void wrapper();


};

#endif
