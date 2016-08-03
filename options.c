// Author: Geomar Manzano

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include "options.h"
#include "queue.h"

int convert(char *option) {
    if (!strcmp(option, "--rdonly"))
        return O_RDONLY;
    else if (!strcmp(option, "--wronly"))
        return O_WRONLY;
    else if (!strcmp(option, "--rdwr"))
        return O_RDWR;
    else if (!strcmp(option, "--append"))
        return O_APPEND;
    else if (!strcmp(option, "--cloexec"))
        return O_CLOEXEC;
    else if (!strcmp(option, "--creat"))
        return O_CREAT;
    else if (!strcmp(option, "--directory"))
        return O_DIRECTORY;
    else if (!strcmp(option, "--dsync"))
        return O_DSYNC;
    else if (!strcmp(option, "--excl"))
        return O_EXCL;
    else if (!strcmp(option, "--nofollow"))
        return O_NOFOLLOW;
    else if (!strcmp(option, "--nonblock"))
        return O_NONBLOCK;
    else if (!strcmp(option, "--rsync"))
        return O_RSYNC;
    else if (!strcmp(option, "--sync"))
        return O_SYNC;
    else if (!strcmp(option, "--trunc"))
        return O_TRUNC;
    else return -1;
}

bool isOption(const char *str) {
    int i;
    for (i = 0; i < OPTIONS_SIZE; i++)
        if (!strcmp(str, OPTIONS[i])) 
            return true;
    return false;
}

bool isOptionFileMode(char *arg) {
    if (!(strcmp(arg, "--rdonly") &&
          strcmp(arg, "--wronly") &&
          strcmp(arg, "--rdwr") &&
          strcmp(arg, "--pipe"))) return true;
    else return false;
}

bool isOptionFileFlag(char *arg) {
    if (!(strcmp(arg, "--append")    &&
          strcmp(arg, "--cloexec")   &&
          strcmp(arg, "--creat")     &&
          strcmp(arg, "--directory") &&
          strcmp(arg, "--dsync")     &&
          strcmp(arg, "--excl")      &&
          strcmp(arg, "--nofollow")  &&
          strcmp(arg, "--nonblock")  &&
          strcmp(arg, "--rsync")     &&
          strcmp(arg, "--sync")      &&
          strcmp(arg, "--trunc"))) return true;
    else return false; 
}

int flags(int filemode, struct Queue *fileFlags) {
    int join = filemode;
    struct Queue_Element *element;
    char *flag_str;
    int flag_int;
    
    while (!isQueueEmpty(fileFlags)) {
        element = front(fileFlags);
        flag_str = element->string;
        flag_int = convert(flag_str);
        join = join | flag_int;
        dequeue(fileFlags);
    }
    return join;
}

int numArgsCmd(int argc, char **argv,  int i) {
    int numArgs = 0;
    while (i < argc) {
        if (isOption(argv[i]))
            break;
        numArgs += 1;
        i += 1;
    }
    return numArgs;
}

char** construct_cmd_argList(int argc, char **argv, int i) {
    int argSize = numArgsCmd(argc, argv, i);
    char **argsList = malloc(sizeof(char*) * (argSize + 2)); 
    argsList[0] = argv[i - 1]; 
    
    int j;
    for (j = 0; j < argSize; j++)
        argsList[j + 1] = argv[i + j];
    argsList[argSize + 1] = NULL;
    return argsList;
}
