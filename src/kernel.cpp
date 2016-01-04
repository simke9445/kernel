#include "kernel.h"
#include "SCHEDULER.H"
#include "thread.h"
#include <dos.h>
#include <iostream.h>
#include "idle.h"
#include "event.h"
#include "entry.h"
#include "kernelse.h"
#include "kernelev.h"
#include "semaphor.h"
#include "PCB.h"
#include "queue.h"
#include "entrylst.h"
#include "lists.h"


volatile unsigned int Kernel::dispatched = 0;
void interrupt (*Kernel::oldRoutine)(...) = NULL;
void interrupt (*Kernel::old60)(...) = NULL;
PCB* Kernel::starting = NULL;
IdleThread* Kernel::idle = NULL;
volatile unsigned int Kernel::counter = 0;
volatile unsigned int Kernel::interruptCounter = 0;
volatile unsigned int Kernel::eventIDGenerator = 0;
volatile unsigned int Kernel::semIDGenerator = 0;
volatile unsigned int Kernel::PCBIDGenerator = 0;
volatile unsigned int Kernel::systemCallFlag = 0;

PCBList Kernel::pcbCollection = PCBList();
EventList Kernel::eventCollection = EventList();
SemList Kernel::semCollection = SemList();



enum systemFunctions
{
	MAKE_THREAD = 1, START_THREAD = 2, SLEEP = 3, WAIT_TO_COMPLETE = 4, THREAD_WRAPPER = 5, DESTROY_THREAD = 6,
	MAKE_EVENT = 7, SIGNAL_EVENT = 8, WAIT_EVENT = 9, DESTROY_EVENT = 10,
	MAKE_SEM = 11, WAIT_SEM = 12, SIGNAL_SEM = 13, SEM_VALUE = 14, DESTROY_SEM = 15
};




void Kernel::init()
{
	// we're locking the code in here, because it's most crucial part of the program
	// nevertheless, system is designed so that interrupts are allowed all the time.
	
	lock();
	
	// securing old routines
	oldRoutine = replaceRoutine(0x08, timerIR);
	old60 = replaceRoutine(0x60, sys_call);
	
	starting = new PCB(NULL, 0, 4096, 1);
	starting->status = PCB::READY;
	
	PCB::running = (volatile PCB*)starting;
	
	idle = new IdleThread();
	idle->start();
	
	unlock();
	
}


