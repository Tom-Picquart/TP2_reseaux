#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAGIC_NUMBER 0xBADA110CBADA110CUL

typedef struct HEADER_TAG {
	struct HEADER_TAG * ptr_next; /* pointer towards the next free block */
	size_t bloc_size; /* size of the memory block in Bytes*/
	long magic_number; /* 0xBADA110CBADA110CL */
} HEADER;

void* malloc_3IS(size_t size){
	if (size==0){
		return NULL;
		}
	//in this scenario the stack is increased every allocation
	HEADER *h = (HEADER *)sbrk(sizeof(HEADER) + size + sizeof(unsigned long));
	h->ptr_next = NULL;
	h->bloc_size=size;
	h->magic_number=MAGIC_NUMBER;
	
	//assigning the magic_number at the end of the bloc
	unsigned long magic = MAGIC_NUMBER;
    void *bloc_end = (char *)(h + 1) + size;
    memcpy(bloc_end, &magic, sizeof(magic));
	return (void *)(h + 1);
}

void free_3IS(void* ptr){
	
	}



int main() {
	char *p1 = malloc_3IS(20);
    char *p2 = malloc_3IS(1);

	strcpy(p1, "Hello world");
    strcpy(p2, "second test");

    printf("p1 = %s\n", p1);
    printf("p2 = %s\n", p2);

	return 0;
}
