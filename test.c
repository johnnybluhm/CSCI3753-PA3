#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "util.h"

#define BUFFER_SIZE 1000
#define NUMBER_OF_STRINGS 50
#define STRING_LENGTH 1025
void *requesterFunction( void *file);
void *resolverFunction(void *file);

char shared_array[NUMBER_OF_STRINGS][STRING_LENGTH+1];

int in = 0;
int out= 0;
pthread_mutex_t lock; 
pthread_mutex_t results_lock; 

FILE *serviced;
FILE *results;

int main(int argc, char *argv[]){

   // how to handle how many threafs to make: pthread_t reqtid[2];

    //clear serviced file for each run
        serviced = fopen("serviced.txt", "w");
        fputs("", serviced);
        fclose(serviced);

	int num_requesters = atoi(argv[1]);
	int num_resolvers = atoi(argv[2]);
	char *requester_log= argv[3];
	char *resolver_log= argv[4];

	char *inputFiles[argc - 5];
	int i;
	for (i = 5; i < argc; i++){
    	inputFiles[i-5] = argv[i];
    	int num = i-5;
	}


	char buffer[BUFFER_SIZE];
    int totalRead = 0;

	
    //initialize lock

        if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

    //initialize results lock

    if (pthread_mutex_init(&results_lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

    pthread_t reqtid[num_requesters];
    pthread_t restid[num_resolvers];

    //create requester threads
        printf("argc is %d\n",argc );
        int num_files = argc - 5;

        printf(" num of files is %d\n",num_files );

        

       for(int i = 0; i < num_requesters; i++) {
        pthread_create(&reqtid[i], NULL, requesterFunction, (void *)inputFiles[i]);
    }

    for(int i = 0; i < num_requesters; i++) {
        pthread_join(reqtid[i], NULL);
    }

           for(int i = 0; i < num_resolvers; i++) {
        pthread_create(&restid[i], NULL, resolverFunction, NULL);
    }

        for(int i = 0; i < num_resolvers; i++) {
        pthread_join(restid[i], NULL);
    }

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


  /* pthread_t thread1, thread2;
    int iret1,iret2;
    printf("before creation\n");
    iret1 = pthread_create(&thread1, NULL, requesterFunction, (void*)inputFiles[0]);
    pthread_join( thread1, NULL);
    iret2 = pthread_create(&thread2, NULL, requesterFunction, (void*)inputFiles[1]);
    pthread_join( thread2, NULL); 
    pthread_mutex_destroy(&lock); 
*/
    printf("VALUES AFTER THREADS\n");

    for(i =0;i< sizeof(shared_array)/sizeof(shared_array[i]);i++){
        printf("value at location %d is %s\n", i, shared_array[i]);
        
    }
    

}//main

void *requesterFunction(void *file){

    FILE *fPtr;
    printf("thread is in requester function\n");
    char *file_name = file;


    //parses file and adds lines to shared_array one by one

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

       
        pthread_mutex_lock(&lock); 
        /*CRITICAL SECTION
----------------------------------------------------------------------------------------------------
        */
        printf("in requester critical section\n");
        strcpy(shared_array[in], buffer);
        in= (in +1)%NUMBER_OF_STRINGS;
        printf("%s\n",buffer );
        printf("%d\n",in );
        serviced = fopen("serviced.txt", "a");
        fputs(buffer, serviced);
        fputs("\n", serviced);
        fclose(serviced);

        /*
        END CRITICAL SECTION
-----------------------------------------------------------------------------------------
        */
       pthread_mutex_unlock(&lock);     


    } 

    /* Done with this file, close file to release resource */
    fclose(fPtr);

}//requesterFunction

void *resolverFunction(void *file){

        char* ip_add;


        pthread_mutex_lock(&lock); 
        /*CRITICAL SECTION
----------------------------------------------------------------------------------------------------
        */
        printf("in resolver critical section\n");

        ip_add = shared_array[out];
        out = (out+1)%BUFFER_SIZE;

        /*
        END CRITICAL SECTION
-----------------------------------------------------------------------------------------
        */
       pthread_mutex_unlock(&lock); 

       char *ip_num;

       dnslookup(ip_add, ip_num,1000);


        pthread_mutex_lock(&results_lock); 

                /*CRITICAL SECTION
----------------------------------------------------------------------------------------------------
        */

       results= fopen("results.txt", "a");
       fputs(ip_add,results);
       fputs(",",results);
       fputs(ip_num,results);
       fputs("\n",results);

/*
        END CRITICAL SECTION
-----------------------------------------------------------------------------------------
        */
              pthread_mutex_unlock(&results_lock); 



    }

int dnslookup(const char* hostname, char* firstIPstr, int maxSize){

    /* Local vars */
    struct addrinfo* headresult = NULL;
    struct addrinfo* result = NULL;
    struct sockaddr_in* ipv4sock = NULL;
    struct in_addr* ipv4addr = NULL;
    char ipv4str[INET_ADDRSTRLEN];
    char ipstr[INET6_ADDRSTRLEN];
    int addrError = 0;

    /* DEBUG: Print Hostname*/
#ifdef UTIL_DEBUG
    fprintf(stderr, "%s\n", hostname);
#endif
   
    /* Lookup Hostname */
    addrError = getaddrinfo(hostname, NULL, NULL, &headresult);
    if(addrError){
    fprintf(stderr, "Error looking up Address: %s\n",
        gai_strerror(addrError));
    return UTIL_FAILURE;
    }
    /* Loop Through result Linked List */
    for(result=headresult; result != NULL; result = result->ai_next){
    /* Extract IP Address and Convert to String */
    if(result->ai_addr->sa_family == AF_INET){
        /* IPv4 Address Handling */
        ipv4sock = (struct sockaddr_in*)(result->ai_addr);
        ipv4addr = &(ipv4sock->sin_addr);
        if(!inet_ntop(result->ai_family, ipv4addr,
              ipv4str, sizeof(ipv4str))){
        perror("Error Converting IP to String");
        return UTIL_FAILURE;
        }
#ifdef UTIL_DEBUG
        fprintf(stdout, "%s\n", ipv4str);
#endif
        strncpy(ipstr, ipv4str, sizeof(ipstr));
        ipstr[sizeof(ipstr)-1] = '\0';
    }
    else if(result->ai_addr->sa_family == AF_INET6){
        /* IPv6 Handling */
#ifdef UTIL_DEBUG
        fprintf(stdout, "IPv6 Address: Not Handled\n");
#endif
        strncpy(ipstr, "UNHANDELED", sizeof(ipstr));
        ipstr[sizeof(ipstr)-1] = '\0';
    }
    else{
        /* Unhandlded Protocol Handling */
#ifdef UTIL_DEBUG
        fprintf(stdout, "Unknown Protocol: Not Handled\n");
#endif
        strncpy(ipstr, "UNHANDELED", sizeof(ipstr));
        ipstr[sizeof(ipstr)-1] = '\0';
    }
    /* Save First IP Address */
    if(result==headresult){
        strncpy(firstIPstr, ipstr, maxSize);
        firstIPstr[maxSize-1] = '\0';
    }
    }

    /* Cleanup */
    freeaddrinfo(headresult);

    return UTIL_SUCCESS;
}//dbs lookup