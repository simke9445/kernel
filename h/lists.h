#ifndef LISTE_H_
#define LISTE_H_

#include "thread.h"

class PCB;
class KernelEvent;
class KernelSem;



class ElemPCB
{
public:
	ID id;
	PCB *pcb;
	ElemPCB *next;
	ElemPCB(PCB *pcb, ID id);
	~ElemPCB();
};

class ElemEv
{
public:
	ID id;
	KernelEvent *kev;
	ElemEv *next;
	ElemEv(KernelEvent *kev, ID id);
	~ElemEv();
};


class ElemSem
{
public:
	KernelSem *sem;
	ID id;
	ElemSem *next;
	ElemSem(KernelSem *sem, ID id);
	~ElemSem();
};



class PCBList
{
private:
	ElemPCB *first, *last;
public:
	PCBList();
	void add(PCB *pcb, ID id);
	void remove(ID id);
	PCB* operator[](ID id);
	~PCBList();
};

class EventList
{
private:
	ElemEv *first, *last;
public:
	EventList();
	void add(KernelEvent *kev, ID id);
	void remove(ID id);
	KernelEvent* operator[](ID id);
	~EventList();
};

class SemList
{
private:
	ElemSem *first, *last;
public:
	SemList();
	void add(KernelSem *sem, ID id);
	void remove(ID id);
	KernelSem* operator[](ID id);
	~SemList();
};


#endif