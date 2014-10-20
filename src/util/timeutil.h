#ifndef TIMEUTIL_H_
#define TIMEUTIL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <math.h>
#include "macroutil.h"

timer_t createTimer(void (*handler) (union sigval), void *arg);

void freeTimer(const timer_t timerid);

void setTimer(const timer_t timerid, const long double value, const long double ivalue);

struct itimerspec getTimer(const timer_t timerid);

short isTimerDisarmed(const timer_t timerid);

long double getElapsed(const struct timespec start, const struct timespec end);

struct timespec getTimespec(const long double value);

struct timeval getTimeval(const long double value);

struct timespec getTimestamp(void);

#endif /* TIMEUTIL_H_ */