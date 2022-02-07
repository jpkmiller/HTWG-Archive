#define _GNU_SOURCE

#include <limits.h>
#include <errno.h>
#include <stdbool.h>

#include "simulator.h"

/* scheduler type */
static bool flag_dm;  /*!< flag_dm activates the deadline monotonic scheduler */
static bool flag_edf; /*!< flag_edf activates the earliest deadline first scheduler */

/* scheduler tick */
static unsigned int tick; /*!< scheduler tick is updated in sim_scheduler() */

/* scheduler flags, params,... */
static unsigned int t_start; /*!< start of simulation */
static int t_stop;           /*!< stop of simulation */
static bool flag_nokill;
static bool flag_kill;

#define STATUS_STRING_LENGTH 100
#define STATUS_STRING_AMOUNT 10

/* Deadlines, etc. */
static char actions_status[STATUS_STRING_AMOUNT][STATUS_STRING_LENGTH]; /*!< Buffer for action message.\n Is reset every tick and set if e.g., a task is preempted or aborted because it missed its deadline */
int actions_status_index;

/* Resources */
static char resource_status[STATUS_STRING_AMOUNT][STATUS_STRING_LENGTH];
int resource_status_index;

/* tasks */
static list_node *tasks;         /*!< List with all tasks.\n New instances of tasks are created from this list. */
static list_node *stats;         /*!< List with all stats.\n Each task has its own stats. Every instance of a task has a reference to the same stats */
static list_node *killed_tasks;  /*!< List with all tasks that were killed during schedule simulator. */
static list_node *ready_queue;   /*!< Queue with all runnable/running tasks */
static list_node *pending_queue; /*!< Queue with all ready tasks. Because tick scheduling is used, these task must be put into ready_queue */

/* resources */
static list_node *blocked_queue; /*!< Queue with all blocked tasks depending on the ressource */
static list_node *resources;     /*!< List with all (available) resources */

/**
 * Prints the content of a buffer.
 * @param status is the buffer that is read.
 * @param index gives information about how "full" the buffer is.
 */
void print_status(char status[STATUS_STRING_AMOUNT][STATUS_STRING_LENGTH], int index)
{
    int print_index;
    for (print_index = 0; print_index < index; ++print_index)
    {
        printf("%s", status[print_index]);
        if (print_index < index - 1)
        {
            printf(" + ");
        }
    }
}

/**
 * Adds a status message to a buffer.
 * @param dst is a pointer to the buffer.
 * @param src is where the status message is read from.
 * @param dst_index points to the next free slot.
 */
void add_status(char (*dst)[STATUS_STRING_LENGTH], const char src[STATUS_STRING_LENGTH], int *dst_index)
{
    strcpy(dst[(*dst_index)], src);
    (*dst_index)++;
}

/**
 * @brief handle_pending_queue() prepares the pending_queue for the ready_queue.
 *
 * It iterates through all tasks in tasks and checks if they are ready to be executed.
 * If so a deep copy of the task is created and added to the pending queue.
 *
 * This function only adds entries to the pending_queue, but it does not alter in any other way.
 * When sim_schedule() is called pending_queue is reset because it expects all tasks to be transferred to ready_queue.
 */
void handle_pending_queue()
{
    list_node *tmp_task_list = tasks;
    while (tmp_task_list != NULL)
    {
        if (((tick >= (tmp_task_list->task->t_ph + tmp_task_list->task->t_per)) &&
             (0 == ((tick - tmp_task_list->task->t_ph) % tmp_task_list->task->t_per))) ||
            ((tick < (tmp_task_list->task->t_ph + tmp_task_list->task->t_per)) && (tick == tmp_task_list->task->t_ph)))
        {
            create_and_add_node_to_list(&pending_queue, copy_node(tmp_task_list->task, sim_task_t), sim_task_t);
            ++tmp_task_list->task->task_num;
        }
        tmp_task_list = tmp_task_list->next; /* select next task */
    }
}

/**
 * @brief handle_ready_queue() prepares the ready queue for pick_next_task().
 *
 * It adds all "new" tasks from the pending_queue to the ready_queue.\n
 * This function only adds items but does not remove them.
 */
void handle_ready_queue()
{
    list_node *tmp_pending_queue = pending_queue;
    while (tmp_pending_queue != NULL)
    {
        create_and_add_node_to_list(&ready_queue, copy_node(tmp_pending_queue->task, sim_task_t), sim_task_t);
        tmp_pending_queue = tmp_pending_queue->next;
    }
}

