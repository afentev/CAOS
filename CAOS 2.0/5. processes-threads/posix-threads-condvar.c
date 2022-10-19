#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int64_t begin;
    int64_t end;
    int64_t current_prime;
    int32_t count;
    bool updated;
    pthread_mutex_t* syncer;
    pthread_cond_t* prime_ready;

} thread_data;

void* worker(void* arg)
{
    int count = 0;
    thread_data* data_ptr = (thread_data*)arg;
    for (int64_t number = data_ptr->begin; count < data_ptr->count; number++) {
        pthread_mutex_lock(data_ptr->syncer);
        while (data_ptr->updated) {
            pthread_cond_wait(data_ptr->prime_ready, data_ptr->syncer);
        }
        while (!data_ptr->updated) {
            bool is_prime = true;
            for (int64_t divisor = 2; divisor * divisor <= number; ++divisor) {
                if (number % divisor == 0) {
                    is_prime = false;
                    break;
                }
            }
            if (is_prime && number != 1) {
                ++count;
                data_ptr->current_prime = number;
                data_ptr->updated = true;
            } else {
                ++number;
            }
        }
        pthread_cond_signal(data_ptr->prime_ready);
        pthread_mutex_unlock(data_ptr->syncer);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    assert(argc == 4);
    int64_t begin = atoi(argv[1]);
    int64_t end = atoi(argv[2]);
    int32_t count = atoi(argv[3]);

    pthread_mutex_t syncer;
    pthread_mutex_init(&syncer, NULL);
    pthread_cond_t prime_ready;
    pthread_cond_init(&prime_ready, NULL);
    thread_data data;
    data.syncer = &syncer;
    data.prime_ready = &prime_ready;
    data.begin = begin;
    data.end = end;
    data.updated = false;
    data.count = count;

    pthread_t finder;

    pthread_create(&finder, NULL, worker, (void*)&data);
    for (int i = 0; i < count; ++i) {
        pthread_mutex_lock(&syncer);
        while (!data.updated) {
            pthread_cond_wait(&prime_ready, &syncer);
        }
        printf("%ld\n", data.current_prime);
        fflush(stdout);
        pthread_cond_signal(data.prime_ready);
        data.updated = false;
        pthread_mutex_unlock(&syncer);
    }

    pthread_join(finder, NULL);

    return 0;
}