//
// Created by Sam Leyva on 1/13/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include "macros.h"
#include "structures.h"

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


// # Ready List
PCB * findProcess(const char * pid, ReadyList * readylist){
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        ProcessNode * pnode = readylist->priorities[i];
        while(pnode != NULL){
            if(!strcmp(pid, getProcessName(pnode->process))){
                return pnode->process;
            }
            pnode = pnode->next;
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



// ## Scheduler
void preempt(PCB ** active_proc, PCB * new_proc){
    setProcessState(new_proc, RUNNING);
    *active_proc = new_proc;
}
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
    if(*active_proc == NULL ||
            getProcessPriority(*active_proc) < getProcessPriority(highest_proc) ||
            getProcessState(*active_proc) != RUNNING) {
        preempt(active_proc, highest_proc);
    }
}


// ## Create
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

// ## Delete
void killProcess(ReadyList * readylist, const char * pid){
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        ProcessNode ** priority = &(readylist->priorities[i]);
        ProcessNode * head = *priority;
        ProcessNode * prev = head;

        if(head != NULL && head->process != NULL && !strcmp(pid, getProcessName(head->process))){
            *priority = head->next;
            free(head->process);
            free(head);
            break;
        }

        while(head != NULL && head->process != NULL && strcmp(pid, getProcessName(head->process))){
            prev = head;
            head = head->next;
        }
        if(head == NULL){
            continue;
        }
        prev->next = head->next;
        free(head->process);
        free(head);
    }
}
void killChild(ProcessNode ** pnode, const char * pid){
    ProcessNode * head = *pnode;
    if(head != NULL) {
        PCB *child = head->process;
        if (!strcmp(child->pid, pid)) {
            *pnode = head->next;
        } else {
            ProcessNode * prev = head;
            head = head->next;
            while (head != NULL) {
                child = head->process;
                if (!strcmp(child->pid, pid)) {
                    prev->next = head->next;
                    break;
                }
                prev = head;
                head = head->next;
            }
        }
    }
}
void killTree(PCB * src, ReadyList * readylist){
    ProcessNode * child = src->child;
    while(child != NULL){
        killTree(child->process, readylist);
        killChild(&(src->child), getProcessName(child->process));
        killProcess(readylist, getProcessName(child->process));
        child = child->next;
    }
}
int updateParent(PCB * src){
    PCB * parent = src->parent;
    ProcessNode ** child = &(parent->child);
    ProcessNode * head = *child;
    ProcessNode * prev = head;

    if(head != NULL &&
       head->process != NULL &&
       !strcmp(getProcessName(src), getProcessName(head->process))){
        *child = head->next;
        return 1;
    }
    while(head != NULL &&
          head->process != NULL &&
          strcmp(getProcessName(src), getProcessName(head->process))){
        prev = head;
        head = head->next;
    }
    if(head == NULL){
        return 0;
    }
    prev->next = head->next;
    return 1;
}
int delete(const char * pid, ReadyList * readylist, PCB ** active_proc){
    ProcessNode ** prioritylist = readylist->priorities;
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        ProcessNode ** pnode = &prioritylist[i];
        ProcessNode * temp = *pnode;
        ProcessNode * prev = temp;
        PCB * proc;

        if(temp != NULL && !strcmp(getProcessName(proc = temp->process), pid)) {
            if (!strcmp(getProcessName(*active_proc), getProcessName(proc))) {
                *active_proc = NULL;
            }
            killTree(proc, readylist);
            updateParent(proc);
            killProcess(readylist, pid);
            scheduler(active_proc, readylist);
            return 1;
        }
        while(temp != NULL && strcmp(getProcessName(proc), pid)){
            prev = temp;
            temp = temp->next;
            if(temp != NULL){
                proc = temp->process;
            }
        }
        if(temp == NULL){
            continue;
        }
        if(!strcmp(getProcessName(*active_proc), getProcessName(proc))){
            *active_proc = NULL;
        }
        killTree(proc, readylist);
        updateParent(proc);
        killProcess(readylist, pid);
        scheduler(active_proc, readylist);
        return 1;
    }
    return 0;
}


// # Input-Checking
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