#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct mem_block {
    void *next;
    void *prev;
    int size; //struct size + payload
} BLOCK;
#define block_size sizeof(BLOCK)
void *head = NULL;
//TODO find free

void *find_free_block(unsigned int size) {
    //first fit
    BLOCK *current = (BLOCK *) head;
    while (current != NULL) {
        if (current->size < 0) {
            if (((current->size * -1)) > size + block_size) {
                return current;
            }
        }
        current = current->next;
    }

    return NULL;

    /* BLOCK* tmp1 = (BLOCK*)head;
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
    printf("adresa noveho bloku bez payload %d\n ",&*best_fiit);
    return best_fiit;*/
}

void *memory_alloc(unsigned int size) {
    if (head == NULL || size <= 0) {
        return NULL;
    }
    void *free = find_free_block(size);
    //no free blocks left - memory full
    if (free == NULL) {
        return NULL;
    } else {
        //free->next = free+block_size+size;
        BLOCK *next = free + block_size + size;
        next->size = ((BLOCK *) free)->size + block_size + size;
        ((BLOCK *) free)->size = (int) size;
        next->next = ((BLOCK *) free)->next;
        ((BLOCK *) free)->next = next;
        next->prev = ((BLOCK *) free);
    }
    return free + block_size;
    //return free+block_size;

    /*void *ptr = ((BLOCK*)head)->next+block_size;
    ((BLOCK*)head)->next = ptr+size;//((BLOCK*)head)->next+block_size+size;
    ((BLOCK*)head)->next->size = ((BLOCK*)head)->size-size-block_size;
    return ptr;*/

}

int memory_free(void *valid_ptr) {
    valid_ptr = valid_ptr-block_size;//zaciatok hlavicky;
    BLOCK* help = valid_ptr;
    if(help->size<0){
        return 1;//nemame co odalokovat
    }
    help->size *= -1;
    BLOCK* help2 = help->next;
    BLOCK* help3 = help->prev;
    if(help->next != NULL){
        if(help2->size<0){
            help->size+=help2->size-block_size;
            help->next = help2->next;
        }
    }
    if(help->prev !=NULL){
        if(help3->size<0){
            help3->size+=help->size-block_size;
            help3->next = help->next;
        }
    }
    return 0;

}
//TODO memory check
int memory_check(void *ptr) {
    if (head == NULL || ptr == NULL) {
        return 0;
    }
    BLOCK * check = ptr-block_size;
    if(!check || check->size<0){
        return 0;
    }

    return 1;
}

void memory_init(void *ptr, unsigned int size) {

    head = ptr;
    memset(head, 0, size);
    BLOCK *temp = (BLOCK *) head;
    //BLOCK* temp2 = head+block_size;
    temp->size = -1 * (size - block_size); //free<0  taken>0
    //temp2->size = temp->size - block_size;
    temp->next = NULL;
    temp->prev = NULL;
    // temp2->next = NULL;


    //  printf("adress of head[0] %d \n adress of ptr[0] %d\n", &*head, &*ptr);
    //printf("block size %lu\n",block_size);
    //printf("first available block of memory from head %d \nfirst available block of memory from ptr %d\n", &*temp2, &*(ptr+block_size));
}

int main() {
    char region[100];

    //printf(" original mempool[0] adress %d\n", &region[0]);
    memory_init(region, 100);
    //printf("adresa headu %d\n next free block %d\n size left after main head and first free block head %d\n",head, ((BLOCK*)head)->next, ((BLOCK*)head)->next->size);

    //printf("original mempool first available adress %d\n", &region[block_size]);
    char *pointer = (char *) memory_alloc(10);
    BLOCK *abc = ((BLOCK *) head)->next;
    printf("%d\n", block_size);
    printf("adresa sucasneho %d velkost sucasneho %d\n adresa dalsieho %d velkost dalsieho %d\n", head,
           ((BLOCK *) head)->size, abc, abc->size);
    if (pointer) {
        memset(pointer, 0, 10);
    }
    if (pointer) {
        memory_free(pointer);
    }
    return 0;
}