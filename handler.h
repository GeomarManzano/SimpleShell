// Author: Geomar Manzano

#ifndef HANDLER_H
#define HANDLER_H

#include <stdbool.h>
#include "stack.h"
#include "queue.h"

#define PROTECTION_MODE 0777

long int stringToInt(char *str);
char *readline_get(char *prompt);
char **tokenize_string(char *line, char *delimiter, int *result_size);
void handleFileFlag(char *flag, struct Queue *fileFlags, int *index);
void handleFileMode(char *filemode, char *filename, struct Queue *fileFlags,
                    struct Stack *file_fd, int *index);
void handleCommand(int argc, char **argv, int *index,
                   struct Stack *file_fd, bool verbose_flag, bool wait_flag);
void handleVerboseFile(int argc, char **argv,
                       struct Queue *fileFlags, const int *index);
void handleVerboseSubCmd(int argc, char **argv, const int *index);
void handleProfile(int *index);
void handleClose(int fd, int *index);
void handleAbort(int *index);
void handlePause(int *index);
void sig_catch(int signo);
void handleCatch(int signo, int *index);
void handleIgnore(int signo, int *index);
void handleDefault(int signo, int *index);
void handleError(char **argv, int *index);
void manipVerbose(bool *verbose_flag, int *index);
void manipWait(bool *wait_flag, int *index);
void handleSubCmd(int argc, char **argv, int *index,
                  struct Stack *file_fd, bool *verbose_flag, bool *wait_flag);
void interactive(void);
void noninteractive(int argc, char **argv);
void handleExit(void);
void sig_catch_interrupt(int signo);
void profiler(int argc, char **argv, int *index);
#endif
