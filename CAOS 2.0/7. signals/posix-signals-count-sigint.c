#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t received_sigint = 0;
volatile sig_atomic_t received_sigterm = 0;

void sigint_handler(int signum)
{
    received_sigint = 1;
}

void sigterm_handler(int signum)
{
    received_sigterm = 1;
}

void set_handler(int signum, void* handler, struct sigaction* action)
{
    action->sa_handler = handler;
    action->sa_flags = SA_RESTART;
    sigaction(signum, action, NULL);
}

int main()
{
    size_t sigint_count = 0;
    struct sigaction sigterm_action;
    struct sigaction sigint_action;
    memset(&sigint_action, 0, sizeof(sigint_action));
    memset(&sigterm_action, 0, sizeof(sigterm_action));
    set_handler(SIGINT, sigint_handler, &sigint_action);
    set_handler(SIGTERM, sigterm_handler, &sigterm_action);

    while (1) {
        pause();
        if (received_sigint) {
            ++sigint_count;
            received_sigint = 0;
        }
        if (received_sigterm) {
            printf("%zu", sigint_count);
            return 0;
        }
    }
}