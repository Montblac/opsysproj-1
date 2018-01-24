//
// Created by Sam Leyva on 1/13/18.
//

#ifndef CS143B_OPERATIONS_H
#define CS143B_OPERATIONS_H

// # Process Control Block
char * getProcessName(PCB * process);
int getProcessState(PCB * process);
int getProcessPriority(PCB * process);
void setProcessState(PCB * process, int state);

// # Resource Control Block
char * getResourceName(RCB * resource);
int getResourceSize(RCB * resource);
int getResourceSpace(RCB * resource);
ProcessNode * getResourceWaitlist(RCB * resource);

// # Linked List
ProcessNode * nodeCreate(PCB * process, ProcessNode * next);
ProcessNode * childAdd(PCB * src, PCB * new_pcb);


// # Ready List
PCB * findProcess(const char * pid, ReadyList * readylist);
ReadyList * readyInit();
void readyFree(ReadyList * readylist);
void insertProcess(ReadyList * readylist, PCB * process);
PCB * removeProcess(ReadyList * readylist, PCB * process);
PCB * init(ReadyList * readylist);

// # Resource List
RCB * resourceCreate(const char * name, int resource_count);
ResourceList * resourceInit();
void resourceFree(ResourceList * resourcelist);



// # Scheduler
void preempt(PCB ** active_proc, PCB * new_proc);
void scheduler(PCB ** active_proc, ReadyList * readylist);

// # Time-out Interrupt
void timeout(ReadyList * readylist, PCB ** active_process);


// ## Create
void create(const char * name, int priority, ReadyList * readylist, PCB ** active_process);

// ## Delete
void killProcess(ReadyList * readylist, const char * pid);
void killChild(ProcessNode ** pnode, const char * pid);
void killTree(PCB * src, ReadyList * readylist);
int updateParent(PCB * src);
int delete(const char * pid, ReadyList * readylist, PCB ** active_process);


// Input-Checking
int isWord(const char * input);
int isNumber(const char * input);
int isInRange(int num);

// # Debugging
void printReadyList(ReadyList * readylist);
void printTree(const char * name, ReadyList * readylist);



#endif //CS143B_OPERATIONS_H
