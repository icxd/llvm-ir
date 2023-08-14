#ifndef LLVM_VALUE_H
#define LLVM_VALUE_H

#include <lib/base.h>
#include "type.h"

typedef struct llvm_value_t {
    enum {
        LLVM_VALUE_STRING_,
        LLVM_VALUE_CSTRING_,
        LLVM_VALUE_INT_,
        LLVM_VALUE_FLOAT_,
        LLVM_VALUE_DOUBLE_,
        LLVM_VALUE_LOCAL_,
        LLVM_VALUE_TYPE_,
    } type;
    union {
        str string_;
        str cstring_;
        int int_;
        float float_;
        double double_;
        struct {
            uint idx;
        } local;
        llvm_type_t type_;
    };
} llvm_value_t;
array_proto(llvm_value_t); array_impl(llvm_value_t);

#define LLVM_VALUE_STRING(s) ((llvm_value_t){LLVM_VALUE_STRING_, .string_=STR(s)})
#define LLVM_VALUE_CSTRING(s) ((llvm_value_t){LLVM_VALUE_CSTRING_, .cstring_=STR(s)})
#define LLVM_VALUE_INT(n) ((llvm_value_t){LLVM_VALUE_INT_, .int_=n})
#define LLVM_VALUE_FLOAT(n) ((llvm_value_t){LLVM_VALUE_FLOAT_, .float_=n})
#define LLVM_VALUE_DOUBLE(n) ((llvm_value_t){LLVM_VALUE_DOUBLE_, .double_=n})
#define LLVM_VALUE_LOCAL(i) ((llvm_value_t){LLVM_VALUE_LOCAL_, .local={i}})
#define LLVM_VALUE_TYPE(t) ((llvm_value_t){LLVM_VALUE_TYPE_, .type_=t})

#endif // LLVM_VALUE_H