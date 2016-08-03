// Author: Geomar Manzano

#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

struct Queue_Element {
    char *string;
    struct Queue_Element *next;
};

struct Queue {
    struct Queue_Element *front;
    struct Queue_Element *rear;
    size_t queueSize;
};

struct Queue *Queue();
char *copyStringQueue(char *str);
void enqueue(struct Queue *queue, char *valueA);
void dequeue(struct Queue *queue);
struct Queue_Element *front(struct Queue *queue);
bool isQueueEmpty(struct Queue *queue);
void clearQueue(struct Queue *queue);
void deleteQueue(struct Queue **queue);

#endif
