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
                char * temp = strtok(NULL, " \n");
                char * name = isWord(temp) ? strdup(temp) : NULL;

                temp = strtok(NULL, " \n");
                char * priority = isNumber(temp) ? strdup(temp) : NULL;
                int pval = priority != NULL ? (int) strtol(priority, NULL, 10) : -1;

                if(name && priority && strtok(NULL, " \n") == NULL && isInRange(pval) ){
                    create(name, pval, readylist, &active_process);
                }
                else {
                    printf("\tInvalid command; please try again.\n");
                }

            } else if (!strcmp(command, "de")) {
                char * temp = (strtok(NULL, " \n"));
                char * pid = isWord(temp) ? strdup(temp) : NULL;

                if(!pid || strtok(NULL, " \n")){
                    printf("\tInvalid command; please try again.\n");
                } else if (!strcmp(pid, "init")){
                    printf("\tCannot delete %s process.\n", pid);
                } else if (!delete(pid, readylist, &active_process)){
                    printf("\tProcess %s does not exist.\n", pid);
                }


            } else if (!strcmp(command, "req")) {
                printf("\tReceived 'req' command.\n");

            } else if (!strcmp(command, "rel")) {
                printf("\tReceived 'rel' command.\n");

            } else if (!strcmp(command, "to")) {
                printf("\tReceived 'to' command.\n");
                timeout(readylist, &active_process);

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
