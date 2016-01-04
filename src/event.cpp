#include "event.h"
#include "DEF.h"
#include "kernelev.h"
#include "kernel.h"
#include <dos.h>
#include <iostream.h>
#include "PCB.h"


Event::Event(IVTNo num)
{
	lock();
	static ID val;
	static IVTNo nm;
	nm = num;
	asm{
		mov ax, 7
		mov bl, nm
		int 60h
	}
	asm{
		mov val, ax
	}
	this->myID = val;
	unlock();
}

Event::~Event()
{
	lock();
	static ID id;
	id = this->myID;
	asm{
		mov ax, 10
		mov bx, id
		int 60h
	}
	unlock();
}

void Event::wait()
{
	lock();
	static ID id;
	id = this->myID;
	asm{
		mov ax, 9
		mov bx, id
		int 60h
	}
	unlock();
}

void Event::signal()
{
	lock();
	static ID id;
	id = this->myID;
	asm{
		mov ax, 8
		mov bx, id
		int 60h
	}
	unlock();
}