#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "libthrd.h"


/* Cette fonction prend *funcParameters en paramètre et lance le thread */
int getFunction(Parameters funcParameters) {
    pthread_t tid;
    pthread_attr_t tattr;
    
    int status = pthread_attr_init(&tattr);
    status += pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
    status += pthread_create(&tid, &tattr, funcParameters.fonction, funcParameters.argument);
    
    if (ret < 0) { perror("getFunction.pthread_create"); return ret; }
    #ifdef DEBUG
        fprintf(stderr, "Started thread tid: %d\n", tid);
    #endif
    
    return 0;
}


int lanceThread(void *(*func)(void *), void *arg, int size) {    
    Parameters *funcParameters;
    
    funcParameters = (Parameters *) malloc(sizeof(Parameters));
    funcParameters->fonction = func;
    funcParameters->argument = arg;
    
    #ifdef DEBUG
        fprintf(stderr, "Attempting to start new thread\n");
    #endif
    if (getFunction(*funcParameters) < 0) { perror("lanceThread.getFunction"); exit(EXIT_FAILURE); }
    
    return 0;
}
