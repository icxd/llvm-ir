#define BASE_IMPL
#include "llvm.h"

void llvm_init(llvm_generator_t *gen) {
    array_init(llvm_global_t)(&gen->globals);
    array_init(llvm_function_t)(&gen->functions);
}

void llvm_free(llvm_generator_t *gen) {
    array_free(llvm_global_t)(&gen->globals);
    array_free(llvm_function_t)(&gen->functions);
}

void llvm_add_global(llvm_generator_t *gen, llvm_global_t global) {
    array_push(llvm_global_t)(&gen->globals, global);
}

void llvm_add_function(llvm_generator_t *gen, llvm_function_t function) {
    array_push(llvm_function_t)(&gen->functions, function);
}

str llvm_generate(llvm_generator_t *gen) {
    str out = STR("");
    array_foreach(llvm_global_t, gen->globals, {
        llvm_global_t global = it;
        str_append_cstr(&out, "@");
        str_append(&out, global.name);
        str_append_cstr(&out, " = ");
        for (int i = 0; i < global.attributes.size; i++) {
            llvm_attribute_t attr = global.attributes.data[i];
            switch (attr) {
                case LLVM_ATTR_INTERNAL: {
                    str_append_cstr(&out, "internal ");
                } break;
                case LLVM_ATTR_CONSTANT: {
                    str_append_cstr(&out, "constant ");
                } break;
            }
        }
        str_append(&out, llvm_generate_type(gen, global.type));
        str_append_cstr(&out, " ");
        str_append(&out, llvm_generate_value(gen, global.value));
        str_append_cstr(&out, "\n");
    });
    array_foreach(llvm_function_t, gen->functions, {
        llvm_function_t function = it;
        if (function.is_native) str_append_cstr(&out, "declare ");
        else str_append_cstr(&out, "define ");
        str_append(&out, llvm_generate_type(gen, function.return_type));
        str_append_cstr(&out, " @");
        str_append(&out, function.name);
        str_append_cstr(&out, "(");
        for (int i = 0; i < function.args.size; i++) {
            llvm_type_t arg = function.args.data[i];
            str_append(&out, llvm_generate_type(gen, arg));
            if (i < function.args.size - 1)
                str_append_cstr(&out, ", ");
        }
        if (function.is_vararg)
            str_append_cstr(&out, ", ...");
        str_append_cstr(&out, ")");
        if (function.is_native) {
            str_append_cstr(&out, "\n");
        } else {
            str_append_cstr(&out, " {\n");
            if (function.body == NULL)
                fatal("function body is null.");
            for (int i = 0; i < function.body->basic_blocks.size; i++) {
                llvm_basic_block_t basic_block = function.body->basic_blocks.data[i];
                str_append(&out, basic_block.name);
                str_append_cstr(&out, ":\n");
                for (int j = 0; j < basic_block.instructions.size; j++) {
                    llvm_basic_block_instruction_t instruction = basic_block.instructions.data[j];
                    str_append_cstr(&out, "  ");
                    if (instruction.local != NULL)
                        str_append(&out, llvm_generate_local(gen, *instruction.local));
                    if (instruction.instruction != NULL)
                        str_append(&out, llvm_generate_instruction(gen, *instruction.instruction));
                    str_append_cstr(&out, "\n");
                }
            }
            str_append_cstr(&out, "}\n");
        }
    });
    return out;
}

str llvm_generate_local(llvm_generator_t *gen, llvm_local_t local) {
    str out = STR("");
    str_append_cstr(&out, "%");
    str_append_int(&out, local.idx);
    str_append_cstr(&out, " = ");
    str_append(&out, llvm_generate_value(gen, local.value));
    return out;
}

str llvm_generate_type(llvm_generator_t *gen, llvm_type_t type) {
    str out = STR("");
    switch (type.type) {
        case LLVM_TYPE_INT: {
            str_append_cstr(&out, "i");
            str_append_int(&out, type.int_);
        } break;
        case LLVM_TYPE_POINTER: {
            str_append(&out, llvm_generate_type(gen, *type.pointer));
            str_append_cstr(&out, "*");
        } break;
        case LLVM_TYPE_ARRAY: {
            str_append_cstr(&out, "[");
            str_append_int(&out, type.array.size);
            str_append_cstr(&out, " x ");
            str_append(&out, llvm_generate_type(gen, *type.array.inner));
            str_append_cstr(&out, "]");
        } break;
    }
    return out;
}

