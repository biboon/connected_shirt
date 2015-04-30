/* Ce fichier contient des fonctions de thread */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "libthrd.h"

int livingThreads = 0;

static pthread_mutex_t dataMutex[NB_TEAMS];
static pthread_mutex_t filesMutex[NB_TEAMS];
static pthread_mutex_t graphesMutex;
static pthread_mutex_t valeursMutex;
static pthread_mutex_t cptThreads;


void initMutexes() {
    int i;
    for (i = 0; i < NB_TEAMS; i++) {
        pthread_mutex_init((dataMutex + i), NULL);
        pthread_mutex_init((filesMutex + i), NULL);
    }
    pthread_mutex_init(&graphesMutex, NULL);
    pthread_mutex_init(&valeursMutex, NULL);
    pthread_mutex_init(&cptThreads, NULL);
}

void P(int index) {
    if (index >= DATA_MUTEX && index < DATA_MUTEX + NB_TEAMS)
        pthread_mutex_lock(dataMutex + index);
    else if (index >= FILE_MUTEX && index < FILE_MUTEX + NB_TEAMS)
        pthread_mutex_lock(filesMutex + index);
    else if (index == GRAPHES_MUTEX)
        pthread_mutex_lock(&graphesMutex);
    else if (index == VALEURS_MUTEX)
        pthread_mutex_lock(&valeursMutex);
}

void V(int index) {
    if (index >= DATA_MUTEX && index < DATA_MUTEX + NB_TEAMS)
        pthread_mutex_unlock(dataMutex + index);
    else if (index >= FILE_MUTEX && index < FILE_MUTEX + NB_TEAMS)
        pthread_mutex_unlock(filesMutex + index);
    else if (index == GRAPHES_MUTEX)
        pthread_mutex_unlock(&graphesMutex);
    else if (index == VALEURS_MUTEX)
        pthread_mutex_unlock(&valeursMutex);
}

void* lanceFunction(void *arg) {
    pthread_mutex_lock(&cptThreads);
    livingThreads++;
    pthread_mutex_unlock(&cptThreads);
    
    /* Copie de l'argument */
    Parameters *funcParameters = arg;
    /* Appel de la fonction avec l'argument dans la structure */
    funcParameters->fonction(funcParameters->argument);
    /* Liberation de la memoire */
    free(funcParameters->argument);
    free(funcParameters);
    
    pthread_mutex_lock(&cptThreads);
    livingThreads--;
    pthread_mutex_unlock(&cptThreads);
    #ifdef DEBUG
        fprintf(stderr, "Thread terminated, total remaining: %d\n", livingThreads);
    #endif
    
    pthread_exit(NULL);
}


/* Returns 0 on success, negative integer if failed */
int lanceThread(void (*func)(void *), void *arg, int size) {
    pthread_mutex_lock(&cptThreads);
    if (livingThreads == MAX_THREADS) return -10;
    pthread_mutex_unlock(&cptThreads);
    
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
    int res;
    pthread_mutex_lock(&cptThreads);
    res = livingThreads;
    pthread_mutex_unlock(&cptThreads);
    return res;
}
