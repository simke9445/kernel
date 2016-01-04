#ifndef KERNEL_H_
#define KERNEL_H_

typedef unsigned char IVTNo;
typedef void interrupt (*InterruptRoutine)(...);

class KernelEvent;
class KernelSem;
class IVTEntry;
class IdleThread;
class PCB;
class PCBList;
class SemList;
class EventList;



// System organisation is the following: User has access to Event, Thread, Semaphore classes
// which he can use for development, while the core system code(PCB, KernelEvent...) is "hidden"
// behind a predefined interrupt routine. This way the system is provided with a more sophisticated
// way of protecting it's internal work by placing itself into "kernel" mode, as well as
// avoiding overfilling of the user stack when executing system calls.
// Parameters for execution & identification of system calls are passed
// via processor registers, mainly ax, bx, cx, dx, si, di - general & index registers
// User available class objects are identified in the system through a generated ID value

// idle thread, as well as starting PCB are there to ensure proper usage - 
// idle is active while there are no active threads(does nothing, busy wait)
// starting PCB is the first and last PCB to be placed/taken from Scheduler,
// and is a sign of a properly finished program. 

// pcbCollection, eventCollection, semCollection serve as collections of objects
// which are active, since the only information available to the user is the ID

// sys_call is the system interrupt routine which contains decoding of received parameters
// from user function calls, as well as all the management within the system.

// old60 is the saved interrupt routine, before we substituted it with our own (e.g. sys_call),
// which will be restored at the system shutdown, ensuring consistency

// timerIR is the overloaded interrupt routine from the system's timer

// dispatch is the method for taking the control from one process, and giving it to the other,
// that's ready for work.

class Kernel
{
private:
	static volatile unsigned int dispatched;
	static PCB *starting;
	static IdleThread *idle;
	static volatile unsigned int counter;
	static volatile unsigned int interruptCounter;
	static void interrupt sys_call(...);
	static volatile unsigned int PCBIDGenerator, eventIDGenerator, semIDGenerator;
	static void interrupt (*oldRoutine)(...);
	static void interrupt timerIR(...);
	static InterruptRoutine replaceRoutine(IVTNo intN, InterruptRoutine newRoutine);
	
	static void interrupt (*old60)(...);

	static PCBList pcbCollection;
	static EventList eventCollection;
	static SemList semCollection;

public:
	friend class PCB;
	friend class Thread;
	friend class Event;
	friend class IdleThread;
	friend class Semaphore;
	friend class IVTEntry;
	friend class KernelEvent;
	
	static volatile unsigned int systemCallFlag;
	
	static void dispatch();
	static void init();
	static void term();
	
};

extern void tick();







#endif