void interrupt Kernel::sys_call(...)
{
	static unsigned int tsp, tss, tax, tbx, tcx, tdx, tdi, tsi, tret;
	static Time time;
	static ID id;
	static systemFunctions function;
	static volatile PCB *newThread;
	static PCB *tempPCB;
	static Thread *running;
	static StackSize *lhp, stackSize;
	static IVTNo tivtn;
	
	// take all the values of the registers, and place it in their respective t-clone(temporary),
	// tho not all of them will be used all the time
	asm{
		mov tax, ax
		mov tbx, bx
		mov tcx, cx
		mov tdx, dx
		mov tsi, si
		mov tdi, di
	}
	
	// save the current stack pointer and stack segment
	asm {
		mov tsp, sp
		mov tss, ss
	}
	
	function = (systemFunctions)tax;
	
	systemCallFlag = 1;
	
	// allow interrupts by setting I flag to 1
	asm sti;
	
	
	switch(function)
	{
	case MAKE_THREAD: // Thread::Thread();
	{
		// making a far pointer which points at our stackSize
		lhp = (unsigned long*)MK_FP(tcx,tbx);
		stackSize = *lhp;
		
		// same thing for thread
		running = (Thread*)MK_FP(tsi,tdx);
		time = tdi;
		id = ++PCBIDGenerator;
		
		// crucial work, so lock
		lock();
		pcbCollection.add(new PCB(running, id, stackSize, time), id);
		unlock();
		
		// set the return value to tret, id of created object
		tret = id;
	}
	break;
	case START_THREAD: //start
	{
		id = tbx;
		pcbCollection[id]->status = PCB::READY;
		
		// create the thread stack and simulation of interrupt so
		// that later in execution the process can get the processor
		// by preemption
		pcbCollection[id]->initStack();
		Scheduler::put((PCB*)pcbCollection[id]);
	}
	break;
	case SLEEP: //sleep
	{
		time = tbx;
		
		// invalid condition, might not be needed, but just in case
		if ( PCB::running->myThread == Kernel::idle || PCB::running == Kernel::starting ) break;
		
		PCB::running->status = PCB::BLOCKED;
		PCB::sleepQueue->push((PCB*)PCB::running, time);
	}
	break;
	case WAIT_TO_COMPLETE: //waitToComplete
	{
		id = tbx;
		
		// invalid condition, might not be needed, but just in case
		if (pcbCollection[id]->status == PCB::OVER) { break; }
		if (pcbCollection[id] == (PCB*)PCB::running) { break; }
		if (pcbCollection[id] == Kernel::starting) { break; }
		if (pcbCollection[id]->myThread == Kernel::idle) { break; }
		PCB::running->status = PCB::BLOCKED;
		pcbCollection[id]->waitQueue->push((PCB*)PCB::running);
	}
	break;
	case THREAD_WRAPPER: //Thread::wrapper
	{
		PCB::running->status = PCB::OVER;
		
		// empty the waitQueue cuz the thread is done
		while( ((PCB*)PCB::running)->waitQueue->getSize() > 0 )
		{
			tempPCB =  ((PCB*)PCB::running)->waitQueue->pop();
			tempPCB->status = PCB::READY;
			Scheduler::put(tempPCB);
		}
	}
	break;
	case DESTROY_THREAD: // Thread::~Thread()
	{
		id = tbx;
		pcbCollection.remove(id);
		if( id == 1 ) // ~IdleThread()
		{
			asm{	
				mov sp, tsp
				mov ss, tss
			}
			systemCallFlag = 0;
			return;
		}
	}
	break;
	case MAKE_EVENT: // Event();
	{
		// taking the bottom half of bx register as ivt num
		// push/pop bx for restoring the previous state
		asm{
			push bx
			mov bx, tbx
			mov tivtn, bl
			pop bx
		}
		id = ++eventIDGenerator;
		lock();
		eventCollection.add(new KernelEvent(tivtn), id);
		unlock();
		tret = id;
	}
	break;
	case SIGNAL_EVENT:// Event::signal()
	{
		id = tbx;
		eventCollection[id]->signal();
	}
	break;
	case WAIT_EVENT: //Event::wait()
	{
		id = tbx;
		eventCollection[id]->wait();
	}
	break;
	case DESTROY_EVENT: //Event::~Event()
	{
		id = tbx;
		eventCollection.remove(id);
	}
	break;
	case MAKE_SEM: //Sem::Sem()
	{
		id = ++semIDGenerator;
		tret = id;
		time = tbx;
		lock();
		semCollection.add(new KernelSem(time), id);
		unlock();
	}
	break;
	case WAIT_SEM: // Sem::wait()
	{
		id = tbx;
		semCollection[id]->wait();
	}
	break;
	case SIGNAL_SEM: //Sem::signal()
	{
		id = tbx;
		semCollection[id]->signal();
	}
	break;
	case SEM_VALUE: // Sem::val()
	{
		id = tbx;
		tret = semCollection[id]->val();
	}
	break;
	case DESTROY_SEM: //Sem::~Sem()
	{
		id = tbx;
		semCollection.remove(id);
	}
	break;
	default: break;
	}

	//setting return value on place of callers ax register, which sits at bp + 16...
	asm{
		push ax
		mov ax, tret
		mov bp[16], ax
		pop ax
	}
	
	
	// if we're dispatching a ready process, just place it back in scheduler
	if( PCB::running->status == PCB::READY && ((PCB*)PCB::running)->id != idle->myID)  
	{
		Scheduler::put((PCB*)PCB::running);
	}
	
	newThread = Scheduler::get();
	
	// if the scheduler is empty, idle kicks in
	if (newThread == NULL) 
	{
		newThread = pcbCollection[idle->myID];
	}
	
	
	//context_switch
	PCB::running->sp = tsp;
	PCB::running->ss = tss;
	PCB::running = newThread;
	tsp = PCB::running->sp;
	tss = PCB::running->ss;

	// restore the current(switched) thread's stack pointer and stack segment
	asm {
		mov sp, tsp
		mov ss, tss
	}
	
	systemCallFlag = 0;
	
	((EntryList*)IVTEntry::myEntryList)->doAllMissedInterrupts();
}

