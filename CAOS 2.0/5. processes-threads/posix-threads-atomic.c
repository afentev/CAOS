#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define THREADS_LIMIT 8

typedef struct Node {
    struct Node* next;
    int64_t value;
} node_t;

typedef struct {
    node_t* node;
} head_t;

typedef struct {
    pthread_t thread;
    _Atomic head_t* head;
    int64_t number;
    int64_t elements_count;
} arg_struct;

void push(_Atomic head_t* head_ptr, int64_t value)
{
    head_t new_head;
    head_t head = atomic_load(head_ptr);
    node_t* node = malloc(sizeof(node_t));
    node->value = value;
    do {
        node->next = head.node;
        new_head.node = node;
    } while (!atomic_compare_exchange_weak(head_ptr, &head, new_head));
}

void* worker(void* arguments)
{
    arg_struct* args = arguments;
    for (int64_t i = 0; i < args->elements_count; ++i) {
        int64_t number = args->number * args->elements_count + i;
        push(args->head, number);
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    assert(argc == 3);
    int64_t threads_count = atoi(argv[1]);
    int64_t elements_count = atoi(argv[2]);

    head_t root = {.node = NULL};
    _Atomic head_t head = root;

    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setstacksize(&tattr, PTHREAD_STACK_MIN + 160);
    arg_struct threads[threads_count];
    int oldest = 0;
    for (int64_t i = 0; i < threads_count; ++i) {
        threads[i].number = i;
        threads[i].elements_count = elements_count;
        threads[i].head = &head;
        pthread_create(&threads[i].thread, &tattr, worker, (void*)&threads[i]);
        if (i - oldest + 1 >= THREADS_LIMIT) {
            pthread_join(threads[oldest].thread, NULL);
            ++oldest;
        }
    }
    for (int64_t i = oldest; i < threads_count; ++i) {
        pthread_join(threads[i].thread, NULL);
    }

    head_t head_normal = atomic_load(&head);
    node_t* top = head_normal.node;
    while (top != NULL) {
        printf("%ld\n", top->value);
        node_t* next = top->next;
        free(top);
        top = next;
    }
    return 0;
}
