#include "memory.h"
#include <sys/mman.h>
#include <stddef.h>
#include <stdio.h>

typedef struct header {
    size_t size;
    struct header * prev;
    struct header * next;
    int in_use;
} m_header;

const size_t HEADER_SIZE = sizeof(m_header);
m_header* freelist = NULL;

void print_freelist() {
    m_header* curr = freelist;
    while(curr != NULL) {
        printf("[%p: size:%lu prev:%p next:%p use:%d]\n", curr, curr->size, curr->prev, curr->next, curr->in_use);
        curr = curr->next;
    }
    printf("\n --- \n");
}

void * new_malloc(size_t size) {
    // round up to nearest multiple of 16
    size_t remainder = size % 16;
    if (remainder != 0) {
        size = size + (16 - remainder);
    }

    // freelist 1 time initialization
    if(freelist == NULL) {
        printf("MMAP\n");
         // Use mmap to get anonymous, private memory
        freelist = mmap(NULL,                    // Desired start address (NULL lets OS choose)
                      2048,                  // Length of the mapping (rounded up to page size)
                      PROT_READ | PROT_WRITE,  // Memory protection: readable and writable
                      MAP_PRIVATE | MAP_ANONYMOUS, // Visibility: private to the process, not file-backed
                      -1,                      // File descriptor: -1 for anonymous mapping
                      0);                      // Offset: 0 for anonymous mapping

        if (freelist == MAP_FAILED) {
            printf("map failed\n");
            return NULL;
        }
        // setting address values
        freelist->size = 2048 - HEADER_SIZE;
        freelist->prev = NULL;
        freelist->next = NULL;
        freelist->in_use = 0;
    }

    m_header *curr = freelist;

    while (curr != NULL) {
        // checks if free and enough space
        if (curr->in_use == 0 && size <= curr->size) {

            // checks if we need to split
            if (size + HEADER_SIZE < curr->size) {
                // create a new section
                m_header *new_temp = (m_header *)((char *)curr   // cast to byte, then back
                                    + HEADER_SIZE     // header size
                                    + size               // data size
                                    );

                new_temp->size = curr->size - size - HEADER_SIZE;
                new_temp->in_use = 0;
                new_temp->next = curr->next;
                new_temp->prev = curr;

                if (curr->next != NULL) {
                    curr->next->prev = new_temp;
                }

                curr->next = new_temp;
                curr->size = size;
            }
            break;
        }
        curr = curr->next;
    }

    if (curr == NULL) {
        return NULL;
    }

    curr->in_use = 1;
    return (void *)((char *)curr + HEADER_SIZE); // ptr of data section
    
}

void new_free(void * ptr) {
    if (ptr == NULL) {
        return;
    }
    // freeing
    m_header *cur = (m_header *)((char *)ptr - HEADER_SIZE);
    cur->in_use = 0;

    // coalescing next
    if (cur->next != NULL && cur->next->in_use == 0) {
        cur->size += HEADER_SIZE + cur->next->size;
        cur->next = cur->next->next;
        if (cur->next != NULL) {
            cur->next->prev = cur;
        }
    }
    // completing proof with coalescing prev
    if (cur->prev != NULL && cur->prev->in_use == 0) {
        cur->prev->size += HEADER_SIZE + cur->size;
        cur->prev->next = cur->next;
        if (cur->next != NULL) {
            cur->next->prev = cur->prev;
        }
    }
    return;
}