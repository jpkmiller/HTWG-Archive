#include "helpergpio.h"
#include <pthread.h>
#include <signal.h>

#define MAXWAITREACTION 10
#define MINWAITREACTION 3
#define BUTTONDEACTIVATE 3

#define NANOSECONDS_PER_SECOND 1000000000

static volatile int button_state;
static volatile int button;
static pthread_mutex_t button_mut;

void unexportGPIO()
{
    SetVal("/sys/class/gpio/unexport", "18", 3);
    SetVal("/sys/class/gpio/unexport", "17", 3);
}

void sigint_handler()
{
    unexportGPIO();
}

int debounce(int buttonValue)
{
    // Code abgeändert von https://elektro.turanis.de/html/prj059/index.html
    pthread_mutex_lock(&button_mut);
    if (button_state == 0 && buttonValue == 48)
    {
        // Taster wird gedrückt (steigende Flanke)
        button_state = 1;
        button = 1;
    }
    else if (button_state == 1 && buttonValue == 48)
    {
        // Taster wird gehalten
        button_state = 2;
        button = 0;
    }
    else if (button_state == 2 && buttonValue == 49)
    {
        // Taster wird losgelassen (fallende Flanke)
        button_state = 3;
        button = 0;
    }
    else if (button_state == 3 && buttonValue == 49)
    {
        // Taster losgelassen
        button_state = 0;
        button = 0;
    }
#ifdef DEBUG_BUTTON
    printf("debounce: button: %d\tbutton_state: %d\n", button, button_state);
#endif
    pthread_mutex_unlock(&button_mut);
    return button;
}

int *handleButton()
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &action, NULL) == -1)
    {
        perror("handleButton: sigaction");
        exit(1);
    }

#ifdef DEBUG
    printf("handleButton: setup GPIO pin 17 ...\n");
#endif
    /* setup GPIO pin 17 */
    SetVal("/sys/class/gpio/export", "17", 3);
    SetVal("/sys/class/gpio/gpio17/direction", "in", 3);

    struct timespec button_time_start;
    struct timespec button_time_end;

    int button_pressed;

    while (1)
    {
        button_pressed = 0;
        button = 0;
        clock_gettime(CLOCK_MONOTONIC, &button_time_start);
        while (1)
        {
            button_pressed += debounce(ReadVal("/sys/class/gpio/gpio17/value", 1));
#ifdef DEBUG_BUTTON
            printf("button: %d\n", button_pressed);
#endif
            if (button_pressed == BUTTONDEACTIVATE)
            {
                break;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &button_time_end);
        struct timespec button_result;
        DiffTime(&button_time_start, &button_time_end, &button_result);
        if (button_pressed == BUTTONDEACTIVATE && button_result.tv_sec <= 2 && button_result.tv_sec >= 1)
        {
            printf("handleButton: Button clicked 3 times. Exiting now!\n");
            unexportGPIO();
            exit(0);
        }
    }
    return 0;
}

int *reactionTimeMeasurement()
{
    SetVal("/sys/class/gpio/export", "18", 2);
    SetVal("/sys/class/gpio/gpio18/direction", "out", 3);
    SetVal("/sys/class/gpio/gpio18/value", "1", 1);

    int activate = 0;

    printf("Press button to start!\n");
    while (1)
    {
        if (pthread_mutex_trylock(&button_mut) != EBUSY)
        {
            if (button == 1)
            {
                activate = 1;
                button = 0;
            }
            pthread_mutex_unlock(&button_mut);
        }

        if (activate == 1)
        {
            // do reaction time test
            pthread_mutex_lock(&button_mut);
            // button pressed
            button = 0;
            pthread_mutex_unlock(&button_mut);

            struct timespec start_time;
            struct timespec end_time;
            struct timespec result; // S. Buch 118

            srand(time(0));
            int randTimer = (rand() % (MAXWAITREACTION - MINWAITREACTION + 1)) + MINWAITREACTION;
            printf("react.c: Wait for %d seconds\n", randTimer);
            WaitNsec(randTimer, 0); // wait random amount of time

            SetVal("/sys/class/gpio/gpio18/value", "0", 1); // turn on 18 18
            clock_gettime(CLOCK_MONOTONIC, &start_time);
            while (1)
            {
                if (button == 1)
                {
                    clock_gettime(CLOCK_MONOTONIC, &end_time);
                    SetVal("/sys/class/gpio/gpio18/value", "1", 1);
                    break;
                }
            }
            DiffTime(&start_time, &end_time, &result);

            printf("Reactiontime: %ld s and %ld ns\n", result.tv_sec, result.tv_nsec);
            if (result.tv_sec == 0 && result.tv_nsec <= NANOSECONDS_PER_SECOND * 0.2)
            {
                printf("You're pretty fast!\n");
            }
        }
    }
    return 0;
}

int main()
{
    // threads initialization
    pthread_mutex_init(&button_mut, NULL);

    // thread for button
    pthread_t handleButton_t;
#ifdef DEBUG
    printf("react.c: creating new thread handleButton ...\n");
#endif
    if (pthread_create(&handleButton_t, NULL, (void *)handleButton, NULL) != 0)
    {
        fprintf(stderr, "react.c: failed creating new thread handleButton ..!\n");
        return -1;
    }

    // thread for time measurement
    pthread_t reactionTimeMeasurement_t;
#ifdef DEBUG
    printf("react.c: creating new thread reactionTimeMeasurement ...\n");
#endif
    if (pthread_create(&reactionTimeMeasurement_t, NULL, (void *)reactionTimeMeasurement, NULL) != 0)
    {
        fprintf(stderr, "react.c: failed creating new thread reactionTimeMeasurement ..!\n");
        return -1;
    }

    pthread_join(handleButton_t, NULL);
    pthread_join(reactionTimeMeasurement_t, NULL);

    unexportGPIO();
    printf("react.c: finished!");
    return 0;
}