#define _GNU_SOURCE

#include <limits.h>
#include <errno.h>

#include "simulator.h"

/* scheduler type */
static int flag_dm; /*!< flag_dm activates the deadline monotonic scheduler */

/* scheduler tick */
static unsigned int tick; /*!< scheduler tick is updated in sim_scheduler() */

/* scheduler flags, params,... */
static unsigned int t_start; /*!< start of simulation */
static unsigned int t_stop;  /*!< stop of simulation */
static int flag_nokill;
static int flag_kill;

/* action actions_status */
#define ACTIONS_STATUS_LENGTH 100
static char actions_status[ACTIONS_STATUS_LENGTH]; /*!< Buffer for action message.\n Is reset every tick and set if e.g. a task is preempted or aborted because it missed its deadline */

static struct sim_task_struct *tasks;         /*!< List with all tasks.\n New instances of tasks are created from this list. */
static struct sim_task_stats *stats;          /*!< List with all stats.\n Each task has its own stats. Every instance of a task has a reference to the same stats */
static struct sim_task_struct *ready_queue;   /*!< Queue with all runnable/running tasks */
static struct sim_task_struct *pending_queue; /*!< Queue with all ready tasks. Because tick scheduling is used these task must be put into ready_queue */

/**
 * \brief handle_pending_queue() prepares the pending_queue for the ready_queue.
 *
 * It iterates through all tasks in tasks and checks if they are ready to be executed.
 * If so a deep copy of the task is created and added to the pending queue.
 *
 * This function only adds entries to the pending_queue but it does not alter in any other way.
 * When sim_schedule() is called pending_queue is reset because it expects all tasks to be transferred to ready_queue.
 */
void handle_pending_queue()
{
    /* list of all tasks */
    struct sim_task_struct *tmp_task_list;

    /* temporary task, in loop this is a shallow copy of the selected task from tmp_task_list */
    struct sim_task_struct *tmp_task;
    tmp_task_list = tasks;

    while (tmp_task_list != NULL)
    {
        if ((tick >= tmp_task_list->t_ph + tmp_task_list->t_per &&
             (tick - tmp_task_list->t_ph) % tmp_task_list->t_per == 0) ||
            (tick < tmp_task_list->t_ph + tmp_task_list->t_per && tick == tmp_task_list->t_ph))
        {
#ifdef DEBUG
            printf("handle_pending_queue: creating copy of %s\n", tmp_task_list->name);
#endif
            tmp_task = (struct sim_task_struct *)malloc(sizeof(struct sim_task_struct));
            if (tmp_task == NULL)
            {
                fprintf(stderr, "handle_pending_queue: malloc failed.\n");
                exit(-1);
            }
            create_copy(tmp_task, tmp_task_list);
            tmp_task_list->task_num++;

            /* remove pointer to next task to avoid unwanted tasks in list */
            tmp_task->next = NULL;

            if (pending_queue == NULL)
            {
                pending_queue = tmp_task;
            }
            else
            {
                goto_last_list_element(pending_queue)->next = tmp_task;
            }
        }
        tmp_task_list = tmp_task_list->next; /* select next task */
    }
}

/**
 * \brief handle_ready_queue() prepares the ready queue for pick_next_task().
 *
 * It adds all "new" tasks from the pending_queue to the ready_queue.\n
 * This function only adds items but does not remove them.
 */
void handle_ready_queue()
{
    struct sim_task_struct *tmp_task_list;
    struct sim_task_struct *tmp_task;

    if (pending_queue != NULL)
    {
        tmp_task_list = pending_queue;
        while (tmp_task_list != NULL)
        {
            tmp_task = (struct sim_task_struct *)malloc(sizeof(struct sim_task_struct));
            if (tmp_task == NULL)
            {
                fprintf(stderr, "handle_ready_queue: malloc failed.\n");
                exit(-1);
            }
            create_copy(tmp_task, tmp_task_list); /* deep copy of tmp_task_list */
            tmp_task->next = NULL;

            if (ready_queue == NULL)
            {
                ready_queue = tmp_task;
            }
            else
            {
                goto_last_list_element(ready_queue)->next = tmp_task;
            }

            tmp_task_list = tmp_task_list->next;
        }
    }
}

