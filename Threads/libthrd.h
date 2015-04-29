#ifndef __LIBTHRD_H__
#define __LIBTHRD_H__

#define NB_TEAMS 11

typedef struct parameters {
    void (*fonction)(void *);
    void *argument;
} Parameters;

/* Threads */
void* lanceFunction(void *arg);
int lanceThread(void (*threadTCP)(void *), void *arg, int val);
int getLivingThreads();

/* Mutexes */
void mutex_P(int index);
void mutex_V(int index);
void initMutexes();

#endif
