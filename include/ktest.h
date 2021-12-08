#ifndef KTEST_C_H
#define KTEST_C_H

#include <inttypes.h>
#include <stdio.h>

struct test_list_s;
typedef struct test_list_s kTestList;

struct ktest_status_s {
    FILE* output;
    int   result;
};
typedef struct ktest_status_s kTestStatus;

typedef int (*tcFn) (kTestStatus*, void*);
typedef int (*fixFn)(kTestStatus*, void*);
typedef int (*tearFn)(kTestStatus*, void*);

int ktest_main(int argc, char** argv, const char* name, int (*test_setup)(kTestList*));
int ktest_add_test_case(unsigned* handle, kTestList* list, tcFn test_func, const char* name, const char* description);
int ktest_set_fixture(unsigned handle, kTestList* list, fixFn setup, tearFn teardown);
int ktest_clean_up();

int ktest_str_eq(FILE* out, const char* file, unsigned line, const char* str1, const char* str2);
int ktest_str_ne(FILE* out, const char* file, unsigned line, const char* str1, const char* str2);

#define _KTEST_GENERAL_ERR  0x0000
#define _KTEST_MEMORY_ERR   0xF000

// General Statuses
#define KTEST_SUCCESS        _KTEST_GENERAL_ERR
#define KTEST_BAD_HANDLE     (0x0001 | _KTEST_GENERAL_ERR)
#define KTEST_UNKNOWN_ERR    (0x0FFF | _KTEST_GENERAL_ERR)
// Memory Statuses
#define KTEST_MALLOC_FAIL    (0x0001 | _KTEST_MEMORY_ERR)
#define KTEST_REALLOC_FAIL   (0x0002 | _KTEST_MEMORY_ERR)
#define KTEST_UNKOWN_MEM     (0x0FFF | _KTEST_MEMORY_ERR)

#define KTEST_CASE(NAME)          int ktest_case_##NAME(kTestStatus* status__, void* fix)
#define KTEST_CASE_FIX(NAME, FIX) int ktest_case_##NAME(kTestStatus* status__, struct FIX * fix)

#define KTEST_FIX(NAME)          int ktest_fixture_##NAME(kTestStatus* status__, struct NAME* fix)
#define KTEST_FIX_TEARDOWN(NAME) int ktest_teardown_##NAME(kTestStatus* status__, struct NAME* fix)