/**
 * @brief pick_next_task_dm() chooses the task depending on deadline monotonic scheduling.
 * Tasks are chosen by their deadline. The smaller the deadline, the higher the task-priority.
 * Tasks that are finished with their execution are marked by a `killed flag` or `removed flag` and then are potentially removed by handle_tasks_to_delete().
 *
 * @return sim_task pointer pointing to next_task
 */
sim_task *pick_next_task_dm(list_node *list)
{
    /**
     * 1. choose job from ready queue (it must not be sorted)
     * 2. remove job from ready queue
     */
    list_node *tmp_queue;
    sim_task *next_task;
    const sim_task *next_task_same_deadline;

    if ((NULL == list) || (NULL == (list->task)))
    {
        return NULL;
    }

    tmp_queue = list;
    next_task = tmp_queue->task;
    next_task_same_deadline = NULL;

    /**
     * Iterate through ready_queue and choose next task depending on smallest deadline.\n
     * If this is not possible, use FIFO.\n
     * If this is still not possible, use order of tasks.
     */
    while (tmp_queue != NULL)
    {
        if (tmp_queue->next != NULL)
        {
            /* choosing min(deadline) */
            if (next_task->t_d > tmp_queue->next->task->t_d)
            {
                next_task = tmp_queue->next->task;
                next_task_same_deadline = NULL;
            }
            else if ((next_task->t_d == tmp_queue->next->task->t_d) && (NULL == next_task_same_deadline))
            {
                next_task_same_deadline = tmp_queue->next->task;
            }
        }
        tmp_queue = tmp_queue->next;
    }
    return next_task;
}

/**
 * @brief pick_next_task_edf() chooses the task depending on a job's deadline.
 * The deadline tells which job has to be done earlier.
 * Tasks that are finished with their execution are marked by a `task state` and then are potentially removed by handle_tasks_to_delete().
 *
 * @return sim_task pointer pointing to next_task
 */
sim_task *pick_next_task_edf(list_node *list)
{
    /**
     * 1. choose job from ready queue (it must not be sorted).
     * 2. remove job from ready queue by setting REMOVE_TASK flag.
     */
    list_node *tmp_queue;
    sim_task *next_task;

    if ((NULL == list) || (NULL == list->task))
    {
        return NULL;
    }

    tmp_queue = list;
    next_task = tmp_queue->task;

    /* Iterate through ready_queue and choose next task depending on smallest remaining deadline. */
    while (tmp_queue != NULL)
    {
        if ((NULL != tmp_queue->next) &&
            (next_task->t_remaining_d > tmp_queue->next->task->t_remaining_d))
        {
            next_task = tmp_queue->next->task;
        }
        tmp_queue = tmp_queue->next;
    }
    return next_task;
}

/**
 * @brief Checks if tasks finished running and sets status `removed`.
 * @param task is a reference to task.
 */
void remove_task_if_finished_execution(sim_task *task)
{
    /* mark task with REMOVE_TASK flag. It is later removed from ready_queue. */
    if ((task != NULL) && ((task->t_remaining_e - 1) <= 0) && (task->task_state != killed))
    {
        task->task_state = removed;
    }
}

/**
 * @brief Checks if any task in a list has passed its deadline.
 * If so, depending on the kill flag, it is either killed or only removed from ready_queue.
 *
 * @param task_node
 * @param kill_from_queue
 */
void handle_deadline(list_node *task_node, list_node *kill_from_queue)
{
    list_node *tmp_queue;
    sim_task *task;
    char buffer[STATUS_STRING_LENGTH];

    task = task_node->task;
#ifdef DEBUG
    printf("%d handle_deadline: %s%d: deadline %d, exec %d!\n", tick, task->name, task->task_num, task->t_remaining_d, task->t_remaining_e);
#endif
    if ((0 >= task->t_remaining_d--) && (0 < task->t_remaining_e))
    {
#ifdef DEBUG
        printf("%d handle_deadline: %s%d missed deadline!\n", tick, task->name, task->task_num);
#endif
        if (flag_kill)
        {
            snprintf(buffer, STATUS_STRING_LENGTH, "%s%d missed Deadline (aborted)",
                     task->name,
                     task->task_num);
            add_status(&actions_status[0], buffer, &actions_status_index);
            task->task_state = killed;

            /* all other instances of task should be killed too. */
            tmp_queue = kill_from_queue;
            while (tmp_queue != NULL)
            {
                if (0 == strcmp(task->name, tmp_queue->task->name))
                {
                    tmp_queue->task->task_state = killed;
                }
                tmp_queue = tmp_queue->next;
            }
        }
        else if (flag_nokill)
        {
            snprintf(buffer, STATUS_STRING_LENGTH, "%s%d missed Deadline", task->name,
                     task->task_num);
            add_status(&actions_status[0], buffer, &actions_status_index);
            task->task_state = removed;
        }
    }
}

