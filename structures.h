//
// Created by Sam Leyva on 1/13/18.
//

#ifndef CS143B_STRUCTURES_H
#define CS143B_STRUCTURES_H

typedef struct LinkedList {
    int data;
    struct LinkedList * next;
} Node;

typedef struct PCB {
    char pid;	                // Process Name
    Node resources;	            // Resource Block
    int status;	                // Status
    struct PCB * parent;	    // Parent
    //struct PCB * children;	// List of Children
    int priority;	            // Priority
} PCB;

#endif //CS143B_STRUCTURES_H
