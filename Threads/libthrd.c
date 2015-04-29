/* Ce fichier contient des fonctions de thread */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "libthrd.h"

static int livingThreads = 0;
static pthread_mutex_t dataMutex[NB_TEAMS];
static pthread_mutex_t webpageMutex;


void initMutexes() {
    int i;
    for (i = 0; i < NB_TEAMS; i++) {
        pthread_mutex_init((dataMutex + i), NULL);
        pthread_mutex_unlock(dataMutex + i);
    }
    pthread_mutex_init(&webpageMutex, NULL);
    pthread_mutex_unlock(&webpageMutex);
}

void mutex_P(int index) {
    if (index < NB_TEAMS)
        pthread_mutex_lock(dataMutex + index);
    else if (index == 20)
        pthread_mutex_lock(&webpageMutex);
}

void mutex_V(int index) {
    if (index < NB_TEAMS)
        pthread_mutex_unlock(dataMutex + index);
    else if (index == 20)
        pthread_mutex_unlock(&webpageMutex);
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
    livingThreads++;
    Parameters *funcParameters;
    pthread_t tid;
    pthread_attr_t tattr;
    int ret = 0;
    void *argument;
    
    argument = malloc(size);
    memcpy(argument, arg, (size_t)size);
    
    funcParameters = (Parameters*) malloc(sizeof(Parameters));
    
    funcParameters->fonction = func;
    funcParameters->argument = argument;
    
    ret = pthread_attr_init(&tattr);
    ret += pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
    ret += pthread_create(&tid, &tattr, lanceFunction, funcParameters);
    #ifdef DEBUG
        fprintf(stderr, "Thread started, total running: %d\n", livingThreads);
    #endif
    return ret;
}


int getLivingThreads() {
    return livingThreads;
}
