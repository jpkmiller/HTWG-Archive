#include "helpergpio.h"
#include <pthread.h>
#include <signal.h>
#include <string.h>

#define NANOSECONDS_PER_SECOND 1000000000

static int button;
static pthread_mutex_t button_mut;

void sigint_handler()
{
    SetVal("/sys/class/gpio/unexport", "18", 3);
    SetVal("/sys/class/gpio/unexport", "17", 3);
}

_Noreturn void *handleButton(void *args)
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &action, NULL) == -1)
    {
        perror("blink: sigaction");
        exit(1);
    }

    printf("blink: setup GPIO pin 17 ...\n");
    /* setup GPIO pin 17 */
    SetVal("/sys/class/gpio/export", "17", 3);
    SetVal("/sys/class/gpio/gpio17/direction", "in", 3);
    while (1)
    {
        WaitNsec(0, NANOSECONDS_PER_SECOND / 10); // 100 msec;
        if (ReadVal("/sys/class/gpio/gpio17/value", 1) == 48)
        {
            pthread_mutex_lock(&button_mut);
            // button pressed
            button = 1;
            pthread_mutex_unlock(&button_mut);
        }
    }
}

_Noreturn void *blink(void *args)
{
    printf("blink: started thread ...\n");
    int hz = *(int *)args;
    int blink = 0;

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &action, NULL) == -1)
    {
        perror("blink: sigaction");
        exit(1);
    }

    printf("blink: setup GPIO pin 18 ...\n");
    /* setup GPIO pin 18 */
    SetVal("/sys/class/gpio/export", "18", 3);
    SetVal("/sys/class/gpio/gpio18/direction", "out", 4);

    long sleep_time_nsec = (long)(NANOSECONDS_PER_SECOND / hz) / 2; // div
    printf("blink: blinking every %ld nsec ...\n", sleep_time_nsec);

    printf("blink: finished preparing ...\n");
    while (1)
    {
        if (pthread_mutex_trylock(&button_mut) != EBUSY)
        {
            if (button == 1)
            {
                button = 0;
                blink = (blink + 1) % 2;
                printf("blink: Button pressed. Now %d\n", blink);
                if (!blink)
                {
                    SetVal("/sys/class/gpio/gpio18/value", "1", 2);
                }
            }
            pthread_mutex_unlock(&button_mut);

            if (blink)
            {
                SetVal("/sys/class/gpio/gpio18/value", "0", 2);
                WaitNsec(0, sleep_time_nsec);
                SetVal("/sys/class/gpio/gpio18/value", "1", 2);
                WaitNsec(0, sleep_time_nsec);
            }
        }
    }
}

int main(int argc, char **argv)
{
    int hzValue = 5;
    if (argc == 3)
    {
        // -hz flag potentially set
        const char *hzFlag = "-hz";
        if (strcmp(hzFlag, argv[1]) == 0)
        {
            hzValue = (int)strtol(argv[2], NULL, 10);
        }
        else
        {
            printf("led5.c: hz flag not set. Call ./led5.c -hz <value> ...");
        }
    }
    if (hzValue == 0)
    {
        hzValue = 5;
    }
    printf("Blinking LED at %d hz.\n", hzValue);

    // thread init
    pthread_mutex_init(&button_mut, NULL);

    pthread_t handleButton_t;
    printf("led5.c: creating new thread handleButton ...\n");
    if (pthread_create(&handleButton_t, NULL, handleButton, NULL) != 0)
    {
        fprintf(stderr, "led5.c: failed creating new thread handleButton ..!\n");
        return -1;
    }

    pthread_t blinker_t;
    printf("led5.c: creating new thread blinker ...\n");
    if (pthread_create(&blinker_t, NULL, blink, &hzValue) != 0)
    {
        fprintf(stderr, "led5.c: failed creating new thread blinker ..!\n");
        return -1;
    }

    pthread_join(handleButton_t, NULL);
    pthread_join(blinker_t, NULL);
    printf("led5.c: finished!");
    return 0;
}