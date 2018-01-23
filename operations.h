//
// Created by Sam Leyva on 1/13/18.
//

#ifndef CS143B_OPERATIONS_H
#define CS143B_OPERATIONS_H

// # Linked List
ProcessNode * nodeCreate(PCB * process, ProcessNode * next);
ProcessNode * childAdd(PCB * src, PCB * new_pcb);
ProcessNode * deleteChild(PCB * src, const char * child_id);

// # Process Control Block
char * getProcessName(PCB * process);
int getProcessState(PCB * process);
int getProcessPriority(PCB * process);

// # Resource Control Block
char * getResourceName(RCB * resource);
int getResourceSize(RCB * resource);
int getResourceSpace(RCB * resource);
ProcessNode * getResourceWaitlist(RCB * resource);

// # Ready List
PCB * getActiveProcess(ReadyList * readylist);
ReadyList * readyInit();
void readyFree(ReadyList * readylist);
void readyInsert(ReadyList * readylist, PCB * process);
PCB * init(ReadyList * readylist);
void preempt(PCB ** active_proc, PCB * new_proc);

// # Resource List
RCB * resourceCreate(const char * name, int resource_count);
ResourceList * resourceInit();
void resourceFree(ResourceList * resourcelist);

// Input-Checking
int isWord(const char * input);
int isNumber(const char * input);
int isInRange(const int num);

// # Debugging
void printReadyList(ReadyList * readylist);
void printTree(const char * name, ReadyList * readylist);




// # Main Operations

void scheduler(PCB ** active_proc, ReadyList * readylist);
void create(const char * name, int priority, ReadyList * readylist, PCB ** active_process);
int delete(const char * pid, ReadyList * readylist, PCB ** active_process);



// WORK IN PROGRESS
void deleteChildren(PCB * src);

#endif //CS143B_OPERATIONS_H
