#include "semaphor.h"
#include "DEF.h"
#include "kernelsem.h"
#include "kernel.h"
#include <iostream.h>

Semaphore::Semaphore(int init)
{
	lock();
	static ID tmp;
	tmp = init;
	asm{
		mov ax, 11
		mov bx, tmp
		int 60h
	}
	asm{ 
		mov tmp, ax
	}
	this->myID = tmp;
	unlock();
}

Semaphore::~Semaphore()
{
	lock();
	static ID id;
	id = this->myID;
	asm{
		mov ax, 15
		mov bx, id
		int 60h
	}
	unlock();
}

void Semaphore::wait()
{
	lock();
	static ID id;
	id = this->myID;
	asm{
		mov ax, 12
		mov bx, id
		int 60h
	}
	unlock();
}

void Semaphore::signal()
{
	lock();
	static ID id;
	id = this->myID;
	asm{
		mov ax, 13
		mov bx, id
		int 60h
	}
	unlock();
}

int Semaphore::val() const 
{
	lock();
	static ID id;
	id = this->myID;
	asm{
		mov ax, 14
		mov bx, id
		int 60h
	}
	asm{
		mov id, ax
	}
	unlock();
	return id;
}