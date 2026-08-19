#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct block {
    size_t size;
    int free;
    struct block *next;
} block_t;

#define BLOCK_SIZE sizeof(block_t)

static block_t *head = NULL;

void *my_malloc(size_t size,block_t *head);  // forward declaration

int main() {
    void *p = my_malloc(100,head);
    printf("Got pointer: %p\n", p);
    return 0;
}

void *my_malloc(size_t size,block_t *head) {
    block_t *curr = head;
    block_t *last = NULL;

    // search existing blocks for a free one that fits
    while (curr) {
        if (curr->free && curr->size >= size) {
            curr->free = 0;
            return (void *)(curr + 1); //skip past the header
        }
        last = curr;
        curr = curr->next;
    }

    //ask for mem
    block_t *new_block = sbrk(BLOCK_SIZE + size);
    if (new_block == (void *)-1) return NULL; // sbrk failed

    new_block->size = size;
    new_block->free = 0;
    new_block->next = NULL;

    if (last) {
        last->next = new_block;
    } else {
        head = new_block; // this is the very first alloc
    }

    return (void *)(new_block + 1);
}