void Kernel::term()
{
	lock();
	
	delete IVTEntry::myEntryList;
	delete PCB::sleepQueue;
	delete [] IVTEntry::IVTEntries;
	delete idle;
	delete starting;
	
	replaceRoutine(0x08, oldRoutine);
	replaceRoutine(0x60, old60);
	unlock();
}

InterruptRoutine Kernel::replaceRoutine(IVTNo intN, InterruptRoutine newRoutine) 
{
	static unsigned int tseg, toff, oldSEG, oldOFF, tempN;
	static InterruptRoutine oldIR;
	
	lock();
	
	// will be restored later
	asm {
		push es;
		push ax;
		push bx;
	}
	
	// spliting the address of newRoutine
	tseg = FP_SEG(newRoutine);
	toff = FP_OFF(newRoutine);
   
	// interrupt routine lies in the 4*intN place, where intN is the IVT num
	// we cast it to unsigned int(32 bit) cuz IVTNo is a char(8 bit)
	tempN = 4*((unsigned int)intN);

	// save the old seg & off, and place the new ones
	// oldSeg = bx[2], oldOff = bx[0]
	// bx[2] = tseg, bx[0] = toff
	asm {
		mov ax, 0
		mov es, ax
		mov bx, word ptr tempN;
	   
		mov ax, word ptr es:bx + 2
		mov word ptr oldSEG, ax
		mov ax, word ptr es:bx
		mov word ptr oldOFF, ax
	   
		mov ax, word ptr tseg
		mov word ptr es:bx + 2, ax
		mov ax, word ptr toff
		mov word ptr es:bx, ax
	}
   
	// make the new routine 
	oldIR = (InterruptRoutine)MK_FP(oldSEG, oldOFF);
	
	// restore
	asm {
		pop bx;
		pop ax;
		pop es;
	}      
	
	unlock();
	
	return oldIR;  
}

void Kernel::dispatch()
{
	lock();
	dispatched = 1;
	timerIR();
	dispatched = 0;
	unlock();
}

void interrupt Kernel::timerIR(...)
{
	static volatile unsigned int tsp, tss;
	static volatile PCB *newThread;
	
	// doing nothing if the system call is running
	// just incremeting the counter
	if(!dispatched && systemCallFlag)
	{
		interruptCounter++;
		return;
	}
	
	if(!dispatched) interruptCounter++;
	
	if(!dispatched) 
	{
		while( interruptCounter-- > 0 )
		{
			tick();
			PCB::sleepQueue->update();
			
			// call the old timer routine
			(*oldRoutine)();
		}
		interruptCounter = 0;
	}
	
	if(!dispatched && PCB::running->myTimeSlice == 0) 
	{
		return;
	}
	
	if (!dispatched && PCB::running->myTimeSlice != 0)
	{
		PCB::running->counter++;
	}
	
	// if the running's timeslice isn't over, continue with current running
	if (!dispatched && PCB::running->myTimeSlice > PCB::running->counter) return;
	if (!dispatched && PCB::running->myTimeSlice <= PCB::running->counter) 
	{
		PCB::running->counter = 0;
	}
	if (dispatched) dispatched = 0;

	if( PCB::running->status == PCB::READY && ((PCB*)PCB::running)->id != idle->myID)  
	{
		Scheduler::put((PCB*)PCB::running);
	}
	
	newThread = Scheduler::get();
	
	// swaping to idle if scheduler is empty
	if (newThread == NULL) 
	{
		newThread = pcbCollection[idle->myID];
		
		// not gonna swap from idle to idle
		if( PCB::running->myThread == idle && newThread->myThread == idle ) return;
	}
	
	// save old process's sp and ss
	asm {
		mov tsp, sp
		mov tss, ss
	}
	
	// context switch
	PCB::running->sp = tsp;
	PCB::running->ss = tss;
	PCB::running = newThread;
	tsp = PCB::running->sp;
	tss = PCB::running->ss;
	
	// restore new process's sp and ss
	asm {
		mov sp, tsp
		mov ss, tss
	}
}



