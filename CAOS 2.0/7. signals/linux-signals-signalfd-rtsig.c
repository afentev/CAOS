#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

FILE** files = NULL;

static void handler(int signum, siginfo_t* info, void* ucontext)
{
    if (signum == SIGRTMIN) {
        int file_no = info->si_value.sival_int;
        if (file_no == 0) {
            FILE** real_ptr = files;
            while (*files != NULL) {
                fclose(*files);
                ++files;
            }
            free(real_ptr);
            exit(0);
        }
        char* line = NULL;
        size_t len = 0;
        getline(&line, &len, files[file_no - 1]);
        if (line) {
            printf("%s", line);
            fflush(stdout);
            free(line);
        }
    }
}

int main(int argc, char** argv)
{
    assert(argc > 1);
    assert(SIGRTMIN < SIGRTMAX);

    files = malloc(argc * sizeof(FILE*));

    for (int i = 1; i < argc; ++i) {
        files[i - 1] = fopen(argv[i], "r");
    }
    files[argc - 1] = NULL;

    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sigaction(
        SIGRTMIN,
        &(struct sigaction){
            .sa_sigaction = handler,
            .sa_flags = SA_RESTART | SA_SIGINFO,
            .sa_mask = mask},
        NULL);
    sigemptyset(&mask);

    while (1) {
        sigsuspend(&mask);
    }

    return 0;
}