/**
 * \brief pick_next_task_dm() chooses the task depending on deadline monotonic scheduling.
 *
 * Tasks are chosen by their deadline. The smaller the deadline, the higher the task-priority.
 *
 * Tasks that are finished with their execution are marked by a `kill_flag` and then are potentially removed by handle_tasks_to_delete().
 *
 * @return struct sim_task_struct pointer pointing to next_task
 */
struct sim_task_struct *pick_next_task_dm()
{
    /**
     * 1. choose job from ready queue (it must not be sorted)
     * 2. remove job from ready queue
     */
    struct sim_task_struct *tmp_task_list;
    struct sim_task_struct *next_task;
    const struct sim_task_struct *next_task_same_deadline;

    if (ready_queue == NULL)
    {
        return NULL;
    }

    tmp_task_list = ready_queue;
    next_task = tmp_task_list;
    next_task_same_deadline = NULL;

    /**
     * Iterate through ready_queue and choose next task depending on smallest deadline.\n
     * If this is not possible then use FIFO.\n
     * If this is still not possible use order of tasks.
     */
    while (tmp_task_list != NULL)
    {
        if (tmp_task_list->next != NULL)
        {
            /* choosing min(deadline) */
            if (next_task->t_d > tmp_task_list->next->t_d)
            {
                next_task = tmp_task_list->next;
                next_task_same_deadline = NULL;
            }
            else if (next_task->t_d == tmp_task_list->next->t_d)
            {
#ifdef DEBUG
                printf("Task: %s%d and Task: %s%d have same prio.\n", next_task->name, next_task->task_num, tmp_task_list->next->name,
                       tmp_task_list->next->task_num);
#endif
                if (next_task_same_deadline == NULL)
                {
                    next_task_same_deadline = tmp_task_list->next;
                }
            }
        }
        tmp_task_list = tmp_task_list->next;
    }

    /* mark task with REMOVE_TASK flag. It is later removed from ready_queue. */
    if (next_task->t_remaining_e - 1 <= 0)
    {
#ifdef DEBUG
        printf("pick_next_task: Task: %s has %d flag\n", next_task->name, next_task->kill_flag);
#endif
        if (next_task->kill_flag < REMOVE_TASK)
        {
            next_task->kill_flag = REMOVE_TASK;
        }
    }
    return next_task;
}

/**
 * \brief handle_deadline() checks if tasks in ready_queue missed their deadline.
 *
 * Depending on the flag they are either removed from ready_queue or completely aborted.
 *
 * if `flag_kill` is set, the task is completely removed from the task list and is never added again to pending_queue\n
 * if `flag_nokill` is set, the task is only removed from ready_queue but can still continue its execution in another instance.
 */
void handle_deadline()
{
    struct sim_task_struct *tmp_task_list;
    struct sim_task_struct *tmp_task_list_kill;

    tmp_task_list = ready_queue;
    /* iterate through ready_queue and check every entry */
    while (tmp_task_list != NULL)
    {
#ifdef DEBUG
        printf("Task %s%d: deadline: %d, execution: %d\n", tmp_task_list->name, tmp_task_list->task_num, tmp_task_list->t_remaining_d, tmp_task_list->t_remaining_e);
#endif
        if (tmp_task_list->t_remaining_d-- <= 0 && tmp_task_list->t_remaining_e > 0)
        {
            if (flag_kill)
            {
                snprintf(actions_status, ACTIONS_STATUS_LENGTH, "%s%d missed deadline at %d and was aborted", tmp_task_list->name,
                         tmp_task_list->task_num, tick);
                tmp_task_list->kill_flag = KILL_TASK;

                /* all other instances of task should be killed too. */
                tmp_task_list_kill = ready_queue;
                while (tmp_task_list_kill != NULL)
                {
                    if (strcmp(tmp_task_list->name, tmp_task_list_kill->name) == 0)
                    {
                        tmp_task_list_kill->kill_flag = KILL_TASK;
                    }
                    tmp_task_list_kill = tmp_task_list_kill->next;
                }
            }
            else if (flag_nokill)
            {
                snprintf(actions_status, ACTIONS_STATUS_LENGTH, "%s%d missed deadline at %d", tmp_task_list->name,
                         tmp_task_list->task_num, tick);
                tmp_task_list->kill_flag = REMOVE_TASK;
            }
        }
        tmp_task_list = tmp_task_list->next;
    }
}

