#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t value = 0;
volatile sig_atomic_t killed = 0;

void handler(int signum)
{
    if (signum == SIGUSR1) {
        value += 1;
        printf("%d\n", value);
        fflush(stdout);
    } else if (signum == SIGUSR2) {
        value *= -1;
        printf("%d\n", value);
        fflush(stdout);
    } else if (signum == SIGINT || signum == SIGTERM) {
        killed = 1;
    }
}

int main()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = handler;
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    scanf("%d", &value);
    while (!killed) {
        pause();
    }
    return 0;
}