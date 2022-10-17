#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int sum;
} task_res;

void* thread_sum()
{
    task_res* res = malloc(sizeof(task_res));
    res->sum = 0;
    int number;
    while (scanf("%d", &number) != EOF) {
        res->sum += number;
    }
    return res;
}

int main(int argc, char** argv)
{
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setstacksize(
        &tattr, PTHREAD_STACK_MIN + sizeof(task_res*) + sizeof(int));

    const size_t THREAD_COUNT = atoi(argv[1]);
    pthread_t threads[THREAD_COUNT];
    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        pthread_create(&threads[i], &tattr, thread_sum, NULL);
    }

    int sum = 0;
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        void* res_p;
        pthread_join(threads[i], &res_p);
        task_res* res = (task_res*)res_p;
        sum = (sum + res->sum);
        free(res);
    }

    printf("%d", sum);
    return 0;
}