#ifndef HELPERGPIO_H_
#define HELPERGPIO_H_

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define NANOSECONDS_PER_SECOND 1000000000

int Open(const char *pathname, int flags);
int Write(int fd, const char *pathname, const void *buf, size_t count);
int Read(int fd, const char *pathname, void *buf, size_t count);
void SetVal(const char *pathname, const void *buf, size_t count);
int ReadVal(const char *pathname, size_t count);
int WaitNsec(long sleep_time_sec, long sleep_time_nsec);
void DiffTime(const struct timespec *start_time, const struct timespec *end_time, struct timespec *result);


int Open(const char *pathname, int flags)
{
    int fd = open(pathname, flags);
#ifdef DEBUG
    printf("opening %s ...\n", pathname);
#endif
    if (fd == -1)
    {
        fprintf(stderr, "failed to open %s ..!\n", pathname);
        exit(1);
    }
    return fd;
}

int Write(int fd, const char *pathname, const void *buf, size_t count)
{
    ssize_t writtenBytes = write(fd, buf, count);
#ifdef DEBUG
    printf("writing %s ...\n", pathname);
#endif
    if (writtenBytes == -1)
    {
        fprintf(stderr, "failed to write %s to %s ..!\n", (char *)buf, pathname);
        exit(1);
    }
    return (int)writtenBytes;
}

int Read(int fd, const char *pathname, void *buf, size_t count)
{
    ssize_t readBytes = read(fd, buf, count);
#ifdef DEBUG
    printf("reading %s ...\n", pathname);
#endif
    if (readBytes == -1)
    {
        fprintf(stderr, "failed to read from %d and write into %s ..!\n", fd, (char *)buf);
        exit(1);
    }
    return (int)readBytes;
}

void SetVal(const char *pathname, const void *buf, size_t count)
{
    int fd = Open(pathname, O_WRONLY);
    Write(fd, pathname, buf, count);
    close(fd);
}

int ReadVal(const char *pathname, size_t count)
{
    int fd = Open(pathname, O_RDONLY);
    void *buf = (void *)malloc(count);
    Read(fd, pathname, buf, count);
    int value = *(int *)buf;
    free(buf);
    close(fd);
    return value;
}

int WaitNsec(long sleep_time_sec, long sleep_time_nsec)
{
    struct timespec sleep_time;
    clock_gettime(CLOCK_MONOTONIC, &sleep_time);
    sleep_time.tv_sec += sleep_time_sec;
    sleep_time.tv_nsec += sleep_time_nsec;

    struct timespec remaining_time;

    if (sleep_time.tv_nsec > 999999999)
    {
        sleep_time.tv_sec += sleep_time.tv_nsec / 1000000000;
        sleep_time.tv_nsec = sleep_time.tv_nsec % 1000000000;
    }

    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &sleep_time, &remaining_time) == EINTR)
    {
        printf("interrupted in clock_nanosleep loop ..!\n");
        sleep_time = remaining_time;
    }
    return 0;
}

void DiffTime(const struct timespec *start_time, const struct timespec *end_time, struct timespec *result)
{
    if ((end_time->tv_sec < start_time->tv_sec) ||
        ((end_time->tv_sec == start_time->tv_sec) && (end_time->tv_nsec < start_time->tv_nsec)))
    {
        result->tv_sec = result->tv_nsec = 0;
    }

    result->tv_sec = end_time->tv_sec - start_time->tv_sec;
    result->tv_nsec = end_time->tv_nsec - start_time->tv_nsec;

    if (result->tv_nsec<0)
    {
        result->tv_sec--;
        result->tv_nsec = NANOSECONDS_PER_SECOND+result->tv_nsec;
    }
}

#endif