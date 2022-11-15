#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

volatile sig_atomic_t last_signal = 0;
volatile sig_atomic_t last_signal_value = 0;
volatile sig_atomic_t sender_pid = 0;

static void handler(int signum, siginfo_t* info, void* ucontext)
{
    last_signal = signum;
    last_signal_value = info->si_value.sival_int;
    sender_pid = info->si_pid;
}

int main()
{
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
        if (last_signal) {
            if (last_signal == SIGRTMIN) {
                if (last_signal_value == 0) {
                    exit(0);
                }
                sigqueue(
                    sender_pid,
                    SIGRTMIN,
                    (union sigval){.sival_int = last_signal_value - 1});
                if (last_signal_value == 1) {
                    exit(0);
                }
            }
            last_signal = 0;
        }
    }
    return 0;
}