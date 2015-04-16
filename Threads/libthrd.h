#ifndef __LIBTHRD_H__
#define __LIBTHRD_H__

typedef struct parameters {
    void (*fonction)(void *);
    void *argument;
} Parameters;

void* lanceFunction(void *arg);
int lanceThread(void (*threadTCP)(void *), void *arg, int val);
int getLivingThreads();

#endif
