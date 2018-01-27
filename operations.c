//
// Created by Sam Leyva on 1/13/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include "macros.h"
#include "structures.h"


// # Allocation
ProcessNode * createPNode(PCB * process, ProcessNode * next){
    ProcessNode * new_node = (ProcessNode *)malloc(sizeof(ProcessNode));
    new_node->process = process;
    new_node->next = next;
    return new_node;
}
ResourceNode * createRNode(RCB * resource, ResourceNode * next){
    ResourceNode * node = (ResourceNode*)malloc(sizeof(ResourceNode));
    node->resource = resource;
    node->next = next;
    return node;
}
PCB * createProcess(const char * pid, int priority, PCB * parent){
    PCB * process = malloc(sizeof(struct PCB));
    process->pid = strdup(pid);
    process->priority = priority;
    process->parent = parent;
    return process;
}
RCB * createResource(const char * name, int resource_count){
    RCB * resource = (RCB *)malloc(sizeof(RCB));
    resource->rid = strdup(name);
    resource->size = resource_count;
    resource->inventory = resource_count;
    resource->waitinglist = NULL;
    return resource;
}

// # De-allocation
void freeReadylist(ReadyList * readylist){
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
void freeResourcelist(ResourceList * resourcelist){
    RCB ** resources = resourcelist->resources;
    for(int i = 0; i < NUM_OF_RESOURCES; ++i){
        RCB * resource = resources[i];
        free(resource);
    }
    free(resourcelist);
}
void freeWaitlist(ResourceList * resourcelist){
    for(int i = 0; i < NUM_OF_RESOURCES; ++i){
        RCB * resource = resourcelist->resources[i];
        ProcessNode ** waitlist = &resource->waitinglist;
        ProcessNode * head = *waitlist;
        while(*waitlist != NULL){
            *waitlist = head->next;
            free(head);
            head = *waitlist;
        }
    }
}

// # Process Control Block (PCB) Mutator / Accessor
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
void setProcessList(PCB * process, ProcessNode * list){
    process->list = list;
}
void setProcessRequested(PCB * process, int amount){
    process->requested = amount;
}

// # Resource Control Block (RCB) Mutator / Accessor
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

// # Process Control Block Utility
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
PCB * findProcess2(const char * rid, ReadyList * readylist){
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        ProcessNode * pnode = readylist->priorities[i];
        while(pnode != NULL){
            PCB * process = pnode->process;
            ResourceNode * rnode = process->resources;
            if(rnode != NULL && !strcmp(rid, getResourceName(rnode->resource))){
                return pnode->process;
            }
            pnode = pnode->next;
        }
    }
    return NULL;
}

// # Resource Control Block Utility
RCB * findResource(const char * rid, ResourceList * resourcelist){
    for(int i = 0; i < NUM_OF_RESOURCES; ++i){
        if(!strcmp(getResourceName(resourcelist->resources[i]), rid)){
            return resourcelist->resources[i];
        }
    }
    return NULL;
}

// # Insertion
ProcessNode * insertChild(PCB * src, PCB * child){
    if (src->child == NULL){
        src->child = createPNode(child, NULL);
        return src->child;
    } else {
        ProcessNode * temp = src->child;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = createPNode(child, NULL);
        return temp->next;
    }
}
void insertProcess(ReadyList * readylist, PCB * process){
    int priority = getProcessPriority(process);
    ProcessNode ** pnode_list = &readylist->priorities[priority];
    ProcessNode * new_node = createPNode(process, NULL);

    if(priority == INIT || *pnode_list == NULL){
        *pnode_list = new_node;
    } else {
        ProcessNode * temp = *pnode_list;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = new_node;
    }
}
void insertResource(PCB * process, RCB * resource, int n){
    ResourceNode ** presources = &process->resources;
    for(int i = 0; i < n; ++i) {
        ResourceNode *new_node = createRNode(resource, NULL);

        if(*presources == NULL){
            *presources = new_node;
        } else {
            ResourceNode * head = *presources;
            while(head->next != NULL){
                head = head->next;
            }
            head->next = new_node;
        }
    }
}
void insertWaitlist(PCB * process, RCB * resource){
    ProcessNode ** waitlist = &resource->waitinglist;
    ProcessNode * new_node = createPNode(process, NULL);

    if(*waitlist == NULL){
        *waitlist = new_node;
    } else {
        ProcessNode * head = *waitlist;
        while(head->next != NULL){
            head = head->next;
        }
        head->next = new_node;
    }
}

