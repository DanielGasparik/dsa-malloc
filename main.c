#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct mem_block {
    struct mem_block *next;
    struct mem_block *prev;
    unsigned int size; //struct size + payload
} BLOCK;
#define block_size sizeof(BLOCK)
void *head;
//TODO find free
void *find_free_block(unsigned int size){
    BLOCK* tmp1 = (BLOCK*)head;
    BLOCK* tmp2 = ((BLOCK*)head)->next;
    BLOCK* best_fiit = NULL;
    unsigned int rozdiel_best = -1, rozdiel_temp;
    while(tmp2->next!=NULL){

        if(tmp2->size>=size){
            rozdiel_temp = tmp2->size-size;
            if (rozdiel_best == -1){rozdiel_best == rozdiel_temp;}
            if(rozdiel_temp<=rozdiel_best){
                best_fiit = tmp2;
            }

        }
        tmp1 = tmp2;
        tmp2 = tmp2->next;
    }
    tmp1->next = tmp2->next;
    tmp2->next = NULL;
    return best_fiit;
}
void *memory_alloc(unsigned int size) {
    if (head == NULL || size <= 0 ) {
        return NULL;
    }
    void *ptr = ((BLOCK*)head)->next+block_size;
    ((BLOCK*)head)->next = ptr+size;//((BLOCK*)head)->next+block_size+size;
    ((BLOCK*)head)->next->size = ((BLOCK*)head)->size-size-block_size;
    return ptr;

}

int memory_free(void *valid_ptr) {}

int memory_check(void *ptr) {
    if(head == NULL || ptr == NULL) {
        return 0;
    }

    return 1;
}

void memory_init(void *ptr, unsigned int size) {

    head = ptr;
    memset(head,0,size);
    BLOCK* temp = (BLOCK*)head;
    BLOCK* temp2 = head+block_size;
    temp->size = size-block_size;
    temp->prev = NULL;
    temp2->size = temp->size - block_size;
    temp->next = temp2;
    temp2->next = NULL;
    temp2->prev = NULL;




    printf("adress of head[0] %d \n adress of ptr[0] %d\n", &*head, &*ptr);
    printf("block size %lu\n",block_size);
    printf("first available block of memory from head %d \nfirst available block of memory from ptr %d\n", &*(head+block_size), &*(ptr+block_size));
}

int main() {
    char region[100];

    printf(" original mempool[0] adress %d\n", &region[0]);
    memory_init(region, 100);
    printf("adresa headu %d\n next free block %d\n size left after main head and first free block head %d\n",head, ((BLOCK*)head)->next, ((BLOCK*)head)->next->size);

    printf("original mempool first available adress %d\n", &region[block_size]);
    char *pointer = (char *) memory_alloc(10);
    printf("%d pointer adress",&pointer);
    if (pointer) {
        memset(pointer, 0, 10);
    }
    if (pointer) {
        memory_free(pointer);
    }
    return 0;
}