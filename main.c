#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct mem_block {
    struct mem_block *next;
    struct mem_block *prev;
    int size; //struct size + payload
    bool free;
} BLOCK;
#define block_size sizeof(struct mem_block)
void *head;

void *memory_alloc(unsigned int size) {
    if (head == NULL) {
        return NULL;
    }
}

int memory_free(void *valid_ptr) {}

int memory_check(void *ptr) {}

void memory_init(void *ptr, unsigned int size) {
    if (head != NULL) {
        head = NULL;
    }
    //head = ptr;
   // head->size = size;
   // head->free = true;
    //head->prev = NULL;

    printf("adress of head[0] %p \n adress of ptr[0] %p\n", &*head, &*ptr);
    printf("block size %d\n",block_size);
    printf("first available block of memory from head %p \nfirst available block of memory from ptr %p\n", &*(head+block_size), &*(ptr+block_size));
}

int main() {
    char region[50];

    printf(" original mempool[0] adress %p\n", &region[0]);
    memory_init(region, 50);
    printf("original mempool first available adress %p\n", &region[block_size]);
    char *pointer = (char *) memory_alloc(10);
    if (pointer) {
        memset(pointer, 0, 10);
    }
    if (pointer) {
        memory_free(pointer);
    }
    return 0;
}