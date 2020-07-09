#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "util.h"

#define BUFFER_SIZE 1000
#define NUMBER_OF_STRINGS 10
#define STRING_LENGTH 1025
void *testThread(void *arg);
char shared_array[NUMBER_OF_STRINGS][STRING_LENGTH+1];



struct Thread_object{

	int* in;
	int* out;
	pthread_mutex_t buffer_lock; 
	pthread_mutex_t results_file_lock; 
	pthread_mutex_t serviced_file_lock;
	sem_t producer;
	sem_t consumer;
	FILE *serviced;
	FILE *results;
	char inputFiles[10][1000];


};

int main(int argc, char *argv[]){

	pthread_mutex_t buffer_lock; 
	pthread_mutex_t results_file_lock; 
	pthread_mutex_t serviced_file_lock;
	int in =0;
	int out = 0;
	sem_t producer;
	sem_t consumer;
	FILE *serviced;
	FILE *results;

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

	}//for 
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
	
	//instaniate thread object
	struct Thread_object thread_object;

	thread_object.buffer_lock = buffer_lock;	
	thread_object.results_file_lock = results_file_lock;
	thread_object.serviced_file_lock= serviced_file_lock;
	thread_object.producer= producer;
	thread_object.consumer = consumer;
	thread_object.serviced = serviced;
	thread_object.results = results;
	thread_object.in = &in;
	thread_object.out = &out;

	//create array of strings in thread object
		for (i = 0; i < sizeof(inputFiles)/sizeof(inputFiles[i]); i++){
			strcpy(thread_object.inputFiles[i], inputFiles[i]);

	}//for 

	
		
/*	//spawn requester threads
	for(int i = 0; i < num_requesters; i++) {
		pthread_create(&reqtid[i], NULL, requesterThread, (void *)thread_object);

	}

	//spawn resolver threads
	for(int i = 0; i < num_resolvers; i++) {
		pthread_create(&restid[i], NULL, resolverThread, (void *)thread_object);
	}   

	//wait for resolver threads
	for(int i = 0; i < num_resolvers; i++) {
		pthread_join(restid[i], NULL);
	}

	//wait for requester threads
	for(int i = 0; i < num_requesters; i++) {
		pthread_join(reqtid[i], NULL);
	}*/

	pthread_create(&reqtid[0], NULL, testThread, (void *)&thread_object);

}//main

void *testThread(void* arg){

	struct Thread_object *thread_object;

	thread_object = arg;

	pthread_mutex_lock(&thread_object->results_file_lock);
	printf("WORKIE");
	pthread_mutex_unlock(&thread_object->results_file_lock);

	printf("AM IN IN THIS SHIT?\n");


	printf("%d\n",*thread_object->in);
	printf("%s\n",thread_object->inputFiles);
	


}//test

