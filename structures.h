//
// Created by Sam Leyva on 1/13/18.
//

#ifndef CS143B_STRUCTURES_H
#define CS143B_STRUCTURES_H

typedef struct ReadyList {
    struct PCB * init;
    struct PCB * user;
    struct PCB * system;
} ReadyList;

typedef struct BlockList {
    struct PCB * r1;
    struct PCB * r2;
    struct PCB * r3;
    struct PCB * r4;
};

typedef struct ProcessLL {
    struct PCB * process;
    struct ProcessLL * next;
} ProcessNode;

typedef struct ResourceLL {
    // resource * resource;
    struct ResourceLL * next;
} ResourceNode;

typedef struct PCB {
    char pid;	                // Process Name
    //Node resources;	            // Linked List of Resource Block
    int status;	                // Status
    struct PCB * parent;	    // Parent
    struct ProcessLL * child;	// Linked List of Children
    int priority;	            // Priority
} PCB;


#endif //CS143B_STRUCTURES_H
