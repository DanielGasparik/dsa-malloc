#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct mem_block {
    void *next;
    void *prev;
    int size; //struct size + payload
} BLOCK;
#define block_size sizeof(BLOCK)
void *head = NULL;

void iterate() {
    void *iterator = (BLOCK *) head;
    int i = 1;
    while ((BLOCK *) iterator != NULL) {
        printf("\nI am header %d\nMy adress: %d\nAddress of my payload: %d\nMy size %d\nAm I free?: %s\nSize of my payload %d\n",
               i, iterator,
               iterator + block_size, block_size, ((BLOCK *) iterator)->size > 0 ? "NO" : "YES",
               ((BLOCK *) iterator)->size < 0 ? ((BLOCK *) iterator)->size * -1 : ((BLOCK *) iterator)->size);

        iterator = ((BLOCK *) iterator)->next;
        i++;
    }
}

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

}

void *memory_alloc(unsigned int size) {
    if (head == NULL || size <= 0) {
        printf("Size is smaller than 0 or head points to NULL(bad initialization)\n");
        return NULL;
    }
    void *free = find_free_block(size);
    //no free blocks left - memory full
    if (free == NULL) {
        printf("Not enough room to allocate a ptr of size %d\n", size);
        return NULL;
    } else {
        BLOCK *next = free + block_size + size;
        next->size = ((BLOCK *) free)->size + block_size + size;
        ((BLOCK *) free)->size = (int) size;
        next->next = ((BLOCK *) free)->next; //next->next = NULL;
        ((BLOCK *) free)->next = next;
        next->prev = ((BLOCK *) free);

    }
    printf("Podarilo sa mi alokovat o velkosti %d, na adrese %d\n", size, free + block_size);

    return free + block_size;


}

int memory_free(void *valid_ptr) {
    valid_ptr -= block_size;//zaciatok hlavicky;
    BLOCK *help = valid_ptr;
    if (help->size < 0) {
        printf("Pointer beeing freed is already free");
        return 1;//nemame co odalokovat
    }
    help->size *= -1;
    BLOCK *help2 = help->next;
    BLOCK *help3 = help->prev;
    if (help->next != NULL) {
        if (help2->size < 0) {
            help->size += help2->size - block_size;
            help->next = help2->next;
        }
    }
    if (help->prev != NULL) {
        if (help3->size < 0) {
            help3->size += help->size - block_size;
            help3->next = help->next;
        }
    }
    return 0;

}

int memory_check(void *ptr) {
    BLOCK *help = head;
    void *iterator;
    if (head == NULL || ptr == NULL || ptr <= head) {
        return 0;
    }

    while (help != NULL) {
        void *help2 = help;
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
    if (ptr > iterator) {
        printf("Pointer passed to memory check is out of bounds to right\n");
        return 0;
    }


    return 1;
}

void memory_init(void *ptr, unsigned int size) {

    head = ptr;
    memset(head, 0, size);
    BLOCK *temp = (BLOCK *) head;
    temp->size = (int) (-1 * (size - block_size)); //free<0  taken>0
    temp->next = NULL;
    temp->prev = NULL;

}


#include <time.h>

void z1_testovac(char *region, char **pointer, int minBlock, int maxBlock, int minMemory, int maxMemory,
                 int testFragDefrag) {
    srand(time(0));
    unsigned int allocated = 0;
    unsigned int mallocated = 0;
    unsigned int allocated_count = 0;
    unsigned int mallocated_count = 0;
    unsigned int i = 0;
    int random_memory = 0;
    int random = 0;
    memset(region, 0, 100000);
    random_memory = (rand() % (maxMemory - minMemory + 1)) + minMemory;
    memory_init(region + 500, random_memory);
    if (testFragDefrag) {
        do {
            pointer[i] = memory_alloc(8);
            //printf("Adresa mallocovaneho %p\n",pointer[i]);
            if (pointer[i])
                i++;
        } while (pointer[i]);
        for (int j = 0; j < i; j++) {
            if (memory_check(pointer[j])) {
                memory_free(pointer[j]);
            } else {
                printf("Error: Wrong memory check.\n");
            }
        }
    }
    i = 0;
    while (allocated <= random_memory - minBlock) {
        random = (rand() % (maxBlock - minBlock + 1)) + minBlock;
        if (allocated + random > random_memory)
            continue;
        allocated += random;
        allocated_count++;
        pointer[i] = memory_alloc(random);
        //printf("Adresa mallocovaneho %p\n",pointer[i]);
        if (pointer[i]) {
            i++;
            mallocated_count++;
            mallocated += random;
        }
    }
    for (int j = 0; j < i; j++) {
        if (memory_check(pointer[j])) {
            memory_free(pointer[j]);
        } else {
            printf("Error: Wrong memory check.\n");
        }
    }
    memset(region + 500, 0, random_memory);
    for (int j = 0; j < 100000; j++) {
        if (region[j] != 0) {
            region[j] = 0;
            printf("Error: Modified memory outside the managed region. index: %d\n", j - 500);
        }
    }
    float result = ((float) mallocated_count / allocated_count) * 100;
    float result_bytes = ((float) mallocated / allocated) * 100;
    printf("Memory size of %d bytes: allocated %.2f%% blocks (%.2f%% bytes).\n", random_memory, result, result_bytes);
}

/*
int main() {
    char region[100000];
    char *pointer[13000];
    z1_testovac(region, pointer, 8, 24, 50, 100, 1);
    z1_testovac(region, pointer, 8, 1000, 10000, 20000, 0);
    z1_testovac(region, pointer, 8, 35000, 50000, 99000, 0);
    return 0;
}
*/
int rand_int(int min, int max) {
    return (rand() % (max + 1 - min) + min);
}

void frag_printout(int size) {
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

    char region1[50];
    char region2[100];
    char region3[200];
    char *ptr[2];
    char *ptr2[4];
    char *ptr3[8];
    memory_init(region1, 50);
    printf("/-------------------TEST1a---------------/\n");
    printf("Region size = 50\nHead size = %d\nMaximum size you can alloc: %d\n", block_size, 50 - (2 * block_size) - 1);
    int a = 8;
    for (int i = 0; i < 2; i++) {
        ptr[i] = memory_alloc(a);
    }
    frag_printout(50);
    iterate();

    //ak len alokujem
    //spocitam si vsetky hlavicky
    //kolko z celkovej alokacie tvoria hlavicky
    //toto je framgentácia

    //5 hlaviciek
    // a 5 blokov
    //Bloky co som alokoval(5) / cela velkost je % co sa mi podarilo zaplnit
    a = 11;
    for (int i = 0; i < 2; i++) {
        ptr[i] = memory_alloc(a);
        if (i % 2 == 0) {
            if (memory_check(ptr[i])) {
                memory_free(ptr[i]);
            }
        }
    }
    a = 21;
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
    int random = rand_int(8, 5000);
    printf("\n\n/-------------------TEST4---------------/\n");
    printf("Region size = 100000\nHead size = %d\nMaximum size you can alloc: %d\n", block_size,
           100000 - (2 * block_size) - 1);
    memory_init(region1, 100000);
    while ((ptr[i++] = memory_alloc(random)) != NULL) {
        random = rand_int(8, 50000);

    }
    /*
    for (int j = 0;; j++) {
        if (memory_check(ptr[j * 2]) && memory_check(ptr[j * 2 + 1])) {
            memory_free(ptr[j * 2]);
            memory_free(ptr[j * 2 + 1]);
        }
        if(j==3){
            break;
        }
    }*/

    iterate();
    frag_printout(100000);

}


int main() {
    srand(time(0));
    char region[100];

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