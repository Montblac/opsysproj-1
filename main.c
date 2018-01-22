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
        PCB * active_process = init(readylist);
        printf("\t*Process %s is running\n", getProcessName(active_process));


        // Initialize Shell
		size_t size; ssize_t input_size; char * input = NULL;
        printf("shell >> ");
        while( (input_size = getline(&input, &size, stdin) ) != EOF ){
            char * command = strtok(input, " \n");

            if(!strcmp(command, "init")) {
                readyFree(readylist);
                resourceFree(resourcelist);
                readylist = readyInit();
                resourcelist = resourceInit();
                active_process = init(readylist);
                // Write to file : "init"


            } else if (!strcmp(command, "cr")) {
                char * name = strdup(strtok(NULL, " \n"));
                int priority = (int) strtol(strdup(strtok(NULL, " \n")), NULL, 10);

                if(strtok(NULL, " \n") || priority == INIT || !strcmp(name, "init")){
                    printf("\tInvalid command; please try again.\n");
                } else {
                    create(name, priority, readylist, &active_process);
                }

            } else if (!strcmp(command, "de")) {
                char * pid = strdup(strtok(NULL, " \n"));
                if(!strcmp(pid, "init")){
                    printf("\tCannot delete %s process.\n", pid);
                } else if (!delete(pid, readylist)){
                    printf("\tProcess %s does not exist.\n", pid);
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
            printf("\t*Process %s is running\n", getProcessName(active_process));
            printf("shell >> ");
		}

        // Freeing Resources
        readyFree(readylist);
        resourceFree(resourcelist);
	}

}
