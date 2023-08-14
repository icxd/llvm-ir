#ifndef LLVM_H
#define LLVM_H

#include <lib/base.h>

typedef enum llvm_linkage_type_t {
    LLVM_LINKAGE_PRIVATE,
    LLVM_LINKAGE_INTERNAL,
    LLVM_LINKAGE_AVAILABLE_EXTERNALLY,
    LLVM_LINKAGE_LINKONCE,
    LLVM_LINKAGE_WEAK,
    LLVM_LINKAGE_COMMON,
    LLVM_LINKAGE_APPENDING,
    LLVM_LINKAGE_EXTERN_WEAK,
    LLVM_LINKAGE_EXTERNAL,
} llvm_linkage_type_t;

typedef enum llvm_call_convention_t {
    LLVM_CALL_CONVENTION_C,
    LLVM_CALL_CONVENTION_FAST,
    LLVM_CALL_CONVENTION_COLD,
    LLVM_CALL_CONVENTION_GHC,
} llvm_call_convention_t;

typedef enum llvm_dll_storage_class_t {
    LLVM_DLL_STORAGE_CLASS_DEFAULT,
    LLVM_DLL_STORAGE_CLASS_DLLIMPORT,
    LLVM_DLL_STORAGE_CLASS_DLLEXPORT,
} llvm_dll_storage_class_t;

typedef enum llvm_visibility_t {
    LLVM_VISIBILITY_DEFAULT,
    LLVM_VISIBILITY_HIDDEN,
    LLVM_VISIBILITY_PROTECTED,
} llvm_visibility_t;

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

typedef struct llvm_function_arg_t {
    llvm_type_t arg_type;
    llvm_value_t arg_value;
} llvm_function_arg_t;
array_proto(llvm_function_arg_t); array_impl(llvm_function_arg_t);

typedef struct llvm_instruction_t {
    enum {
        LLVM_INSTR_CALL,
        LLVM_INSTR_RETURN,
        LLVM_INSTR_GETELEMENTPTR,
        LLVM_INSTR_GETELEMENTPTR_INBOUNDS,
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
        struct {
            str name;
            llvm_type_t type;
            struct llvm_value_t *value;
            struct llvm_value_t *index;
        } getelementptr; // also used for getelementptr inbounds
    };
} llvm_instruction_t;
array_proto(llvm_instruction_t); array_impl(llvm_instruction_t);

#define LLVM_INSTR_CALL(r, n, a) ((llvm_instruction_t){LLVM_INSTR_CALL, .call={r, STR(n), a}})
#define LLVM_INSTR_RETURN(r, v) ((llvm_instruction_t){LLVM_INSTR_RETURN, .return_={r, v}})
#define LLVM_INSTR_GETELEMENTPTR(n, t, v, i) ((llvm_instruction_t){LLVM_INSTR_GETELEMENTPTR, .getelementptr={STR(n), t, &(v), &(i)}})
#define LLVM_INSTR_GETELEMENTPTR_INBOUNDS(n, t, v, i) ((llvm_instruction_t){LLVM_INSTR_GETELEMENTPTR_INBOUNDS, .getelementptr={STR(n), t, &(v), &(i)}})

// @<GlobalVarName> = [Linkage] [PreemptionSpecifier] [Visibility]
//                    [DLLStorageClass] [ThreadLocal]
//                    [(unnamed_addr|local_unnamed_addr)] [AddrSpace]
//                    [ExternallyInitialized]
//                    <global | constant> <Type> [<InitializerConstant>]
//                    [, section "name"] [, partition "name"]
//                    [, comdat [($name)]] [, align <Alignment>]
//                    [, no_sanitize_address] [, no_sanitize_hwaddress]
//                    [, sanitize_address_dyninit] [, sanitize_memtag]
//                    (, !name !N)*
typedef struct llvm_global_t {
    str name;
    llvm_linkage_type_t linkage;
    llvm_visibility_t visibility;
    llvm_dll_storage_class_t dll_storage_class;
    int address_space;
    bool is_constant;
    bool is_global;
    llvm_type_t *type;
    llvm_value_t value;
    int alignment;
} llvm_global_t;
array_proto(llvm_global_t); array_impl(llvm_global_t);

#define LLVM_GLOBAL(n, c, t, v) ((llvm_global_t){STR(n), LLVM_LINKAGE_INTERNAL, c, &(t), v})
#define LLVM_GLOBAL_LINKAGE(n, l, c, t, v) ((llvm_global_t){STR(n), l, c, &(t), v})

typedef struct llvm_local_value_t {
    llvm_value_t *value;
    llvm_instruction_t *instruction;
} llvm_local_value_t;

#define LLVM_LOCAL_VALUE(v) ((llvm_local_value_t){&(v), NULL})
#define LLVM_LOCAL_INSTRUCTION(i) ((llvm_local_value_t){NULL, &(i)})

typedef struct llvm_local_t {
    uint idx;
    llvm_local_value_t value;
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

// define [linkage] [PreemptionSpecifier] [visibility] [DLLStorageClass]
//        [cconv] [ret attrs]
//        <ResultType> @<FunctionName> ([argument list])
//        [(unnamed_addr|local_unnamed_addr)] [AddrSpace] [fn Attrs]
//        [section "name"] [partition "name"] [comdat [($name)]] [align N]
//        [gc] [prefix Constant] [prologue Constant] [personality Constant]
//        (!name !N)* { ... }
typedef struct llvm_function_t {
    bool is_native;
    str name;
    llvm_linkage_type_t linkage;
    llvm_visibility_t visibility;
    llvm_dll_storage_class_t dll_storage_class;
    llvm_call_convention_t call_convention;
    llvm_type_t return_type;
    array(llvm_type_t) args;
    bool is_vararg;
    int address_space;
    int alignment;
    llvm_function_body_t *body;
} llvm_function_t;
array_proto(llvm_function_t); array_impl(llvm_function_t);

typedef struct llvm_type_declaration_t {
    str name;
    llvm_type_t type;
} llvm_type_declaration_t;
array_proto(llvm_type_declaration_t); array_impl(llvm_type_declaration_t);

#define LLVM_TYPE_DECLARATION(n, t) ((llvm_type_declaration_t){STR(n), t})

typedef struct llvm_generator_t {
    array(llvm_type_declaration_t) type_declarations;
    array(llvm_global_t) globals;
    array(llvm_function_t) functions;
} llvm_generator_t;

void llvm_init(llvm_generator_t *gen);
void llvm_free(llvm_generator_t *gen);
void llvm_add_type_declaration(llvm_generator_t *gen, llvm_type_declaration_t type_declaration);
void llvm_add_global(llvm_generator_t *gen, llvm_global_t global);
void llvm_add_function(llvm_generator_t *gen, llvm_function_t function);

str llvm_generate(llvm_generator_t *gen);
str llvm_generate_local(llvm_generator_t *gen, llvm_local_t local);
str llvm_generate_type(llvm_generator_t *gen, llvm_type_t type);
str llvm_generate_value(llvm_generator_t *gen, llvm_value_t value);
str llvm_generate_instruction(llvm_generator_t *gen, llvm_instruction_t instruction);

str llvm_generate_linkage_type(llvm_linkage_type_t linkage);
str llvm_generate_call_convention(llvm_call_convention_t call_convention);

#endif // LLVM_H