void handle_reaction_time()
{
    struct sim_task_struct *tmp_task_list;
    tmp_task_list = ready_queue;
    while (tmp_task_list != NULL)
    {
        tmp_task_list->t_elapsed++;
        tmp_task_list = tmp_task_list->next;
    }
}

/**
 * \brief handle_preemption() checks whether the next task preempted the previous task.
 *
 * This happens when the remaining execution time of prev is above 0
 * and has not a reference to next.
 *
 * @param next is the actual task.
 * @param prev is the previous task in the queue.
 */
void handle_preemption(const struct sim_task_struct *next, struct sim_task_struct *prev)
{
    if (next == NULL || prev == NULL)
    {
        return;
    }

    if (prev != next && prev->t_remaining_e > 0)
    {
        snprintf(actions_status, ACTIONS_STATUS_LENGTH, "%s%d preempted", prev->name, prev->task_num);
    }
}

/**
 * \brief handle_tasks_to_delete() iterates through ready_queue and checks if there are tasks that have a set kill_flag.
 *
 * if kill_flag is set to `REMOVE_TASK` then the task is removed from the ready_queue\n
 * if kill_flag is set to `KILL_FLAG` then the task is completely aborted and removed from the tasks list.
 */
void handle_tasks_to_delete()
{
    struct sim_task_struct *tmp_task_ready_queue;
    struct sim_task_struct *tmp_prev_ready_queue;

    struct sim_task_struct *tmp_task_tasks_list;
    struct sim_task_struct *tmp_prev_tasks_list;

    tmp_task_ready_queue = ready_queue;
    while (tmp_task_ready_queue != NULL)
    {
#ifdef DEBUG
        printf("handle_tasks_to_delete: Task %s: has %d flag\n", tmp_task_ready_queue->name, tmp_task_ready_queue->kill_flag);
#endif
        if (tmp_task_ready_queue->kill_flag >= REMOVE_TASK)
        {
            if (tmp_task_ready_queue->kill_flag == KILL_TASK)
            {

                /* iterate through tasks and remove task */
                tmp_task_tasks_list = tasks;
                while (tmp_task_tasks_list != NULL)
                {
                    if (strcmp(tmp_task_tasks_list->name, tmp_task_ready_queue->name) == 0)
                    {
                        /* remove task from tasks */
                        if (tmp_task_tasks_list == tasks)
                        {
                            tasks = tmp_task_tasks_list->next;
                        }
                        else
                        {
                            tmp_prev_tasks_list->next = tmp_task_tasks_list->next;
                        }
                    }
                    tmp_prev_tasks_list = tmp_task_tasks_list;
                    tmp_task_tasks_list = tmp_task_tasks_list->next;
                }
            }

            /* update "global" t_elapsed to print later max. reaction time */
            if (tmp_task_ready_queue->t_elapsed > tmp_task_ready_queue->stats->t_elapsed)
            {
                tmp_task_ready_queue->stats->t_elapsed = tmp_task_ready_queue->t_elapsed;
            }

            /* remove task from ready_queue */
            if (tmp_task_ready_queue == ready_queue)
            {
                ready_queue = tmp_task_ready_queue->next;
            }
            else
            {
                tmp_prev_ready_queue->next = tmp_task_ready_queue->next;
            }
        }
        tmp_prev_ready_queue = tmp_task_ready_queue;
        tmp_task_ready_queue = tmp_task_ready_queue->next;
    }
}

/**
 * \brief sim_scheduler() is the main scheduler function.
 *
 * It works with a timer tick and thus is called `tick scheduling`.
 * Following things happen here each tick:
 *  - pending_queue is updated via handle_pending_queue(). This retrieves all tasks that are released.
 *  - ready_queue is updated via handle_ready_queue(). This gets all tasks from pending_queue.
 *  - all deadlines are checked. Tasks that miss their deadline are marked with a flag.
 *  - to be executed task is picked via pick_next_task(). The algorithm for picking the task is passed as a pointer via arguments.
 *  - after the task has been picked preemption is checked in handle_preemption().
 *  - then the task is "executed" and printed to the output.
 *
 *  After its execution (which is determined by t_stop) sim_scheduler() calculates each maximum reaction time.
 *
 */
