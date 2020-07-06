#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BUFFER_SIZE 1000
int main(int argc, char *argv[]){

	int num_requests = atoi(argv[1]);
	int num_resolvers = atoi(argv[2]);
	char *requester_log= argv[3];
	char *resolver_log= argv[4];

	char *inputFiles[argc - 4];
	int i;
	for (i = 5; i < argc; i++){
		printf("looped value = %s\n",argv[i] );
    	inputFiles[i-5] = argv[i];
    	int num = i-5;
    	printf("value of i-5 = %d\n",num );
	}


	char buffer[BUFFER_SIZE];
    int totalRead = 0;

	char *file = argv[5];

	FILE *fPtr;

	fPtr = fopen(inputFiles[0], "r");
	
	    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* Unable to open file hence exit */
        printf("Unable to open file.\n");
        printf("Please check whether file exists and you have read privilege.\n");
        exit(EXIT_FAILURE);
    }


    /* File open success message */
    printf("File opened successfully. Reading file contents line by line. \n\n");


    /* Repeat this until read line is not NULL */
    while(fgets(buffer, BUFFER_SIZE, fPtr) != NULL) 
    {
        /* Total character read count */
        totalRead = strlen(buffer);


        /*
         * Trim new line character from last if exists.
         */
        buffer[totalRead - 1] = buffer[totalRead - 1] == '\n' 
                                    ? '\0' 
                                    : buffer[totalRead - 1];


        /* Print line read on cosole*/
        printf("%s\n", buffer);

    } 


    /* Done with this file, close file to release resource */
    fclose(fPtr);

	
	printf("%d \n",num_requests);
	printf("%d \n",num_resolvers);
	printf("%s \n",requester_log);
	printf("%s \n",resolver_log);
}