#ifndef LLVM_H
#define LLVM_H

#include <lib/base.h>

typedef struct llvm_type_t {
    enum {
        LLVM_TYPE_INT,
        LLVM_TYPE_POINTER,
        LLVM_TYPE_ARRAY,
    } type;
    union {
        int int_;
        struct llvm_type_t *pointer;
        struct {
            struct llvm_type_t *inner;
            int size;
        } array;
    };
} llvm_type_t;
array_proto(llvm_type_t); array_impl(llvm_type_t);

#define LLVM_TYPE_INT(s) ((llvm_type_t){LLVM_TYPE_INT, .int_=(s)})
#define LLVM_TYPE_POINTER(inner) ((llvm_type_t){LLVM_TYPE_POINTER, .pointer=&(inner)})
#define LLVM_TYPE_ARRAY(inner, s) ((llvm_type_t){LLVM_TYPE_ARRAY, .array={&(inner), (s)}})
// Custom wrappers for LLVM types
#define LLVM_TYPE_CHAR() LLVM_TYPE_INT(8)
#define LLVM_TYPE_STRING() LLVM_TYPE_POINTER(LLVM_TYPE_CHAR())

typedef struct llvm_value_t {
    enum {
        LLVM_VALUE_STRING,
        LLVM_VALUE_CSTRING,
        LLVM_VALUE_INT,
        LLVM_VALUE_GETELEMENTPTR,
        LLVM_VALUE_GETELEMENTPTR_INBOUNDS,
        LLVM_VALUE_LOCAL,
    } type;
    union {
        str string_;
        str cstring_;
        int int_;
        struct {
            str name;
            llvm_type_t type;
            struct llvm_value_t *value;
            struct llvm_value_t *index;
        } getelementptr; // also used for getelementptr inbounds
        struct {
            uint idx;
        } local;
    };
} llvm_value_t;
array_proto(llvm_value_t); array_impl(llvm_value_t);

#define LLVM_VALUE_STRING(s) ((llvm_value_t){LLVM_VALUE_STRING, .string_=STR(s)})
#define LLVM_VALUE_CSTRING(s) ((llvm_value_t){LLVM_VALUE_CSTRING, .cstring_=STR(s)})
#define LLVM_VALUE_INT(n) ((llvm_value_t){LLVM_VALUE_INT, .int_=n})
#define LLVM_VALUE_GETELEMENTPTR(n, t, v, i) ((llvm_value_t){LLVM_VALUE_GETELEMENTPTR, .getelementptr={STR(n), t, &(v), &(i)}})
#define LLVM_VALUE_GETELEMENTPTR_INBOUNDS(n, t, v, i) ((llvm_value_t){LLVM_VALUE_GETELEMENTPTR_INBOUNDS, .getelementptr={STR(n), t, &(v), &(i)}})
#define LLVM_VALUE_LOCAL(i) ((llvm_value_t){LLVM_VALUE_LOCAL, .local={i}})

typedef struct llvm_function_arg_t {
    llvm_type_t arg_type;
    llvm_value_t arg_value;
} llvm_function_arg_t;
array_proto(llvm_function_arg_t); array_impl(llvm_function_arg_t);

typedef struct llvm_instruction_t {
    enum {
        LLVM_INSTR_CALL,
        LLVM_INSTR_RETURN,
    } type;
    union {
        struct {
            llvm_type_t return_type;
            str function_name;
            array(llvm_function_arg_t) args;
        } call;
        struct {
            llvm_type_t return_type;
            llvm_value_t value;
        } return_;
    };
} llvm_instruction_t;
array_proto(llvm_instruction_t); array_impl(llvm_instruction_t);

#define LLVM_INSTR_CALL(r, n, a) ((llvm_instruction_t){LLVM_INSTR_CALL, .call={r, STR(n), a}})
#define LLVM_INSTR_RETURN(r, v) ((llvm_instruction_t){LLVM_INSTR_RETURN, .return_={r, v}})

typedef enum llvm_attribute_t {
    LLVM_ATTR_INTERNAL, LLVM_ATTR_CONSTANT,
} llvm_attribute_t;
array_proto(llvm_attribute_t); array_impl(llvm_attribute_t);

typedef struct llvm_global_t {
    str name;
    array(llvm_attribute_t) attributes;
    llvm_type_t type;
    llvm_value_t value;
} llvm_global_t;
array_proto(llvm_global_t); array_impl(llvm_global_t);

#define LLVM_GLOBAL(n, a, t, v) ((llvm_global_t){STR(n), a, t, v})

typedef struct llvm_local_t {
    uint idx;
    llvm_value_t value; // TODO: convert to union type with llvm_value_t and llvm_instruction_t
} llvm_local_t;
array_proto(llvm_local_t); array_impl(llvm_local_t);

#define LLVM_LOCAL(i, v) ((llvm_local_t){i, v})

typedef struct llvm_basic_block_instruction_t {
    llvm_local_t *local;
    llvm_instruction_t *instruction;
} llvm_basic_block_instruction_t;
array_proto(llvm_basic_block_instruction_t); array_impl(llvm_basic_block_instruction_t);

#define LLVM_BASIC_BLOCK_INSTRUCTION_LOCAL(l) ((llvm_basic_block_instruction_t){&(l), NULL})
#define LLVM_BASIC_BLOCK_INSTRUCTION_INSTRUCTION(i) ((llvm_basic_block_instruction_t){NULL, &(i)})

typedef struct llvm_basic_block_t {
    str name;
    array(llvm_basic_block_instruction_t) instructions;
} llvm_basic_block_t;
array_proto(llvm_basic_block_t); array_impl(llvm_basic_block_t);

#define LLVM_BASIC_BLOCK(n, i) ((llvm_basic_block_t){STR(n), i})

typedef struct llvm_function_body_t {
    array(llvm_basic_block_t) basic_blocks;
} llvm_function_body_t;

typedef struct llvm_function_t {
    str name;
    llvm_type_t return_type;
    array(llvm_type_t) args;
    llvm_function_body_t *body;
    bool is_native;
    bool is_vararg;
} llvm_function_t;
array_proto(llvm_function_t); array_impl(llvm_function_t);

#define LLVM_FUNCTION(n, r, a, b, v) ((llvm_function_t){STR(n), r, a, &b, false, v})
#define LLVM_NATIVE_FUNCTION(n, r, a, v) ((llvm_function_t){STR(n), r, a, NULL, true, v})

typedef struct llvm_generator_t {
    array(llvm_global_t) globals;
    array(llvm_function_t) functions;
} llvm_generator_t;

void llvm_init(llvm_generator_t *gen);
void llvm_free(llvm_generator_t *gen);
void llvm_add_global(llvm_generator_t *gen, llvm_global_t global);
void llvm_add_function(llvm_generator_t *gen, llvm_function_t function);

str llvm_generate(llvm_generator_t *gen);
str llvm_generate_local(llvm_generator_t *gen, llvm_local_t local);
str llvm_generate_type(llvm_generator_t *gen, llvm_type_t type);
str llvm_generate_value(llvm_generator_t *gen, llvm_value_t value);
str llvm_generate_instruction(llvm_generator_t *gen, llvm_instruction_t instruction);

#endif // LLVM_H