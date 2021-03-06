#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/*
 * Prostredie CLion - Daniel Gašparík , kompilátor - CLang
 * implementácia 1 projektu z DSA
 *
 * @author Daniel Gašparík
 *      https://github.com/DanielGasparik/dsa-malloc
 *
 * My implementation of memory_alloc is based on the implicit linked list of memory blocks
 * Each node of the list contains a pointer to the next and previous node
 * To keep track whether the memory is free or not, I simply multiply the size of the payload by -1
 * If the memory is free size of the payload is <0, if it is >0 then the memory is allocated
 *
 * */

typedef struct mem_block {
    //next block
    void *next;
    //previous block
    void *prev;
    //size of the payload; can be >0(if occupied) or <0(if free)
    int size; //struct size + payload
} BLOCK;
//size of block macro, clang, macOS 10.15.5 -> 24B
#define block_size sizeof(BLOCK)
//global pointer
void *head = NULL;

/*********** IMPLEMENTATION OF FUNCTIONS STARTS HERE ***********/

void iterate() {
    /** \brief Iterates through the linked list of memory blocks and prints all the information it can get such as:
     * Address of the block's payload
     * Size of the header(In my case 24)
     * Whether the payload is free or not
     * Size of the payload
     *
     */
    void *iterator = (BLOCK *) head;
    int i = 1;
    while ((BLOCK *) iterator != NULL) {
        //some conditionals to do a nice printout, should be self explanatory
        printf("\nI am header %d\nMy adress: %d\nAddress of my payload: %d\nMy size %d\nAm I free?: %s\nSize of my payload %d\n",
               i, iterator,
               iterator + block_size, block_size, ((BLOCK *) iterator)->size > 0 ? "NO" : "YES",
               ((BLOCK *) iterator)->size < 0 ? ((BLOCK *) iterator)->size * -1 : ((BLOCK *) iterator)->size);

        iterator = ((BLOCK *) iterator)->next;
        i++;
    }
}

void *find_free_block(unsigned int size) {
    /** \brief finds a first block of free memory that fits the size criteria,
    *   if none of the blocks fit the criteria or there is no space left it will @return NULL
    *
    *  @param size - size of the payload that we want to malloc
    *  @return pointer to a beginning of a new allocated film linked list structure
    */

    /* Locals
     * current          - iterates through the linked list of blocks looking for the required memory block
    */
    //first fit
    //start at the beginning of the list
    BLOCK *current = (BLOCK *) head;
    while (current != NULL) {
        if (current->size < 0) {
            //if the node is free and there's space left to allocate the memory and create another header
            //return the current node to malloc
            if (((current->size * -1)) > size + block_size) {
                return current;
            }
        }
        current = current->next;
    }
    //if there s no free node return NULL
    return NULL;

}

void *memory_alloc(unsigned int size) {
    /** \brief allocates a chunk of memory from the static region, uses the find_free_block function to find a free block
    *   of needed size.
    *   Allocates the memory by changing the size value to a positive number and then splits the block to 2 blocks
    *   One block of the size of the payload and another block of the rest of the size
    *
    *  @param size - size of the payload that we want to malloc
    *  @return If the allocation goes well, returns an address to the beginning of the mallocated payload
    */

    /* Locals
     * free          - variable to which a free block of memory is assigned
     * next          - utility variable used to split the free chunk of memory
    */

    //precautions in cases that initialization goes wrong or we request a size smaller or equal to 0.
    if (head == NULL || size <= 0) {
        printf("Size is smaller than 0 or head points to NULL(bad initialization)\n");
        return NULL;
    }
    //returns a free block of memory from the linked list
    void *free = find_free_block(size);
    //no free blocks left - memory full
    if (free == NULL) {
        printf("Not enough room to allocate a ptr of size %d\n", size);
        return NULL;
    } else {
        //splits the blocks and adjusts the correct sizes
        BLOCK *next = free + block_size + size;
        //pay attention to the implementation of sizing (<0 free, >0 allocated) hence -(+) = - /we still counted the correct
        //size but kept the negative size so we don t have to multiply by -1
        next->size = ((BLOCK *) free)->size + block_size + size;
        ((BLOCK *) free)->size = (int) size;
        //sets the correct adresses to previous and next blocks after splitting
        next->next = ((BLOCK *) free)->next; //next->next = NULL;
        ((BLOCK *) free)->next = next;
        next->prev = ((BLOCK *) free);

    }
    //utility printout
    printf("Podarilo sa mi alokovat o velkosti %d, na adrese %d\n", size, free + block_size);
    //returns the address of the payload
    return free + block_size;

}

