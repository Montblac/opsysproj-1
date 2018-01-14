//
// Created by Sam Leyva on 1/13/18.
//

#ifndef CS143B_OPERATIONS_H
#define CS143B_OPERATIONS_H

// Main Operations
PCB * create (const char * name, int priority, PCB * curr_process, ReadyList * readylist);


// Helper Functions
ProcessNode * createNode(PCB * process, ProcessNode * next);
ProcessNode * addChild(PCB * src, PCB * dest);
ProcessNode * deleteChild(PCB * src, const char * child_id);

#endif //CS143B_OPERATIONS_H
