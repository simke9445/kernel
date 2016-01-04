#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "thread.h"

class KernelSem;

class Semaphore
{
private:
	ID myID;
public:
	Semaphore(int init = 1);
	virtual ~Semaphore();
	virtual void wait();
	virtual void signal();
	int val() const;
};

#endif