#ifndef __LIBTHRD_H__
#define __LIBTHRD_H__

typedef struct parameters
{
	void *(*fonction)(void *);
	void *argument;
} Parameters;

int getFunction(Parameters funcParameters);

int lanceThread(void *(*func)(void *), void *arg, int val);

#endif
