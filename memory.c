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
        freelist->size = 2048 - sizeof(m_header);
        freelist->prev = NULL;
        freelist->next = NULL;
        freelist->in_use = 0;
    }

    m_header *curr = freelist;

    while (curr != NULL) {
        // checks if free and enough space
        if (curr->in_use == 0 && size <= curr->size) {

            // checks if we need to split
            if (size + sizeof(m_header) < curr->size) {
                // create a new section
                m_header *temp = (m_header *)((char *)curr   // cast to byte, then back
                                    + sizeof(m_header)     // header size
                                    + size               // data size
                                    );

                temp->size = curr->size - size - sizeof(m_header);
                temp->in_use = 0;
                temp->next = curr->next;
                temp->prev = curr;

                if (curr->next != NULL) {
                    curr->next->prev = temp;
                }

                curr->next = temp;
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
    return (void *)((char *)curr + sizeof(m_header)); // ptr of data section
    
}

void new_free(void * ptr) {

}