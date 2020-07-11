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

struct Thread_object{

	int* in;
	int* out;
	int* num_files;
	int* file_tracker;
	pthread_mutex_t* buffer_lock; 
	pthread_mutex_t* results_file_lock; 
	pthread_mutex_t* serviced_file_lock;
	pthread_mutex_t* file_array_lock;
	sem_t* producer;
	sem_t* consumer;
	FILE *serviced;
	FILE *results;
	FILE *file_array[NUMBER_OF_STRINGS];
	char shared_array[NUMBER_OF_STRINGS][STRING_LENGTH+1];
};

int main(int argc, char *argv[]){

	pthread_mutex_t* buffer_lock = malloc(sizeof(pthread_mutex_t)); 
	pthread_mutex_t* results_file_lock= malloc(sizeof(pthread_mutex_t));
	pthread_mutex_t* serviced_file_lock= malloc(sizeof(pthread_mutex_t));
	pthread_mutex_t* file_array_lock= malloc(sizeof(pthread_mutex_t));
	int *in = malloc(4);
	int *out = malloc(4);
	int *num_files = malloc(4);
	int *file_tracker = malloc(4);
	FILE *serviced;
	FILE *results;
	*in=0;
	*out=0;
	//char shared_array[NUMBER_OF_STRINGS][STRING_LENGTH+1];

//	char * shared_array = (char *) malloc(NUMBER_STRINGS * STRING_LENGTH * sizeof(char))

	*file_tracker = 0;

	//ascii to integer
	int num_requesters = atoi(argv[1]);
	int num_resolvers = atoi(argv[2]);

	char *requester_log= argv[3];
	char *resolver_log= argv[4];

	//clear serviced file for each run
		serviced = fopen(requester_log, "w");
		fputs("", serviced);
		fclose(serviced);

	//clear results file for each run
		results= fopen(resolver_log, "w");
		fputs("", results);
		fclose(results);

	//open serviced file to be passed to threads
		serviced = fopen(requester_log, "a");

		if(serviced == NULL){
			printf("BROKEN\n");
		}

	//open results file to be passed to threads
		results = fopen(resolver_log, "a");		

	//get array of file names
	char *inputFiles[argc - 5];
	int i;
	for (i = 5; i < argc; i++)
	{
		inputFiles[i-5] = argv[i];
		int num = i-5;
	}

	//initialize buffer_lock
	if (pthread_mutex_init(buffer_lock, NULL) != 0) { 
		printf("\n mutex init has failed\n"); 
		return 1; 
	} 

	//initialize results file lock
	if (pthread_mutex_init(results_file_lock, NULL) != 0) { 
		printf("\n mutex init has failed\n"); 
		return 1; 
	} 
	//initialize serviced file lock
	if (pthread_mutex_init(serviced_file_lock, NULL) != 0) { 
		printf("\n mutex init has failed\n"); 
		return 1; 
	} 
		//initialize file array lock
	if (pthread_mutex_init(file_array_lock, NULL) != 0) { 
		printf("\n mutex init has failed\n"); 
		return 1; 
	} 
	int test;
	
	//initialize threads
	pthread_t reqtid[num_requesters];
	pthread_t restid[num_resolvers];

	//instaniate thread object
	struct Thread_object thread_object;

	thread_object.buffer_lock = buffer_lock;	
	thread_object.results_file_lock = results_file_lock;
	thread_object.serviced_file_lock= serviced_file_lock;
	thread_object.producer = malloc(sizeof(sem_t));
	thread_object.consumer = malloc(sizeof(sem_t));


	thread_object.serviced = serviced;
	thread_object.results = results;
	thread_object.in = in;
	thread_object.out = out;
	thread_object.file_tracker =file_tracker;
	thread_object.file_array_lock= file_array_lock;

	//initialize producer semaphore
	sem_init(thread_object.producer, 0, NUMBER_OF_STRINGS);
	
	//initialize consumer
	sem_init(thread_object.consumer,0,0);

	int inputFiles_len;
	FILE* file_array[inputFiles_len];	
	inputFiles_len = sizeof(inputFiles)/sizeof(inputFiles[0]); 

	*num_files = inputFiles_len;

	thread_object.num_files= num_files;

	//fill struct file_array with file pointers 
	for(int i=0;i<inputFiles_len;i++){
		FILE* input_file;
		printf("%s\n", inputFiles[i]);
		input_file= fopen(inputFiles[i], "r");
		thread_object.file_array[i] = input_file;

	}

	/*fprintf(serviced, "TESTINg"); 
	fprintf(thread_object.serviced, "HELLO"); */


	//spawn requester threads
	for(int i = 0; i < num_requesters; i++) {
		pthread_create(&reqtid[i], NULL, requesterThread, (void *)&thread_object);

	}

	//spawn resolver threads
	for(int i = 0; i < num_resolvers; i++) {
		pthread_create(&restid[i], NULL, resolverThread, (void *)&thread_object);
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

void *requesterThread(void *arg){

	printf("Entered requester thread\n");

	struct Thread_object *thread_object;

	thread_object = arg;
	FILE *fPtr;

		//if file_tracker == num_files, we have processed all files.

	char buffer[STRING_LENGTH+1];
	int files_processed;
	int totalRead = 0;
	files_processed=0;

	while(1){

	//-----------------CRITICAL SECTION-----------------
	
	pthread_mutex_lock(thread_object->file_array_lock);
	//if files availbale
	
	if(*thread_object->file_tracker == *thread_object->num_files){
		printf("Thread %u serviced %d file(s)\n", pthread_self(), files_processed); 
		
		// release lock and exit. There are no more files to process. 
		pthread_mutex_unlock(thread_object->file_array_lock);


		pthread_mutex_lock(thread_object->serviced_file_lock);
			/*CRITICAL SECTION
----------------------------------------tid------------------------------------------------------------
		*/
		
		fprintf(thread_object->serviced, "Thread %u serviced %d files\n", pthread_self(), files_processed); 
		// fputs("Thread ",thread_object->results);
		// printf("SEG\n");
	 //   fputs(pthread_self(),thread_object->results);
	 //   printf("SEG\n");
	 //   fputs("serviced ",thread_object->serviced);
	 //   fputs(files_processed,thread_object->serviced);
	 //   fputs("files \n",thread_object->serviced);
		// fclose(thread_object->serviced);
		/*
		END CRITICAL SECTION
-----------------------------------------------------------------------------------------
		*/
		pthread_mutex_unlock(thread_object->serviced_file_lock);
		printf("Thread exited normally\n");
		pthread_exit(NULL);

	}//if
	else{
		//do something
		//increment file_tracker by one to show file was removed
		//still locked so do not need to lock
		int file_num;

		file_num = *thread_object->file_tracker;
		*thread_object->file_tracker = *thread_object->file_tracker + 1;
		fPtr = thread_object->file_array[file_num];
		pthread_mutex_unlock(thread_object->file_array_lock);


		//----------------END CRITICAL SECTION --------------------

		// reads text until newline is encountered and puts in website
		//loops until EOF is reached
		while(fgets(buffer, BUFFER_SIZE, fPtr) != NULL){

			/* Total character read count */
		totalRead = strlen(buffer);
		/*
		 * Trim new line character from last if exists.
		 */
		buffer[totalRead - 1] = buffer[totalRead - 1] == '\n' 
									? '\0' 
									: buffer[totalRead - 1];


		sem_wait(thread_object->producer);
		
		pthread_mutex_lock(thread_object->buffer_lock);
		/*CRITICAL SECTION
----------------------------------------------------------------------------------------------------
		*/
		//printf("Requester accessing shared array\n");
		strcpy(thread_object->shared_array[*thread_object->in], buffer);

		*thread_object->in= (*thread_object->in +1)%NUMBER_OF_STRINGS;
		//printf("%s\n",buffer);
		/*
		END CRITICAL SECTION
-----------------------------------------------------------------------------------------
		*/
		pthread_mutex_unlock(thread_object->buffer_lock);  
		sem_post(thread_object->consumer);

		}//while

		printf("Processed file\n");
		/* Done with this file, close file to release resource */
		fclose(fPtr);
		files_processed++;

	}//else

}//big loop

}//requesterThread


void *resolverThread(void *arg){	

	printf("Entered resolver thread\n");

	struct Thread_object *thread_object;

	thread_object = arg;	

	//if file_tracker == num_files, we have processed all files.

	while(1){

	pthread_mutex_lock(thread_object->file_array_lock);
	//-----------------CRITICAL SECTION-----------------	
	
	//loop until requester threads are done and buffer is empty

	
	if((*thread_object->file_tracker == *thread_object->num_files)&& strcmp(thread_object->shared_array[*thread_object->out],"")==0){
		
		// release lock and exit. There are no more websites to get
		pthread_mutex_unlock(thread_object->file_array_lock);
	
		pthread_exit(NULL);

	}//if
	else{

		pthread_mutex_unlock(thread_object->file_array_lock);
	
		char ip_add[50];
		sem_wait(thread_object->consumer);
		pthread_mutex_lock(thread_object->buffer_lock); 
		/*CRITICAL SECTION
----------------------------------------------------------------------------------------------------
		*/
		printf("Resolver accessing shared array\n");
		strcpy(ip_add, thread_object->shared_array[*thread_object->out]);		
		
		strcpy(thread_object->shared_array[*thread_object->out],"");

		*thread_object->out =(*thread_object->out+1)%NUMBER_OF_STRINGS;

		/*
		END CRITICAL SECTION
-----------------------------------------------------------------------------------------
		*/

	  	pthread_mutex_unlock(thread_object->buffer_lock); 
	  	
	  	sem_post(thread_object->producer);	  
	  	
	  	char *ip_num = malloc(50);
	   	int dns_ret = dnslookup(ip_add, ip_num,50);
	   	
	   	pthread_mutex_lock(thread_object->results_file_lock); 
	   
				/*CRITICAL SECTION
----------------------------------------------------------------------------------------------------
		*/
	 
	   if(dns_ret == -1){
	   fputs(ip_add,thread_object->results);
	   fputs(",",thread_object->results);
	   fputs("BOGUS HOSTNAME",thread_object->results);
	   fputs("\n",thread_object->results);
	   }
	   else{
	   fputs(ip_add,thread_object->results);
	   fputs(",",thread_object->results);
	   fputs(ip_num,thread_object->results);
	   fputs("\n",thread_object->results);
	}

/*
		END CRITICAL SECTION
-----------------------------------------------------------------------------------------
		*/
		pthread_mutex_unlock(thread_object->results_file_lock); 

}//else
	
}//while

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
}//dns lookup

