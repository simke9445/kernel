#ifndef _thread_h_
#define _thread_h_


typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms
typedef int ID;



class PCB;


class Thread {
private:
	ID myID;
	static void wrapper(Thread *running);
public:
	void start();
	void waitToComplete();
	virtual ~Thread();
	static void sleep(Time timeToSleep);
protected:
	friend class PCB;
	friend class IdleThread;
	friend class Kernel;
	
	Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run();
};

void dispatch ();

#endif