str llvm_generate_value(llvm_generator_t *gen, llvm_value_t value) {
    str out = STR("");
    switch (value.type) {
        case LLVM_VALUE_STRING: {
            str_append_cstr(&out, "\"");
            str_append(&out, value.string_);
            str_append_cstr(&out, "\"");
        } break;
        case LLVM_VALUE_CSTRING: {
            str_append_cstr(&out, "c\"");
            str_append(&out, value.cstring_);
            str_append_cstr(&out, "\\00\"");
        } break;
        case LLVM_VALUE_INT: {
            str_append_int(&out, value.int_);
        } break;
        case LLVM_VALUE_GETELEMENTPTR: {
            str_append_cstr(&out, "getelementptr ");
            str_append(&out, llvm_generate_type(gen, value.getelementptr.type));
            str_append_cstr(&out, ", ");
            str_append(&out, llvm_generate_type(gen, value.getelementptr.type));
            str_append_cstr(&out, "* @");
            str_append(&out, value.getelementptr.name);
            str_append_cstr(&out, ", i32 ");
            str_append(&out, llvm_generate_value(gen, *value.getelementptr.value));
            str_append_cstr(&out, ", i32 ");
            str_append(&out, llvm_generate_value(gen, *value.getelementptr.index));
        } break;
        case LLVM_VALUE_GETELEMENTPTR_INBOUNDS: {
            str_append_cstr(&out, "getelementptr inbounds (");
            str_append(&out, llvm_generate_type(gen, value.getelementptr.type));
            str_append_cstr(&out, ", ");
            str_append(&out, llvm_generate_type(gen, value.getelementptr.type));
            str_append_cstr(&out, "* @");
            str_append(&out, value.getelementptr.name);
            str_append_cstr(&out, ", i32 ");
            str_append(&out, llvm_generate_value(gen, *value.getelementptr.value));
            str_append_cstr(&out, ", i32 ");
            str_append(&out, llvm_generate_value(gen, *value.getelementptr.index));
            str_append_cstr(&out, ")");
        } break;
        case LLVM_VALUE_LOCAL: {
            str_append_cstr(&out, "%");
            str_append_int(&out, value.local.idx);
        } break;
    }
    return out;
}

str llvm_generate_instruction(llvm_generator_t *gen, llvm_instruction_t instruction) {
    str out = STR("");
    switch (instruction.type) {
        case LLVM_INSTR_CALL: {
            str_append_cstr(&out, "call ");
            str_append(&out, llvm_generate_type(gen, instruction.call.return_type));
            str_append_cstr(&out, " (");
            bool is_vararg = false;
            for (int i = 0; i < gen->functions.size; i++) {
                llvm_function_t function = gen->functions.data[i];
                if (str_eq(function.name, instruction.call.function_name)) {
                    is_vararg = function.is_vararg;
                    break;
                }
            }
            for (int i = 0; i < instruction.call.args.size; i++) {
                llvm_function_arg_t arg = instruction.call.args.data[i];
                str_append(&out, llvm_generate_type(gen, arg.arg_type));
                if (i < instruction.call.args.size - 1)
                    str_append_cstr(&out, ", ");
            }
            if (is_vararg)
                str_append_cstr(&out, ", ...");
            str_append_cstr(&out, ") @");
            str_append(&out, instruction.call.function_name);
            str_append_cstr(&out, "(");
            for (int i = 0; i < instruction.call.args.size; i++) {
                llvm_function_arg_t arg = instruction.call.args.data[i];
                str_append(&out, llvm_generate_type(gen, arg.arg_type));
                str_append_cstr(&out, " ");
                str_append(&out, llvm_generate_value(gen, arg.arg_value));
                if (i < instruction.call.args.size - 1)
                    str_append_cstr(&out, ", ");
            }
            str_append_cstr(&out, ")");
        } break;
        case LLVM_INSTR_RETURN: {
            str_append_cstr(&out, "ret ");
            str_append(&out, llvm_generate_type(gen, instruction.return_.return_type));
            str_append_cstr(&out, " ");
            str_append(&out, llvm_generate_value(gen, instruction.return_.value));
        } break;
    }
    return out;
}