//
// Created by Sam Leyva on 1/13/18.
//

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


// Main Operations
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

