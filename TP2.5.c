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
	HEADER **bloc = &list;
	//checking available blocks
	while (*bloc != NULL) {
        HEADER *h = *bloc;
        //checking if it is possible to create another block
        if (h->bloc_size > size){
			size_t size_rest= h->bloc_size - size;
			// the new block must be big enough to include a header and the magic number (and at least one byte for the memory bloc)
			if (size_rest > (sizeof(HEADER) + sizeof(unsigned long)) ){
				 HEADER *new = (HEADER *)((char *)(h + 1) + size + sizeof(unsigned long));
				 new->bloc_size = size_rest-(sizeof(HEADER) + sizeof(unsigned long));
				 new->magic_number= MAGIC_NUMBER;
				 new->ptr_next = h->ptr_next;
				 *bloc = new;
				 h->bloc_size = size;
                 h->ptr_next = NULL;
                 
                 unsigned long magic = MAGIC_NUMBER;
                 
				 void *h_end = (char *)(h + 1) + h->bloc_size;
				 memcpy(h_end, &magic, sizeof(magic));
				 void *new_end = (char *)(h + 1) + new->bloc_size;
				 memcpy(new_end, &magic, sizeof(magic));
				 return (void *)(h+1);
			}
		}
		else if (h->bloc_size == size){
			*bloc = h->ptr_next;
			h->ptr_next = NULL;
			return (void *)(h+1);
		}
		bloc = &h->ptr_next;
	}
	//no block available
	HEADER *h = (HEADER *)sbrk(sizeof(HEADER) + size + sizeof(unsigned long));
	h->ptr_next = NULL;
	h->bloc_size=size;
	h->magic_number=MAGIC_NUMBER;
	
	//assigning the magic_number at the end of the bloc
	unsigned long magic = MAGIC_NUMBER;
    void *bloc_end = (char *)(h + 1) + size;
    memcpy(bloc_end, &magic, sizeof(magic));
	return (void *)(h+1);
}

void free_3IS(void* ptr){
	if (ptr == NULL){
        return;
        }
        
     HEADER *h = (HEADER *)ptr-1;   
     if (!check_bloc(h)){
                return;
				}
				
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
	char *p1 = malloc_3IS(50);
    char *p2 = malloc_3IS(15);

	strcpy(p1, "Hello world");
    strcpy(p2, "second test");

    printf("p1 = %s\n", p1);
    printf("p2 = %s\n", p2);
    
    printf("freeing blocks \n");
	free_3IS(p1);
	free_3IS(p2);
	show_list_elements(list);
	
	printf("allocating \n");
	//will separate the first block in two
	char *p3 = malloc_3IS(15);
	//will create another bloc
	char *p4 = malloc_3IS(10);
	show_list_elements(list);
	
	printf("freeing blocks \n");
	free_3IS(p3);
	free_3IS(p4);
	show_list_elements(list);
	return 0;
}




