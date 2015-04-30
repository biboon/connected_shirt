#ifndef __LIBTHRD_H__
#define __LIBTHRD_H__

#define NB_TEAMS 11
#define MAX_THREADS 100

/* Mutexes indexes */
#define FILE_MUTEX 20
#define GRAPHES_MUTEX 40
#define VALEURS_MUTEX 41

typedef struct parameters {
    void (*fonction)(void *);
    void *argument;
} Parameters;

/* Threads */
void* lanceFunction(void *arg);
int lanceThread(void (*threadTCP)(void *), void *arg, int val);
int getLivingThreads();

/* Mutexes */
void P(int index);
void V(int index);
void initMutexes();

#endif
