#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef float f32;
typedef double f64;
typedef unsigned int uint;

#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_MAGENTA "\033[0;35m"
#define COLOR_RESET "\033[0m"
#define COLOR(color, s) color s COLOR_RESET

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((b) > (a) ? (a) : (b))
#define ABS(a) ((a) > 0 ? (a) : -(a))
#define CLAMP(x, a, b) MIN(MAX(x, a), b)
#define BETWEEN(x, a, b) ((x) >= (a) && (x) <= (b))

#define SWAP(a, b) \
    do { \
        typeof(a) tmp = a; \
        a = b; \
        b = tmp; \
    } while (0)

#define assert(cond) \
    if (!(cond)) { \
        error("ASSERTION FAILED: %s", #cond); \
        exit(1); \
    }

#define assert_msg(cond, s, ...) \
    if (!(cond)) { \
        error("ASSERTION FAILED: " s " (%s)", ##__VA_ARGS__, #cond); \
        exit(1); \
    }

#if defined(BASE_DEBUG)    
#   define debug(s, ...) printf("%s:%d: %sDEBUG%s: " s "\n", __FILE__, __LINE__, COLOR_BLUE, COLOR_RESET, ##__VA_ARGS__)
#else
#   define debug(s, ...)
#endif

#define info(s, ...) printf("%s:%d: %sINFO%s: " s "\n", __FILE__, __LINE__, COLOR_GREEN, COLOR_RESET, ##__VA_ARGS__)
#define warning(s, ...) printf("%s:%d: %sWARNING%s: " s "\n", __FILE__, __LINE__, COLOR_YELLOW, COLOR_RESET, ##__VA_ARGS__)
#define error(s, ...) printf("%s:%d: %sERROR%s: " s "\n", __FILE__, __LINE__, COLOR_RED, COLOR_RESET, ##__VA_ARGS__)
#define fatal(s, ...) \
    do { \
        printf("%s:%d: %sFATAL%s: " s "\n", __FILE__, __LINE__, COLOR_RED, COLOR_RESET, ##__VA_ARGS__); \
        exit(1); \
    } while (0);

#define UNUSED(x) (void)(x)

#define BIT(n) (1 << (n))

#define BIT_SET(x, n) ((x) |= BIT(n))
#define BIT_CLEAR(x, n) ((x) &= ~BIT(n))
#define BIT_FLIP(x, n) ((x) ^= BIT(n))
#define BIT_CHECK(x, n) ((x) & BIT(n))

#define BITMASK_SET(x, mask) ((x) |= (mask))
#define BITMASK_CLEAR(x, mask) ((x) &= ~(mask))
#define BITMASK_FLIP(x, mask) ((x) ^= (mask))
#define BITMASK_CHECK(x, mask) ((x) & (mask))

#define array(type) array_##type
#define array_init(type) array_##type##_init
#define array_new(type) array_##type##_new
#define array_push(type) array_##type##_push
#define array_pop(type) array_##type##_pop
#define array_free(type) array_##type##_free
#define array_proto(type) \
    typedef struct { \
        type *data; \
        size_t size; \
        size_t capacity; \
    } array(type); \
    void array_init(type)(array(type) *array); \
    array(type) array_new(type)(); \
    void array_push(type)(array(type) *array, type value); \
    type array_pop(type)(array(type) *array); \
    void array_free(type)(array(type) *array);
#ifdef BASE_IMPL
#define array_impl(type) \
    void array_init(type)(array(type) *array) { \
        array->data = NULL; \
        array->size = 0; \
        array->capacity = 0; \
    } \
    array(type) array_new(type)() { \
        array(type) arr; \
        array_init(type)(&arr); \
        return arr; \
    } \
    void array_push(type)(array(type) *array, type value) { \
        if (array->size == array->capacity) { \
            array->capacity = MAX(array->capacity * 2, 1); \
            array->data = realloc(array->data, array->capacity * sizeof(type)); \
        } \
        array->data[array->size++] = value; \
    } \
    type array_pop(type)(array(type) *array) { \
        assert(array->size > 0); \
        return array->data[--array->size]; \
    } \
    void array_free(type)(array(type) *array) { \
        free(array->data); \
        array->data = NULL; \
        array->size = 0; \
        array->capacity = 0; \
    }
#else
#define array_impl(type)
#endif // BASE_IMPL
#define array_foreach(type, array, body) \
    for (size_t abcbabc = 0; abcbabc < array.size; abcbabc++) { \
        type it = array.data[abcbabc]; \
        body; \
    }
    
#define result(type) result_##type
#define result_ok(type) result_##type##_ok
#define result_error(type) result_##type##_error
#define result_is_ok(type) result_##type##_is_ok
#define result_is_error(type) result_##type##_is_error
#define result_proto(type) \
    typedef struct { \
        bool ok; \
        type *value; \
        str error; \
    } result(type); \
    result(type) result_ok(type)(type *val); \
    result(type) result_error(type)(str err); \
    bool result_is_ok(type)(result(type) *result); \
    bool result_is_error(type)(result(type) *result);
#define result_implementation(type) \
    result(type) result_ok(type)(type *val) { \
        return (result(type)){.ok=true, .value=val}; \
    } \
    result(type) result_error(type)(str err) { \
        return (result(type)){.ok=false, .error=err}; \
    } \
    bool result_is_ok(type)(result(type) *result) { \
        return result->ok; \
    } \
    bool result_is_error(type)(result(type) *result) { \
        return !result->ok; \
    }

typedef struct string_view {
    char *chars;
    size_t count;
} str;
#define STR(s) ((str){s, strlen(s)})
#define STR_ARG "%.*s"
#define STR_FMT(str) (int)str.count, str.chars

typedef struct arena_t {
    char *ptr;
    size_t size;
    size_t capacity;
} arena_t;

void arena_init(arena_t *a);
void arena_free(arena_t *a);
void *arena_alloc(arena_t *a, size_t size);
void arena_clear(arena_t *a);

char str_at(str *s, size_t idx);
bool str_eq(str s1, str s2);
void str_append(str *s1, str s2);
void str_append_cstr(str *s1, char *s2);
void str_append_int(str *s1, int i);
void str_append_float(str *s1, float f);
void str_append_double(str *s1, double d);
void str_append_char(str *s1, char c);
str str_substring(str s, size_t start, size_t end);
void str_free(str *s);

str file_read_to_str(str path);
void file_write(str path, str contents);

#endif // BASE_H