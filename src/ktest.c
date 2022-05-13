#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ktest.h"
#include "console.h"
#include "timer.h"

typedef struct test_case_s {
    tcFn   test_func;
    fixFn  setup;
    tearFn tear;
    char*  name;
    char*  description;
    size_t fix_sz;
    int    status;
    int    skip;
} TestCase;

struct test_list_s {
    size_t    count;
    size_t    capacity;
    TestCase* tests;
};

int ktest_add_test_case(size_t* handle, kTestList* list, tcFn test_func, const char* name, const char* description) {
    if(list->count >= list->capacity) {
        size_t new_cap = list->capacity ? list->capacity * 2 : 16;
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
    cur->fix_sz      = 0;
    cur->status      = 0;
    cur->skip        = 0;

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

static void ktest_free_tests(kTestList* list) {
    for(size_t i = 0; i < list->count; i++) {
        free(list->tests[i].name);
        free(list->tests[i].description);
    }
    free(list->tests);
}

int ktest_set_fixture(size_t handle, kTestList* list, fixFn setup, tearFn teardown, size_t fixture_size) {
    if(handle >= list->count) {
        return KTEST_BAD_HANDLE;
    }
    list->tests[handle].setup  = setup;
    list->tests[handle].tear   = teardown;
    list->tests[handle].fix_sz = fixture_size;
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

int ktest_run_test_case(outputInfo* out, TestCase* tc) {
    char        buffer[14]  = { 0 };
    timerData   t           = { 0 };
    void*       fix         = NULL;
    kTestStatus stat        = {
        .output = out->output
    };
    fprintf(out->output, "+===========================+\n");
    fprintf(
        out->output,
        "[    %sRunning%s : %s%-13s%s]\n",
        out->fg.l_blue,
        out->reset,
        out->fg.l_cyan,
        tc->name,
        out->reset
    );

    if(tc->fix_sz) {
        fix = malloc(tc->fix_sz);
        if(fix == NULL) {
            fprintf(out->output, "%s+===========================+\n", out->fg.l_red);
            fprintf(out->output, "| ALLOCATING FIXTURE FAILED |\n");
            fprintf(out->output, "+===========================+%s\n", out->reset);
            return 1;
        }
        memset(fix, 0, tc->fix_sz);
    }

    timer_start(&t);
    if(tc->setup != NULL) {
        tc->setup(&stat, fix);
    }

    tc->test_func(&stat, fix);

    if(tc->tear  != NULL) {
        tc->tear(&stat, fix);
    }
    timer_stop(&t);
    free(fix);
    timer_get_str(&t, buffer);

    if(stat.result) {
        fprintf(out->output, " Expects Ran : %u\n", stat.expects);
        fprintf(out->output, " Asserts Ran : %u\n", stat.asserts);
    }
    fprintf(
        out->output,
        "[     %sResult%s : %s%-13s%s]\n",
        out->fg.l_cyan,
        out->reset,
        stat.result ? out->fg.l_red : out->fg.l_green,
        stat.result ? "Failed" : "Passed",
        out->reset
    );
    fprintf(
        out->output,
        "[       %sTime%s : %s%-13s%s]\n",
        out->fg.l_yellow,
        out->reset,
        out->fg.l_magenta,
        buffer,
        out->reset
    );
    return stat.result;
}

int ktest_run_tests(outputInfo* out, const char* name, const kTestList* list) {
    fprintf(out->output, "+===========================+\n");
    fprintf(
        out->output,
        "| %sRunning%s: %s%-17s%s|\n",
        out->fg.l_blue,
        out->reset,
        out->fg.l_cyan,
        name,
        out->reset
    );
    fprintf(
        out->output,
        "| %sTest Cases%s: %s%-14zu%s|\n",
        out->fg.l_blue,
        out->reset,
        out->fg.l_magenta,
        list->count,
        out->reset
    );
    int failures = 0;
    int passed   = list->count;
    timerData  t = { 0 };

    timer_start(&t);
    for(size_t i = 0; i < list->count; i++) {
        failures += ktest_run_test_case(out, &(list->tests[i]));
    }
    timer_stop(&t);

    passed -= failures;
    fprintf(out->output, "+===========================+\n");
    fprintf(
        out->output,
        "Summary for %s'%s'%s\n",
        out->fg.l_cyan,
        name,
        out->reset
    );
    fprintf(
        out->output,
        "Test Cases %sPassed%s : %s%d%s\n",
        passed > 0 ? out->fg.l_green : out->reset,
        out->reset,
        out->fg.l_magenta,
        passed,
        out->reset
    );
    fprintf(
        out->output,
        "Test Cases %sFailed%s : %s%d%s\n",
        failures ? out->fg.l_red : out->reset,
        out->reset,
        out->fg.l_magenta,
        failures,
        out->reset
    );

    char buffer[14] = { 0 };
    timer_get_str(&t, buffer);
    fprintf(
        out->output,
        "       %sTotal Time%s : %s%s%s\n",
        out->fg.l_yellow,
        out->reset,
        out->fg.l_magenta,
        buffer,
        out->reset
    );
    return failures;
}

int ktest_str_eq(FILE* out, const char* file, unsigned line, const char* str1, const char* str2) {
    const char* cur1 = str1;
    const char* cur2 = str2;
    // Compare manually instead of using strcmp so I know where they differ.
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
            fprintf(out, "%s%c%s%s\n", get_fg_color_if_tty(L_RED, out), *cur1, get_reset_if_tty(out), cur1 + 1);
        }
        // print pointer to where it first differs
        fprintf(out, "               "); // Padding
        for(size_t i = 0; i < good_bytes; i++) {
            fprintf(out, " ");// pad till reaching the point of difference.
        }
        fprintf(out, "%s^%s\n\n", get_fg_color_if_tty(L_RED, out), get_reset_if_tty(out));
        return 1;
    }
    return 0;
}

int ktest_str_ne(FILE* out, const char* file, unsigned line, const char* str1, const char* str2) {
    if(strcmp(str1, str2) == 0) {
        fprintf(out, "Test Failure : %s:%u\n", file, line);
        fprintf(out, "Not Expected : %s\n", str2);
        fprintf(out, "      Actual : %s%s%s\n\n", get_fg_color_if_tty(L_RED, out), str2, get_reset_if_tty(out));
        return 1;
    }
    return 0;
}

int ktest_main(int argc, char** argv, const char* name, int (*test_setup)(kTestList*, char**, int*)) {
    console_init();
    kTestList list  = { 0 };
    char  no_file[] = "NO FILE";
    int   line      = -1;
    char* file      = no_file;
    int   ret       = -1;
    outputInfo out  = { 0 };
    console_set_output_info(&out, stdout);

    fprintf(
        out.output,
        "Setting Up: %s'%s'%s\n",
        out.fg.l_cyan,
        name,
        out.reset
    );

    ret = test_setup(&list, &file, &line);
    if(ret != KTEST_SUCCESS) {
        fprintf(
            out.output,
            "%sSetup Failure%s: %s:%d\n",
            out.fg.l_red,
            out.reset,
            file,
            line
        );
        printf("Return Code: %d\n", ret);
        ktest_free_tests(&list);
        return EXIT_FAILURE;
    }
    fprintf(
        out.output,
        "Setup: %sDone%s\n",
        out.fg.l_green,
        out.reset
    );
    ret = ktest_run_tests(&out, name, &list);
    ktest_free_tests(&list);

    if(ret) {
        return EXIT_FAILURE;
    }
    // Just here temporary so I don't see unused variable warnings
    char b = argv[argc-1][0];
    return EXIT_SUCCESS + b;
}