int memory_free(void *valid_ptr) {
    /** \brief frees a valid pointer(first address of the payload)
    *    First we check if the valid pointer isn't already free
    *    Then we free the pointer by changing the size to <0
    *    Afterwards we look for possible merging of free block from left and right
    *
    *  @param valid_ptr - valid pointer
    *  @return 1 or 0 depending on the success of the memory_free
    */

    /* Locals
     * help          - points to the header of the block we want to free
     * help2         - points to the next block of memory
     * help3         - points to the previous block of memory
    */
    //header adress
    valid_ptr -= block_size;//zaciatok hlavicky;
    BLOCK *help = valid_ptr;
    //there's nothing left to memory_free since the block is free already
    if (help->size < 0) {
        printf("Pointer beeing freed is already free");
        return 1;//nemame co odalokovat
    }
    // "actual" memory free
    help->size *= -1;
    BLOCK *help2 = help->next;
    BLOCK *help3 = help->prev;
    //merging of free blocks from the right
    if (help->next != NULL) {
        //is the next block free?
        if (help2->size < 0) {
            //change the size of the actual block
            help->size += help2->size - block_size;
            //and merge
            help->next = help2->next;
        }
    }
    //merging of free blocks from the left
    if (help->prev != NULL) {
        if (help3->size < 0) {
            help3->size += help->size - block_size;
            help3->next = help->next;
        }
    }
    //returns 0 if successful
    return 0;

}

int memory_check(void *ptr) {
    /** \brief checks if the pointer is in fact an address pointing to the beginning of a block's payload
    *    First we check if the ptr is NULL or head is NULL or the pointer's out of range to the left
    *    Then we iterate through the linked list looking for a match in the ptr address and block+block_size address(otherwise known as payload's address)
    *    If the addresses match and we currently are in a free block return 0
    *    If the ptr somehow ended up in the block's header memory range, we return 0
    *    If the ptr is outside the manned region to the right return 0
    *    Otherwise return 1
    *  @param ptr - pointer we want to check
    *  @return 1 or 0 depending on the success of our checks
    */

    /* Locals
     * help          - points to the head, iterates through the linked list
     * iterator      - points to help, checks whether the ptr is in the head address region
     * help2         - utility pointer so our address counting doesn't get mismatched by something like this help += block_size(which would put us 24*sizeof(BLOCK) forward
    */
    BLOCK *help = head;
    void *iterator;
    //self explanatory
    if (head == NULL || ptr == NULL || ptr <= head) {
        return 0;
    }
    //iterates through the linked list
    while (help != NULL) {
        void *help2 = help;
        //iterates through the addresses from start to end of the block
        for (iterator = help; iterator <= help2 + help->size + block_size; iterator++) {
            if (iterator == ptr) {
                if (iterator > help2 && iterator < help2 + block_size) {
                    printf("Pointer is in the head\n");
                    return 0;
                }
                if (help->size < 0) {
                    printf("Pointer is already free\n");
                    return 0;
                }
            }
        }
        help = help->next;
    }
    //out of bounds to the right
    if (ptr > iterator) {
        printf("Pointer passed to memory check is out of bounds to right\n");
        return 0;
    }

    //memory check successful
    return 1;
}

void memory_init(void *ptr, unsigned int size) {
    /** \brief initializes our global pointer and clears the mempool of any garbage
    *
    *
    *  @param ptr - beginning of the managed region
    *  @param size - size of the region
    *
    */

    /* Locals
     * temp          - variable used to set the first block of the mempool
    */
    //set the global pointer
    head = ptr;
    //clear the memory
    memset(head, 0, size);
    //temporary variable so we can set next,prev to NULL and set the correct size to the first block
    BLOCK *temp = (BLOCK *) head;
    temp->size = (int) (-1 * (size - block_size)); //free<0  taken>0
    temp->next = NULL;
    temp->prev = NULL;

}


//return s random number in the range specified in the arguments
int rand_int(int min, int max) {
    return (rand() % (max + 1 - min) + min);
}

