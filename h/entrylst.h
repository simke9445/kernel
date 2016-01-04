#ifndef ENTRYLIST_H_
#define ENTRYLIST_H_


class IVTEntry;

class EntryListElem
{
public:
	IVTEntry *entry;
	EntryListElem *next;
	EntryListElem(IVTEntry *entry);
};

class EntryList
{
public:
	EntryListElem *first, *last;
	int size;
	
	EntryList();
	~EntryList();
	
	int getSize() const;
	void push(IVTEntry *entry);
	void doAllMissedInterrupts();
};



#endif