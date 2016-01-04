#ifndef KERNELSEM_H_
#define KERNELSEM_H_

class Queue;

class KernelSem
{
private:
	int value;
	Queue *waitQueue;
public:
	KernelSem(int initValue);
	virtual ~KernelSem();
	void wait();
	void signal();
	int val() const;
};

#endif