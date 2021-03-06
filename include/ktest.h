#ifndef KTEST_C_H
#define KTEST_C_H

#include <stddef.h>
#include <inttypes.h>
#include <stdio.h>

struct test_list_s;
typedef struct test_list_s kTestList;

struct ktest_status_s {
    FILE*    output;
    int      result;
    unsigned asserts;
    unsigned expects;
};
typedef struct ktest_status_s kTestStatus;

typedef void (*tcFn) (kTestStatus*, void*);
typedef void (*fixFn)(kTestStatus*, void*);
typedef void (*tearFn)(kTestStatus*, void*);

int ktest_main(int argc, char** argv, const char* name, int (*test_setup)(kTestList*, char**, int*));
int ktest_add_test_case(size_t* handle, kTestList* list, tcFn test_func, const char* name, const char* description);
int ktest_set_fixture(size_t handle, kTestList* list, fixFn setup, tearFn teardown, size_t fixture_size);

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

#define KTEST_CASE(NAME)          void ktest_case_##NAME(kTestStatus* status__, void* fix)
#define KTEST_CASE_FIX(NAME, FIX) void ktest_case_##NAME(kTestStatus* status__, struct FIX* fix)

#define KTEST_FIX(NAME)           void ktest_fixture_##NAME(kTestStatus* status__, struct NAME* fix)
#define KTEST_FIX_TEARDOWN(NAME)  void ktest_teardown_##NAME(kTestStatus* status__, struct NAME* fix)

#define KTEST_SETUP(NAME) \
    int ktest_setup_##NAME(kTestList* ktest_list__, char** ktest_file__, int* ktest_line__); \
    int main(int argc, char **argv) { \
        return ktest_main(argc, argv, #NAME, ktest_setup_##NAME); \
    } \
    int ktest_setup_##NAME(kTestList* ktest_list__, char** ktest_file__, int* ktest_line__)

#define KTEST_ADD_CASE(NAME, HANDLE_OUT) KTEST_ADD_CASE_EX(NAME, HANDLE_OUT, "")

#define KTEST_ADD_CASE_EX(NAME, HANDLE_OUT, DESCRIPTION) \
    do { \
        int ktest_err = ktest_add_test_case((HANDLE_OUT), ktest_list__, (tcFn)ktest_case_##NAME, #NAME, DESCRIPTION); \
        if(ktest_err != KTEST_SUCCESS) { \
            *ktest_file__ = __FILE__; \
            *ktest_line__ = __LINE__; \
            return ktest_err; \
        } \
    } while (0)

#define KTEST_SET_FIXTURE(NAME, HANDLE) \
    do { \
        int ktest_err = ktest_set_fixture((HANDLE), ktest_list__, (fixFn)ktest_fixture_##NAME, (tearFn)ktest_teardown_##NAME, sizeof(struct NAME)); \
        if(ktest_err != KTEST_SUCCESS) { \
            *ktest_file__ = __FILE__; \
            *ktest_line__ = __LINE__; \
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
            char          : "%d",           \
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
        status__->asserts++;\
        if ((x) != 1) {     \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Asserted : true\n");     \
            if((x) == 0) {                             \
                KTEST_PRINTF("      Actual : false\n\n");\
            } else {                                   \
                KTEST_PRINTF("      Actual : ");       \
                KTEST_VAL_PRINT(x);                    \
                KTEST_PRINTF("\n\n");                  \
            }                                          \
            status__->result = 1;                      \
            return;                                    \
        }                                              \
    } while (0)

#define K_ASSERT_EQ_FALSE(x) \
    do {                     \
        status__->asserts++; \
        if ((x) != 0) {      \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Asserted : false\n");    \
            if((x) == 1) {                             \
                KTEST_PRINTF("      Actual : true\n\n"); \
            } else {                                   \
                KTEST_PRINTF("      Actual : ");       \
                KTEST_VAL_PRINT(x);                    \
                KTEST_PRINTF("\n\n");                  \
            }                                          \
            status__->result = 1;                      \
            return;                                    \
        }                                              \
    } while (0)

#define K_EXPECT_EQ_TRUE(x) \
    do {                    \
        status__->expects++; \
        if ((x) != 1) {     \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Expected : true\n");     \
            if((x) == 0) {                             \
                KTEST_PRINTF("      Actual : false\n\n");\
            } else {                                   \
                KTEST_PRINTF("      Actual : ");       \
                KTEST_VAL_PRINT(x);                    \
                KTEST_PRINTF("\n\n");                  \
            }                                          \
            status__->result = 1;                      \
        }                                              \
    } while (0)

#define K_EXPECT_EQ_FALSE(x) \
    do {                     \
        status__->expects++; \
        if ((x) != 0) {      \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Expected : false\n");    \
            if((x) == 1) {                             \
                KTEST_PRINTF("      Actual : true\n\n"); \
            } else {                                   \
                KTEST_PRINTF("      Actual : ");       \
                KTEST_VAL_PRINT(x);                    \
                KTEST_PRINTF("\n\n");                  \
            }                                          \
            status__->result = 1;                      \
        }                                              \
    } while (0)

#define K_ASSERT(x, y, cmp)  \
    do {                     \
        status__->asserts++; \
        if(!((x) cmp (y))) { \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Asserted : {value} %s ", #cmp); \
            KTEST_VAL_PRINT(y);                   \
            KTEST_PRINTF("\n");                   \
            KTEST_PRINTF("      Actual : ");      \
            KTEST_VAL_PRINT(x);                   \
            KTEST_PRINTF(" %s ", #cmp);           \
            KTEST_VAL_PRINT(y);                   \
            KTEST_PRINTF("\n\n");                 \
            status__->result = 1;                 \
            return;                               \
        }                                         \
    } while(0)

#define K_EXPECT(x, y, cmp)  \
    do {                     \
        status__->expects++; \
        if(!((x) cmp (y))) { \
            KTEST_PRINTF("Test Failure : %s:%u\n", __FILE__, __LINE__); \
            KTEST_PRINTF("    Expected : {value} %s ", #cmp); \
            KTEST_VAL_PRINT(y);                   \
            KTEST_PRINTF("\n");                   \
            KTEST_PRINTF("      Actual : ");      \
            KTEST_VAL_PRINT(x);                   \
            KTEST_PRINTF(" %s ", #cmp);           \
            KTEST_VAL_PRINT(y);                   \
            KTEST_PRINTF("\n\n");                 \
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
        status__->asserts++;      \
        if( ktest_str_eq(status__->output, __FILE__, __LINE__, (x), (y)) ) { \
            status__->result = 1; \
            return;               \
        }                         \
    } while(0)

#define K_ASSERT_STR_NE(x, y)     \
    do {                          \
        status__->asserts++;      \
        if( ktest_str_ne(status__->output, __FILE__, __LINE__, (x), (y)) ) { \
            status__->result = 1; \
            return;               \
        }                         \
    } while(0)

#define K_EXPECT_STR_EQ(x, y)     \
    do {                          \
        status__->expects++;      \
        if( ktest_str_eq(status__->output, __FILE__, __LINE__, (x), (y)) ) { \
            status__->result = 1; \
        }                         \
    } while(0)

#define K_EXPECT_STR_NE(x, y)     \
    do {                          \
        status__->expects++;      \
        if( ktest_str_ne(status__->output, __FILE__, __LINE__, (x), (y)) ) { \
            status__->result = 1; \
        }                         \
    } while(0)

#endif