/**
 * @brief Checks if tasks in ready_queue missed their deadline.
 *
 * Depending on the flag they are either removed from ready_queue or completely aborted.
 *
 * if `flag_kill` is set, the task is completely removed from the task list and is never added again to pending_queue\n
 * if `flag_nokill` is set, the task is only removed from ready_queue but can still continue its execution in another instance.
 */
void handle_deadline_queues()
{
    /* iterate through ready_queue and check every entry */
    list_node *tmp_ready_queue;
    list_node *tmp_blocked_queue;

    tmp_ready_queue = ready_queue;
    while (tmp_ready_queue != NULL)
    {
        handle_deadline(tmp_ready_queue, ready_queue);
        tmp_ready_queue = tmp_ready_queue->next;
    }

    tmp_blocked_queue = blocked_queue;
    while (tmp_blocked_queue != NULL)
    {
        handle_deadline(tmp_blocked_queue, blocked_queue);
        tmp_blocked_queue = tmp_blocked_queue->next;
    }
}

/**
 * @brief Locks resource.
 * It iterates through all available resources and checks if a task needs the resource.
 * Before this happens it is checked if t_for and t_from are within range.
 * @param task is a reference to the task that allocates the resource.
 * @param resource is a reference to the task resources that gets allocated.
 */
void lock_resource(sim_task *task, sim_task_resource_stats *resource)
{
    list_node *tmp_ready_queue;
    list_node *tmp_prev_ready_queue;
    list_node *tmp_resource;

    char buffer[STATUS_STRING_LENGTH];

#ifdef DEBUG
    if (task != NULL)
    {
        printf("%d lock_resource: %s%d wants to lock resource %s. It can lock it from %d (it has %d) and holds it for %d (it has %d)!\n", tick, task->name, task->task_num, resource->name, resource->t_from, resource->t_from_elapsed, resource->t_for, resource->t_for_elapsed);
    }
#endif
    if ((NULL == task) || (blocked == task->task_state))
    {
#ifdef DEBUG
        if (task != NULL)
        {
            printf("%d lock_resource: %s%d wanted to lock resource %s. But task is already blocked\n", tick, task->name, task->task_num, resource->name);
        }
#endif
        return;
    }
    else if ((resource->t_from_elapsed < resource->t_from) ||
             (resource->t_for_elapsed >= resource->t_for))
    {
#ifdef DEBUG
        if (task != NULL)
        {
            printf("%d lock_resource: %s%d wanted to lock resource %s. But it is too early or too late\n", tick, task->name, task->task_num, resource->name);
        }
#endif
        resource->t_from_elapsed++;
        return;
    }

    resource->t_from_elapsed++;

    tmp_resource = resources;
    while (tmp_resource != NULL)
    {
        if (0 == (strcmp(tmp_resource->task_resource->name, resource->name)))
        {
            /* if resource is free */
            if (NULL == tmp_resource->task_resource->used_by)
            {
                tmp_resource->task_resource->used_by = task;
                snprintf(buffer, STATUS_STRING_LENGTH, "%s%d locks %s -> ok",
                         task->name,
                         task->task_num,
                         resource->name);
                add_status(&resource_status[0], buffer, &resource_status_index);
#ifdef DEBUG
                printf("%d lock_resource: %s%d locks resource %s!\n", tick, task->name, task->task_num, resource->name);
#endif
            }
            else if (0 != (strcmp(tmp_resource->task_resource->used_by->name, task->name)))
            {
                /* if resource is blocked by another task */
                /* to know which tasks are blocked */
                if (task->blocked_by_resc != NULL)
                {
                    free(task->blocked_by_resc);
                }
                task->blocked_by_resc = strdup(tmp_resource->task_resource->name);
                task->task_state = blocked;
#ifdef DEBUG
                printf("%d lock_resource: %s%d was blocked because resource %s is locked!\n", tick, task->name, task->task_num, resource->name);
#endif
                create_and_add_node_to_list(&blocked_queue, copy_node(task, sim_task_t), sim_task_t);
                snprintf(buffer, STATUS_STRING_LENGTH, "%s%d locks %s -> blocked",
                         task->name,
                         task->task_num,
                         resource->name);
                add_status(&resource_status[0], buffer, &resource_status_index);

                /* remove blocked task from ready_queue */
                tmp_ready_queue = ready_queue;
                tmp_prev_ready_queue = ready_queue;
                while (tmp_ready_queue != NULL)
                {
                    if (tmp_ready_queue->task == task)
                    {
                        remove_node_from_list(&ready_queue, tmp_ready_queue, &tmp_prev_ready_queue);
                    }
                    tmp_prev_ready_queue = tmp_ready_queue;
                    tmp_ready_queue = tmp_ready_queue->next;
                }
            }
            else
            {
#ifdef DEBUG
                printf("%d lock_resource: %s%d locks already resource %s!\n", tick, task->name, task->task_num, resource->name);
#endif
                resource->t_for_elapsed++;
            }
        }
        tmp_resource = tmp_resource->next;
    }
}

