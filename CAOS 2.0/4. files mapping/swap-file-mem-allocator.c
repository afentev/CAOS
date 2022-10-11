#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

typedef struct memory_block {
    bool is_free;
    size_t size;
    void* mem_addr;
    struct memory_block* next;
} m_block;
off_t file_size;
m_block* pool;

extern void myalloc_initialize(int fd)
{
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        return;
    }
    file_size = file_stat.st_size;
    char* map = mmap(0, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        return;
    }
    pool = (m_block*)malloc(sizeof(m_block));
    pool->is_free = true;
    pool->size = file_stat.st_size;
    pool->mem_addr = map;
    pool->next = NULL;
}

extern void myalloc_finalize()
{
    munmap(pool->mem_addr, file_size);
    do {
        m_block* next = pool->next;
        free(pool);
        pool = next;
    } while (pool != NULL);
}

extern void* my_malloc(size_t size)
{
    m_block* block = pool;
    do {
        if (block->is_free && block->size >= size) {
            block->is_free = false;
            if (block->size != size) {
                m_block* new_block = (m_block*)malloc(sizeof(m_block));
                new_block->is_free = true;
                new_block->size = block->size - size;
                new_block->mem_addr = block->mem_addr + size;
                new_block->next = block->next;
                block->next = new_block;
            }
            block->size = size;
            return block->mem_addr;
        }
        block = block->next;
    } while (block != NULL);
    return NULL;
}

void restore_structure()
{
    m_block* block = pool;
    m_block* next = block->next;
    while (next != NULL) {
        if (block->is_free && next->is_free) {
            block->size += next->size;
            block->next = next->next;
            free(next);
            next = block->next;
        } else {
            block = next;
            next = next->next;
        }
    }
}

extern void my_free(void* ptr)
{
    m_block* block = pool;
    while (block->mem_addr != ptr) {
        block = block->next;
    }
    block->is_free = true;
    restore_structure();
}