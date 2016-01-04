#include "PCB.h"
#include "DEF.h"
#include <dos.h>
#include <stdlib.h>
#include <iostream.h>
#include <kernel.h>
#include "SCHEDULER.H"
#include "queue.h"

volatile PCB* PCB::running = NULL;
PriorityQueue* PCB::sleepQueue = new PriorityQueue();

const int PCB::NEW = 0;
const int PCB::READY = 1;
const int PCB::BLOCKED = 2;
const int PCB::OVER =3;

const StackSize maxStackSize = 536870912; //64KB




PCB::PCB(Thread *myThread, ID myID, StackSize stackSize, Time timeSlice)
{
	this->status = PCB::NEW;
	this->myThread = myThread;
	this->myStackSize = (stackSize > maxStackSize)? maxStackSize : stackSize;
	this->myTimeSlice = timeSlice;
	this->id = myID;
	this->mySP = NULL;
	lock();
	this->waitQueue = new Queue();
	unlock();
	this->counter = 0;
}

PCB::~PCB()
{
	delete mySP;
	delete waitQueue;
}


void PCB::initStack()
{
	// now we simulate the interrupt call so that next time, when we
	// do the context switch, the context gets restored to this process
	
	lock();
	static volatile unsigned int tsp, tss, tip, tcs, oldss, oldsp, thl, thh;
	static unsigned char *tempStack;
	
	// take the pointer to PCB's thread object
	thl = FP_OFF(this->myThread);
	thh = FP_SEG(this->myThread);
	
	
	lock();
	tempStack = new unsigned char[this->myStackSize];
	unlock();

	this->mySP = tempStack;

	// getting sp and ss from the end of the stack
	tsp = this->sp = FP_OFF(tempStack + this->myStackSize);
	tss = this->ss = FP_SEG(tempStack + this->myStackSize);
	
	// take the PC of the wrapper function that will be continued with
	// after the context switch
	tip = FP_OFF(&(Thread::wrapper));
	tcs = FP_SEG(&(Thread::wrapper));
	
	// pushing myThread, 2x ax-dummy address(expecting a return one,
	// but we will never return), allowing interrupts(that's the big or),
	// pushing the function address(wrapper -> tcs/tip)
	// push all registers(which will be restored when we leave the IR),
	// save the current sp & restore sp to the old value
	asm {
		mov oldss, ss
		mov oldsp, sp
		mov ss, tss
		mov sp, tsp
		
		
		mov ax, 0
		push thh
		push thl
		push ax
		push ax
		
		pushf
		pop ax
		or ax, 0000001000000000b
		push ax
		
		mov ax, tcs
		push ax
		mov ax, tip
		push ax

		mov ax, 0
		
		
		push ax
		push bx
		push cx
		push dx
		push es
		push ds
		push si
		push di
		push bp
		

		mov tsp, sp
		mov sp, oldsp
		mov ss, oldss
	}
	this->sp = tsp;
	unlock();
}

void PCB::wrapper()
{
	
	PCB::running->myThread->run();
	
	lock();
	PCB::running->status = PCB::OVER;
	PCB* temp;
	
	// empty the waiting queue, cuz the process is done
	while( PCB::running->waitQueue->getSize() > 0 )
	{
		temp = PCB::running->waitQueue->pop();
		temp->status = PCB::READY;
		Scheduler::put(temp);
	}
	
	Kernel::dispatch();
	unlock();
}