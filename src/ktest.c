#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ktest.h"
#include "console.h"

typedef struct test_case_s {
    tcFn   test_func;
    fixFn  setup;
    tearFn tear;
    char*  name;
    char*  description;
    int    status;
} TestCase;

struct test_list_s {
    size_t    count;
    size_t    capacity;
    TestCase* tests;
};

int ktest_add_test_case(unsigned* handle, kTestList* list, tcFn test_func, const char* name, const char* description) {
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
    cur->setup       = NULL;
    cur->tear        = NULL;
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
    *handle = list->count;
    list->count++;
    return KTEST_SUCCESS;
}

int ktest_set_fixture(unsigned handle, kTestList* list, fixFn setup, tearFn teardown) {
    if(handle >= list->count) {
        return KTEST_BAD_HANDLE;
    }
    list->tests[handle].setup = setup;
    list->tests[handle].tear  = teardown;
    return KTEST_SUCCESS;
}

void ktest_free_test_list(kTestList* list) {
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

int ktest_str_eq(FILE* out, const char* file, unsigned line, const char* str1, const char* str2) {
    const char* cur1 = str1;
    const char* cur2 = str2;
    while(*cur1 && *cur2 && (*cur1 == *cur2)) {
        cur1++;
        cur2++;
    }
    if(*cur1 != *cur2) {
        size_t good_bytes = cur1 - str1;
        char fmt[48] = { 0 };
        fprintf(out,      "Test Failure : %s:%u\n", file, line);
        fprintf(out,      "    Expected : %s\n", str2);
        snprintf(fmt, 47, "      Actual : %%.%zus", good_bytes);
        fprintf(out, fmt, str1);
        // Make sure it's not the NULL byte
        if(*cur1) {
            printf("%s%c%s%s\n", get_fg_color(L_RED), *cur1, get_reset(), cur1 + 1);
        }
        // print pointer to where it first differs
        fprintf(out, "             ");
        for(size_t i = 0; i < good_bytes; i++) {
            fprintf(out, " ");
        }
        fprintf(out, "%s^\n%s", get_fg_color(L_RED), get_reset());
        return 1;
    }
    return 0;
}

int ktest_str_ne(FILE* out, const char* file, unsigned line, const char* str1, const char* str2) {
    if(strcmp(str1, str2) == 0) {
        fprintf(out, "Test Failure : %s:%u\n", file, line);
        fprintf(out, "Not Expected : %s\n", str2);
        fprintf(out, "      Actual : %s%s%s\n", get_fg_color(L_RED), str2, get_reset());
        return 1;
    }
    return 0;
}

struct test_status_s {
    FILE* output;
};

int ktest_main(int argc, char** argv, const char* name, int (*test_setup)(kTestList*)) {
    console_init();
    int ret;
    kTestList list;
    char a = name[0];
    char b = argv[argc-1][0];
    ret = test_setup(&list);
    return EXIT_SUCCESS + ret + b + a;
}