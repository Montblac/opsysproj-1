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
                printf("Received 'init' command.\n");
                freeReadyList(readylist);
                freeBlockList(blocklist);
                readylist = initReadyList();
                blocklist = initBlockList();
                active_process = create("init", 0, NULL, readylist);
                // Write to file : "init"


            } else if (!strcmp(command, "cr")) {
                printf("Received 'cr' command.\n");
                char * name = strdup(strtok(NULL, " \n"));
                int priority = (int) strtol(strdup(strtok(NULL, " \n")), NULL, 10);
                if(strtok(NULL, " \n")){
                    printf("Invalid command; please try again.\n");
                }
                PCB * temp = create(name, priority, active_process, readylist);

            } else if (!strcmp(command, "de")) {
                printf("Received 'de' command.\n");

                ProcessNode *new_test = readylist->system;
                while(new_test != NULL) {
                    PCB * new_test2 = new_test->process;
                    PCB * parent = new_test2->parent;
                    printf("Parent %s\n", parent->pid);
                    new_test = new_test->next;
                }

            } else if (!strcmp(command, "req")) {
                printf("Received 'req' command.\n");

            } else if (!strcmp(command, "rel")) {
                printf("Received 'rel' command.\n");

            } else if (!strcmp(command, "to")) {
                printf("Received 'to' command.\n");

            } else if (!strcmp(command, "exit")) {
                printf("Exiting session...\n");
                break;

            } else if (!strcmp(command, "showProcesses")) {
                // For Debugging Purposes
                printf("Received 'showProcesses' command.\n");
                printReadyList(readylist, SYSTEM);
                printReadyList(readylist, USER);
                printReadyList(readylist, INIT);

            } else if (!strcmp(command, "showTree")) {
                // For Debugging Purposes
                printf("Received 'showTree' command.\n");
                char * name = strdup(strtok(NULL, "\n"));
                printTree(name, readylist);

            } else {
                printf("Invalid command; please try again.\n");
            }
            printf("shell >> ");
		}

        // Freeing Resources
        freeBlockList(blocklist);
        freeReadyList(readylist);
	}

}
