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
} BlockList;

typedef struct ProcessLL {
    struct PCB * process;
    struct ProcessLL * next;
} ProcessNode;

typedef struct ResourceLL {
    // resource * resource;
    struct ResourceLL * next;
} ResourceNode;

typedef struct PCB {
    char * pid;	                // Process Name
    int state;                  // State
    int priority;               // Priority
    int blocked;                // Blocked Status

    //Node resources;	            // Linked List of Resource Block
    struct PCB * parent;	        // Parent
    ProcessNode * child;    	    // Linked List of Children
    BlockList * blocklist;   // Pointer to blocklist
    ReadyList * readylist;   // Pointer to readylist

} PCB;


#endif //CS143B_STRUCTURES_H
