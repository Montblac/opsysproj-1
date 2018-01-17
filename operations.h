//
// Created by Sam Leyva on 1/13/18.
//

#ifndef CS143B_OPERATIONS_H
#define CS143B_OPERATIONS_H

// Helper Functions

// # Linked List
ProcessNode * nodeCreate(PCB * process, ProcessNode * next);
ProcessNode * childAdd(PCB * src, PCB * new_pcb);
ProcessNode * deleteChild(PCB * src, const char * child_id);

// # Process Control Block
char * getProcessName(PCB * process);
int getProcessState(PCB * process);
int getProcessPriority(PCB * process);

// # Resource Control Block

// # Ready List
ReadyList * readyInit();
void readyFree(ReadyList * readylist);
void readyInsert(ReadyList * readylist, PCB * process);

// Resource List






RCB * resourceInit(const char * name, int resource_count);
ResourceList * resourceInit();
void freeResourceList(ResourceList * resourcelist);

void deleteChildren(PCB * src);

// Main Operations
PCB * create(const char * name, int priority, PCB * curr_process, ReadyList * readylist);
void scheduler(PCB * active_process, ReadyList * readylist);

// Debugging
//void printReadyList(ReadyList * readylist, int priority);
void printReadyList(ReadyList * readylist);
void printTree(char * name, ReadyList * readylist);


// Testing
ResourceListFixed * resourceInitFixed();

#endif //CS143B_OPERATIONS_H
