// Author: Geomar Manzano

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "handler.h"
#include "options.h"
#include "stack.h"
#include "queue.h"

const int BASE = 10;
static pid_t pid;
static char *line_read = (char *)NULL;
static bool p_flag = false;
static bool prof_flag = false;

long int stringToInt(char *str) {
    char *ptr;
    long int converted = strtol(str, &ptr, BASE);
    return converted;
}

char *readline_get(char *prompt) {
    if (line_read) {
        free(line_read);
        line_read = (char *)NULL;
    }

    // readline uses malloc and the last newline is removed    
    line_read = readline(prompt);

    // Add to history if the line has text
    if (line_read && *line_read)
        add_history(line_read);
    
    return line_read;
}

// Free the return value of this function after usage is done.
char **tokenize_string(char *line, char *delimiter, int *result_size) {
    char **result = NULL;
    char *p = strtok(line, delimiter);
    int total_delimiters = 0;

    // Split string and append tokens to result
    while (p) {
        result = realloc(result, sizeof(char*) * ++total_delimiters);
        if (result == NULL)
            exit(-1); // Memory allocation failure.
        result[total_delimiters - 1] = p;
        p = strtok(NULL, delimiter);
    }

    // Realloc one extra element for the last NULL
    result = realloc(result, sizeof(char*) * (total_delimiters + 1));
    result[total_delimiters] = 0;
    *result_size = total_delimiters;
    return result;
}

void handleFileFlag(char *flag, struct Queue *fileFlags, int *index) {
    *index += 1;
    enqueue(fileFlags, flag);
}

void handleFileMode(char *filemode, char *filename, struct Queue *fileFlags,
                    struct Stack *file_fd, int *index) {
    if (!strcmp(filemode, "--pipe")) {
        *index += 1;     
        int pipefd[2];
        int status = pipe(pipefd);
        
        if (status == -1) {
            fprintf(stderr, "[ERROR]: Unsuccessful pipe in handleFileMode.\n");
            return;
        }
        
        push(file_fd, "pipe_input", pipefd[0]);
        push(file_fd, "pipe_output", pipefd[1]);            
    } else {
        *index += 2;
        int fd = open(filename,
                      flags(convert(filemode), fileFlags),
                      PROTECTION_MODE);

        if (fd == -1) {
            fprintf(stderr, "[ERROR]: Invalid file (%s)\n", filename);
            clearQueue(fileFlags);
            return;
        }

        clearQueue(fileFlags);
        push(file_fd, filename, fd);
    }
}

void handleCommand(int argc, char **argv, int *index,
                   struct Stack *file_fd, bool verbose_flag, bool wait_flag) {
    int i = *index;
    long int in = stringToInt(argv[i + 1]);
    long int out = stringToInt(argv[i + 2]);
    long int err = stringToInt(argv[i + 3]);    
    bool invalid = false;
    
    if (!isNumInStack(file_fd, in)) {
        fprintf(stderr, "[ERROR]: Invalid file descriptor %ld\n", in);
        invalid = true;
    }
    if (!isNumInStack(file_fd, out)) {
        fprintf(stderr, "[ERROR]: Invalid file descriptor %ld\n", out);
        invalid = true;
    }
    if (!isNumInStack(file_fd, err)) {
        fprintf(stderr, "[ERROR]: Invalid file descriptor %ld\n", err);
        invalid = true;
    }

    if (invalid) {
        *index = numArgsCmd(argc, argv, i + 5) + i + 5;
        return;
    }
    
    char **cmd_args = construct_cmd_argList(argc, argv, i + 5);

    pid = fork();
    int status = 0;
    
    signal(SIGINT, &sig_catch_interrupt);

    if (pid == 0) {       
        dup2(in, STDIN_FILENO);
        dup2(out, STDOUT_FILENO);
        dup2(err, STDERR_FILENO);

        if (isPairInStack(file_fd, "pipe_output", out))
            close(out - 1);
        if (isPairInStack(file_fd, "pipe_input", in))
            close(in + 1);

        execvp(cmd_args[0], cmd_args);
        exit(EXIT_FAILURE);
    } else if (pid == -1)
        fprintf(stderr, "[ERROR]: Unsuccessful fork.\n");
    else {
        if (isPairInStack(file_fd, "pipe_output", out)) {
            close(out);
            waitpid(pid, &status, 0);
            p_flag = true;
        }
        else if (isPairInStack(file_fd, "pipe_input", in)) {
            close(in);
            if (!p_flag)
                waitpid(pid, &status, WNOHANG);
            else waitpid(pid, &status, 0);
        } else {
            waitpid(pid, &status, 0);
            rewind(fdopen(in, "r"));
        }

        if (prof_flag) {
            profiler(argc, argv, index);
        }
        
        *index = numArgsCmd(argc, argv, i + 5) + i + 5;

        if (wait_flag) {
            int argSize = numArgsCmd(argc, argv, i + 5) + 1;

            printf("%d ", status);

            int j;
            for (j = 0; j < argSize + 1; j++) {
                if (cmd_args[j] == NULL) break;
                else printf("%s ", cmd_args[j]);
            }
            printf("\n");
        }
    }
}

