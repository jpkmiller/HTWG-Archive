#ifndef HELPFUNCTION_H_
#define HELPFUNCTION_H_

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define NANOSECONDS_PER_SECOND 1000000000

void Difftime(const struct timespec *before, const struct timespec *after, struct timespec *result)
{
    if ((after->tv_sec < before->tv_sec) ||
        ((after->tv_sec == before->tv_sec) && (after->tv_nsec <= before->tv_nsec)))
    {
        result->tv_sec = result->tv_nsec = 0;
        return;
    }

    result->tv_sec = after->tv_sec - before->tv_sec;
    result->tv_nsec = after->tv_nsec - before->tv_nsec;

    if (result->tv_nsec < 0)
    {
        result->tv_sec--;
        result->tv_nsec = NANOSECONDS_PER_SECOND + result->tv_nsec;
    }
}
#endif