void frag_printout(int size) {
    /** \brief prints how much % of our mempool is taken up by headers after mallocation
    *   and how much space we did occupy with our mem_alloc calls
    *
    *
    *
    *  @param size - size of the whole region available for mallocc
    *
    */

    /* Locals
     * hlavicka_count          - counts how many heads did we allocate
     * iterator                - iterates through the linked list
     * obsadene_bytes          - how many bytes did we allocate
     * frag                    - the ratio between sum of head sizez / by the mempool size
     * zaplnilo                - the ratio between sum of payloads / by the mempool size
    */
    double hlavicka_count = 0;
    BLOCK *iterator = head;
    double obsadene_bytes = 0;
    while (iterator != NULL) {
        if (iterator->size > 0) {
            obsadene_bytes += iterator->size;
        }
        hlavicka_count++;
        iterator = iterator->next;

    }
    double frag = (hlavicka_count * block_size) / size;
    double zaplnilo = obsadene_bytes / size;
    printf("Fragmentacia je %.2f%%\nZaplnene je %.2f%%\n", frag * 100, zaplnilo * 100);
}

void test1() {
    /** \brief tests our memory_alloc,nemory_check,memory_free,memory_init calls with various different sizes, callings and test scenarios
    *   All the tests are similiar in structure so I will only document this one
    *
    *
   */

    /* Locals
     *  region1       -this represents a mempool of a certain size
     *  region2       -the sizes usually go up
     *  region3       -this behaviour repeats in almost all test cases
     *  ptr           -pointer arrays so we don't have to create many pointer variables to which we will malloc
     *  ptr2
     *  ptr3
    */
    char region1[50];
    char region2[100];
    char region3[200];
    char *ptr[2];
    char *ptr2[4];
    char *ptr3[8];
    //inits a region with a fixed size
    memory_init(region1, 50);
    printf("/-------------------TEST1a---------------/\n");
    printf("Region size = 50\nHead size = %d\nMaximum size you can alloc: %d\n", block_size, 50 - (2 * block_size) - 1);
    int a = 8;
    for (int i = 0; i < 2; i++) {
        ptr[i] = memory_alloc(a);
    }
    //prints out the fragmentation and occupied blocks
    frag_printout(50);
    //iterates through the linked list printing all the information it can
    iterate();

    a = 15;
    for (int i = 0; i < 2; i++) {
        ptr[i] = memory_alloc(a);
        if (i % 2 == 0) {
            if (memory_check(ptr[i])) {
                memory_free(ptr[i]);
            }
        }
    }
    a = 24;
    for (int i = 0; i < 2; i++) {
        ptr[i] = memory_alloc(a);
        if (i % 2 == 0) {
            if (memory_check(ptr[i])) {
                memory_free(ptr[i]);
            }
        }
    }


    memory_init(region2, 100);

    printf("\n\n/-------------------TEST1b---------------/\n");
    printf("Region size = 100\nHead size = %d\nMaximum size you can alloc: %d\n", block_size, 100 - (2 * block_size));
    for (int i = 0; i < 4; i++) {
        ptr2[i] = memory_alloc(12);

    }
    iterate();
    frag_printout(100);


    memory_init(region3, 200);

    printf("\n\n/-------------------TEST1c---------------/\n");
    printf("Region size = 200\nHead size = %d\nMaximum size you can alloc: %d\n", block_size, 200 - (2 * block_size));
    for (int i = 0; i < 8; i++) {
        ptr3[i] = memory_alloc(24);

        if (i == 2) {
            if (memory_check(ptr[i])) {
                memory_free(ptr[i]);
            }
        }
    }
    iterate();
    frag_printout(200);
}

