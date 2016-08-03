// Author: Geomar Manzano

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include "queue.h"

static const char *OPTIONS[] = {
    "--append",
    "--cloexec",
    "--creat",
    "--directory",
    "--dsync",
    "--excl",
    "--nofollow",
    "--nonblock",
    "--rsync",
    "--sync",
    "--trunc",
    "--rdonly",
    "--rdwr",
    "--wronly",
    "--pipe",
    "--command",
    "--wait",
    "--verbose",
    "--profile",
    "--close",
    "--abort",
    "--catch",
    "--ignore",
    "--default",
    "--pause",
    "--exit"
};

static const int OPTIONS_SIZE = sizeof(OPTIONS)/sizeof(OPTIONS[0]);

int convert(char *option);
bool isOption(const char *str);
bool isOptionFileMode(char *arg);
bool isOptionFileFlag(char *arg);
int flags(int filemode, struct Queue *fileFlags);
int numArgsCmd(int argc, char **argv,  int i);
char **construct_cmd_argList(int argc, char **argv, int i);

#endif
