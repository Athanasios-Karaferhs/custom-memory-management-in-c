#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h> //terminal ui

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
    initscr();
    cbreak(); // read input without enter
    echo();

    int choice;
    void *p;
    void **ptrs = NULL;
    int capacity = 0;
    int count = 0;

    while (1)
    {

        mvprintw(0, 0, "| OPTIONS |                              ");
        mvprintw(1, 0, "1. malloc()                              ");
        mvprintw(2, 0, "2. free()                                ");
        mvprintw(3, 0, "3. realloc()                             ");
        mvprintw(4, 0, "4. exit                                  ");
        mvprintw(5, 0, "> ");
        clrtoeol();
        refresh();
        scanw("%d", &choice);

        if (choice == 1)
        {
            mvprintw(8, 0, "            ");
            mvprintw(9, 0, "           ");
            p = my_malloc(100);
            mvprintw(7, 0, "New pointer available: %p          ", p);

            if (count == capacity)
            {
                capacity = (capacity == 0) ? 4 : capacity * 2;
                ptrs = realloc(ptrs, capacity * sizeof(void *));
            }
            ptrs[count++] = p;
        }
        else if (choice == 2)
        {
            mvprintw(8, 0, "            ");
            mvprintw(9, 0, "           ");
            if (count == 0)
            {
                mvprintw(7, 0, "No memory available.                ");
                refresh();
            }
            else
            {
                int idx;
                mvprintw(7, 0, "Which slot to free (0 to %d): ", count - 1);
                clrtoeol();
                refresh();
                scanw("%d", &idx);
                while (idx > count - 1)
                {
                    mvprintw(8, 0, " ");
                    mvprintw(7, 0, "Please select from (0-%d)   \n", count - 1);
                    scanw("%d", &idx);
                }
                if (idx < 0 || idx >= count)
                {
                    mvprintw(8, 0, "Invalid slot.                       ");
                }
                else
                {
                    mvprintw(7, 0, "slot is free                      ");
                    mvprintw(8, 0, " ");
                    my_free(ptrs[idx]);
                    ptrs[idx] = NULL;
                }
                count--;
            }
        }
        else if (choice == 3)
        {
            mvprintw(8, 0, "            ");
            mvprintw(9, 0, "           ");
            int idx = -1;
            size_t new_size = 0;

            if (count > 0)
            {
                mvprintw(7, 0, "Which slot to realloc (0-%d): ", count - 1);
                clrtoeol();
                refresh();
                scanw("%d", &idx);
                while (idx > count - 1)
                {
                    mvprintw(8, 0, " ");
                    mvprintw(7, 0, "Please select from (0-%d)   \n", count - 1);
                    scanw("%d", &idx);
                }
                mvprintw(8, 0, "New size: ");
                clrtoeol();
                refresh();
                scanw("%zu", &new_size);
            }

            if (idx < 0 || idx >= count || ptrs[idx] == NULL)
            {
                mvprintw(9, 0, "Invalid slot.                       ");
                refresh();
            }
            else
            {
                ptrs[idx] = my_realloc(ptrs[idx], new_size);
                mvprintw(9, 0, "realloc finished successfully.        ");
            }
        }
        else if (choice == 4)
        {
            endwin();
            return 0;
        }
        else
        {
            mvprintw(7, 0, "Wrong option, try again.              ");
        }

        refresh();
    }

    endwin();
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
        return my_malloc(new_size);

    if (new_size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    block_t *block = (block_t *)ptr - 1; // παρομια με το free()

    if (block->size >= new_size)
    {
        return ptr;
    }

    void *new_ptr = my_malloc(new_size);
    if (new_ptr == NULL)
        return NULL; // σε περιπτωση αποτυχιας να εχουμε ακομα τα δεδομενα.

    memcpy(new_ptr, ptr, block->size);
    my_free(ptr);
    return new_ptr;
}