void handleVerboseFile(int argc, char **argv,
                       struct Queue *fileFlags, const int *index) {
    int i = *index;
    struct Queue *tmp = Queue();
    struct Queue_Element *element;

    if (!strcmp(argv[i], "--pipe"))
        printf("--pipe\n");
    else {
        while (!isQueueEmpty(fileFlags)) {
            element = front(fileFlags);
            printf("%s ", element->string);
            enqueue(tmp, element->string);
            dequeue(fileFlags);
        }

        clearQueue(fileFlags);

        while (!isQueueEmpty(tmp)) {
            element = front(tmp);
            enqueue(fileFlags, element->string);
            dequeue(tmp);
        }

        printf("%s %s\n", argv[i], argv[i + 1]);
    }
}

void handleVerboseSubCmd(int argc, char **argv, const int *index) {
    int i = *index;
    if (!strcmp(argv[i], "--command")) {
        printf("%s %s %s %s %s ",
               argv[i], argv[i + 1], argv[i + 2], argv[i + 3], argv[i + 4]);
        int numArgs = numArgsCmd(argc, argv, i + 5);
        
        int j;
        for (j = 0; j < numArgs; j++)
            printf("%s ", argv[(i + 5) + j]);
        printf("\n");
    } else if (!(strcmp(argv[i], "--wait") &&
                 strcmp(argv[i], "--verbose") &&
                 strcmp(argv[i], "--profile") &&
                 strcmp(argv[i], "--abort") &&
                 strcmp(argv[i], "--pause")))
        printf("%s \n", argv[i]);
    else if (!(strcmp(argv[i], "--catch") &&
               strcmp(argv[i], "--ignore") &&
               strcmp(argv[i], "--close") &&
               strcmp(argv[i], "--default")))
        printf("%s %s \n", argv[i], argv[i + 1]);
    else fprintf(stderr, "[ERROR]: Undefined command %s\n", argv[i]);
}

void handleProfile(int *index) {
    *index += 1;
    prof_flag = true;
}

void handleClose(int fd, int *index) {
    *index += 2;
    int status = close(fd);
    if (status == -1)
        fprintf(stderr, "[ERROR]: Unable to close file.\n");
}

void handleAbort(int *index) {
    *index += 1;
    raise(SIGSEGV);
}

void handlePause(int *index) {
    *index += 1;    
    getchar();
}

void sig_catch(int signo) {
    fprintf(stderr, "[MSG]: Caught Signal %d\n", signo);
    exit(signo);
}

void sig_catch_interrupt(int signo) {
    kill(pid, SIGTERM);
    printf("\n");
}

void handleCatch(int signo, int *index) {
    *index += 2;    
    struct sigaction sa_catch;
    sa_catch.sa_handler = &sig_catch;
    sa_catch.sa_flags = SA_RESTART;
    sigfillset(&sa_catch.sa_mask);

    if (sigaction(signo, &sa_catch, NULL) == -1)
        fprintf(stderr, "[ERROR]: Could not handle signal %d\n", signo);
}

void handleIgnore(int signo, int *index) {
    *index += 2;
    struct sigaction sa_ignore;
    sa_ignore.sa_handler = SIG_IGN;
    sa_ignore.sa_flags = SA_RESTART;
    sigfillset(&sa_ignore.sa_mask);

    if (sigaction(signo, &sa_ignore, NULL) == -1) 
        fprintf(stderr, "[ERROR]: Could not handle signal %d\n", signo);  
}

void handleDefault(int signo, int *index) {
    *index += 2;
    struct sigaction sa_default;
    sa_default.sa_handler = SIG_DFL;
    sa_default.sa_flags = SA_RESTART;
    sigfillset(&sa_default.sa_mask);

    if (sigaction(signo, &sa_default, NULL) == -1)
        fprintf(stderr, "[ERROR]: Could not handle signal %d\n", signo);
}

void handleError(char **argv, int *index) {    
    int i = *index;
    *index += 1;    
    fprintf(stderr, "[ERROR]: Unknown option --%s\n", argv[i]);
    fprintf(stderr, "[USAGE]: simpsh [option] ...\n");
}

void manipVerbose(bool *verbose_flag, int *index) {
    *index += 1;    
    *verbose_flag = true;
}

void manipWait(bool *wait_flag, int *index) {
    *index += 1;    
    *wait_flag = true;
}

void handleExit(void) {
    exit(0);
}

