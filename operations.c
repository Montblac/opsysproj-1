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
    PCB * process = (PCB *)malloc(sizeof(PCB));
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
void freeProcessNodes(ProcessNode * head){
	ProcessNode * temp = head;
	while(head != NULL){
		head = head->next;
		free(temp);
		temp = head;
	}
}
void freeResourceNodes(ResourceNode * head){
	ResourceNode * temp = head;
	while(head != NULL){
		head = head->next;
		free(temp);
		temp = head;
	}
}
void freeReadylist(ReadyList * readylist){
	for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
		ProcessNode * head = readylist->priorities[i];
		ProcessNode * temp = head;
		while(temp != NULL){
			PCB * process = temp->process;
			freeProcessNodes(process->child);
			freeResourceNodes(process->resources);
			free(process->pid);
			free(process);
			temp = temp->next;
		}
		freeProcessNodes(head);
	}
	free(readylist);
	/*
    ProcessNode ** prioritylist = readylist->priorities;
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
        ProcessNode * node = prioritylist[i];
		ProcessNode * head = node;
        while(node != NULL){
            ProcessNode * temp = node;
            node = node->next;
            PCB * proc = temp->process;

			freeProcessNodes(proc->child);
			freeResourceNodes(proc->resources);	
			free(proc->pid);
            free(temp->process);
        }
		freeProcessNodes(head);
    }
    free(readylist);
	*/
}
void freeResourcelist(ResourceList * resourcelist){
    RCB ** resources = resourcelist->resources;
    for(int i = 0; i < NUM_OF_RESOURCES; ++i){
        RCB * resource = resources[i];
        free(resource->rid);
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
void insertChild(PCB * src, PCB * child){
    ProcessNode * node = createPNode(child, NULL);
    if (src->child == NULL){
        src->child = node;
    } else {
        ProcessNode * temp = src->child;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = node;
    }
}
void insertProcess(ReadyList * readylist, PCB * process){
    int priority = getProcessPriority(process);
    ProcessNode ** pnodes = &readylist->priorities[priority];
    ProcessNode * node = createPNode(process, NULL);

    if(priority == INIT || *pnodes == NULL){
        *pnodes = node;
    } else {
        ProcessNode * temp = *pnodes;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = node;
    }
}
void insertResource(PCB * process, RCB * resource, int n){
    ResourceNode ** presources = &process->resources;
    for(int i = 0; i < n; ++i) {
        ResourceNode *node = createRNode(resource, NULL);

        if(*presources == NULL){
            *presources = node;
        } else {
            ResourceNode * head = *presources;
            while(head->next != NULL){
                head = head->next;
            }
            head->next = node;
        }
    }
}
void insertWaitlist(PCB * process, RCB * resource){
    ProcessNode ** waitlist = &resource->waitinglist;
    ProcessNode * node = createPNode(process, NULL);

    if(*waitlist == NULL){
        *waitlist = node;
    } else {
        ProcessNode * head = *waitlist;
        while(head->next != NULL){
            head = head->next;
        }
        head->next = node;
    }
}

// # Deletion
void removeProcess(ReadyList * readylist, PCB * process){
    const char * pid = getProcessName(process);
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i) {
        ProcessNode **priority = &readylist->priorities[i];
        ProcessNode *head = *priority;
        ProcessNode *prev = head;

        if (head != NULL &&
            head->process != NULL &&
            !strcmp(pid, getProcessName(head->process))) {
            *priority = head->next;
			free(head);
            break;
        }
        while (head != NULL &&
               head->process != NULL &&
               strcmp(pid, getProcessName(head->process)) != 0) {
            prev = head;
            head = head->next;
        }
        if (head == NULL) {
            continue;
        }
        prev->next = head->next;
		free(head);
    }
}
void removeResource(PCB * process, RCB * resource, int n){
    const char * rid = getResourceName(resource);
    ResourceNode ** presources = &process->resources;

    for(int i = 0; i < n; ++i){
        ResourceNode * head = *presources;
        if(*presources != NULL && !strcmp(rid, getResourceName((*presources)->resource))){
            *presources = (*presources)->next;
            free(head);
        } else {
            ResourceNode * prev = head;
            while(head != NULL && strcmp(rid, getResourceName((*presources)->resource)) != 0){
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
    PCB * new_process = (PCB *)malloc(sizeof(PCB));
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
    removeProcess(readylist, *active_process);
    setProcessState(*active_process, READY);
    insertProcess(readylist, *active_process);
    scheduler(active_process, readylist);
}

// ## Create
void create (const char * name, int priority, ReadyList * readylist, PCB ** active_process){
    PCB * new_process = createProcess(name, priority, *active_process);
    insertChild(*active_process, new_process);
    insertProcess(readylist, new_process);
    setProcessState(new_process, READY);
    scheduler(active_process, readylist);
}


/*
int delete(const char * pid, ReadyList * readylist, ResourceList * resourcelist, PCB ** active_proc){
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
                killTree(proc, readylist, resourcelist);

                for(int j = 0; j < NUM_OF_RESOURCES; ++j){
                    RCB ** resource = &resourcelist->resources[j];
                    removeWaitlisted(*resource, getProcessName(proc));
                }

                ResourceNode * rnode = proc->resources;
                while(rnode != NULL){
                    RCB * resource = rnode->resource;
                    removeResource(proc, resource, 1);
                    rnode = rnode->next;
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
*/

// ## Request
void request(const char * rid, int units, ReadyList * readylist, ResourceList * resourcelist, PCB ** active_process){
    RCB * resource = findResource(rid, resourcelist);
    if(resource != NULL) {
        if(units > getResourceSize(resource)){
            printf("\tInvalid command; request amount larger than size.\n");
        }
        else if (units <= getResourceSpace(resource)) {
            resource->inventory -= units;
            setProcessRequested(*active_process, units);
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
// ## Delete
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
            free(head);
        } else {
            ProcessNode * prev = head;
            head = head->next;
            while (head != NULL) {
                child = head->process;
                if (!strcmp(child->pid, pid)) {
                    prev->next = head->next;
                    free(head);
                    break;
                }
                prev = head;
                head = head->next;
            }
        }
    }
}

/*
void killTree(PCB * src, ReadyList * readylist){
    ProcessNode * child = src->child;
    while(child != NULL){
        killTree(child->process, readylist);
        killChild(&(src->child), getProcessName(child->process));

        ResourceNode * rnode = src->resources;
        RCB * resource;
        while(rnode != NULL){
            resource = rnode->resource;
            removeResource(child->process, resource, 1); // Remove if broken
            removeWaitlisted(resource, getProcessName(src));
            rnode = rnode->next;
        }
        killProcess(readylist, getProcessName(child->process));
        child = child->next;
    }
}
*/
void killTree(PCB * src, ReadyList * readylist, ResourceList * resourcelist, PCB ** active_process){
    ProcessNode * child = src->child;
    while(child != NULL){
        killTree(child->process, readylist, resourcelist, active_process);
        killChild(&(src->child), getProcessName(child->process));
        child = child->next;
    }
    ResourceNode * rnode = src->resources;
    RCB * resource;
    while(rnode != NULL){
        resource = rnode->resource;
        //removeResource(src, resource, 1);
        rnode = rnode->next;
        release(getResourceName(resource), 1, resourcelist, readylist, active_process);
        //rnode = rnode->next;
    }

    for(int j = 0; j < NUM_OF_RESOURCES; ++j){
        RCB ** res = &resourcelist->resources[j];
        removeWaitlisted(*res, getProcessName(src));
    }
    updateParent(src);
    killProcess(readylist, getProcessName(src));
}


int delete(const char * pid, ReadyList * readylist, ResourceList * resourcelist, PCB ** active_proc) {
    ProcessNode ** prioritylist = readylist->priorities;
    for(int i = 0; i < NUM_OF_PRIORITIES; ++i) {
        ProcessNode **pnode = &prioritylist[i];
        ProcessNode *temp = *pnode;
        PCB *proc;

        while (temp != NULL) {
            if (!strcmp(getProcessName(proc = temp->process), pid)) {
                if (!strcmp(getProcessName(*active_proc), getProcessName(proc))) {
                    *active_proc = NULL;
                }
                killTree(proc, readylist, resourcelist, active_proc);
                scheduler(active_proc, readylist);
                return 1;
            }
            temp = temp->next;
        }
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
int isInRange(int num){
    return num == USER || num == SYSTEM;
}
int isInRange2(int num){
    return num >= 1 && num <= 4;
}

// # I/O
void writeoutput(const char * pid, FILE * file){
    fputs(pid, file);
    fputs(" ", file);
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
