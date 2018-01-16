#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "structures.h"
#include "macros.h"
#include "operations.h"


int main(int argc, char * argv[]){
    // Checks for input and output files
    FILE * infile, * outfile;
    infile = (argc > 1) ? fopen(argv[1], "r") : fopen("input.txt", "r");
    outfile = (argc > 2) ? fopen(argv[2], "w") : fopen("output.txt", "r");


	if(infile){
		printf("Processing file...\n");

	} else {

        // Initialize System
        ReadyList * readylist = initReadyList();
        BlockList * blocklist = initBlockList();
        PCB * active_process = create("init", 0, NULL, readylist);


        //printf(readylist->user);
        //printf(readylist->system);


		size_t size; ssize_t input_size; char * input = NULL;

        printf("shell >> ");
        while( (input_size = getline(&input, &size, stdin) ) != EOF ){
            char * command = strtok(input, " \n");

            if(!strcmp(command, "init")) {
                printf("\tReceived 'init' command.\n");
                freeReadyList(readylist);
                freeBlockList(blocklist);
                readylist = initReadyList();
                blocklist = initBlockList();
                active_process = create("init", 0, NULL, readylist);
                // Write to file : "init"


            } else if (!strcmp(command, "cr")) {
                printf("\tReceived 'cr' command.\n");
                char * name = strdup(strtok(NULL, " \n"));
                int priority = (int) strtol(strdup(strtok(NULL, " \n")), NULL, 10);
                if(strtok(NULL, " \n")){
                    printf("\tInvalid command; please try again.\n");
                }
                PCB * temp = create(name, priority, active_process, readylist);

            } else if (!strcmp(command, "de")) {
                printf("\tReceived 'de' command.\n");
                char * name = strdup(strtok(NULL, " \n"));
                int counter = 3;
                while(counter > 0) {
                    ProcessNode ** head;

                    if (counter == 3) {
                        head = &readylist->system;
                    } else if (counter == 2) {
                        head = &readylist->user;
                    } else if (counter == 1) {
                        head = &readylist->init;
                    }

                    ProcessNode * temp = *head;
                    ProcessNode * prev = temp;
                    PCB * proc;

                    if(temp != NULL && !strcmp((proc = temp->process)->pid, name)) {
                        *head = temp->next;
                        deleteChildren(proc);
                        free(proc);
                        free(temp);
                        break;
                    }

                    while (temp != NULL && strcmp(proc->pid, name) != 0) {
                        prev = temp;
                        temp = temp->next;
                        if(temp != NULL) {
                            proc = temp->process;
                        }
                    }
                    if(temp == NULL) {
                        --counter;
                        continue;
                    }
                    prev->next = temp->next;
                    deleteChildren(proc);
                    free(proc);
                    free(temp);
                    break;
                }

                if(counter <= 0){
                    printf("\tProcess %s does not exist.\n", name);
                }


            } else if (!strcmp(command, "req")) {
                printf("\tReceived 'req' command.\n");

            } else if (!strcmp(command, "rel")) {
                printf("\tReceived 'rel' command.\n");

            } else if (!strcmp(command, "to")) {
                printf("\tReceived 'to' command.\n");

            } else if (!strcmp(command, "exit")) {
                printf("\tExiting session...\n");
                break;

            } else if (!strcmp(command, "showProcesses")) {
                // For Debugging Purposes
                printReadyList(readylist, SYSTEM);
                printReadyList(readylist, USER);
                printReadyList(readylist, INIT);

            } else if (!strcmp(command, "showTree")) {
                // For Debugging Purposes
                char * name = strdup(strtok(NULL, "\n"));
                printTree(name, readylist);

            } else {
                printf("\tInvalid command; please try again.\n");
            }
            printf("shell >> ");
		}

        // Freeing Resources
        freeBlockList(blocklist);
        freeReadyList(readylist);
	}

}
