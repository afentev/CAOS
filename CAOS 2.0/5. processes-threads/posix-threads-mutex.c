#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    pthread_t thread;
    double* left;
    double* central;
    double* right;
    int iters_count;
    pthread_mutex_t* syncer;
} thread_data;

static void* thread_func(void* arg)
{
    thread_data* data_ptr = (thread_data*)arg;
    for (int i = 0; i < data_ptr->iters_count; i++) {
        pthread_mutex_lock(data_ptr->syncer);
        *data_ptr->left += 0.99;
        *data_ptr->central += 1.0;
        *data_ptr->right += 1.01;
        pthread_mutex_unlock(data_ptr->syncer);
    }
    return NULL;
}

int main(int argc, char** argv)
{
    assert(argc == 3);
    int iters_count = atoi(argv[1]);
    int threads_count = atoi(argv[2]);
    pthread_mutex_t syncer;
    pthread_mutex_init(&syncer, NULL);

    double numbers[threads_count];
    for (int i = 0; i < threads_count; ++i) {
        numbers[i] = 0;
    }
    thread_data threads[threads_count];
    for (int i = 0; i < threads_count; i++) {
        threads[i].iters_count = iters_count;
        threads[i].left = numbers + (threads_count + i - 1) % threads_count;
        threads[i].central = numbers + i;
        threads[i].right = numbers + (i + 1) % threads_count;
        threads[i].syncer = &syncer;
        pthread_create(
            &threads[i].thread, NULL, thread_func, (void*)&threads[i]);
    }
    for (int i = 0; i < threads_count; i++) {
        pthread_join(threads[i].thread, NULL);
    }
    for (int i = 0; i < threads_count; ++i) {
        printf("%.10g\n", numbers[i]);
    }
    return 0;
}
