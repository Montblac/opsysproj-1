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
        ReadyList * readylist = readyInit();
        ResourceList * resourcelist = resourceInit();
        PCB * active_process = create("init", 0, readylist);

        // Initialize Shell
		size_t size; ssize_t input_size; char * input = NULL;
        printf("shell >> ");
        while( (input_size = getline(&input, &size, stdin) ) != EOF ){
            char * command = strtok(input, " \n");

            if(!strcmp(command, "init")) {
                printf("\tReceived 'init' command.\n");
                readyFree(readylist);
                resourceFree(resourcelist);
                readylist = readyInit();
                resourcelist = resourceInit();
                active_process = create("init", 0, readylist);
                // Write to file : "init"


            } else if (!strcmp(command, "cr")) {
                printf("\tReceived 'cr' command.\n");
                char * name = strdup(strtok(NULL, " \n"));
                int priority = (int) strtol(strdup(strtok(NULL, " \n")), NULL, 10);

                if(strtok(NULL, " \n") || priority == INIT || !strcmp(name, "init")){
                    printf("\tInvalid command; please try again.\n");
                } else {
                    PCB *temp = create(name, priority, readylist);
                }

            } else if (!strcmp(command, "de")) {
                printf("\tReceived 'de' command.\n");
                char * name = strdup(strtok(NULL, " \n"));
                ProcessNode ** prioritylist = readylist->priorities;
                int found = 0;

                for(int i = 0; i < NUM_OF_PRIORITIES; ++i){
                    ProcessNode ** pnode = &prioritylist[i];
                    ProcessNode * temp = *pnode;
                    ProcessNode * prev = temp;
                    PCB * proc;

                    if(temp != NULL && !strcmp(getProcessName(proc = temp->process), name)) {
                        *pnode = temp->next;
                        deleteChildren(proc);
                        free(proc);
                        free(temp);
                        found = 1;
                        break;
                    }

                    while (temp != NULL && strcmp(getProcessName(proc), name) != 0) {
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
                    found = 1;
                }

                if(!found){
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
                printReadyList(readylist);

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
        readyFree(readylist);
        resourceFree(resourcelist);
	}

}
