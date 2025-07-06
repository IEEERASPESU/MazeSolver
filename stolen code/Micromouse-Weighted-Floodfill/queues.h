#ifndef QUEUES_H
#define QUEUES_H

#include "solver.h"

typedef neighbor item_type;
typedef struct _queue* queue;
queue queue_create();
void queue_destroy(queue q);
void queue_push(queue q, item_type elem);
item_type queue_pop(queue q);
item_type queue_first(queue q);
int queue_is_empty(queue q);
int queue_size(queue q);
void queue_clear(queue q);

// queue type implementation

struct node {
    item_type data;
    struct node* next;
};

#endif