// # Deletion
PCB * removeProcess(ReadyList * readylist, PCB * process){
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        ProcessNode ** priority = &readylist->priorities[i];
        ProcessNode * head = *priority;
        ProcessNode * prev = head;

        if(head != NULL &&
           head->process != NULL &&
           !strcmp(getProcessName(process), getProcessName(head->process))){
            *priority = head->next;
            return head->process;
        }

        while(head != NULL &&
              head->process != NULL &&
              strcmp(getProcessName(process), getProcessName(head->process)) != 0){
            prev = head;
            head = head->next;
        }
        if(head == NULL){
            continue;
        }
        prev->next = head->next;
        return head->process;
    }
    return NULL;
}
void removeResource(PCB * process, RCB * resource, int n){
    ResourceNode ** presources = &process->resources;

    for(int i = 0; i < n; ++i){
        ResourceNode * head = *presources;
        if(*presources != NULL && !strcmp(getResourceName((*presources)->resource), getResourceName(resource))){
            *presources = (*presources)->next;
            free(head);
        } else {
            ResourceNode * prev = head;
            while(head != NULL && strcmp(getResourceName((*presources)->resource), getResourceName(resource)) != 0){
                prev = head;
                head = head->next;
            }
            if( head != NULL) {
                prev->next = head->next;
                free(head);
            }
        }
    }
    resource->inventory += n;
}
void removeWaitlisted(RCB * resource, const char * pid){
    ProcessNode ** waitlist = &resource->waitinglist;
    ProcessNode * head = *waitlist;
    ProcessNode * prev = head;

    if(head != NULL && !strcmp(getProcessName(head->process), pid)){
        *waitlist = (*waitlist)->next;
        free(head);
    } else {
        while(head != NULL && strcmp(getProcessName(head->process), pid) != 0){
            prev = head;
            head = head->next;
        }
        if(head != NULL){
            prev->next = head->next;
            free(head);
        }
    }
}

// # Initialization
ReadyList * initReadylist(){
    ReadyList * readylist = (ReadyList *)malloc(sizeof(ReadyList));
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        readylist->priorities[i] = NULL;
    }
    return readylist;
}
ResourceList * initResourcelist() {
    ResourceList * resourcelist = (ResourceList *)malloc(sizeof(ResourceList));
    const char * names[] = {"R1", "R2", "R3", "R4"};
    for(int i = 0; i < NUM_OF_RESOURCES; ++i){
        resourcelist->resources[i] = createResource(names[i], i+1);
    }
    return resourcelist;
}
PCB * initProcess(ReadyList * readylist){
    PCB * new_process = (PCB *)malloc(sizeof(struct PCB));
    new_process->pid = strdup("init");
    new_process->priority = INIT;
    insertProcess(readylist, new_process);
    setProcessState(new_process, RUNNING);
    return new_process;
}