/**
 * @brief Frees the resource without any further checking.
 *
 * @param task that should release its resources.
 * @param resource which is blocked by the task.
 */
void unlock_resource(sim_task *task, sim_task_resource_stats *resource)
{
    char buffer[STATUS_STRING_LENGTH];
    list_node *tmp_resource;
    list_node *tmp_prev_resource;

    tmp_resource = resources;
    while (tmp_resource != NULL)
    {
        if ((0 == strcmp(tmp_resource->task_resource->name, resource->name)) &&
            (tmp_resource->task_resource->used_by == task))
        {
#ifdef DEBUG
            printf("%d unlock_resource: %s%d unlocks resource %s!\n", tick, task->name, task->task_num, resource->name);
#endif
            snprintf(buffer, STATUS_STRING_LENGTH, "%s%d unlocks: %s",
                     task->name,
                     task->task_num,
                     resource->name);
            add_status(&resource_status[0], buffer, &resource_status_index);

            /* remove resource from task so that it cannot relock it */
            remove_node_from_list(&resources, tmp_resource, &tmp_prev_resource);
            tmp_resource->task_resource->used_by = NULL;
        }
        tmp_prev_resource = tmp_resource;
        tmp_resource = tmp_resource->next;
    }
}

/**
 * @brief Frees all resources of a task.
 * @param task is a reference to the task.
 */
void unlock_resources(sim_task *task)
{
    list_node *tmp_resource = task->resources;
    while (tmp_resource != NULL)
    {
        unlock_resource(task, tmp_resource->task_resource_stats);
        tmp_resource = tmp_resource->next;
    }
}

/**
 * @brief Iterates through task resources and checks if a resource can be unlocked.
 * @param task is a reference to the task.
 */
void unlock_resources_if_finished(sim_task *task)
{
    list_node *task_resource;
    if (task != NULL)
    {
        task_resource = task->resources; /* task_resource of task */
        while (task_resource != NULL)
        {
            if (task_resource->task_resource_stats->t_from_elapsed >= task_resource->task_resource_stats->t_from &&
                task_resource->task_resource_stats->t_for_elapsed >= task_resource->task_resource_stats->t_for)
            {
#ifdef DEBUG
                printf("%d unlock_resources_if_finished: %s%d unlocks resource %s because [t_for_elapsed] %d >= %d [t_for] !\n", tick, task->name, task->task_num, task_resource->task_resource_stats->name, task_resource->task_resource_stats->t_for_elapsed, task_resource->task_resource_stats->t_for);
#endif
                unlock_resource(task, task_resource->task_resource_stats);
            }
            task_resource = task_resource->next;
        }
    }
}

/**
 * @brief Iterates through the blocked_queue and puts all tasks that wait for a specific resource
 * into a separate temporary queue. After that the "right" task is selected depending on the scheduler.
 * This selected task is put into ready state and readded to ready_queue.
 *
 * @param pick_next_task is the task selection algorithm. It depends on the scheduling flag.
 */
