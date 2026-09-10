#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "memory.h"

#define SMALL_SIZE 128
#define LARGE_COUNT 100000
#define MANY_COUNT 500000

// makes a malloc call, uses the memory, and then frees
int test_one() {
    char *ptr = (char *)new_malloc(SMALL_SIZE);
    printf("%p\n", ptr);
    if (!ptr) return 0;

    printf("here\n");

    // use the memory
    memset(ptr, 0xAB, SMALL_SIZE);

    // simple correctness check
    for (int i = 0; i < SMALL_SIZE; i++) {
        if (ptr[i] != (char)0xAB) {
            printf("corrupt");
            new_free(ptr);
            return 0;
        }
    }

    new_free(ptr);
    return 1;
}


// makes many malloc calls, records when first failure
int test_no_free() {
    int count = 0;
    while (count < LARGE_COUNT) {
        void *ptr = new_malloc(SMALL_SIZE);
        if (!ptr) {
            printf("First failure at allocation %d\n", count);
            return 1;  // expected eventual failure
        }
        count++;
    }

    printf("No failure after %d allocations\n", LARGE_COUNT);
    return 1;
}


// makes many malloc/free calls
int test_many() {
    for (int i = 0; i < MANY_COUNT; i++) {
        unsigned char *ptr = new_malloc(SMALL_SIZE);
        printf("recv %p\n",ptr);
        if (!ptr) return 0;

        memset(ptr, i % 256, SMALL_SIZE);

        for (int j = 0; j < SMALL_SIZE; j++) {
            if (ptr[j] != i % 256) {
                printf("Corrupt %d != %d", ptr[j], i%256);
                new_free(ptr);
                return 0;
            }
        }
        new_free(ptr);
    }
    return 1;
}


// makes many malloc/free calls and outputs timing and failure data
int test_many_output_stats(int max_alloc_size) {
     clock_t start = clock();
    srand((unsigned)time(NULL));

    void *ptrs[MANY_COUNT] = {0};
    int top = -1;
    int failures = 0;

    for (int i = 0; i < MANY_COUNT; i++) {

        // Free the top element 90% of the time
        if(rand() % 100 < 90 && (top != -1)) {
            int prev_alloc = top;
            top -= 1;
            if (ptrs[prev_alloc]) {
                new_free(ptrs[prev_alloc]);
                ptrs[prev_alloc] = NULL; 
            } 
        }

        size_t size = (rand() % max_alloc_size) + 1;  // random size 1..max_alloc_size
        void *ptr = new_malloc(size);

        if (!ptr) {
            failures++;
            continue;
        }

        memset(ptr, 0, size);
        top += 1;
        ptrs[top] = ptr;
    }

    for (int i = 0; i < top; i++) {
        if (ptrs[i]) {
            new_free(ptrs[i]);
        }
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Allocations attempted: %d\n", MANY_COUNT);
    printf("Failures: %d\n", failures);
    printf("Elapsed time: %.6f seconds\n", elapsed);
    printf("Fail Rate: %lf\n", (double)failures / (double)MANY_COUNT);
    printf("Throughput: %f\n", (double) MANY_COUNT / elapsed);

    return failures < MANY_COUNT * 0.3;
}

int test_many_output_stats32() {
    printf("Benchmark 32 Byte Allocation Maximum\n");
    return test_many_output_stats(32);
}

int test_many_output_stats64() {
    printf("Benchmark 64 Byte Allocation Maximum\n");
    return test_many_output_stats(64);
}

int test_many_output_stats128() {
    printf("Benchmark 128 Byte Allocation Maximum\n");
    return test_many_output_stats(128);
}

int test_many_output_stats256() {
    printf("Benchmark 256 Byte Allocation Maximum\n");
    return test_many_output_stats(256);
}


int run_test(char * test_name, int (*test_func)()) {
    int test_res = test_func();
    printf("Test %-25s: %d/1\n", test_name, test_res);
    return test_res;
}

int main(int argc, char **argv){
    if(argc != 2) {
        printf("ERROR: expected format ./test_one <test_num>\n");
        return -1;
    }
    int passed = 0;
    int test_num = atoi(argv[1]);
    
    char *test_list[] = {"Test One Malloc","Test Many Malloc", "Test Many Malloc/Free", "Test Many Malloc/Free Stats", "Bench32", "Bench64", "Bench128","Bench256"};
    int (*test_func[])() = {&test_one, &test_no_free, &test_many, &test_many_output_stats128, &test_many_output_stats32, &test_many_output_stats64, &test_many_output_stats128, &test_many_output_stats256};

    passed += run_test(test_list[test_num], test_func[test_num]);
    int tests_ran = 1;

    printf("Total: %d/%d\n", passed, tests_ran);
    return passed;
}