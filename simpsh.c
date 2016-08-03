// Author: Geomar Manzano

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "stack.h"
#include "queue.h"
#include "handler.h"
#include "options.h"

int main(int argc, char **argv) {
    // Interactive Mode:    simpsh
    // Interactive Mode:    simpsh --interact
    // Noninteractive Mode: simpsh [option] ...
    if ((argc == 1) || (argc == 2 && !strcmp(argv[1], "--interact")))
        interactive();
    else noninteractive(argc, argv);
    return 0;
}
