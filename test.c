#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BUFFER_SIZE 1000
#define NUMBER_OF_STRINGS 50
#define STRING_LENGTH 1025
void *requesterFunction( void *ptr );

char shared_array[NUMBER_OF_STRINGS][STRING_LENGTH+1];

int main(int argc, char *argv[]){

	int num_requesters = atoi(argv[1]);
	int num_resolvers = atoi(argv[2]);
	char *requester_log= argv[3];
	char *resolver_log= argv[4];

	char *inputFiles[argc - 4];
	int i;
	for (i = 5; i < argc; i++){
    	inputFiles[i-5] = argv[i];
    	int num = i-5;
	}


	char buffer[BUFFER_SIZE];
    int totalRead = 0;

	

	// FILE *fPtr;

	// fPtr = fopen(inputFiles[0], "r");
	
	//     /* fopen() return NULL if last operation was unsuccessful */
 //    if(fPtr == NULL)
 //    {i
 //        /* Unable to open file hence exit */
 //        printf("Unable to open file.\n");
 //        printf("Please check whether file exists and you have read privilege.\n");
 //        exit(EXIT_FAILURE);
 //    }


 //    /* File open success message */
 //    printf("File opened successfully. Reading file contents line by line. \n\n");


 //    /* Repeat this until read line is not NULL */
 //    while(fgets(buffer, BUFFER_SIZE, fPtr) != NULL) 
 //    {
 //        /* Total character read count */
 //        totalRead = strlen(buffer);


 //        /*
 //         * Trim new line character from last if exists.
 //         */
 //        buffer[totalRead - 1] = buffer[totalRead - 1] == '\n' 
 //                                    ? '\0' 
 //                                    : buffer[totalRead - 1];


 //        /* Print line read on cosole*/
 //        printf("%s\n", buffer);

 //    } 


 //    /* Done with this file, close file to release resource */
 //    fclose(fPtr);


   pthread_t thread1, thread2;
    int iret1,iret2;
    printf("before creation\n");
    iret1 = pthread_create(&thread1, NULL, requesterFunction, (void*)inputFiles[0]);
    pthread_join( thread1, NULL);
    iret2 = pthread_create(&thread2, NULL, requesterFunction, (void*)inputFiles[1]);
    pthread_join( thread2, NULL); 

    printf("VALUES AFTER THREADS\n");

    for(i =0;i< sizeof(shared_array)/sizeof(shared_array[i]);i++){
        printf("%s\n",shared_array[i] );
        
    }
    

}//main

/*void requesters(int num_requesters){
    pthread_t thread1, thread2;
    int iret1,iret2;
    iret1 = pthread_create( &thread1, NULL, requesterFunction, (void*) inputFiles[0]);
    iret2 = pthread_create( &thread2, NULL, requesterFunction, (void*) inputFiles[1]);


}//requesters
*/

void *requesterFunction(void *file){

    FILE *fPtr;
    printf("thread is in function\n");
    char *file_name = file;


    char buffer[BUFFER_SIZE];
    int totalRead = 0;
    fPtr = fopen(file_name, "r");
    
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
    int i =0;
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
        printf("value of i is %d\n",i );
        strcpy(shared_array[i], buffer);
        printf("length of shared array is %d\n",sizeof(shared_array)/sizeof(shared_array[i]));
        i++;


    } 


    /* Done with this file, close file to release resource */
    fclose(fPtr);

}//requesterFunction