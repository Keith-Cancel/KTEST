#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ktest.h"
#include "console.h"

typedef struct test_case_s {
    int (*test_func)(TestStatus*);
    char* name;
    char* description;
    int   status;
} TestCase;

struct test_list_s {
    size_t    count;
    size_t    capacity;
    TestCase* tests;
};

int ktest_add_test_case(TestList* list, int (*test_func)(TestStatus*), const char* name, const char* description) {
    if(list->count >= list->capacity) {
        size_t new_cap = list->capacity + 16;
        TestCase* new  = realloc(list->tests, sizeof(TestCase) * new_cap);
        if(new == NULL) {
            return KTEST_REALLOC_FAIL;
        }
        list->tests    = new;
        list->capacity = new_cap;
    }

    TestCase* cur    = &(list->tests[list->count]);
    cur->test_func   = test_func;
    cur->name        = NULL;
    cur->description = NULL;
    cur->status      = 0;

    size_t name_len  = strlen(name) + 1;
    size_t desc_len  = strlen(description) + 1;

    cur->name = malloc(name_len);
    if(cur->name == NULL) {
        return KTEST_MALLOC_FAIL;
    }

    cur->description = malloc(desc_len);
    if(cur->description == NULL) {
        free(cur->name);
        cur->name = NULL;
        return KTEST_MALLOC_FAIL;
    }

    memcpy(cur->name, name, name_len);
    memcpy(cur->description, description, desc_len);
    list->count++;
    return KTEST_SUCCESS;
}

void ktest_free_test_list(TestList* list) {
    for(size_t i = 0; i < list->count; i++) {
        TestCase* cur = list->tests + i;
        free(cur->name);
        cur->name = NULL;
        free(cur->description);
        cur->description = NULL;
    }
    free(list->tests);
    list->count    = 0;
    list->capacity = 0;
    list->tests    = NULL;
}