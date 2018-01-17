//
// Created by Sam Leyva on 1/13/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "macros.h"
#include "structures.h"

// Helper Functions

// # Linked List

ProcessNode * nodeCreate(PCB * process, ProcessNode * next){
    ProcessNode * new_node = (ProcessNode *)malloc(sizeof(ProcessNode));
    new_node->process = process;
    new_node->next = next;
    return new_node;
}
ProcessNode * childAdd(PCB * src, PCB * new_pcb){
    if (src->child == NULL){
        src->child = nodeCreate(new_pcb, NULL);
        return src->child;
    } else {
        ProcessNode * temp = src->child;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = nodeCreate(new_pcb, NULL);
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
    if(!strcmp(child->pid, child_id)){
        src->child = head->next;
        free(head);
    } else {
        ProcessNode *prev = head;
        head = head->next;

        while (head != NULL) {
            child = head->process;
            // Compares with every other node
            if (!strcmp(child->pid, child_id)) {
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

// # Process Control Block
char * getProcessName(PCB * process){
    return process->pid;
}
int getProcessState(PCB * process){
    return process->state;
}
int getProcessPriority(PCB * process){
    return process->priority;
}

// # Resource Control Block


// # Ready List
ReadyList * readyInit(){
    ReadyList * readylist = (ReadyList *)malloc(sizeof(ReadyList));
    const char * names[] = {"init", "user", "system"};
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        readylist->priorities[i] = NULL;
    }
    return readylist;
}
void readyFree(ReadyList * readylist){
    // Insert some check that all ready lists (i.e. init, sys, user are free)
    ProcessNode ** prioritylist = readylist->priorities;
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        ProcessNode * node = prioritylist[i];
        while(node != NULL){
            ProcessNode * temp = node;
            node = node->next;
            free(temp->process);
            free(temp);
        }
    }
    free(readylist);
}
void readyInsert(ReadyList * readylist, PCB * process){
    int priority = getProcessPriority(process);
    ProcessNode ** pnode_list = &(readylist->priorities)[priority];
    ProcessNode * new_node = nodeCreate(process, NULL);

    if(priority == INIT){
        *pnode_list = new_node;
    } else {
        if(pnode_list == NULL){
            *pnode_list = new_node;
        } else {
            ProcessNode * temp = *pnode_list;
            while(temp->next != NULL){
                temp = temp->next;
            }
            (*pnode_list)->next = new_node;
        }
    }
}

// # Resource List






RCB * resourceInit(const char * name, int resource_count){
    RCB * resource = (RCB *)malloc(sizeof(RCB));
    resource->rid = strdup(name);
    resource->size = resource_count;
    resource->inventory = resource_count;
    resource->waitinglist = NULL;
    return resource;
}


ResourceList * resourceInit(){
    ResourceList * resourcelist = (ResourceList *)malloc(sizeof(ResourceList));
    resourcelist->r1 = resourceInit("R1", 1);
    resourcelist->r2 = resourceInit("R2", 2);
    resourcelist->r3 = resourceInit("R3", 3);
    resourcelist->r4 = resourceInit("R4", 4);
    return resourcelist;
}
void freeResourceList(ResourceList * resourcelist){
    free(resourcelist->r1);
    free(resourcelist->r2);
    free(resourcelist->r3);
    free(resourcelist->r4);
    free(resourcelist);
}

void deleteChildren(PCB * src){
    ProcessNode * child = src->child;

    // WIP
    while(child != NULL) {
        printf("Gets here 0\n");
        ProcessNode * temp = child;
        printf("Gets here 1\n");
        deleteChildren(temp->process);
        printf("Gets here 2\n");
        child = child->next;
        free(temp->process);
        free(temp);
    }
}

RCB * getResource(const char * rid, ResourceList * resourcelist){
    return resourcelist->r1;
}

//
// Main Operations
//
PCB * create (const char * name, int priority, PCB * curr_process, ReadyList * readylist){
    PCB * new_process = malloc(sizeof(struct PCB));
    new_process->pid = strdup(name);
    new_process->priority = priority;

    if(strcmp(name, "init") != 0){
        new_process->parent = curr_process;
        childAdd(curr_process, new_process);
    }

    readyInsert(readylist, new_process);
    //Scheduler()
    return new_process;
}
void request(const char * rid, int units, ResourceList * resourcelist){
    RCB * resource = resourcelist->r1;
}
void scheduler(PCB * active_process, ReadyList * readylist){
    PCB * highest_ready;
    if(readylist->system != NULL){
        ProcessNode * temp = readylist->system;
        highest_ready = temp->process;
    } else if (readylist->user){
        ProcessNode * temp = readylist->user;
        highest_ready = temp->process;
    } else {
        ProcessNode * temp = readylist->init;
        highest_ready = temp->process;
    }

    if(active_process->priority < highest_ready->priority ||
            active_process->state != RUNNING ||
            active_process == NULL){
        active_process = highest_ready;
    }
}

//
// Debugging
//

/*
void printReadyList(ReadyList * readylist, int priority){
    // Prints every process in the ready list with specified priority
    ProcessNode * temp = NULL;
    if(priority == INIT) {
        temp = readylist->init;
    } else if (priority == USER) {
        temp = readylist->user;
    } else if (priority == SYSTEM){
        temp = readylist->system;
    }
    if(temp != NULL) {
        printf("\tProcesses with priority level %d: ", priority);
        while (temp != NULL) {
            PCB *proc = temp->process;
            printf("%s", proc->pid);
            temp = temp->next;
            if(temp != NULL){
                printf(", ");
            }
        }
        printf("\n");
    } else {
        printf("\tNo processes with priority level %d\n", priority);
    }
}
*/


void printReadyList(ReadyList * readylist){
    // Prints every process in the ready list

    ProcessNode ** prioritylist = readylist->priorities;
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i) {
        ProcessNode * node = prioritylist[i];
        if(node != NULL) {
            printf("\tProcesses with priority level %d: ", i);

            while (node != NULL) {
                printf("%s", getProcessName(node->process));
                node = node->next;
                if (node != NULL) {
                    printf(", ");
                }
            }
        } else {
            printf("\tNo processes with priority level %d\n", i);
        }
    }
}



void printTree(char * name, ReadyList * readylist){
    int counter = 3;
    while(counter != 0) {
        ProcessNode *temp = NULL;
        if(counter == 3){
            temp = readylist->system;
        } else if (counter == 2){
            temp = readylist->user;
        } else if (counter == 1){
            temp = readylist->init;
        }
        while (temp != NULL) {
            PCB *proc = temp->process;
            if (!strcmp(proc->pid, name)) {
                printf("\tProcess: %s\n", proc->pid);
                printf("\tParent: %s\n", proc->parent->pid);
                ProcessNode * child = proc->child;
                printf("\tChildren: ");
                if(child == NULL){
                    printf("None.\n");
                }
                while(child != NULL){
                    PCB * child_proc = child->process;
                    printf("%s", child_proc->pid);
                }
            }
            temp = temp->next;
        }
        --counter;
    }
}
