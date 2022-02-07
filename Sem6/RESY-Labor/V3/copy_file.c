#include <fcntl.h>
#include <malloc.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

/* rt */
#define PRIORITY_OF_THIS_TASK 50
#define __USE_UNIX98 /* needed for PTHREAD_PRIO_INHERIT */

static pthread_mutex_t rtmutex;
static pthread_mutexattr_t attr;
const char *const Policies[] = {"SCHED_RR"};

/* time */
struct copy_time
{
    int bufsize;
    clock_t user;
    clock_t sys;
    /* te = user + sys */ /* execution */
    clock_t tw;
    clock_t tr; /* reaction */
};
static struct copy_time copy_times[18]; /* (2¹² * 2¹⁰) / 2⁴ */
static int times_index;

static void print_time(struct copy_time *time)
{
    /* time */
    long ticks_per_second, tickduration_ms;

    /* print out all times */
    ticks_per_second = sysconf(_SC_CLK_TCK);
    tickduration_ms = 1000 / ticks_per_second;

    int te, tw, tr;
    te = time->user + time->sys;
    tw = time->tr - te;
    tr = time->tr;

    printf("| %7.1d | %6.1d | %5.1d | %5.1d | %5.1d | %5.1d |\n",
           time->bufsize,
           (int) (time->user * tickduration_ms),
           (int) (time->sys * tickduration_ms),
           (int) (te * tickduration_ms),
           (int) (tw * tickduration_ms),
           (int) (tr * tickduration_ms));
}


static int copy_file(const char *const infilename, const char *const outfilename, const int bufsize)
{
    char *buffer;

    /* return values */
    int in, out, count, ret, tmp;

    /* time */
    struct tms start_copy, end_copy;
    clock_t start_tr, end_tr;

    if (infilename == NULL || outfilename == NULL)
    {
        /* parameter check */
        fprintf(stderr, "copy_file: wrong parameter\n");
        return 0;
    }

    in = open(infilename, O_RDONLY); /* read-only */
    if (in < 0)
    {
        perror(infilename);
        return -1;
    }

    /*
     * https://www.zz9.co.za/chmod-permissions-flags-explained-600-0600-700-777-100-etc/
     * (0, owner -> write and read, users in file group -> read, users not in file group -> read)
     */
    out = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0)
    {
        fprintf(stderr, "copy_file: %s", outfilename);
        return -1;
    }

    buffer = (char *) malloc(bufsize); /* dynamic allocation for varying the buffersize */
    if (buffer == NULL)
    {
        perror("copy_file: malloc\n"); /* cleanup */
        goto failed_action;
    }

    /*
     * read data in buffer with bufersize length
     * write data in outfilename with count length
     */
    start_tr = times(&start_copy);
    do
    {
        /* read */
        count = read(in, buffer, bufsize);

        if (count == -1)
        {
            perror("copy_file: read\n");
            goto failed_action;
        }

        /* write */
        /* check if all read bytes are written */
        ret = 0;
        while (ret != count)
        {
            tmp = write(out, buffer + ret, count - ret);

            if (tmp == -1)
            {
                perror("copy_file: write\n");
                goto failed_action;
            }
            ret += tmp;
        }

    } while (count);
    end_tr = times(&end_copy);
    struct copy_time cp = {
            .bufsize = bufsize, .user = end_copy.tms_utime - start_copy.tms_utime, .sys =
            end_copy.tms_stime - start_copy.tms_stime, .tr = end_tr - start_tr
    };

    copy_times[times_index] = cp;


    free(buffer);
    close(in);
    close(out);

#ifdef DEBUG
    printf("copy_file: finished\n");
#endif
    /* success */
    return 0;

    failed_action:
    /* failed */
    close(in);
    close(out);
    return -1;
}


int main(int argc, char **argv)
{
    const char *infilename, *outfilename;

    /* flags */
    int flag_rt, flag_rtarch, buffer_size, print_time_index;

    /* sched */
    struct sched_param SchedulingParameter;

    flag_rt = flag_rtarch = 0;

#ifdef DEBUG
    if (argc > 1)
    {
        printf("copy_file: args\n");
    }
#endif

    /* handle flags */
    const char *const opt_rt = "-rt";         /* rt prio */
    const char *const opt_rtarch = "-rtarch"; /* multicore */
    int arg_index;
    for (arg_index = 1; arg_index < argc; arg_index++)
    {
#ifdef DEBUG
        printf("copy_file: setting %s flag\n", argv[arg_index]);
#endif
        if (strcmp(opt_rt, argv[arg_index]) == 0)
        {
            flag_rt = 1;
        }
        else if (strcmp(opt_rtarch, argv[arg_index]) == 0)
        {
            flag_rtarch = 1;
        }
    }

#ifdef DEBUG
    printf("rt: %d\n", flag_rt);
    printf("rtarch: %d\n", flag_rtarch);
#endif


    /* rtarch flag */
    if (flag_rtarch == 1)
    {
        int parent_pid = (int) getpid();
        int child_pid = fork();
        if (child_pid < 0)
        {
            perror("Fork failed\n");
            return -1;
        }
        else if (child_pid == 0)
        {
            /* child */
#ifdef DEBUG
            printf("copy_file: child with pid %d\n", (int)getpid());
#endif
            char pid_as_string[10];
            sprintf(pid_as_string, "%d", parent_pid);

            /* rtarch set-ups the rt environment, needs root privilege to write to /sys/ fs */
            char *argument_list_rmmod[] = {"./rtarch.sh", pid_as_string, NULL};
            execvp("./rtarch.sh", argument_list_rmmod);
        }
        else
        {
#ifdef DEBUG
            printf("copy_file: parent with pid %d\n", parent_pid);
            printf("copy_file: waiting for child\n");
#endif
            wait(&child_pid);
        }
    }



    /* rt flag */
    if (flag_rt == 1)
    {
#ifdef DEBUG
        printf("diffmessung: setup rt\n");
#endif
        /* setup rt */
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
        pthread_mutex_init(&rtmutex, &attr);

        SchedulingParameter.sched_priority = PRIORITY_OF_THIS_TASK;
        if (sched_setscheduler(0, SCHED_RR, &SchedulingParameter) != 0)
        {
            perror("copy_file: set scheduling priority failed\n");
            return -1;
        }
    }


    /* set files */
    infilename = (char const *) "/tmp/foo";
    outfilename = (char const *) "/tmp/copy";

    /* measure time */
    for (buffer_size = 16; buffer_size <= 4096 * 1024; buffer_size *= 2)
    {
        printf("copy_file: copying now with buffersize of %d\n", buffer_size);
        copy_file(infilename, outfilename, buffer_size);
        ++times_index;
    }


#ifdef DEBUG
    printf("tick duration: %ld\n", tickduration_ms);
#endif
    printf("|  buffer |user[ms]|sys[ms]| te[ms]| tw[ms]| tr[ms]|\n");
    printf("|---------+--------+-------+-------+-------+-------|\n");


    for (print_time_index = 0; print_time_index < 18; print_time_index++)
    {
        print_time(&copy_times[print_time_index]);
    }


    return 0;
}