void add_tasks_from_blocked_queue_to_ready_queue(sim_task *(*pick_next_task)(list_node *))
{
    list_node *tmp_blocked_queue;
    list_node *tmp_blocked;
    list_node *tmp_resource;

    sim_task *picked_task;

    tmp_resource = resources;
    while (tmp_resource != NULL)
    {
        if (NULL == tmp_resource->task_resource->used_by)
        {
            tmp_blocked_queue = NULL;
            tmp_blocked = blocked_queue;
            while (tmp_blocked != NULL)
            {
                if (0 == strcmp(tmp_blocked->task->blocked_by_resc, tmp_resource->task_resource->name))
                {
                    add_node_to_list(&tmp_blocked_queue, tmp_blocked);
                }
                tmp_blocked = tmp_blocked->next;
            }

            if (tmp_blocked_queue != NULL)
            {
                picked_task = pick_next_task(tmp_blocked_queue);
                picked_task->task_state = ready;
                create_and_add_node_to_list(&ready_queue, copy_node(picked_task, sim_task_t), sim_task_t);
                picked_task->task_state = removed;
            }
        }
        tmp_resource = tmp_resource->next;
    }
}

/**
 * @brief lock_resources() checks the resources that are dedicated to the task.
 * This function only iterates over a task's resources list.
 *
 * @param task is a reference to task.
 */
void lock_resources(sim_task *task)
{
    if (task != NULL)
    {
        list_node *task_resources = task->resources;
        while (task_resources != NULL)
        {
            lock_resource(task, task_resources->task_resource_stats);
            task_resources = task_resources->next;
        }
    }
}

/**
 * @brief handle_tasks_to_delete() iterates through ready_queue and checks if there are tasks that have a set task state.
 *
 * if task state is set to `removed` then the task is removed from the ready_queue\n
 * if task state is set to `killed` then the task is completely aborted and removed from the tasks list.
 */
void handle_tasks_to_delete(list_node **list)
{
    list_node *tmp_list;
    list_node *tmp_prev_list;

    list_node *tmp_task_tasks_list;
    list_node *tmp_prev_tasks_list;

    sim_task_kill *kill_task;

    tmp_list = *list;
    tmp_prev_list = *list;

    while (tmp_list != NULL)
    {
#ifdef DEBUG
        printf("\n%d handle_tasks_to_delete: %s%d has %u state\n", tick, tmp_list->task->name, tmp_list->task->task_num, tmp_list->task->task_state);
#endif
        if ((tmp_list->task->task_state == killed) || (tmp_list->task->task_state == removed))
        {
            if (killed == (tmp_list->task->task_state))
            {
                /* add killed task to list */
                kill_task = create_node(sim_task_kill_t);
                kill_task->name = strdup(tmp_list->task->name);
                kill_task->task_kill_time = tick;
                kill_task->task_num = tmp_list->task->task_num;
                create_and_add_node_to_list(&killed_tasks, kill_task, sim_task_kill_t);

                /* iterate through tasks and remove task */
                tmp_task_tasks_list = tasks;
                tmp_prev_tasks_list = tasks;
                while (NULL != tmp_task_tasks_list)
                {
                    if (0 == strcmp(tmp_task_tasks_list->task->name, tmp_list->task->name))
                    {
                        unlock_resources(tmp_task_tasks_list->task);
                        if (tmp_list->task->t_elapsed > tmp_list->task->stats->t_elapsed)
                        {
                            tmp_list->task->stats->t_elapsed = tmp_list->task->t_elapsed;
                        }
                        remove_node_from_list(&tasks, tmp_task_tasks_list, &tmp_prev_tasks_list);
                    }
                    tmp_prev_tasks_list = tmp_task_tasks_list;
                    tmp_task_tasks_list = tmp_task_tasks_list->next;
                }
            }
            unlock_resources(tmp_list->task);
            if (tmp_list->task->t_elapsed > tmp_list->task->stats->t_elapsed)
            {
                tmp_list->task->stats->t_elapsed = tmp_list->task->t_elapsed;
            }
            remove_node_from_list(list, tmp_list, &tmp_prev_list);
        }
        tmp_prev_list = tmp_list;
        tmp_list = tmp_list->next;
    }
}

/**
 * @brief handle_reaction_time() iterates through all tasks in ready_queue
 *
 * @param running_task
 */
void handle_reaction_time(const sim_task *running_task)
{
    list_node *tmp_task_list = ready_queue;
    while (NULL != tmp_task_list)
    {
        tmp_task_list->task->t_elapsed++;
        if ((NULL != running_task) && (0 != strcmp(tmp_task_list->task->name, running_task->name)) &&
            (0 < tmp_task_list->task->t_e))
        {
            ++tmp_task_list->task->stats->t_wait;
        }
        tmp_task_list = tmp_task_list->next;
    }
}

