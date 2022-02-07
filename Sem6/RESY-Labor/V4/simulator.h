#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define LIVE_TASK 0   /*!< Default value. Nothing happens here */
#define REMOVE_TASK 1 /*!< Removes the task from ready_queue */
#define KILL_TASK 2   /*!< Removes the task from task list and thus prevents execution of new instances */

struct sim_task_struct
{
    char *name;                   /*!< Name of the task */
    int task_num;                 /*!< Number of instance.\n Tasks can have multiple instances that are differentiated via task_num */
    unsigned int t_ph;            /*!< Phase */
    unsigned int t_per;           /*!< Period */
    unsigned int t_e;             /*!< Execution time */
    unsigned int t_remaining_e;   /*!< Remaining execution time */
    unsigned int t_d;             /*!< Deadline */
    unsigned int t_remaining_d;   /*!< Remaining deadline */
    unsigned int t_elapsed;       /*!< Elapsed time a job. Used to calculate max. reaction time */
    unsigned int kill_flag;       /*!< Flag used to differentiate between different kill modes */
    struct sim_task_stats *stats; /*!< Stats to determine reaction time */
    struct sim_task_struct *next; /*!< Next task.\n All tasks are linked to each other */
};

struct sim_task_stats
{
    char *name;                  /*!< Name of task */
    unsigned int t_elapsed;      /*!< Elapsed time of a task. Used to calculate max. reaction time */
    struct sim_task_stats *next; /*!< Next stat.\n This is useful when printing all the stats */
};

/**
 * \brief print_list() prints linked list of type struct sim_task_struct by
 * iterating through it.
 *
 * @param head is the first element of the list.
 */
void print_list(struct sim_task_struct *head)
{
    struct sim_task_struct *tmp;
    tmp = head;
    while (tmp != NULL)
    {
        if (tmp == tmp->next)
        {
            fprintf(stderr, "simulator: Self assigned next pointer. Exiting to avoid recursion.\n");
            exit(-1);
        }
        printf("%s%d", tmp->name, tmp->task_num);
        tmp = tmp->next;
    }
}

/**
 * \brief create_copy() creates a deep copy of a task.
 *
 * @param dst is the destination task. It must be already allocated!
 * @param src is the source task.
 */
void create_copy(struct sim_task_struct *dst, const struct sim_task_struct *src)
{
#ifdef DEBUG
    printf("simulator: creating copy\n");
#endif
    memcpy(dst, src, sizeof(struct sim_task_struct));
    dst->name = strdup(src->name);
    if (src->next != NULL)
    {
        *dst->next = *src->next;
    }
}

/**
 * \brief copy_list() creates a deep copy of a list using create_copy().
 *
 * @param dst is the destination list.\n It must be already allocated!
 * @param src is the source list.
 */
void copy_list(struct sim_task_struct *dst, struct sim_task_struct *src)
{
    struct sim_task_struct *tmp_task_list;
    struct sim_task_struct *tmp_task;
    struct sim_task_struct **tmp_head; /* pointer to head of dst */

#ifdef DEBUG
    printf("simulator: copying list\n");
#endif

    /**
     * error checking:
     * check if dst or src are NULL to avoid segmentation fault.
     */
    if (dst == NULL || src == NULL)
    {
        return;
    }

    create_copy(dst, src);
    tmp_task_list = src->next;
    tmp_head = &dst;

    while (tmp_task_list != NULL)
    {

#ifdef DEBUG
        printf("copy_list: copying task %s\n", tmp_task_list->name);
#endif

        tmp_task = (struct sim_task_struct *)malloc(sizeof(struct sim_task_struct));
        if (tmp_task == NULL)
        {
            fprintf(stderr, "copy_list: malloc failed.\n");
            exit(-1);
        }
        create_copy(tmp_task, tmp_task_list);

        (*tmp_head)->next = tmp_task;
        tmp_head = &((*tmp_head)->next); /* move head to new element */

        tmp_task_list = tmp_task_list->next; /* select next element */
    }
}

/**
 * \brief goto_last_list_element() iterates through the list and returns a reference to the last element.
 *
 * This function is used to insert the tasks correctly into the different queues and lists.
 *
 * @param head is the start of list.
 * @return the last element of list.
 */
struct sim_task_struct *goto_last_list_element(struct sim_task_struct *head)
{
    if (head == NULL)
    {
        return NULL;
    }
    struct sim_task_struct *tmp;
    tmp = head;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    return tmp;
}

/**
 * \brief free_task_list() frees all entries of a task list to avoid memory leak.
 *
 * @param head is the first element of the list.
 */
void free_task_list(struct sim_task_struct *head)
{
    struct sim_task_struct *tmp;
#ifdef DEBUG
    printf("simulator: freeing list\n");
#endif
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
#ifdef DEBUG
        printf("free_task_list: freeing %s\n", tmp->name);
#endif
        free(tmp);
    }
}

/**
 * \brief free_stat_list() frees all entries of a stat list to avoid memory leak.
 *
 * @param head is the first element of the list.
 */
void free_stat_list(struct sim_task_stats *head)
{
    struct sim_task_stats *tmp;
#ifdef DEBUG
    printf("simulator: freeing list\n");
#endif
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
#ifdef DEBUG
        printf("free_stat_list: freeing %s\n", tmp->name);
#endif
        free(tmp);
    }
}

#endif
