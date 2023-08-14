#ifndef __LLVM_INSTRUCTION_H
#define __LLVM_INSTRUCTION_H

#include <lib/base.h>
#include "type.h"
#include "value.h"

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

#endif // __LLVM_INSTRUCTION_H