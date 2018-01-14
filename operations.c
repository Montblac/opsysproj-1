//
// Created by Sam Leyva on 1/13/18.
//

#include <stdlib.h>
#include <memory.h>
#include "structures.h"

// Main Operations
PCB * create (const char * name, int priority, PCB * curr_process, ReadyList * readylist){
    PCB * new_process = malloc(sizeof(struct PCB));
    new_process->pid = strdup(name);
    new_process->priority = priority;
    new_process->parent = curr_process;

    //addChild(curr_process, new_process);
    //insert(readylist, new_process)
    //Scheduler()
    return new_process;
}


// Helper Functions

ProcessNode * createNode(PCB * process, ProcessNode * next){
    ProcessNode * new_node = (ProcessNode *)malloc(sizeof(ProcessNode));
    new_node->process = process;
    new_node->next = next;
    return new_node;
}

ProcessNode * addChild(PCB * src, PCB * new_pcb){
    if (src->child == NULL){
        src->child = createNode(new_pcb, NULL);
        return src->child;
    } else {
        ProcessNode * temp = src->child;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = createNode(new_pcb, NULL);
        return temp->next;
    }
}

ProcessNode * deleteChild(PCB * src, const char * child_id){
    ProcessNode * head = src->child;
    // Checks if there are no nodes
    if(head == NULL){
        return NULL;
    }

    PCB * child = head->process;
    // Compares with first node
    if(strcmp(child->pid, child_id)){
        src->child = head->next;
        free(head);
    } else {
        ProcessNode *prev = head;
        head = head->next;

        while (head != NULL) {
            child = head->process;
            // Compares with every other node
            if (strcmp(child->pid, child_id)) {
                prev->next = head->next;
                free(head);
                break;
            }
            prev = head;
            head = head->next;
        }
    }
    return src->child;
}
