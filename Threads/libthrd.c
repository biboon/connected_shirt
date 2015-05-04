/* Ce fichier contient des fonctions de thread */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "libthrd.h"

static int livingThreads = 0;

static pthread_mutex_t filesMutex[NB_TEAMS];
static pthread_mutex_t graphesMutex;
static pthread_mutex_t valeursMutex;


void initMutexes() {
	int i;
	for (i = 0; i < NB_TEAMS; i++) {
		pthread_mutex_init((filesMutex + i), NULL);
	}
	pthread_mutex_init(&graphesMutex, NULL);
	pthread_mutex_init(&valeursMutex, NULL);
}

void P(int index) {
	if (index >= FILE_MUTEX && index < FILE_MUTEX + NB_TEAMS)
		pthread_mutex_lock(filesMutex + index);
	else if (index == GRAPHES_MUTEX)
		pthread_mutex_lock(&graphesMutex);
	else if (index == VALEURS_MUTEX)
		pthread_mutex_lock(&valeursMutex);
}

void V(int index) {
	if (index >= FILE_MUTEX && index < FILE_MUTEX + NB_TEAMS)
		pthread_mutex_unlock(filesMutex + index);
	else if (index == GRAPHES_MUTEX)
		pthread_mutex_unlock(&graphesMutex);
	else if (index == VALEURS_MUTEX)
		pthread_mutex_unlock(&valeursMutex);
}

void* lanceFunction(void *arg) {
	/* Copie de l'argument */
	Parameters *funcParameters = arg;
	/* Appel de la fonction avec l'argument dans la structure */
	funcParameters->fonction(funcParameters->argument);
	/* Liberation de la memoire */
	free(funcParameters->argument);
	free(funcParameters);
	
	livingThreads--;
	#ifdef DEBUG
		fprintf(stderr, "Thread terminated, total remaining: %d\n", livingThreads);
	#endif
	
	pthread_exit(NULL);
}


/* Returns 0 on success, negative integer if failed */
int lanceThread(void (*func)(void *), void *arg, int size) {
	if (livingThreads == MAX_THREADS) {
		#ifdef DEBUG
			perror("lanceThread.thread_limit");
		#endif
		return -5;
	}
	
	Parameters* funcParameters = (Parameters*) malloc(sizeof(Parameters));
	if (funcParameters == NULL) {
		#ifdef DEBUG
			perror("lanceThread.funcParameters.malloc");
		#endif
		return -1;
	}
	funcParameters->fonction = func;
	funcParameters->argument = malloc(size);
	if (funcParameters->argument == NULL) {
		#ifdef DEBUG
			perror("lanceThread.funcParameters.argument.malloc");
		#endif
		return -2;
	}
	memcpy(funcParameters->argument, arg, (size_t)size);
	
	pthread_attr_t attr;
	pthread_t tid;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&tid, &attr, lanceFunction, funcParameters) != 0) {
		#ifdef DEBUG
			perror("lanceThread.pthread_create");
		#endif
		return -3;
	}
	
	livingThreads++;
	#ifdef DEBUG
		fprintf(stderr, "Thread started, total running: %d\n", livingThreads);
	#endif
	return 0;
}


int getLivingThreads() {
	return livingThreads;
}
