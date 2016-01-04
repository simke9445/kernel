#include "idle.h"
#include "PCB.h"
#include "DEF.h"
#include <iostream.h>
#include "kernel.h"
#include "lists.h"

IdleThread::IdleThread(): Thread(256,1){}

void IdleThread::start()
{	
	lock();
	
	Kernel::pcbCollection[this->myID]->status = PCB::READY;
	Kernel::pcbCollection[this->myID]->initStack();
	
	unlock();
}

void IdleThread::run()
{
	while(1);
}