/**
 * @brief handle_preemption() checks whether the next task [] the previous task.
 *
 * This happens when the remaining execution time of prev is above 0 and is not the same as next.
 * When prev is preempted it must not be blocked. If it is blocked there is no preemption.
 *
 * @param next is the actual task.
 * @param prev is the previous task in the queue.
 */
void handle_preemption(const sim_task *next, sim_task *prev)
{
    char buffer[STATUS_STRING_LENGTH];
    if ((NULL != next) && (NULL != prev) && (blocked != prev->task_state) && (removed != prev->task_state) && (killed != prev->task_state) && (prev != next) &&
        (prev->t_remaining_e > 0))
    {
        snprintf(buffer, STATUS_STRING_LENGTH, "[%s%d preempted]", prev->name, prev->task_num);
        add_status(&actions_status[0], buffer, &actions_status_index);
    }
}

/**
 * @brief sim_scheduler() is the main scheduler function.
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
 *  After its execution (which is determined by t_stop), sim_scheduler() calculates each maximum reaction time.
 *
 */
void sim_scheduler(sim_task *(*pick_next_task)(list_node *))
{
    sim_task *prev;
    sim_task *next;

    next = NULL;
    prev = NULL;
    printf("Time\tJob\t\tReady-Q\t\tBlocked-Q\t\tActions\n");
    for (tick = 0; tick <= t_stop; ++tick)
    {
        if (next != NULL)
        {
            ++next->stats->t_exec;
        }
        memset(actions_status, '\0', STATUS_STRING_LENGTH); /* reset action actions_status */
        handle_pending_queue();
        handle_ready_queue();
        handle_deadline_queues();

        pending_queue = NULL; /* reset pending queue */

        prev = next;
        do
        {
            next = pick_next_task(ready_queue);
            lock_resources(next);
        } while ((next != NULL) && (next->task_state == blocked));
        remove_task_if_finished_execution(next); /* tasks execution */
        handle_reaction_time(next);              /* tasks reactiontime */

        if (tick >= t_start)
        {
            /* Task */
            printf("%d\t", tick); /* tick & job */
            if (next != NULL)
            {
                --next->t_remaining_e;
                printf("%s%d", next->name, next->task_num);
            }
            else
            {
                printf("I");
            }
            /* Ready-Q */
            printf("\t\t");
            print_list(ready_queue); /* print ready queue */

            /* Blocked-Q */
            printf("\t\t");
            print_list(blocked_queue); /* print blocked queue */
        }

        /* Actions */
        handle_preemption(next, prev);
        handle_tasks_to_delete(&ready_queue);   /* remove all tasks marked with killed or removed flag */
        handle_tasks_to_delete(&blocked_queue); /* remove all tasks marked with killed or removed flag */
        add_tasks_from_blocked_queue_to_ready_queue(pick_next_task);
        if (tick >= t_start)
        {
            printf("\t\t\t");
            print_status(resource_status, resource_status_index);
            print_status(actions_status, actions_status_index);
            printf("\n");
        }
        /* remove tasks from ready_queue and blocked_queue if deadline is exceeded */
        resource_status_index = 0;
        actions_status_index = 0;
        unlock_resources_if_finished(next);
    }
    printf("-----------------------------------------------\n");

    /* print reaction time */
    print_list(stats);

    /* print all killed tasks from scheduler simulator */
    printf("Killed Jobs: ");
    print_list(killed_tasks);
    printf("\n");

    /* free all lists */
    free_all();
}

/**
 * @brief Extracts a number from a string. This is useful because strtol returns 0 when failed.
 * Then errno must be checked.
 * @param token is a reference to the string.
 * @return the number if successful or -1 if it failed.
 */
int get_number(const char *token)
{
    long number;
    char *endptr; /* endpointer for strtol error checking */

    errno = 0;
    number = strtol(token, &endptr, 10);
    if (((ERANGE == errno) && ((LONG_MAX == number) || (LONG_MIN == number))) ||
        ((0 != errno) && (0 == number)) || (endptr == token))
    {
        number = -1;
        errno = 0;
    }
    return (int)number;
}

/**
 * @brief Checks if a resource is already in resources list.
 * @param resc_name is the name of the resource.
 * @return is either true if it already exists or false if not.
 */