void sim_scheduler(struct sim_task_struct *(*pick_next_task)(void))
{
    struct sim_task_struct *prev;
    struct sim_task_struct *next;

    struct sim_task_stats *stat;

    next = NULL;
    prev = NULL;
    printf("Time\tJob\tReady-Queue\t\tActions\n");
    for (tick = 0; tick <= t_stop; tick++)
    {
        memset(actions_status, '\0', ACTIONS_STATUS_LENGTH); /* reset action actions_status */
        handle_pending_queue();
        handle_ready_queue();

        handle_deadline();

        pending_queue = NULL; /* reset pending queue */

        prev = next;
        next = pick_next_task();
        handle_reaction_time();
        handle_preemption(next, prev);

        if (next != NULL)
        {
            next->t_remaining_e--;
        }
        if (tick >= t_start)
        {
            printf("%u\t", tick);
            if (next != NULL)
            {
                printf("%s%d", next->name, next->task_num); /* tick & job */
            }
            else
            {
                printf("I");
            }
            printf("\t");
            print_list(ready_queue);            /* print ready queue */
            printf("\t\t\t%s", actions_status); /* print actions status message from buffer */
            printf("\n");
        }
        handle_tasks_to_delete(); /* remove all tasks marked with kill_flag */
    }
    printf("-----------------------------------------------\n");

    /* print reaction time */
    stat = stats;
    while (stat != NULL)
    {
        printf("Task: %s\tmax. Reaktionszeit: %d\n", stat->name, stat->t_elapsed);
        stat = stat->next;
    }

    free_task_list(ready_queue);
    free_task_list(pending_queue);
    free_task_list(tasks);
    free_stat_list(stats);
}

/**
 * read_and_parse_input() reads and parses the input file.
 *
 * 1. it goes through each line
 * 2. it checks for Start & Stop
 * 3. if neither of those are found it must be at the table
 * 4. with the help of tokens, each string separated by whitespaces is parsed.
 *  Each successful parsed number from the table updates a token_counter.
 *  If a number is not parsed successfully token_counter is set to -1
 *
 * @param input_fp is the file path to the input file.
 */
