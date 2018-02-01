//
// Created by Sam Leyva on 1/13/18.
//

#ifndef CS143B_OPERATIONS_H
#define CS143B_OPERATIONS_H

// # Allocation
ProcessNode * createPNode(PCB * process, ProcessNode * next);
ResourceNode * createRNode(RCB * resource, ResourceNode * next);
PCB * createProcess(const char * pid, int priority, PCB * parent);
RCB * createResource(const char * name, int resource_count);

// # De-allocation
void freeProcessNodes(ProcessNode * head);
void freeResourceNodes(ResourceNode * head);
void freeProcess(PCB * proc);
void freeReadylist(ReadyList * readylist);
void freeResourcelist(ResourceList * resourcelist);
void freeWaitlist(ResourceList * resourcelist);

// # Process Control Block (PCB)
char * getProcessName(PCB * process);
int getProcessState(PCB * process);
int getProcessPriority(PCB * process);
void setProcessState(PCB * process, int state);
void setProcessList(PCB * process, ProcessNode * list);
void setProcessRequested(PCB * process, int amount);

// # Resource Control Block (RCB)
char * getResourceName(RCB * resource);
int getResourceSize(RCB * resource);
int getResourceSpace(RCB * resource);
ProcessNode * getResourceWaitlist(RCB * resource);

// # Process Control Block Utility
PCB * findProcess(const char * pid, ReadyList * readylist);
PCB * findProcess2(const char * rid, ReadyList * readylist);

// # Resource Control Block Utility
RCB * findResource(const char * rid, ResourceList * resourcelist);

// # Insertion
void insertChild(PCB * src, PCB * new_pcb);
void insertProcess(ReadyList * readylist, PCB * process);
void insertResource(PCB * process, RCB * resource, int n);
void insertWaitlist(PCB * process, RCB * resource);

// # Deletion
void removeProcess(ReadyList * readylist, PCB * process);
void removeResource(PCB * process, RCB * resource);
void removeNResource(PCB * process, RCB * resource, int n);
void removeWaitlisted(RCB * resource, const char * pid);

// Update
int updateParent(PCB * src);
void returnResources(PCB * src);
void updateWaitlist(RCB * resource, ReadyList * readylist);

// # Initialization
ReadyList * initReadylist();
ResourceList * initResourcelist();
PCB * initProcess(ReadyList * readylist);

// # Scheduler
void preempt(PCB ** active_proc, PCB * new_proc);
void scheduler(PCB ** active_proc, ReadyList * readylist);

// # Time-out Interrupt
void timeout(ReadyList * readylist, PCB ** active_process);

// ## Create
void create(const char * name, int priority, ReadyList * readylist, PCB ** active_process);

// ## Delete
void killTree(PCB * src, ReadyList * readylist, ResourceList * resourcelist, PCB ** active_process);
int delete(const char * pid, ReadyList * readylist, ResourceList * resourcelist, PCB ** active_process);

// ## Request
int request(const char * rid, int units, ReadyList * readylist, ResourceList * resourcelist, PCB ** active_process);

// ## Release
void release(const char * rid, int units, ResourceList * resourcelist, ReadyList * readylist, PCB ** active_process);

// Input-Checking
int isWord(const char * input);
int isNumber(const char * input);
int isInRange(int num);
int isInRange2(int num);

// # I/O
void writeoutput(const char * pid, FILE * file);

// # Debugging
void printReadyList(ReadyList * readylist);
void printTree(const char * name, ReadyList * readylist);
void printWaitlist(const char * rid, ResourceList * resourcelist);
void printResources(const char * pid, ReadyList * readylist);
#endif //CS143B_OPERATIONS_H