void test2() {
    char region1[50];
    char region2[100];
    char region3[200];
    char *ptr[25];
    char *ptr2[50];
    char *ptr3[100];
    int i = 0;
    int random = rand_int(8, 24);
    printf("\n\n/-------------------TEST2a---------------/\n");
    printf("Region size = 50\nHead size = %d\nMaximum size you can alloc: %d\n", block_size, 50 - (2 * block_size) - 1);
    memory_init(region1, 50);
    //my tests are based on a principle of allocationg untill I can't allocate a requested amount -> hence the while loop
    while ((ptr[i++] = memory_alloc(random)) != NULL) {
        random = rand_int(8, 24);

    }
    iterate();
    frag_printout(50);

    printf("\n\n/-------------------TEST2b---------------/\n");
    printf("Region size = 100\nHead size = %d\nMaximum size you can alloc: %d\n", block_size, 100 - (2 * block_size));
    random = rand_int(8, 24);
    i = 0;
    memory_init(region2, 100);
    while ((ptr2[i++] = memory_alloc(random)) != NULL) {
        random = rand_int(8, 24);
        if (i == 1) {
            if (memory_check(ptr2[i])) {
                memory_free(ptr2[i]);
            }
        }

    }
    memory_free(ptr2[0]);

    iterate();
    frag_printout(100);

    printf("\n\n/-------------------TEST2c---------------/\n");
    printf("Region size = 200\nHead size = %d\nMaximum size you can alloc: %d\n", block_size, 200 - (2 * block_size));

    random = rand_int(8, 24);
    i = 0;
    memory_init(region3, 200);
    while ((ptr3[i++] = memory_alloc(random)) != NULL) {
        random = rand_int(8, 24);
        if (i == 2) {
            if (memory_check(ptr3[i - 1])) {
                memory_free(ptr3[i - 1]);
            }
        }


    }
    iterate();
    frag_printout(200);


}

void test3() {
    char region1[1000];
    char region2[10000];
    char region3[31234];
    char *ptr[20];
    char *ptr2[100];
    char *ptr3[200];
    int i = 0;
    int random = rand_int(500, 5000);
    printf("\n\n/-------------------TEST3a---------------/\n");
    printf("Region size = 1000\nHead size = %d\nMaximum size you can alloc: %d\n", block_size,
           1000 - (2 * block_size) - 1);
    memory_init(region1, 1000);
    while ((ptr[i++] = memory_alloc(random)) != NULL) {
        random = rand_int(500, 5000);

    }
    iterate();
    frag_printout(1000);

    printf("\n\n/-------------------TEST3b---------------/\n");
    printf("Region size = 10000\nHead size = %d\nMaximum size you can alloc: %d\n", block_size,
           10000 - (2 * block_size));
    random = rand_int(500, 5000);
    i = 0;
    memory_init(region2, 10000);
    while ((ptr2[i++] = memory_alloc(random)) != NULL) {
        random = rand_int(500, 5000);


    }
    if (memory_check(ptr2[1]) && memory_check(ptr2[2])) {
        memory_free(ptr2[1]);
        memory_free(ptr2[2]);
    }
    iterate();
    frag_printout(10000);


    printf("\n\n/-------------------TEST3c---------------/\n");
    printf("Region size = 31234\nHead size = %d\nMaximum size you can alloc: %d\n", block_size,
           31234 - (2 * block_size));

    random = rand_int(500, 5000);
    i = 0;
    memory_init(region3, 31234);
    while ((ptr3[i++] = memory_alloc(random)) != NULL) {
        random = rand_int(500, 5000);
        if (i == 2) {
            if (memory_check(ptr3[i - 1])) {
                memory_free(ptr3[i - 1]);
            }
        }


    }
    iterate();
    frag_printout(31234);


}

void test4() {
    char region1[1000000];
    char *ptr[1000];
    int i = 0;
    int random = rand_int(8, 50000);
    printf("\n\n/-------------------TEST4---------------/\n");
    printf("Region size = 100000\nHead size = %d\nMaximum size you can alloc: %d\n", block_size,
           100000 - (2 * block_size) - 1);
    memory_init(region1, 100000);
    while ((ptr[i++] = memory_alloc(random)) != NULL) {
        random = rand_int(8, 50000);

    }

    for (int j = 0;; j++) {
        if (memory_check(ptr[j * 2]) && memory_check(ptr[j * 2 - 1])) {
            memory_free(ptr[j * 2]);
            memory_free(ptr[j * 2 - 1]);
        }
        if (j == 3) {
            break;
        }
    }

    iterate();
    frag_printout(100000);

}


int main() {
    //main
    //
    srand(time(0));
    char region[100];
    //run tests
    test1();
    test2();
    test3();
    test4();
    memory_init(region, 100);

    char *pointer = (char *) memory_alloc(10);

    if (pointer) {
        memset(pointer, 0, 10);
    }
    if (pointer) {
        memory_free(pointer);
    }
    return 0;
}