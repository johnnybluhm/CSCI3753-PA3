#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "util.h"

#define BUFFER_SIZE 1000
#define NUMBER_OF_STRINGS 10
#define STRING_LENGTH 1025
void *requesterThread( void *file);
void *resolverThread(void *file);

char shared_array[NUMBER_OF_STRINGS][STRING_LENGTH+1];
pthread_mutex_t buffer_lock; 
pthread_mutex_t results_file_lock; 
pthread_mutex_t serviced_file_lock;
int in =0;
int out = 0;
sem_t producer;
sem_t consumer;
FILE *serviced;
FILE *results;


int main(int argc, char *argv[]){



	//clear serviced file for each run
		serviced = fopen("serviced.txt", "w");
		fputs("", serviced);
		fclose(serviced);

	//clear results file for each run
		results= fopen("results.txt", "w");
		fputs("", results);
		fclose(results);

	//ascii to integer
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
	
	//initialize buffer_lock
	if (pthread_mutex_init(&buffer_lock, NULL) != 0) { 
		printf("\n mutex init has failed\n"); 
		return 1; 
	} 

	//initialize results file lock
	if (pthread_mutex_init(&results_file_lock, NULL) != 0) { 
		printf("\n mutex init has failed\n"); 
		return 1; 
	} 
	//initialize serviced file lock
	if (pthread_mutex_init(&serviced_file_lock, NULL) != 0) { 
		printf("\n mutex init has failed\n"); 
		return 1; 
	} 

	//initialize producer semaphore
	sem_init(&producer, 0, NUMBER_OF_STRINGS);

	//initialize consumer
	sem_init(&consumer,0,0);

	//initialize threads
	pthread_t reqtid[num_requesters];
	pthread_t restid[num_resolvers];

	//spawn requester threads
	for(int i = 0; i < num_requesters; i++) {
		pthread_create(&reqtid[i], NULL, requesterThread, (void *)inputFiles[i]);

	}

	//spawn resolver threads
	for(int i = 0; i < num_resolvers; i++) {
		pthread_create(&restid[i], NULL, resolverThread, (void *)inputFiles[i]);
	}   

	//wait for resolver threads
	for(int i = 0; i < num_resolvers; i++) {
		pthread_join(restid[i], NULL);
	}

	//wait for requester threads
	for(int i = 0; i < num_requesters; i++) {
		pthread_join(reqtid[i], NULL);
	}

}//main

void *requesterThread(void *file){

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


	   	sem_wait(&producer);
		pthread_mutex_lock(&buffer_lock); 
		/*CRITICAL SECTION
----------------------------------------------------------------------------------------------------
		*/
		printf("in requester critical section\n");
		strcpy(shared_array[in], buffer);
		in= (in +1)%NUMBER_OF_STRINGS;
		/*
		END CRITICAL SECTION
-----------------------------------------------------------------------------------------
		*/
	   pthread_mutex_unlock(&buffer_lock);  
	   sem_post(&consumer);
	}//while

	/* Done with this file, close file to release resource */
	fclose(fPtr);

	pthread_mutex_lock(&serviced_file_lock);
			/*CRITICAL SECTION
----------------------------------------tid------------------------------------------------------------
		*/
		serviced = fopen("serviced.txt", "a");
		fprintf(serviced, "Thread %u serviced 1 file\n", pthread_self()); 
		fclose(serviced);
		/*
		END CRITICAL SECTION
-----------------------------------------------------------------------------------------
		*/
	pthread_mutex_unlock(&serviced_file_lock);

}//requesterThread

void *resolverThread(void *file){


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
		//exit(EXIT_FAILURE);
	}


	/* File open success message */
	printf("File opened successfully. Reading file contents line by line. \n\n");


	/* Repeat this until read line is not NULL */
	
	while(fgets(buffer, BUFFER_SIZE, fPtr) != NULL) 
	{

		char* ip_add;
		printf("thread is in resolver function\n");
		sem_wait(&consumer);
		pthread_mutex_lock(&buffer_lock); 
		/*CRITICAL SECTION
----------------------------------------------------------------------------------------------------
		*/
		printf("in resolver critical section\n");

		ip_add = shared_array[out];
		printf("%s\n", ip_add );
		out = (out+1)%NUMBER_OF_STRINGS;

		/*
		END CRITICAL SECTION
-----------------------------------------------------------------------------------------
		*/
	   pthread_mutex_unlock(&buffer_lock); 
	   sem_post(&producer);

	   char *ip_num = malloc(50);

	   int dns_ret = dnslookup(ip_add, ip_num,50);

	   pthread_mutex_lock(&results_file_lock); 
	   
				/*CRITICAL SECTION
----------------------------------------------------------------------------------------------------
		*/
	   printf("Trying to write to results critical section\n");
	   results= fopen("results.txt", "a");
	   printf("%d\n",dns_ret );
	   if(dns_ret == -1){
	   fputs(ip_add,results);
	   fputs(",",results);
	   fputs("BOGUS HOSTNAME",results);
	   fputs("\n",results);
	   }
	   else{
	   fputs(ip_add,results);
	   fputs(",",results);
	   fputs(ip_num,results);
	   fputs("\n",results);
	}

/*
		END CRITICAL SECTION
-----------------------------------------------------------------------------------------
		*/
		pthread_mutex_unlock(&results_file_lock); 


}//while
/* Done with this file, close file to release resource */
	fclose(fPtr);

	pthread_exit(NULL);
}//resolverthread

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

// [212]