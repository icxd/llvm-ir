#ifndef __LLVM_H
#define __LLVM_H

#include <lib/base.h>
#include <llvm/common.h>
#include <llvm/instruction.h>
#include <llvm/type.h>
#include <llvm/value.h>

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

#endif // __LLVM_H