void handleSubCmd(int argc, char **argv, int *index,
                  struct Stack *file_fd, bool *verbose_flag, bool *wait_flag) {
    int i = *index;
    if (!strcmp(argv[i], "--command"))
        handleCommand(argc, argv, index, file_fd, *verbose_flag, *wait_flag);
    else if (!strcmp(argv[i], "--verbose"))
        manipVerbose(verbose_flag, index);
    else if (!strcmp(argv[i], "--wait"))
        manipWait(wait_flag, index);
    else if (!strcmp(argv[i], "--profile"))
        handleProfile(index);
    else if (!strcmp(argv[i], "--close"))
        handleClose(stringToInt(argv[i + 1]), index); 
    else if (!strcmp(argv[i], "--abort"))
        handleAbort(index);
    else if (!strcmp(argv[i], "--catch"))
        handleCatch(stringToInt(argv[i + 1]), index);
    else if (!strcmp(argv[i], "--ignore"))
        handleIgnore(stringToInt(argv[i + 1]), index);
    else if (!strcmp(argv[i], "--default"))
        handleDefault(stringToInt(argv[i + 1]), index);
    else if (!strcmp(argv[i], "--pause"))
        handlePause(index);
    else if (!strcmp(argv[i], "--exit"))
        handleExit();
    else fprintf(stderr, "[ERROR]: Undefined Command (%s)\n", argv[i]);
}

void interactive(void) {
    char *line;
    char **line_tokenized;
    int tokenized_size;

    int i = 0;
    bool v_flag = false;
    bool w_flag = false;
    struct Queue *fileFlags = Queue();
    struct Stack *file_fd = Stack();

    push(file_fd, "", 0); // standard-in file descriptor
    push(file_fd, "", 1); // standard-out file descriptor
    push(file_fd, "", 2); // standard-error file descriptor

    while(1) {
        signal(SIGINT, SIG_DFL);
        line = readline_get("simpsh> ");
        line_tokenized = tokenize_string(line, " ", &tokenized_size);

        while (i < tokenized_size) {
            if (isOption(line_tokenized[i])) {
                if (isOptionFileFlag(line_tokenized[i]))
                    handleFileFlag(line_tokenized[i], fileFlags, &i);
                else if (isOptionFileMode(line_tokenized[i])) {
                    if (v_flag)
                        handleVerboseFile(tokenized_size,
                                          line_tokenized, fileFlags, &i);
                    handleFileMode(line_tokenized[i],
                                   line_tokenized[i + 1],
                                   fileFlags, file_fd, &i);
                }
                else {
                    if (v_flag)
                        handleVerboseSubCmd(tokenized_size, line_tokenized, &i);
                    handleSubCmd(tokenized_size,
                                 line_tokenized, &i, file_fd, &v_flag, &w_flag);
                }
            } else handleError(line_tokenized, &i);
        }
        i = 0;
    }
    free(line);
    free(line_tokenized);
}

void noninteractive(int argc, char **argv) {
    int i = 1;
    bool v_flag = false;
    bool w_flag = false;

    struct Queue *fileFlags = Queue();
    struct Stack *file_fd = Stack();

    push(file_fd, "", 0); // standard-in file descriptor
    push(file_fd, "", 1); // standard-out file descriptor
    push(file_fd, "", 2); // standard-error file descriptor

    while (i < argc) {
        if (isOption(argv[i])) {
            if (isOptionFileFlag(argv[i]))
                handleFileFlag(argv[i], fileFlags, &i);
            else if (isOptionFileMode(argv[i])) {
                if (v_flag)
                    handleVerboseFile(argc, argv, fileFlags, &i);
                handleFileMode(argv[i], argv[i + 1], fileFlags, file_fd, &i);
            }
            else {
                if (v_flag)
                    handleVerboseSubCmd(argc, argv, &i);
                handleSubCmd(argc, argv, &i, file_fd, &v_flag, &w_flag);
            }
        } else handleError(argv, &i);
    }    
}

void profiler(int argc, char **argv, int *index) {
    struct rusage usage;
    getrusage(RUSAGE_CHILDREN, &usage);
    struct timeval utime = usage.ru_utime;
    struct timeval stime = usage.ru_stime;
    printf("\n[Profile]: ");
    handleVerboseSubCmd(argc, argv, index);
    printf("User CPU Time: %ld.%08ld\n", utime.tv_sec, utime.tv_usec);
    printf("Sys CPU Time: %ld.%08ld\n", stime.tv_sec, stime.tv_usec);
    printf("Max Shared Resident Size: %ld\n", usage.ru_maxrss);
    printf("Integral Shared Memory Size: %ld\n", usage.ru_ixrss);
    printf("Integral Unshared Data Size: %ld\n", usage.ru_idrss);
    printf("Integral Unshared Stack Size: %ld\n", usage.ru_isrss);
    printf("Soft Page Faults: %ld\n", usage.ru_minflt);
    printf("Hard Page Faults: %ld\n", usage.ru_majflt);
    printf("Swaps: %ld\n", usage.ru_nswap);
    printf("Blocked Input Operations: %ld\n", usage.ru_inblock);
    printf("Blocked Output Operations: %ld\n", usage.ru_oublock);
    printf("IPC Messages Sent: %ld\n", usage.ru_msgsnd);
    printf("IPC Messages Received: %ld\n", usage.ru_msgrcv);
    printf("Signals Received: %ld\n", usage.ru_nsignals);
    printf("Voluntary Context Switches: %ld\n", usage.ru_nvcsw);
    printf("Involuntary Context Switches: %ld\n\n", usage.ru_nivcsw);
    prof_flag = false;    
}