// ## Scheduler
void preempt(PCB ** active_proc, PCB * new_proc){
    setProcessState(new_proc, RUNNING);
    *active_proc = new_proc;
}
void scheduler(PCB ** active_proc, ReadyList * readylist){
    PCB * highest_proc = NULL;
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

// # Time-out Interrupt
void timeout(ReadyList * readylist, PCB ** active_process){
    PCB * process = removeProcess(readylist, *active_process);
    setProcessState(process, READY);
    insertProcess(readylist, process);
    scheduler(active_process, readylist);
}

// ## Create
void create (const char * name, int priority, ReadyList * readylist, PCB ** active_process){
    PCB * new_process = createProcess(name, priority, *active_process);
    insertChild(*active_process, new_process);
    insertProcess(readylist, new_process);
    setProcessState(*active_process, READY);
    scheduler(active_process, readylist);
}

// ## Delete
void killProcess(ReadyList * readylist, const char * pid){
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        ProcessNode ** priority = &readylist->priorities[i];
        ProcessNode * head = *priority;
        ProcessNode * prev = head;

        if(head != NULL && head->process != NULL && !strcmp(pid, getProcessName(head->process))){
            *priority = head->next;
            free(head->process);
            free(head);
            break;
        }

        while(head != NULL && head->process != NULL && strcmp(pid, getProcessName(head->process)) != 0){
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

        ResourceNode * rnode = src->resources;
        RCB * resource;
        while(rnode != NULL){
            resource = rnode->resource;
            removeWaitlisted(resource, getProcessName(src));
            rnode = rnode->next;
        }

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
          strcmp(getProcessName(src), getProcessName(head->process)) != 0){
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
        PCB * proc;

        while(temp != NULL){
            if(!strcmp(getProcessName(proc = temp->process), pid)){
                if(!strcmp(getProcessName(*active_proc), getProcessName(proc))){
                    *active_proc = NULL;
                }
                killTree(proc, readylist);

                ResourceNode * rnode = proc->resources;
                RCB * resource;
                if(rnode != NULL){
                    resource = rnode->resource;
                    removeWaitlisted(resource, getProcessName(proc));
                }


                updateParent(proc);
                killProcess(readylist, pid);
                scheduler(active_proc, readylist);
                return 1;
            }
            temp = temp->next;
        }
    }
    return 0;
}

// ## Request
void request(const char * rid, int units, ResourceList * resourcelist, ReadyList * readylist, PCB ** active_process){
    RCB * resource = findResource(rid, resourcelist);
    if(resource != NULL) {
        if(units > getResourceSize(resource)){
            printf("\tInvalid command; request amount larger than size.\n");
        }
        else if (units <= getResourceSpace(resource)) {
            resource->inventory -= units;
            insertResource(*active_process, resource, units);
            printf("\tAdded resource %s to process %s.\n", rid, getProcessName(*active_process));
        } else {
            setProcessState(*active_process, BLOCKED);
            setProcessRequested(*active_process, units);
            setProcessList(*active_process, getResourceWaitlist(resource));
            removeProcess(readylist, *active_process);
            insertWaitlist(*active_process, resource);
            printf("\tAdded process %s to resource %s waitlist.\n", getProcessName(*active_process), rid);
        }
        scheduler(active_process, readylist);
    }
}

// ## Release
void release(const char * rid, int units, ResourceList * resourcelist, ReadyList * readylist, PCB ** active_process){
    RCB * resource = findResource(rid, resourcelist);
    PCB * process = findProcess2(rid, readylist);
    if(process != NULL) {
        removeResource(process, resource, units);
        ProcessNode *pnode = resource->waitinglist;

        while (pnode != NULL) {
            PCB *proc = pnode->process;
            if (resource->inventory >= proc->requested) {
                resource->inventory -= proc->requested;
            }
            removeWaitlisted(resource, getProcessName(proc));
            setProcessState(proc, READY);
            setProcessList(proc, readylist->priorities[getProcessPriority(proc)]);
            insertResource(proc, resource, units);
            insertProcess(readylist, proc);
            pnode = pnode->next;
        }
        scheduler(active_process, readylist);

    }
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
int isInRange(int num){
    return num == USER || num == SYSTEM;
}
int isInRange2(int num){
    return num >= 1 && num <= 4;
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
void printWaitlist(const char * rid, ResourceList * resourcelist){
    RCB * resource = findResource(rid, resourcelist);
    ProcessNode * pnode = resource->waitinglist;
    printf("\t%s Waitlist: ", rid);
    if(pnode == NULL){
        printf("None.\n");
    } else {
        while(pnode != NULL){
            printf("%s ", getProcessName(pnode->process));
            pnode = pnode->next;
        }
        printf("\n");
    }
}