//
// Created by Sam Leyva on 1/13/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
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
void setProcessState(PCB * process, int state){
    process->state = state;
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
PCB * init(ReadyList * readylist){
    PCB * new_process = (PCB *)malloc(sizeof(struct PCB));
    new_process->pid = strdup("init");
    new_process->priority = INIT;
    readyInsert(readylist, new_process);
    setProcessState(new_process, RUNNING);
    return new_process;
}
void preempt(PCB ** active_proc, PCB * new_proc){
    setProcessState(new_proc, RUNNING);
    *active_proc = new_proc;
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

// Input-Checking
int isWord(const char * input){
    if(input == NULL) {
        return 0;
    }
    for (int i = 0; i < strlen(input); ++i) {
        if (!isalpha(input[i])) {
            return 0;
        }
    }
    return 1;
}
int isNumber(const char * input){
    if(input == NULL){
        return 0;
    }
    for(int i = 0; i < strlen(input); ++i){
        if(!isdigit(input[i])){
            return 0;
        }
    }
    return 1;

}
int isInRange(const int num){
    return num == 1 || num == 2;
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


void scheduler(PCB ** active_proc, ReadyList * readylist){
    PCB * highest_proc;
    ProcessNode ** prioritylist = readylist->priorities;
    for(int i = NUM_OF_PRIORITIES - 1; i >= 0; --i) {
        ProcessNode * pnode = prioritylist[i];
        if (pnode != NULL) {
            highest_proc = pnode->process;
            break;
        }
    }
    if(getProcessPriority(*active_proc) < getProcessPriority(highest_proc) ||
       getProcessState(*active_proc) != RUNNING ||
       *active_proc == NULL) {
        preempt(active_proc, highest_proc);
    }
}
void create (const char * name, int priority, ReadyList * readylist, PCB ** active_process){
    PCB * new_process = malloc(sizeof(struct PCB));
    new_process->pid = strdup(name);
    new_process->priority = priority;
    new_process->parent = *active_process;
    childAdd(*active_process, new_process);

    readyInsert(readylist, new_process);
    setProcessState(*active_process, READY);
    scheduler(active_process, readylist);
}
int delete(const char * pid, ReadyList * readylist, PCB ** active_proc){
    ProcessNode ** prioritylist = readylist->priorities;
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        ProcessNode ** pnode = &prioritylist[i];
        ProcessNode * temp = *pnode;
        ProcessNode * prev = temp;
        PCB * proc;

        if(temp != NULL && !strcmp(getProcessName(proc = temp->process), pid)) {
            *pnode = temp->next;
            deleteChildren(proc);
            free(proc);
            free(temp);
            scheduler(active_proc, readylist);
            printf("Finishing ...\n");
            return 1;
        }

        while (temp != NULL && strcmp(getProcessName(proc), pid) != 0) {
            prev = temp;
            temp = temp->next;
            if(temp != NULL) {
                proc = temp->process;
            }
        }
        if(temp == NULL) {
            continue;
        }
        prev->next = temp->next;
        deleteChildren(proc);
        free(proc);
        free(temp);
        scheduler(active_proc, readylist);
        printf("Finishing 1...\n");
        return 1;
    }
    return 0;
}

/*
void request(const char * rid, int units, ResourceList * resourcelist){
    RCB * resource = resourcelist->r1;
}
*/




// WORK IN PROGRESS
 /*
RCB * getResource(const char * rid, ResourceList * resourcelist){
    return resourcelist->r1;
}
*/