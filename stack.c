// Author: Geomar Manzano

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "stack.h"

struct Stack *Stack() {
    struct Stack *stack = malloc(sizeof * stack);
    if (stack) {
        stack->head = NULL;
        stack->stackSize = 0;
    }
    return stack;
}

char *copyString(char *str) {
    char *tmp = malloc(strlen(str) + 1);
    if (tmp) strcpy(tmp, str);
    return tmp;
}

void push(struct Stack *stack, char *valueA, int valueB) {
    struct Stack_Element *element = malloc(sizeof * element);
    if (element) {
        element->string = copyString(valueA);
        element->num = valueB;
        element->next = stack->head;
        stack->head = element;
        stack->stackSize++;
    } else {
        // Error handling
    }
}

void pop(struct Stack *stack) {
    if (stack->head != NULL) {
        struct Stack_Element *tmp = stack->head;
        stack->head = stack->head->next;
        free(tmp->string);
        free(tmp);
        stack->stackSize--;
    }
}

struct Stack_Element *top(struct Stack *stack) {
    if (stack && stack->head)
        return stack->head;
    else return NULL;
}

bool isEmpty(struct Stack *stack) {
    if (stack->head == NULL) return true;
    else return false;
}

void clear(struct Stack *stack) {
    while (stack->head != NULL)
        pop(stack);
}

void delete(struct Stack **stack) {
    clear(*stack);
    free(*stack);
    *stack = NULL;
}

bool isNumInStack(struct Stack *stack, int n) {
    bool in = false;
    struct Stack *tmp = Stack();
    struct Stack_Element *element;

    while (!isEmpty(stack)) {
        element = top(stack);
        if (element->num == n)
            in = true;
        push(tmp, element->string, element->num);
        pop(stack);
    }

    clear(stack);

    while (!isEmpty(tmp)) {
        element = top(tmp);
        push(stack, element->string, element->num);
        pop(tmp);
    }
    return in;
}

bool isPairInStack(struct Stack *stack, char *valueA, int valueB) {
    bool in = false;
    struct Stack *tmp = Stack();
    struct Stack_Element *element;

    while (!isEmpty(stack)) {
        element = top(stack);
        if (!strcmp(element->string, valueA) && element->num == valueB)
            in = true;
        push(tmp, element->string, element->num);
        pop(stack);
    }

    clear(stack);

    while (!isEmpty(tmp)) {
        element = top(tmp);
        push(stack, element->string, element->num);
        pop(tmp);
    }
    return in;    
}
