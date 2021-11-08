#ifndef KTEST_C_H
#define KTEST_C_H

struct test_list_s;
typedef struct test_list_s TestList;

struct test_status_s;
typedef struct test_status_s TestStatus;


int ktest_main(int argc, char** argv, const char* name, int (*test_setup)(TestList*));
int ktest_add_test_case(TestList* list, int (*test_func)(TestStatus*), const char* name, const char* description);
int ktest_clean_up();

void test_status_set_result(TestStatus* status, int result);

#define _KTEST_GENERAL_ERR  0x0000
#define _KTEST_MEMORY_ERR   0xF000

// General Statuses
#define KTEST_SUCCESS        _KTEST_GENERAL_ERR
#define KTEST_UNKNOWN_ERR    (0x0FFF | _KTEST_GENERAL_ERR)
// Memory Statuses
#define KTEST_MALLOC_FAIL    (0x0001 | _KTEST_MEMORY_ERR)
#define KTEST_REALLOC_FAIL   (0x0002 | _KTEST_MEMORY_ERR)
#define KTEST_UNKOWN_MEM     (0xFFFF | _KTEST_MEMORY_ERR)

#define KTEST_SETUP(NAME) \
    int ktest_setup_##NAME(TestList* ktest_list__); \
    int main(int argc, char **argv) { \
        return ktest_main(argc, argv, #NAME, ktest_setup_##NAME); \
    } \
    int ktest_setup_##NAME(TestList* ktest_list__)

#define KTEST_ADD_CASE(TEST_FUNC, NAME) KTEST_ADD_CASE_EX(TEST_FUNC, NAME, "")

#define KTEST_ADD_CASE_EX(TEST_FUNC, NAME, DESCRIPTION) \
    do { \
        int ktest_err = ktest_add_test_case(ktest_list__, ktest_test_case_##TEST_FUNC, NAME, DESCRIPTION); \
        if(ktest_err != 0) { \
            return ktest_err; \
        } \
        return KTEST_SUCCESS; \
    } while (0)

#endif

#define KTEST_CASE(NAME) int ktest_test_case_##NAME(TestStatus* status__)

#define KTEST_CASE_PROTO(NAME) KTEST_CASE(NAME)

#define KTEST_PRINTF(...) \
    do { \
        if(status__->output) { \
            fprintf(status__->output, __VA_ARGS__); \
        } \
    } while(0)
