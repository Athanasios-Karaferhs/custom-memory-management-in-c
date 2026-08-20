#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct block
{
    size_t size;
    int free;
    struct block *next;
} block_t;

#define BLOCK_SIZE sizeof(block_t)

static block_t *head = NULL;

void my_free(void *ptr);
void *my_malloc(size_t size);
void *my_realloc(void *ptr, size_t new_size);

int main()
{
    int choice;
    void *p;
    void **ptrs = NULL;
    int capacity = 0;
    int count = 0;
    printf(" | OPTIONS | \n 1.use malloc() \n 2.use free(): ");
    scanf("%d", &choice);
    while (1)
    {
        if (choice == 1)
        {
            p = my_malloc(100);
            printf("The new pointer on the list avaliable for usage is: %p\n", p);
            if (count == capacity)
            {
                capacity = (capacity == 0) ? 4 : capacity * 2;
                ptrs = realloc(ptrs, capacity * sizeof(void *)); // εαν το καπασιτι ειανι 4 και καθε void *(pointer τυπου void) που ειναι 8 βυτεσ ζητας συνολο 8*4 δηλαδη χωρο για 4 καινουργιους ποιντερσ
            }
            ptrs[count++] = p;
            printf("choice ");
            scanf("%d", &choice);
        }
        else if (choice == 2)
        {

            if (count == 0)
            {
                printf("No memory avaliable,please make a new choice ");
            }
            else
            {

                int idx;
                printf("Which slot to free (0 to %d): ", count - 1);
                scanf("%d", &idx);

                if (idx < 0 || idx >= count)
                {
                    printf("Invalid slot.\n");
                }
                else
                {
                    my_free(ptrs[idx]);
                    ptrs[idx] = NULL;
                }
                count--;
            }
            printf("choice a new option ");
            scanf("%d", &choice);
        }
        else
            return 0;
    }
    return 0;
}

void *my_malloc(size_t size)
{
    block_t *curr = head;
    block_t *last = NULL;

    // search existing blocks for a free one that fits
    while (curr)
    {
        if (curr->free && curr->size >= size)
        {
            curr->free = 0;
            return (void *)(curr + 1); //+1=shift by sizeof(type) bytes
        }
        last = curr;
        curr = curr->next;
    }

    // ask for mem
    block_t *new_block = sbrk(BLOCK_SIZE + size);
    if (new_block == (void *)-1)
        return NULL; // sbrk failed

    new_block->size = size;
    new_block->free = 0;
    new_block->next = NULL;

    if (last)
    {
        last->next = new_block;
    }
    else
    {
        head = new_block; // this is the very first alloc
    }

    return (void *)(new_block + 1);
}

void my_free(void *ptr)
{
    if (!ptr)
        return;

    block_t *block = (block_t *)ptr - 1; // step back from payload
    block->free = 1;                     // mark reusable
}

void *my_realloc(void *ptr, size_t new_size)
{
    if (ptr == NULL)
    {
        return my_malloc(new_size);
    }

    if (new_size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    block_t *block = (block_t *)ptr - 1;

    if (block->size >= new_size)
    {
        return ptr;
    }
}