#ifndef QUEUE_H_
#define QUEUE_H_

#include "PCB.h"



class Elem 
{
public:
	PCB *pcb;
	Elem *next;
	Elem(PCB *pcb);
};


class Queue
{
private:
	int size;
	Elem *first;
public:
	Queue();
	~Queue();
	
	PCB* pop();
	void push(PCB *pcb);
	int getSize() const;
};


class SleepElem
{
public:
	PCB *pcb;
	Time sleepTime;
	SleepElem *next;
	SleepElem(PCB *pcb, Time sleepTime);
	
};

class PriorityQueue
{
private:
	int size;
	SleepElem *first;
public:
	PriorityQueue();
	~PriorityQueue();
	
	PCB* pop();
	void push(PCB *pcb, Time sleepTime);
	void update();
	int getSize() const;
};

#endif
