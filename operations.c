//
// Created by Sam Leyva on 1/13/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "macros.h"
#include "structures.h"

// Helper Functions
ReadyList * initReadyList(){
    ReadyList * readylist = (ReadyList *)malloc(sizeof(ReadyList));
    readylist->init = NULL;
    readylist->system = NULL;
    readylist->user = NULL;
    return readylist;
}

void freeReadyList(ReadyList * readylist){
    // Insert some check that all ready lists (i.e. init, sys, user are free)
    ProcessNode * current_node;

    current_node = readylist->system;
    while(current_node != NULL){
        ProcessNode * temp = current_node;
        current_node = current_node->next;
        free(temp->process);
        free(temp);
    }
    current_node = readylist->user;
    while(current_node != NULL){
        ProcessNode * temp = current_node;
        current_node = current_node->next;
        free(temp->process);
        free(temp);
    }
    current_node = readylist->init;
    while(current_node != NULL){
        ProcessNode * temp = current_node;
        current_node = current_node->next;
        free(temp->process);
        free(temp);
    }
    free(readylist);
}

RCB * initResource(const char * name, int resource_count){
    RCB * resource = (RCB *)malloc(sizeof(RCB));
    resource->rid = strdup(name);
    resource->inventory = resource_count;
    resource->waitinglist = NULL;
    return resource;
}

BlockList * initBlockList(){
    BlockList * blocklist = (BlockList *)malloc(sizeof(BlockList));
    blocklist->r1 = initResource("R1", 1);
    blocklist->r2 = initResource("R2", 2);
    blocklist->r3 = initResource("R3", 3);
    blocklist->r4 = initResource("R4", 4);
    return blocklist;
}

void freeBlockList(BlockList * blocklist){
    free(blocklist->r1);
    free(blocklist->r2);
    free(blocklist->r3);
    free(blocklist->r4);
    free(blocklist);
}

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

void insert(ReadyList * readylist, PCB * process){
    ProcessNode * new_node = createNode(process, NULL);

    if(!strcmp(process->pid, "init")){
        readylist->init = new_node;

    } else if (process->priority == USER){
        if(readylist->user == NULL){
            readylist->user = new_node;
        } else {
            ProcessNode * temp = readylist->user;
            while(temp->next != NULL){
                temp = temp->next;
            }
            temp->next = new_node;
        }

    } else if (process->priority == SYSTEM) {
        if (readylist->system == NULL) {
            readylist->system = new_node;
        } else {
            ProcessNode *temp = readylist->system;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = new_node;
        }
    }
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
        addChild(curr_process, new_process);
    }

    insert(readylist, new_process);
    //Scheduler()
    return new_process;
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
        printf("Processes with priority level %d: ", priority);
        while (temp != NULL) {
            PCB *proc = temp->process;
            printf("%s, ", proc->pid);
            temp = temp->next;
        }
        printf("\n");
    } else {
        printf("No processes with priority level %d\n", priority);
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
                printf("Process: %s\n", proc->pid);
                printf("Parent: %s\n", proc->parent->pid);
                ProcessNode * child = proc->child;
                printf("Children: ");
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