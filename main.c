#include <stdio.h>
#include <string.h>

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
		size_t size; ssize_t input_size; char * input = NULL;

        printf("shell >> ");
        while( (input_size = getline(&input, &size, stdin) ) != EOF ){
            char * command = strtok(input, " \n");

            if(!strcmp(command, "init")) {
                printf("Received 'init' command.\n");

            } else if (!strcmp(command, "cr")) {
                printf("Received 'cr' command.\n");

            } else if (!strcmp(command, "de")) {
                printf("Received 'de' command.\n");

            } else if (!strcmp(command, "req")) {
                printf("Received 'req' command.\n");

            } else if (!strcmp(command, "rel")) {
                printf("Received 'rel' command.\n");

            } else if (!strcmp(command, "to")) {
                printf("Received 'to' command.\n");

            } else if (!strcmp(command, "exit")) {
                printf("Exiting session...\n");
                break;

            } else {
                printf("Invalid command; please try again.\n");
            }
            printf("shell >> ");
		}
	}

}
