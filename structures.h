//
// Created by Sam Leyva on 1/13/18.
//

#ifndef CS143B_STRUCTURES_H
#define CS143B_STRUCTURES_H

typedef struct ReadyList {
    struct ProcessNode * init;
    struct ProcessNode * user;
    struct ProcessNode * system;
} ReadyList;

typedef struct BlockList {
    struct RCB * r1;
    struct RCB * r2;
    struct RCB * r3;
    struct RCB * r4;
} BlockList;

typedef struct ProcessNode {
    struct PCB * process;
    struct ProcessNode * next;
} ProcessNode;

typedef struct ResourceNode {
    // resource * resource;
    struct ResourceNode * next;
} ResourceNode;

typedef struct PCB {
    char * pid;	                // Process Name
    int state;                  // State
    int priority;               // Priority
    int blocked;                // Blocked Status

    ResourceNode * resourcelist;	// Linked List of Resources
    struct PCB * parent;	        // Parent
    ProcessNode * child;    	    // Linked List of Children
    BlockList * blocklist;          // Pointer to blocklist
    ReadyList * readylist;          // Pointer to readylist
} PCB;

typedef struct RCB {
    char * rid;
    int inventory;
    ProcessNode * waitinglist;
} RCB;


#endif //CS143B_STRUCTURES_H
