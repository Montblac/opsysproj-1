//
// Created by Sam Leyva on 1/13/18.
//

#ifndef CS143B_OPERATIONS_H
#define CS143B_OPERATIONS_H

// Helper Functions
ReadyList * initReadyList();
void freeReadyList(ReadyList * readylist);

RCB * initResource(const char * name, int resource_count);
BlockList * initBlockList();
void freeBlockList(BlockList * blocklist);

void deleteChildren(PCB * src);

ProcessNode * createNode(PCB * process, ProcessNode * next);
ProcessNode * addChild(PCB * src, PCB * new_pcb);
ProcessNode * deleteChild(PCB * src, const char * child_id);

void insert(ReadyList * readylist, PCB * process);

// Main Operations
PCB * create(const char * name, int priority, PCB * curr_process, ReadyList * readylist);
void scheduler(PCB * active_process, ReadyList * readylist);

// Debugging
void printReadyList(ReadyList * readylist, int priority);
void printTree(char * name, ReadyList * readylist);

#endif //CS143B_OPERATIONS_H
