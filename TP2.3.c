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

static HEADER *list = NULL;

//check value of both magic numbers
int check_bloc(HEADER* h){
	if ((h->magic_number) != (long)MAGIC_NUMBER){
		printf("memory corrupted\n");
        return 0;
		}
	unsigned long *bloc_end =(unsigned long *)((char *)(h + 1) + h->bloc_size);
	if (*bloc_end != MAGIC_NUMBER){
		printf("memory corrupted\n");
		return 0;
	}
	return 1;
	}

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
	if (ptr == NULL){
        return;
        }
        
     HEADER *h = (HEADER *)ptr-1;   
     if (!check_bloc(h)){
		 return;}
		 
	HEADER **bloc = &list;
    while (*bloc != NULL && *bloc < h)
        bloc = &(*bloc)->ptr_next;
    h->ptr_next = *bloc;
    *bloc = h;
	}

void show_list_elements(HEADER* list){
	HEADER* element = list;
    while (element != NULL)
    {
        printf("adress: %p, value: %ld\n",(void *)element,element->bloc_size);
        element = element->ptr_next;
	}
}

        
int main() {
	char *p1 = malloc_3IS(20);
    char *p2 = malloc_3IS(15);
    
    // this will cause a memory overflow
	char *p3 = malloc_3IS(5);
    
	strcpy(p1, "Hello world");
    strcpy(p2, "second test");
    strcpy(p3, "third test");

    printf("p1 = %s\n", p1);
    printf("p2 = %s\n", p2);
    printf("p3 = %s\n", p3);
    printf("freeing blocks \n");
	free_3IS(p1);
	free_3IS(p2);
	free_3IS(p3);
	show_list_elements(list);
	return 0;
}


