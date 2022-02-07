#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum task_state
{
    ready,
    created,
    blocked,
    removed,
    killed
} task_state;

typedef enum node_type
{
    node_t,
    sim_task_t,
    sim_task_resource_t,
    sim_task_resource_stats_t,
    sim_task_stats_t,
    sim_task_kill_t
} node_type;

typedef struct sim_task_resource
{
    char *name;
    struct sim_task *used_by;
} sim_task_resource;

typedef struct sim_task_resource_stats
{
    char *name;
    unsigned int t_from;
    unsigned int t_from_elapsed;
    unsigned int t_for;
    unsigned int t_for_elapsed;
} sim_task_resource_stats;

typedef struct sim_task_stats
{
    char *name;             /*!< Name of task */
    unsigned int t_wait;    /*!< \deprecated Wait time of all instances of task */
    unsigned int t_exec;    /*!< \deprecated Execution time of all instances of task */
    unsigned int t_elapsed; /*!< Elapsed time */
} sim_task_stats;

typedef struct sim_task_kill
{
    char *name;                  /*!< Name of killed task */
    int task_num;                /*!< Number of killed task instance. */
    unsigned int task_kill_time; /*!< Tick when task was killed. */
} sim_task_kill;

typedef struct sim_task
{
    char *name;                 /*!< Name of the task */
    int task_num;               /*!< Number of task instance.\n Tasks can have multiple instances that are differentiated via task_num */
    unsigned int t_ph;          /*!< Phase */
    unsigned int t_per;         /*!< Period */
    unsigned int t_e;           /*!< Execution time */
    unsigned int t_remaining_e; /*!< Remaining execution time */
    unsigned int t_d;           /*!< Deadline */
    unsigned int t_remaining_d; /*!< Remaining deadline */
    unsigned int t_elapsed;     /*!< Elapsed time */
    task_state task_state;      /*!< Flag used to differentiate between different task states */
    char *blocked_by_resc;      /*!< \idea Maybe a pointer to resource struct */
    struct list_node *resources;
    struct sim_task_stats *stats; /*!< Stats to determine reaction time */
} sim_task;

typedef struct list_node
{
    node_type node_type;
    union
    {
        sim_task *task;
        sim_task_resource *task_resource;
        sim_task_stats *task_stats;
        sim_task_resource_stats *task_resource_stats;
        sim_task_kill *task_kill;
        struct list_node *list_node;
    };
    struct list_node *next;
} list_node;

static list_node *nodes = NULL; /*!< Global list of nodes. Every created node is also added here. This avoids unwanted memory leaks or SIGSEGSs. */

void *copy_node(void *src, node_type node_type);

/**
 * @brief Iterates through task list/queue and executes for each
 * task a function passed via reference.
 *
 * @param head is the start of the queue.
 * @param fct is the function that is executed for each task.
 */
void iterator(list_node *head, void (*fct)(list_node *))
{
    list_node *tmp_task = head;
    while (NULL != tmp_task)
    {
        if (tmp_task == tmp_task->next)
        {
            fprintf(stderr, "iterator: Self assigned next pointer. Exiting to avoid recursion.\n");
            exit(-1);
        }
        fct(tmp_task);
        tmp_task = tmp_task->next;
    }
}

/**
 * @brief Prints task.
 * @param head is a reference to the task.
 */
void print_task(list_node *head)
{
    if ((NULL != head) && (NULL != head->task))
    {
        printf("%s%d", head->task->name, head->task->task_num);
    }
}

/**
 * @brief Prints task stats.
 * @param head is a reference to the task stats.
 */
void print_task_stats(list_node *head)
{
    if ((NULL != head) && (NULL != head->task_stats))
    {
        printf("Task: %s\tmax. Reaktionszeit: %d\n", head->task_stats->name, head->task_stats->t_elapsed);
    }
}

/**
 * @brief Prints killed task.
 * @param head is a reference to the killed task.
 */
void print_killed_task(list_node *head)
{
    if ((NULL != head) && (NULL != head->task_kill))
    {
        printf("%s%d(t:%d)", head->task_kill->name, head->task_kill->task_num, head->task_kill->task_kill_time);
        if (NULL != head->next)
        {
            printf(", ");
        }
    }
}

