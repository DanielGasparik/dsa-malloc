#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct mem_block
{
    struct mem_block *next;
    struct mem_block *prev;
    int size;
} BLOCK;
BLOCK *head;

void *memory_alloc(unsigned int size)
{
    if (head == NULL)
    {
        return NULL;
    }
}
int memory_free(void *valid_ptr) {}
int memory_check(void *ptr) {}
void memory_init(void *ptr, unsigned int size)
{
    head = ptr;
    printf("%p \n", &*head);
}
int main()
{
    char region[50];
    printf("%p\n", &region);
    memory_init(region, 50);
    char *pointer = (char *)memory_alloc(10);
    if (pointer)
    {
        memset(pointer, 0, 10);
    }
    if (pointer)
    {
        memory_free(pointer);
    }
    return 0;
}