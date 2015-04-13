/* Ce fichier contient des fonctions de thread */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "libthrd.h"

int global = 0;

void* lanceFunction(void *arg) {
    /* Copie de l'argument */
    Parameters *funcParameters = arg;
    /* Appel de la fonction avec l'argument dans la structure */
    funcParameters->fonction(funcParameters->argument);
    /* Liberation de la memoire */
    free(funcParameters->argument);
    free(funcParameters);
    
    pthread_exit(NULL);
}


int lanceThread(void (*func)(void *), void *arg, int size)
{    
    Parameters *funcParameters;
    pthread_t tid;
    pthread_attr_t tattr;
    int ret = 0;
    void *argument;
    
    argument = malloc(size);
    memcpy(argument, arg, (size_t)size);
    
    funcParameters = (Parameters*)malloc(sizeof(Parameters));
    
    funcParameters->fonction = func;
    funcParameters->argument = argument;
    
    ret = pthread_attr_init(&tattr);
    ret += pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
    ret += pthread_create(&tid, &tattr, lanceFunction, funcParameters);
    
    return ret;
}

void printlol(int* chiffre)
{
    global += 1000;
    printf("Je suis un calamar %d\n", *chiffre);
    
    pthread_exit(NULL);
}


int main(void)
{
    int size = 4;
    int argument = 12;
//    printf("pid:%d\n",getpid());
    lanceThread((void*) &printlol, &argument , size);
    

    pthread_exit(0);
    
    printf("global : %d\n",global);
    
    return 0;
}