#define KTEST_SETUP(NAME) \
    int ktest_setup_##NAME(kTestList* ktest_list__); \
    int main(int argc, char **argv) { \
        return ktest_main(argc, argv, #NAME, ktest_setup_##NAME); \
    } \
    int ktest_setup_##NAME(kTestList* ktest_list__)

#define KTEST_ADD_CASE(NAME, HANDLE_OUT) KTEST_ADD_CASE_EX(NAME, HANDLE_OUT, "")

#define KTEST_ADD_CASE_EX(NAME, HANDLE_OUT, DESCRIPTION) \
    do { \
        int ktest_err = ktest_add_test_case((HANDLE_OUT), ktest_list__, (tcFn)ktest_case_##NAME, #NAME, DESCRIPTION); \
        if(ktest_err != KTEST_SUCCESS) { \
            return ktest_err; \
        } \
    } while (0)

#define KTEST_SET_FIXTURE(HANDLE, NAME) \
    do { \
        int ktest_err = ktest_set_fixture((HANDLE), (fixFn)ktest_fixture_##NAME, (tearFn)ktest_teardown_##NAME); \
        if(ktest_err != KTEST_SUCCESS) { \
            return ktest_err; \
        } \
    } while (0)

#define KTEST_PRINTF(...) \
    do { \
        if(status__->output) { \
            fprintf(status__->output, __VA_ARGS__); \
        } \
    } while(0)

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define KTEST_VAL_PRINT(x)                  \
    KTEST_PRINTF(_Generic((x),              \
            signed char   : "%u",           \
            unsigned char : "%d",           \
            unsigned short: "%u",           \
            short         : "%d",           \
            unsigned int  : "%u",           \
            int           : "%d",           \
            unsigned long : "%lu",          \
            long          : "%ld",          \
            long long     : "%"PRId64,      \
            unsigned long long: "%"PRIu64,  \
            float         : "%f",           \
            double        : "%f",           \
            long double   : "%Lf",          \
            default  : _Generic(((x) - (x)),\
                ptrdiff_t: "0x%p",          \
                default  : "0x%"PRIx64      \
            )                               \
        ), (x))
#else
// No _Generic() so just print as a hex value
#define KTEST_VAL_PRINT(x) KTEST_PRINTF("0x%"PRIx64, (uint64_t)(x))
#endif

#define K_ASSERT_EQ_TRUE(x) \
    do {                    \
        if ((x) != 1) {     \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Expected : true\n");     \
            if((x) == 0) {                             \
                KTEST_PRINTF("      Actual : false\n");\
            } else {                                   \
                KTEST_PRINTF("      Actual : ");       \
                KTEST_VAL_PRINT(x);                    \
                KTEST_PRINTF("\n");                    \
            }                                          \
            status__->result = 1;                      \
            return;                                    \
        }                                              \
    } while (0)

#define K_ASSERT_EQ_FALSE(x) \
    do {                     \
        if ((x) != 0) {      \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Expected : false\n");    \
            if((x) == 1) {                             \
                KTEST_PRINTF("      Actual : true\n"); \
            } else {                                   \
                KTEST_PRINTF("      Actual : ");       \
                KTEST_VAL_PRINT(x);                    \
                KTEST_PRINTF("\n");                    \
            }                                          \
            status__->result = 1;                      \
            return;                                    \
        }                                              \
    } while (0)

#define K_EXPECT_EQ_TRUE(x) \
    do {                    \
        if ((x) != 1) {     \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Expected : true\n");     \
            if((x) == 0) {                             \
                KTEST_PRINTF("      Actual : false\n");\
            } else {                                   \
                KTEST_PRINTF("      Actual : ");       \
                KTEST_VAL_PRINT(x);                    \
                KTEST_PRINTF("\n");                    \
            }                                          \
            status__->result = 1;                      \
        }                                              \
    } while (0)

#define K_EXPECT_EQ_FALSE(x) \
    do {                     \
        if ((x) != 0) {      \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Expected : false\n");    \
            if((x) == 1) {                             \
                KTEST_PRINTF("      Actual : true\n"); \
            } else {                                   \
                KTEST_PRINTF("      Actual : ");       \
                KTEST_VAL_PRINT(x);                    \
                KTEST_PRINTF("\n");                    \
            }                                          \
            status__->result = 1;                      \
        }                                              \
    } while (0)

#define K_ASSERT(x, y, cmp)  \
    do {                     \
        if(!((x) cmp (y))) { \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Expected : {value} %s ", #cmp); \
            KTEST_VAL_PRINT(y);                   \
            KTEST_PRINTF("\n");                   \
            KTEST_PRINTF("      Actual : ");      \
            KTEST_VAL_PRINT(x);                   \
            KTEST_PRINTF(" %s ", #cmp);           \
            KTEST_VAL_PRINT(y);                   \
            KTEST_PRINTF("\n");                   \
            status__->result = 1;                 \
            return;                               \
        }                                         \
    } while(0)

#define K_EXPECT(x, y, cmp)  \
    do {                     \
        if(!((x) cmp (y))) { \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Expected : {value} %s ", #cmp); \
            KTEST_VAL_PRINT(y);                   \
            KTEST_PRINTF("\n");                   \
            KTEST_PRINTF("      Actual : ");      \
            KTEST_VAL_PRINT(x);                   \
            KTEST_PRINTF(" %s ", #cmp);           \
            KTEST_VAL_PRINT(y);                   \
            KTEST_PRINTF("\n");                   \
            status__->result = 1;                 \
        }                                         \
    } while(0)

#define K_ASSERT_EQ(x, y) K_ASSERT(x, y, ==)
#define K_ASSERT_LT(x, y) K_ASSERT(x, y, <)
#define K_ASSERT_LE(x, y) K_ASSERT(x, y, <=)
#define K_ASSERT_GT(x, y) K_ASSERT(x, y, >)
#define K_ASSERT_GE(x, y) K_ASSERT(x, y, >=)
#define K_ASSERT_NE(x, y) K_ASSERT(x, y, !=)

#define K_EXPECT_EQ(x, y) K_EXPECT(x, y, ==)
#define K_EXPECT_LT(x, y) K_EXPECT(x, y, <)
#define K_EXPECT_LE(x, y) K_EXPECT(x, y, <=)
#define K_EXPECT_GT(x, y) K_EXPECT(x, y, >)
#define K_EXPECT_GE(x, y) K_EXPECT(x, y, >=)
#define K_EXPECT_NE(x, y) K_EXPECT(x, y, !=)

#define K_ASSERT_STR_EQ(x, y)     \
    do {                          \
        if( ktest_str_eq(status__->output, __FILE__, __LINE__, (x), (y)) ) { \
            status__->result = 1; \
            return;               \
        }                         \
    } while(0)

#define K_ASSERT_STR_NE(x, y)     \
    do {                          \
        if( ktest_str_ne(status__->output, __FILE__, __LINE__, (x), (y)) ) { \
            status__->result = 1; \
            return;               \
        }                         \
    } while(0)

#define K_EXPECT_STR_EQ(x, y)     \
    do {                          \
        if( ktest_str_eq(status__->output, __FILE__, __LINE__, (x), (y)) ) { \
            status__->result = 1; \
        }                         \
    } while(0)

#define K_EXPECT_STR_NE(x, y)     \
    do {                          \
        if( ktest_str_ne(status__->output, __FILE__, __LINE__, (x), (y)) ) { \
            status__->result = 1; \
        }                         \
    } while(0)

#endif