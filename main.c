#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "structures.h"
#include "operations.h"


int main(int argc, char * argv[]){
    FILE * infile, * outfile;




    // Comment out for file redirection in Linux host
    //infile = (argc > 1) ? fopen(argv[1], "r") : fopen("input.txt", "r");
    //outfile = (argc > 2) ? fopen(argv[2], "w+") : fopen("output.txt", "w+");

    // Comment out for user-specified filepaths
    printf("Input File Path: ");
    size_t isize; ssize_t iinput_size; char * iinput = NULL;
    iinput_size = getline(&iinput, &isize, stdin);
    char * filepath = strtok(iinput, " \n");
    infile = (filepath != NULL) ? fopen(filepath, "r") : fopen("input.txt", "r");

    printf("Output File Path: ");
    iinput_size = getline(&iinput, &isize, stdin);
    filepath = strtok(iinput, " \n");
    outfile = (filepath != NULL) ? fopen(filepath, "w+") : fopen("output.txt", "r");

    //infile = fopen("test.txt", "r");
    //outfile = fopen("out.txt", "w+");

	if(!infile){
		printf("Please include a file...\n");

	} else {

        // Initialize System
        ReadyList * readylist = initReadylist();
        ResourceList * resourcelist = initResourcelist();
        PCB * active_process = initProcess(readylist);
        writeoutput(getProcessName(active_process), outfile);

        //printf("\t*Process %s is running\n", getProcessName(active_process));

        // Initialize Shell
		size_t size; ssize_t input_size; char * input = NULL;
        //printf("shell >> ");

        // Comment out for user input
        // while((input_size = getline(&input, &size, stdin) ) != EOF ){	
		while((input_size = getline(&input, &size, infile)) != EOF) {
			char * command = strtok(input, " \n");

            if(command == NULL){
                //printf("\tPlease include a command.\n");
                fputs("\n", outfile);

            } else if(!strcmp(command, "init")) {
                freeReadylist(readylist);
                freeWaitlist(resourcelist);
                freeResourcelist(resourcelist);
                readylist = initReadylist();
                resourcelist = initResourcelist();
                active_process = initProcess(readylist);
                writeoutput(getProcessName(active_process), outfile);

            } else if (!strcmp(command, "cr")) {
                char * temp = strtok(NULL, " \n");
                char * name = isWord(temp) ? strdup(temp) : NULL;

                temp = strtok(NULL, " \n");
                char * priority = isNumber(temp) ? strdup(temp) : NULL;
                int pval = priority != NULL ? (int) strtol(priority, NULL, 10) : -1;

                if(name && priority
                   && strtok(NULL, " \n") == NULL
                   && isValidName(name)
                   && isInRange(pval)
                   && findProcess(name, readylist) == NULL
                   && findProcessBlocked(name, resourcelist) == NULL){
                    create(name, pval, readylist, &active_process);
                    writeoutput(getProcessName(active_process), outfile);
                }
                else {
                    //printf("\tInvalid command; please try again.\n");
                    writeoutput("error", outfile);
                }
				if(name){free(name);}
				if(priority){free(priority);}

            } else if (!strcmp(command, "de")) {
                char * temp = (strtok(NULL, " \n"));
                char * pid = isWord(temp) ? strdup(temp) : NULL;

                if(!pid || strtok(NULL, " \n")){
                    //printf("\tInvalid command; please try again.\n");
                    writeoutput("error", outfile);
                } else if (!strcmp(pid, "init")){
                    //printf("\tCannot delete %s process.\n", pid);
                    writeoutput("error", outfile);
                } else if (!delete(pid, readylist, resourcelist, &active_process)){
                    //printf("\tProcess %s does not exist.\n", pid);
                    writeoutput("error", outfile);
                } else {
                    writeoutput(getProcessName(active_process), outfile);
                }
				if(pid){free(pid);};


            } else if (!strcmp(command, "req")) {
                char * temp = strtok(NULL, " \n");
                char * rid = strdup(temp);

                temp = strtok(NULL, " \n");
                char * units = isNumber(temp) ? strdup(temp) : NULL;
                int value = units != NULL ? (int)strtol(units, NULL, 10) : -1;

                if(rid && units && strtok(NULL, " \n") == NULL && isInRange2(value)
                    && (!strcmp(rid, "R1") || !strcmp(rid, "R2") ||
                        !strcmp(rid, "R3") || !strcmp(rid, "R4") )){
                    int n = request(rid, value, readylist, resourcelist, &active_process);
                    if(n == 0){
                        writeoutput("error", outfile);
                    } else {
                        writeoutput(getProcessName(active_process), outfile);
                    }
                } else {
                    writeoutput("error", outfile);
                }
				if(rid){free(rid);}
				if(units){free(units);}

            } else if (!strcmp(command, "rel")) {
                char * temp = strtok(NULL, " \n");
                char * rid = strdup(temp);

                temp = strtok(NULL, " \n");
                char * units = isNumber(temp) ? strdup(temp) : NULL;
                int value = units != NULL ? (int)strtol(units, NULL, 10) : -1;

                if(rid && units && strtok(NULL, " \n") == NULL && isInRange2(value)
                        && (!strcmp(rid, "R1") || !strcmp(rid, "R2") ||
                            !strcmp(rid, "R3") || !strcmp(rid, "R4") )){
                    release(rid, value, resourcelist, readylist, &active_process);
                    writeoutput(getProcessName(active_process), outfile);
                } else {
                    writeoutput("error", outfile);
                }
				if(rid){free(rid);}
				if(units){free(units);}

            } else if (!strcmp(command, "to")) {
                timeout(readylist, &active_process);
                writeoutput(getProcessName(active_process), outfile);

            } else if (!strcmp(command, "exit")) {
                //printf("\tExiting session...\n");
                break;

            } else if (!strcmp(command, "showProcesses")) {
                // For Debugging Purposes
                printReadyList(readylist);

            } else if (!strcmp(command, "showTree")) {
                // For Debugging Purposes
                char * name = strdup(strtok(NULL, "\n"));
                printTree(name, readylist);
				if(name){free(name);}

            } else if (!strcmp(command, "showWaitlist")) {
                // For Debugging Purposes
                char *rid = strdup(strtok(NULL, "\n"));
                printWaitlist(rid, resourcelist);
                if (rid) { free(rid); }

            } else if(!strcmp(command, "showResources")){
                char * pid = strdup(strtok(NULL, "\n"));
                printResources(pid, readylist);
                if(pid){free(pid);}

            } else {
                //printf("\tInvalid command; please try again.\n");
            }
            //printf("\t*Process %s is running\n", getProcessName(active_process));
            //printf("shell >> ");

				
		}


        // Freeing Resources
		free(input);
		free(iinput);
        freeReadylist(readylist);
		freeWaitlist(resourcelist);
        freeResourcelist(resourcelist);
        fclose(infile);
        fclose(outfile);
	}

}