void read_and_parse_input(const char *input_fp)
{
    FILE *fp; /* file pointer for input file */
    size_t line_length;
    const char *const delimiter = "\f\v\t\r ";
    char *line;        /* read line */
    char *endptr;      /* endpointer for strtol error checking */
    char *saveptr1;    /* saveptr1 for strtok_r */
    const char *token; /* token for strtok_t */

    int token_counter;
    long tok_num_val;
    struct sim_task_struct *task;
    struct sim_task_stats *stat;

    if (input_fp == NULL)
    {
        fprintf(stderr, "read_and_parse_input: no file path provided.\n");
        exit(EXIT_FAILURE);
    }

    fp = fopen(input_fp, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "read_and_parse_input: %s.", input_fp);
        exit(EXIT_FAILURE);
    }

    line_length = 0;
    while (getline(&line, &line_length, fp) != -1)
    {
        token_counter = 0;
        token = strtok_r(line, delimiter, &saveptr1);
        task = (struct sim_task_struct *)malloc(sizeof(struct sim_task_struct));
        if (task == NULL)
        {
            fprintf(stderr, "read_and_parse_input: malloc failed.\n");
            exit(-1);
        }
        while (token != NULL)
        {
#ifdef DEBUG_PARSE
            printf("read_and_parse_input: parsed token: %s.\n", token);
#endif

            /* Start & Stop */
            if (strcmp(token, "Start") == 0)
            {
                if ((token = strtok_r(NULL, delimiter, &saveptr1)) != NULL)
                {
                    t_start = (unsigned int)strtol(token, NULL, 10);
                }
            }
            else if (strcmp(token, "Stop") == 0)
            {
                if ((token = strtok_r(NULL, delimiter, &saveptr1)) != NULL)
                {
                    t_stop = (unsigned int)strtol(token, NULL, 10);
                }
            }
            else
            {
                /**
                 * this part is responsible for the table rows
                 *
                 * token_counter:
                 *  1: name\n
                 *  2: phase\n
                 *  3: period\n
                 *  4: deadline\n
                 *  5: add task to task list
                 */
                if (token_counter == 0)
                {
                    task->name = strdup(token);
                    token_counter = 1;
                }
                else if (token_counter > 0)
                {
                    errno = 0;
                    tok_num_val = strtol(token, &endptr, 10);
                    if ((errno == ERANGE && (tok_num_val == LONG_MAX || tok_num_val == LONG_MIN)) ||
                        (errno != 0 && tok_num_val == 0) || endptr == token)
                    {
#ifdef DEBUG_PARSE
                        fprintf(stderr, "read_and_parse_input: %s is not a number.\n", token);
#endif
                        token_counter = -1;
                        errno = 0;
                    }
                    else
                    {
                        switch (token_counter)
                        {
                        case 1:
                            task->t_ph = (int)tok_num_val;
                            token_counter = 2;
                            break;
                        case 2:
                            task->t_per = (int)tok_num_val;
                            token_counter = 3;
                            break;
                        case 3:
                            task->t_e = (int)tok_num_val;
                            task->t_remaining_e = task->t_e;
                            token_counter = 4;
                            break;
                        case 4:
                            task->t_d = (int)tok_num_val;
                            task->t_remaining_d = task->t_d;
                            token_counter = 5;
                            break;
                        default:
                            token_counter = -1;
                        }
                    }
                }
            }
            token = strtok_r(NULL, delimiter, &saveptr1);
        }

        /* only add task to tasks if parsing was successful */
        if (token_counter == 5)
        {
#ifdef DEBUG_PARSE
            printf("%s %d %d %d %d\n", task->name, task->t_ph, task->t_per, task->t_remaining_e, task->t_d);
#endif
            stat = malloc(sizeof(struct sim_task_stats));
            if (stat == NULL)
            {
                fprintf(stderr, "read_and_parse_input: malloc failed.\n");
                exit(-1);
            }
            stat->name = strdup(task->name);
            task->stats = stat;

            if (tasks == NULL)
            {
                tasks = task;
            }
            else
            {
                goto_last_list_element(tasks)->next = task;
            }

            if (stats == NULL)
            {
                stats = stat;
            }
            else
            {
                stat->next = stats->next;
                stats->next = stat;
            }
        }
        else
        {
            free(task);
        }
    }
    fclose(fp);
}

/**
 * \brief the main function prepares everything for the simulation.
 *
 * following tasks are handled by this function:
 * - it parses the flags and makes sure there is no wrong combination
 * - it calls the right scheduler based on the flags
 *
 * @param argc is the argument counter that gives information about how many arguments are passed
 * @param argv is the argument vector that holds all passed arguments
 * @return 0 if execution was successful.
 */
int main(int argc, char **argv)
{
    int arg_index;
    const char *input_fp; /* input file path */

    /* scheduler simulator flags */
    const char *nokill;
    const char *kill;
    const char *dm;

    struct sim_task_struct *(*scheduler)(void);

    input_fp = NULL;
    if (argc > 1)
    {
        nokill = (char const *)"-nokill";
        kill = (char const *)"-kill";
        dm = (char const *)"-dm";

        /* flags */
        for (arg_index = 1; arg_index < argc; arg_index++)
        {
            if (strcmp(argv[arg_index], nokill) == 0)
            {
                flag_nokill = 1;
            }
            else if (strcmp(argv[arg_index], kill) == 0)
            {
                flag_kill = 1;
            }
            else if (strcmp(argv[arg_index], dm) == 0)
            {
                flag_dm = 1;
            }
            else
            {
                input_fp = argv[arg_index];
            }
        }
    }
    else
    {
        fprintf(stderr, "simulator: please provide some arguments.\n");
        exit(EXIT_FAILURE);
    }

    if (flag_kill && flag_nokill)
    {
        fprintf(stderr, "simulator: cannot have both nokill and kill flags.\n");
        exit(EXIT_FAILURE);
    }

    if (input_fp == NULL)
    {
        fprintf(stderr, "simulator: please provide an input file.\n");
        exit(EXIT_FAILURE);
    }

    if (flag_dm)
    {
        scheduler = &pick_next_task_dm;
    }
    else
    {
        scheduler = NULL;
    }

    read_and_parse_input(input_fp);
    if (scheduler != NULL)
    {
        sim_scheduler(scheduler);
    }

    return 0;
}