#ifndef LLVM_TYPE_H
#define LLVM_TYPE_H

#include <lib/base.h>

typedef struct llvm_type_t *llvm_type_ptr_t;
array_proto(llvm_type_ptr_t); array_impl(llvm_type_ptr_t);

typedef struct llvm_type_t {
    enum {
        LLVM_TYPE_INT_,
        LLVM_TYPE_FLOAT_,
        LLVM_TYPE_POINTER_,
        LLVM_TYPE_ARRAY_,
        LLVM_TYPE_VECTOR_,
        LLVM_TYPE_STRUCTURE_
    } type;
    union {
        int int_;
        int float_;
        struct {
            struct llvm_type_t *inner;
        } pointer;
        struct {
            struct llvm_type_t *inner;
            int size;
        } array;
        struct {
            struct llvm_type_t *inner;
            int size;
        } vector;
        struct {
            array(llvm_type_ptr_t) members;
            bool is_packed;
        } structure;
    };
} llvm_type_t;
array_proto(llvm_type_t); array_impl(llvm_type_t);

#define LLVM_TYPE_INT(s) ((llvm_type_t){.type=LLVM_TYPE_INT_, .int_=(s)})
#define LLVM_TYPE_FLOAT() ((llvm_type_t){.type=LLVM_TYPE_FLOAT_, .float_=32})
#define LLVM_TYPE_DOUBLE() ((llvm_type_t){.type=LLVM_TYPE_FLOAT_, .float_=64})
#define LLVM_TYPE_POINTER(inner) ((llvm_type_t){.type=LLVM_TYPE_POINTER_, .pointer={&(inner)}})
#define LLVM_TYPE_ARRAY(inner, s) ((llvm_type_t){.type=LLVM_TYPE_ARRAY_, .array={&(inner), (s)}})
#define LLVM_TYPE_VECTOR(inner, s) ((llvm_type_t){.type=LLVM_TYPE_VECTOR_, .vector={&(inner), (s)}})
#define LLVM_TYPE_STRUCTURE(members, p) ((llvm_type_t){.type=LLVM_TYPE_STRUCTURE_, .structure={members, p}})
// Custom wrappers for LLVM types
#define LLVM_TYPE_CHAR() LLVM_TYPE_INT(8)
#define LLVM_TYPE_STRING() LLVM_TYPE_POINTER(LLVM_TYPE_CHAR())

#endif // LLVM_TYPE_H