bool check_if_resource_exists(const char *resc_name)
{
    list_node *tmp = resources;
    while (NULL != tmp)
    {
        if (0 == strcmp(tmp->task_resource->name, resc_name))
        {
            return true;
        }
        tmp = tmp->next;
    }
    return false;
}

typedef enum state
{
    start_st,
    stop_st,
    task_st,
    resc_st,
    finished_st,
    failed_st,
    idle_st
} state;

/**
 * read_and_parse_input() reads and parses the input file.
 *
 * 1. it goes through each line
 * 2. it checks for Start & Stop
 * 3. if neither of those are found it must be at the table
 * 4. with the help of tokens, each string separated by whitespaces is parsed.
 *  Each successful parsed number from the table updates a token_counter and a state.
 *  If a number is not parsed successfully token_counter is set to -1 and the state to failed.
 *
 *  It can handle tables with and without resources.
 *
 * @param input_fp is the file path to the input file.
 */
void read_and_parse_input(const char *input_fp)
{
    FILE *fp; /* file pointer for input file */
    size_t line_length;
    const char *const delimiter = "\f\v\t\r ";
    char *line;        /* read line */
    char *saveptr1;    /* saveptr1 for strtok_r */
    const char *token; /* token for strtok_t */

    state line_state;
    state task_state;
    state resc_state;

    int task_token_counter;
    int tok_num_val;

    /* task_resource */
    int resc_token_counter;
    int resc_num;
    int resc_index;
    char *resc_name;
    unsigned int t_from;

    sim_task *task;
    sim_task_stats *stat;
    sim_task_resource *task_resc;
    sim_task_resource_stats *task_resc_stat;
    list_node *resc_list_task;

    if (NULL == input_fp)
    {
        fprintf(stderr, "read_and_parse_input: no file path provided.\n");
        exit(EXIT_FAILURE);
    }

    fp = fopen(input_fp, "r");
    if (NULL == fp)
    {
        fprintf(stderr, "simulator: %s.", input_fp);
        exit(EXIT_FAILURE);
    }

    /* initialize lists/queues */
    tasks = NULL;
    stats = NULL;
    pending_queue = NULL;
    ready_queue = NULL;
    killed_tasks = NULL;
    blocked_queue = NULL;
    resources = NULL;

    line = 0;

    line_length = 0;
    while (getline(&line, &line_length, fp) != -1)
    {
        resc_token_counter = 0;
        task_token_counter = 0;
        resc_index = 0;
        resc_num = -1;
        line_state = idle_st;
        task_state = idle_st;
        resc_state = idle_st;

        /* task_resource */
        resc_name = NULL;
        t_from = 0;

        task = create_node(sim_task_t);
        task->name = NULL;
        token = strtok_r(line, delimiter, &saveptr1);
        while (token != NULL)
        {
            if (line_state == idle_st)
            {
                /* Start & Stop */
                if (0 == strcmp(token, "Start"))
                {
                    line_state = start_st;
                }
                else if (0 == strcmp(token, "Stop"))
                {
                    line_state = stop_st;
                }
                else
                {
                    line_state = task_st;
                }
            }

            tok_num_val = get_number(token);
            if ((line_state == start_st) && (-1 != tok_num_val))
            {
                t_start = tok_num_val;
                line_state = idle_st;
                task_state = idle_st;
                resc_state = idle_st;
            }
            else if ((line_state == stop_st) && (-1 != tok_num_val))
            {
                t_stop = tok_num_val;
                line_state = idle_st;
                task_state = idle_st;
                resc_state = idle_st;
            }
            else if ((line_state == task_st) && (task_state != failed_st))
            {
                if (0 == task_token_counter)
                {
                    task->name = strdup(token);
                    task_token_counter = 1;
                }
                else
                {
                    if (-1 == tok_num_val)
                    {
                        task_state = failed_st;
                        task_token_counter = -1;
                    }
                    switch (task_token_counter)
                    {
                    case 1:
                        task->t_ph = tok_num_val;
                        task_token_counter = 2;
                        break;
                    case 2:
                        task->t_per = tok_num_val;
                        task_token_counter = 3;
                        break;
                    case 3:
                        task->t_e = tok_num_val;
                        task->t_remaining_e = task->t_e;
                        task_token_counter = 4;
                        break;
                    case 4:
                        task->t_d = tok_num_val;
                        task->t_remaining_d = task->t_d;
                        task_token_counter = 5;
                        task_state = finished_st;
                        line_state = resc_st;
                        break;
                    default:
                        task_token_counter = -1;
                        task_state = failed_st;
                        break;
                    }
                }
            }
            else if ((line_state == resc_st) && (resc_state != failed_st))
            {
                if ((resc_num == -1) && (-1 != tok_num_val))
                {
                    resc_num = tok_num_val;
                    resc_token_counter = 0;
                }
                else
                {
                    if (0 == resc_token_counter)
                    {
                        resc_name = strdup(token);
                        resc_token_counter = 1;
                    }
                    else if (1 == resc_token_counter)
                    {
                        if (tok_num_val == -1)
                        {
                            resc_state = failed_st;
                        }
                        t_from = tok_num_val;
                        resc_token_counter = 2;
                    }
                    else
                    {
                        if (-1 == tok_num_val)
                        {
                            resc_state = failed_st;
                        }
                        ++resc_index;
                        if (!check_if_resource_exists(resc_name))
                        {
                            task_resc = create_node(sim_task_resource_t);
                            if (resc_name != NULL)
                            {
                                task_resc->name = strdup(resc_name);
                            }
                            task_resc->used_by = NULL;
                            create_and_add_node_to_list(&resources, task_resc, sim_task_resource_t);
                        }

                        task_resc_stat = create_node(sim_task_resource_stats_t);
                        task_resc_stat->name = resc_name;
                        task_resc_stat->t_from = t_from;
                        task_resc_stat->t_from_elapsed = 0;
                        task_resc_stat->t_for = tok_num_val;
                        task_resc_stat->t_for_elapsed = 0;

                        resc_token_counter = 0;
                        create_and_add_node_to_list(&resc_list_task, task_resc_stat, sim_task_resource_stats_t);

                        if (resc_index <= resc_num)
                        {
                            task_state = finished_st;
                            resc_state = finished_st;
                        }
                    }
                }
            }
            token = strtok_r(NULL, delimiter, &saveptr1);
        }

        if (task_state == finished_st)
        {
            stat = create_node(sim_task_stats_t);
            stat->name = strdup(task->name);
            task->stats = stat;
            task->resources = resc_list_task;
            task->task_state = created;

            create_and_add_node_to_list(&stats, stat, sim_task_stats_t);
            create_and_add_node_to_list(&tasks, task, sim_task_t);
            resc_list_task = NULL;
        }
        else
        {
            if (task->name != NULL)
            {
                free(task->name);
            }
            free(task);
        }
    }
    free(line);
    fclose(fp);
}

