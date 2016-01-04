#include "thread.h"
#include "SCHEDULE.H"
#include "thread.h"
#include "DEF.h"
#include "PCB.h"
#include "kernel.h"
#include <stdlib.h>
#include <dos.h>
#include <iostream.h>
#include "queue.h"


class System;
class Scheduler;


void Thread::wrapper(Thread *running)
{
	running->run();
	
	lock();
	asm{
		mov ax, 5
		int 60h
	}
	unlock();
}

void Thread::run(){}

Thread::Thread(StackSize stackSize, Time timeSlice) 
{
	lock();
	static ID id;
	static Time time;
	static unsigned int stl, sth, thl, thh;
	time = timeSlice;
	stl = FP_OFF(&stackSize);// In case of values
	sth = FP_SEG(&stackSize);// greater than 16 bit
	thl = FP_OFF(this);// we have to use Far Pointers
	thh = FP_SEG(this);// to reference the requested value
	asm{
		mov ax, 1
		mov bx, stl
		mov cx, sth
		mov dx, thl
		mov si, thh
		mov di, time
		int 60h
	}
	
	asm{
		mov id, ax
	}
	
	this->myID = id;
	unlock();
}

Thread::~Thread()
{
	lock();
	static ID id;
	id = this->myID;
	asm{
		mov ax, 6
		mov bx, id
		int 60h
	}
	unlock();
}


void Thread::start()
{
	lock();
	static ID id;
	id = this->myID;
	asm{
		mov ax, 2
		mov bx, id
		int 60h
	}
	unlock();
}


void Thread::sleep(Time timeToSleep)
{
	lock();
	static Time time;
	time = timeToSleep;
	asm{
		mov ax, 3
		mov bx, time
		int 60h
	}
	unlock();
}


void Thread::waitToComplete()
{
	lock();
	static ID id;
	id = this->myID;
	asm{
		mov ax, 4
		mov bx, id
		int 60h
	}
	unlock();
}


void dispatch() 
{
	lock();
	asm{
		mov ax, 0
		int 60h
	}
	unlock();
}