/**
 * @brief Prints linked list of type sim_task by iterating through it.
 * @param head is the first element of the list.
 */
void print_list(list_node *head)
{
    if (NULL != head)
    {
        switch (head->node_type)
        {
        case sim_task_stats_t:
            iterator(head, &print_task_stats);
            break;
        case sim_task_kill_t:
            iterator(head, &print_killed_task);
            break;
        default:
            iterator(head, &print_task);
            break;
        }
    }
}

/**
 * @brief Creates a new node of a specific type. Can be seen as a constructor.
 *
 * @param node_type is the type of the node that should be created.
 * @return a reference to the new node.
 */
void *create_node(node_type node_type)
{
    void *node;
    size_t malloc_size;
    switch (node_type)
    {
    case sim_task_t:
        malloc_size = sizeof(sim_task);
        break;
    case sim_task_resource_t:
        malloc_size = sizeof(sim_task_resource);
        break;
    case sim_task_stats_t:
        malloc_size = sizeof(sim_task_stats);
        break;
    case sim_task_kill_t:
        malloc_size = sizeof(sim_task_kill);
        break;
    case sim_task_resource_stats_t:
        malloc_size = sizeof(sim_task_resource_stats);
        break;
    default:
        malloc_size = sizeof(list_node);
        break;
    }

    if (NULL == (node = malloc(malloc_size)))
    {
        fprintf(stderr, "create_node: malloc failed.\n");
        exit(-1);
    }
    return node;
}

/**
 * @brief Adds a node the end of a list.
 *
 * @param list is a pointer to the start of the list. It traverses through the list till it finds the last node.
 * @param new_node is the node that should be added.
 */
void add_node_to_list(list_node **list, list_node *new_node)
{
    if (new_node != NULL)
    {
        new_node->next = NULL;
    }

#ifdef DEBUG
    if (new_node == NULL)
    {
        printf("WARNING: add_node_to_list: new_node is NULL!\n");
    }
#endif

    list_node *tmp_node;
    if (NULL == *list)
    {
        (*list) = new_node;
    }
    else
    {
        tmp_node = *list;
        while (NULL != tmp_node->next)
        {
            if (tmp_node == tmp_node->next)
            {
                fprintf(stderr,
                        "create_and_add_node_to_list: Self assigned next pointer. Exiting to avoid recursion.\n");
                exit(-1);
            }
            tmp_node = tmp_node->next;
        }
        tmp_node->next = new_node;
    }
}

/**
 * @brief Creates a new node as a wrapper for a struct and adds it to the end of a list.
 *
 * @param task is a reference to the task that has to be added. It is wrapped in `struct list_node`. Must not be NULL!
 * @param list pointer to list.
 */
void create_and_add_node_to_list(list_node **list, void *node, node_type node_type)
{
    list_node *new_node;
    list_node *new_node_of_node;

    if (NULL != node)
    {
        new_node = create_node(node_t);
        new_node->node_type = node_type;
        new_node->next = NULL;

        switch (node_type)
        {
        case sim_task_kill_t:
            new_node->task_kill = (sim_task_kill *)node;
            break;
        case sim_task_resource_t:
            new_node->task_resource = (sim_task_resource *)node;
            break;
        case sim_task_resource_stats_t:
            new_node->task_resource_stats = (sim_task_resource_stats *)node;
            break;
        case sim_task_stats_t:
            new_node->task_stats = (sim_task_stats *)node;
            break;
        default:
            new_node->task = (sim_task *)node;
            break;
        }
        /* add new_node to list with all created nodes. This allows later easy memory management. */
        new_node_of_node = create_node(node_t);
        new_node_of_node->node_type = node_t;
        new_node_of_node->list_node = new_node;

        add_node_to_list(&nodes, new_node_of_node);
        add_node_to_list(list, new_node);
    }
}

/**
 * @brief Creates a deep copy of a list.
 *
 * @param src is the source task.
 * @return a reference to the first element of the list.
 */
