#ifndef CS143B_STRUCTURES_H
#define CS143B_STRUCTURES_H

typedef struct ReadyList{
    struct ProcessNode * priorities[3];
} ReadyList;

typedef struct ResourceList{
    struct RCB * resources[4];
} ResourceList;

typedef struct ProcessNode {
    struct PCB * process;
    struct ProcessNode * next;
} ProcessNode;

typedef struct ResourceNode {
    struct RCB * resource;
    struct ResourceNode * next;
} ResourceNode;

typedef struct PCB {
    char * pid;	                // Process Name
    int state;                  // State
    int priority;               // Priority
    int requested;              // Resource Amount Requested

    ResourceNode * resources;	// Linked List of Resources
    struct PCB * parent;	    // Parent
    ProcessNode * child;    	// Linked List of Children
    ProcessNode * list;         // Back-pointer to readylist or waitlist
} PCB;

typedef struct RCB {
    char * rid;
    int size;
    int inventory;
    ProcessNode * waitinglist;
} RCB;

#endif //CS143B_STRUCTURES_H
