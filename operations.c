//
// Created by Sam Leyva on 1/13/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "macros.h"
#include "structures.h"

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

// # Process Control Block (PCB)
char * getProcessName(PCB * process){
    return process->pid;
}
int getProcessState(PCB * process){
    return process->state;
}
int getProcessPriority(PCB * process){
    return process->priority;
}

// # Resource Control Block (RCB)
char * getResourceName(RCB * resource){
    return resource->rid;
}
int getResourceSize(RCB * resource){
    return resource->size;
}
int getResourceSpace(RCB * resource){
    return resource->inventory;
}
ProcessNode * getResourceWaitlist(RCB * resource){
    return resource->waitinglist;
}

// # Ready List
PCB * getActiveProcess(ReadyList * readylist){
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        if(readylist->priorities[i] != NULL){
            ProcessNode * temp = readylist->priorities[i];
            return temp->process;
        }
    }
    return NULL;
}
ReadyList * readyInit(){
    ReadyList * readylist = (ReadyList *)malloc(sizeof(ReadyList));
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        readylist->priorities[i] = NULL;
    }
    return readylist;
}
void readyFree(ReadyList * readylist){
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
        if(*pnode_list == NULL){
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
RCB * resourceCreate(const char * name, int resource_count){
    RCB * resource = (RCB *)malloc(sizeof(RCB));
    resource->rid = strdup(name);
    resource->size = resource_count;
    resource->inventory = resource_count;
    resource->waitinglist = NULL;
    return resource;
}
ResourceList * resourceInit() {
    ResourceList * resourcelist = (ResourceList *)malloc(sizeof(ResourceList));
    const char * names[] = {"R1", "R2", "R3", "R4"};
    for(int i = 0; i < NUM_OF_RESOURCES; ++i){
        resourcelist->resources[i] = resourceCreate(names[i], i+1);
    }
    return resourcelist;
}
void resourceFree(ResourceList * resourcelist){
    RCB ** resources = resourcelist->resources;
    for(int i = 0; i < NUM_OF_RESOURCES; ++i){
        RCB * resource = resources[i];
        free(resource);
    }
    free(resourcelist);
}


// # Debugging
void printReadyList(ReadyList * readylist){
    // Prints every process in the ready list
    ProcessNode ** prioritylist = readylist->priorities;
    for(int i = NUM_OF_PRIORITIES - 1; i >= 0; --i) {
        ProcessNode * pnode = prioritylist[i];
        if(pnode != NULL) {
            printf("\tProcesses with priority level %d: ", i);

            while (pnode != NULL) {
                printf("%s", getProcessName(pnode->process));
                pnode = pnode->next;
                if (pnode != NULL) {
                    printf(", ");
                }
            }
            printf("\n");
        } else {
            printf("\tNo processes with priority level %d\n", i);
        }
    }
}
void printTree(const char * name, ReadyList * readylist){
    // Prints process, its parent, and its children
    ProcessNode ** prioritylist = readylist->priorities;
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i) {
        ProcessNode * pnode = prioritylist[i];
        while (pnode != NULL) {
            PCB * proc = pnode->process;
            if (!strcmp(getProcessName(proc), name)) {
                printf("\tProcess: %s\n", getProcessName(proc));

                printf("\tParent: ");
                if(proc->parent) {
                    printf("%s\n", getProcessName(proc->parent));
                } else {
                    printf("None\n");
                }

                printf("\tChildren: ");
                if(proc->child) {
                    ProcessNode *child = proc->child;
                    while (child != NULL) {
                        printf("%s", getProcessName(child->process));
                        child = child->next;
                        printf(child ? ", " : "\n");
                    }
                } else {
                    printf("None\n");
                }
            }
            pnode = pnode->next;
        }
    }
}





//
// Main Operations
//
PCB * create (const char * name, int priority, ReadyList * readylist){
    PCB * new_process = malloc(sizeof(struct PCB));
    PCB * active_process = getActiveProcess(readylist);
    new_process->pid = strdup(name);
    new_process->priority = priority;
    if(priority != INIT) {
        new_process->parent = active_process;
        childAdd(active_process, new_process);
    }

    readyInsert(readylist, new_process);
    //Scheduler()
    return new_process;
}
/*
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

*/
// WORK IN PROGRESS
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

 /*
RCB * getResource(const char * rid, ResourceList * resourcelist){
    return resourcelist->r1;
}
*/