list_node *copy_list(list_node *src)
{
    void *node;
    list_node *copy_list = NULL;
    list_node *tmp_src = src;
    while (tmp_src != NULL)
    {
        switch (tmp_src->node_type)
        {
        case node_t:
            node = tmp_src->list_node;
            break;
        case sim_task_t:
            node = tmp_src->task;
            break;
        case sim_task_resource_t:
            node = tmp_src->task_resource;
            break;
        case sim_task_resource_stats_t:
            node = tmp_src->task_resource_stats;
            break;
        case sim_task_stats_t:
            node = tmp_src->task_stats;
            break;
        case sim_task_kill_t:
            node = tmp_src->task_kill;
            break;
        }
        create_and_add_node_to_list(&copy_list, copy_node(node, tmp_src->node_type), tmp_src->node_type);
        tmp_src = tmp_src->next;
    }
    return copy_list;
}

/**
 * @brief Creates a deep copy of a node.
 *
 * @param src is the source task.
 * @return a reference to the task.
 */
void *copy_node(void *src, node_type node_type)
{
    sim_task *tmp_task;
    sim_task_resource_stats *tmp_resource_stats;

    if (src == NULL)
    {
        return NULL;
    }

    switch (node_type)
    {
    case sim_task_t:
        tmp_task = create_node(sim_task_t);
        memcpy(tmp_task, ((sim_task *)src), sizeof(sim_task));
        tmp_task->name = strdup(((sim_task *)src)->name);
        if (((sim_task *)src)->blocked_by_resc != NULL)
        {
            tmp_task->blocked_by_resc = strdup(((sim_task *)src)->blocked_by_resc); /* ? */
        }
        tmp_task->resources = copy_list(((sim_task *)src)->resources);
        return (void *)tmp_task;
    case sim_task_resource_stats_t:
        tmp_resource_stats = create_node(sim_task_resource_stats_t);
        memcpy(tmp_resource_stats, ((sim_task_resource_stats *)src), sizeof(sim_task_resource_stats));
        tmp_resource_stats->name = strdup(((sim_task_resource_stats *)src)->name);
        return (void *)tmp_resource_stats;
    default:
        break;
    }
    return NULL;
}

/**
 * @brief Frees a node of type list_node.
 * If list_node has a reference to another list_node then both are freed.
 *
 * @param node is pointer to a node.
 */
void free_node(list_node *node)
{
    if (NULL != node)
    {
        switch (node->node_type)
        {
        case sim_task_t:
            free(node->task->name);
            free(node->task);
            break;
        case sim_task_stats_t:
            free(node->task_stats->name); /* strdup */
            free(node->task_stats);
            break;
        case sim_task_kill_t:
            free(node->task_kill->name);
            free(node->task_kill);
            break;
        case sim_task_resource_stats_t:
            free(node->task_resource_stats->name);
            free(node->task_resource_stats);
            break;
        case node_t:
            if (node == node->list_node)
            {
                fprintf(stderr, "free_node: recursive free. Aborting!\n");
                exit(-1);
            }
            free_node(node->list_node);
            break;
        default:
            break;
        }
        free(node);
    }
}

/**
 * @brief Removes a node from a list.
 * @param **list is a reference to the origin list.
 * @param *node_to_remove is a pointer of the node that should be removed.
 * @param **prev_node is the previous node in the list.
 */
void remove_node_from_list(list_node **list, list_node *node_to_remove, list_node **prev_node)
{
    if (*list == node_to_remove)
    {
        *list = node_to_remove->next;
        *prev_node = *list;
    }
    else
    {
        (*prev_node)->next = node_to_remove->next;
    }
}

/**
 * @brief Frees all entries of a task list to avoid memory leak.
 * @param head is the first element of the list.
 */
void free_list(list_node *head)
{
    list_node *tmp;
    while (NULL != (tmp = head))
    {
        head = head->next;
        free_node(tmp);
    }
}

/**
 * @brief call the free_list() function which frees all nodes created during runtime.
 * All nodes are collected in a global list.
 *
 * This decision was taken because freeing during execution may cause a lot of debugging work.
 */
void free_all()
{
    free_list(nodes);
}

#endif
