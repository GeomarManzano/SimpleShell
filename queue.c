// Author: Geomar Manzano

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "queue.h"

struct Queue *Queue() {
    struct Queue *queue = malloc(sizeof * queue);
    if (queue) {
        queue->front = NULL;
        queue->rear = NULL;
        queue->queueSize = 0;
    }
    return queue;
}

char *copyStringQueue(char *str) {
    char *tmp = malloc(strlen(str) + 1);
    if (tmp) strcpy(tmp, str);
    return tmp;
}

void enqueue(struct Queue *queue, char *valueA) {
    struct Queue_Element *element = malloc(sizeof * element);
    element->string = copyStringQueue(valueA);
    element->next = NULL;
    queue->queueSize++;
    
    if (queue->front == NULL && queue->rear == NULL) {
        queue->front = queue->rear = element;
        return;
    }
    
    queue->rear->next = element;
    queue->rear = element;
}

void dequeue(struct Queue *queue) {
    struct Queue_Element *tmp = queue->front;
    if (tmp == NULL)
        return;
    else if (queue->front == queue->rear)
        queue->front = queue->rear = NULL;
    else queue->front = queue->front->next;
    free(tmp);
    queue->queueSize--;
}

struct Queue_Element *front(struct Queue *queue) {
    if (queue->front == NULL) return NULL;
    else return queue->front;
    printf("Queue_Element Function front: %s\n", queue->front->string);
}

bool isQueueEmpty(struct Queue *queue) {
    if (queue->front == NULL && queue->rear == NULL) return true;
    else return false;
}

void clearQueue(struct Queue *queue) {
    while (queue->front != NULL && queue->rear != NULL)
        dequeue(queue);
}

void deleteQueue(struct Queue **queue) {
    clearQueue(*queue);
    free(*queue);
    *queue = NULL;
}