/**
 * @brief the main function prepares everything for the simulation.
 *
 * following tasks are handled by this function:
 * - it parses the flags and makes sure there is no wrong combination
 * - it calls the right scheduler based on the flags
 *
 * @param argc is the argument counter that gives information about how many arguments are passed
 * @param argv is the argument vector that holds all passed arguments
 * @return zero, if execution was successful.
 */
int main(int argc, char **argv)
{
    int arg_index;
    const char *input_fp; /* input file path */

    /* scheduler simulator flags */
    const char *nokill_string;
    const char *kill_string;
    const char *dm_string;
    const char *edf_string;

    sim_task *(*scheduler)(list_node *);

    input_fp = NULL;
    if (argc > 1)
    {
        nokill_string = (char const *)"-nokill";
        kill_string = (char const *)"-kill";
        dm_string = (char const *)"-dm";
        edf_string = (char const *)"-edf";

        /* flags */
        for (arg_index = 1; arg_index < argc; ++arg_index)
        {
            if (0 == strcmp(argv[arg_index], nokill_string))
            {
                flag_nokill = true;
            }
            else if (0 == strcmp(argv[arg_index], kill_string))
            {
                flag_kill = true;
            }
            else if (0 == strcmp(argv[arg_index], dm_string))
            {
                flag_dm = true;
            }
            else if (0 == strcmp(argv[arg_index], edf_string))
            {
                flag_edf = true;
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
        fprintf(stderr, "simulator: cannot have both nokill_string and kill_string flags.\n");
        exit(EXIT_FAILURE);
    }

    if (NULL == input_fp)
    {
        fprintf(stderr, "simulator: please provide an input file.\n");
        exit(EXIT_FAILURE);
    }

    if (flag_dm)
    {
        scheduler = &pick_next_task_dm;
    }
    else if (flag_edf)
    {
        scheduler = &pick_next_task_edf;
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
