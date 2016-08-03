// Author: Geomar Manzano

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

struct Stack_Element {
    char *string;
    int num;
    struct Stack_Element *next;
};

struct Stack {
    struct Stack_Element *head;
    size_t stackSize;
};

struct Stack *Stack();
char *copyString(char *str);
void push(struct Stack *stack, char *valueA, int valueB);
void pop(struct Stack *stack);
struct Stack_Element *top(struct Stack *stack);
bool isEmpty(struct Stack *stack);
void clear(struct Stack *stack);
void delete(struct Stack **stack);
bool isNumInStack(struct Stack *stack, int n);
bool isPairInStack(struct Stack *stack, char *valueA, int valueB);

#endif
