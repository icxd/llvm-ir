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
        str_append(&out, llvm_generate_linkage_type(global.linkage));
        if (global.visibility) {
            switch (global.visibility) {
                case LLVM_VISIBILITY_DEFAULT: break;
                case LLVM_VISIBILITY_HIDDEN: str_append_cstr(&out, "hidden "); break;
                case LLVM_VISIBILITY_PROTECTED: str_append_cstr(&out, "protected "); break;
            }
        }
        if (global.dll_storage_class) {
            switch (global.dll_storage_class) {
                case LLVM_DLL_STORAGE_CLASS_DLLIMPORT: str_append_cstr(&out, "dllimport "); break;
                case LLVM_DLL_STORAGE_CLASS_DLLEXPORT: str_append_cstr(&out, "dllexport "); break;
                case LLVM_DLL_STORAGE_CLASS_DEFAULT: break;
            }
        }
        if (global.address_space) {
            str_append_cstr(&out, "addrspace(");
            str_append_int(&out, global.address_space);
            str_append_cstr(&out, ") ");
        }
        if (global.is_constant) str_append_cstr(&out, "constant ");
        else str_append_cstr(&out, "global ");
        str_append(&out, llvm_generate_type(gen, global.type));
        str_append_cstr(&out, " ");
        str_append(&out, llvm_generate_value(gen, global.value));
        if (global.alignment) {
            str_append_cstr(&out, ", align ");
            str_append_int(&out, global.alignment);
        }
        str_append_cstr(&out, "\n");
    });
    array_foreach(llvm_function_t, gen->functions, {
        llvm_function_t function = it;
        if (function.is_native) str_append_cstr(&out, "declare ");
        else str_append_cstr(&out, "define ");
        str_append(&out, llvm_generate_linkage_type(function.linkage));
        if (function.visibility) {
            switch (function.visibility) {
                case LLVM_VISIBILITY_DEFAULT: break;
                case LLVM_VISIBILITY_HIDDEN: str_append_cstr(&out, "hidden "); break;
                case LLVM_VISIBILITY_PROTECTED: str_append_cstr(&out, "protected "); break;
            }
        }
        if (function.dll_storage_class) {
            switch (function.dll_storage_class) {
                case LLVM_DLL_STORAGE_CLASS_DLLIMPORT: str_append_cstr(&out, "dllimport "); break;
                case LLVM_DLL_STORAGE_CLASS_DLLEXPORT: str_append_cstr(&out, "dllexport "); break;
                case LLVM_DLL_STORAGE_CLASS_DEFAULT: break;
            }
        }
        if (function.call_convention) {
            switch (function.call_convention) {
                case LLVM_CALL_CONVENTION_C: str_append_cstr(&out, "cc "); break;
                case LLVM_CALL_CONVENTION_FAST: str_append_cstr(&out, "fastcc "); break;
                case LLVM_CALL_CONVENTION_COLD: str_append_cstr(&out, "coldcc "); break;
                case LLVM_CALL_CONVENTION_GHC: str_append_cstr(&out, "cc 10 "); break;
            }
        }
        str_append(&out, llvm_generate_type(gen, function.return_type));
        str_append_cstr(&out, " @");
        str_append(&out, function.name);
        str_append_cstr(&out, "(");
        for (size_t i = 0; i < function.args.size; i++) {
            llvm_type_t arg = function.args.data[i];
            str_append(&out, llvm_generate_type(gen, arg));
            if (i < function.args.size - 1)
                str_append_cstr(&out, ", ");
        }
        if (function.is_vararg)
            str_append_cstr(&out, ", ...");
        str_append_cstr(&out, ") ");
        if (function.address_space) {
            str_append_cstr(&out, "addrspace(");
            str_append_int(&out, function.address_space);
            str_append_cstr(&out, ") ");
        }
        if (function.alignment) {
            str_append_cstr(&out, "align ");
            str_append_int(&out, function.alignment);
            str_append_cstr(&out, " ");
        }
        if (function.is_native) {
            str_append_cstr(&out, "\n");
        } else {
            str_append_cstr(&out, "{\n");
            if (function.body == NULL)
                fatal("function body is null.");
            for (size_t i = 0; i < function.body->basic_blocks.size; i++) {
                llvm_basic_block_t basic_block = function.body->basic_blocks.data[i];
                str_append(&out, basic_block.name);
                str_append_cstr(&out, ":\n");
                for (size_t j = 0; j < basic_block.instructions.size; j++) {
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
    if (local.value.value != NULL) {
        str_append(&out, llvm_generate_value(gen, *local.value.value));
    } else if (local.value.instruction != NULL) {
        str_append(&out, llvm_generate_instruction(gen, *local.value.instruction));
    }
    return out;
}

str llvm_generate_type(llvm_generator_t *gen, llvm_type_t type) {
    str out = STR("");
    switch (type.type) {
        case LLVM_TYPE_INT_: {
            str_append_cstr(&out, "i");
            str_append_int(&out, type.int_);
        } break;
        case LLVM_TYPE_FLOAT_: {
            if (type.float_ == 32)
                str_append_cstr(&out, "float");
            else if (type.float_ == 64)
                str_append_cstr(&out, "double");
            else
                fatal("invalid float size.");
        } break;
        case LLVM_TYPE_POINTER_: {
            str_append(&out, llvm_generate_type(gen, *type.pointer.inner));
            str_append_cstr(&out, "*");
        } break;
        case LLVM_TYPE_ARRAY_: {
            str_append_cstr(&out, "[");
            str_append_int(&out, type.array.size);
            str_append_cstr(&out, " x ");
            str_append(&out, llvm_generate_type(gen, *type.array.inner));
            str_append_cstr(&out, "]");
        } break;
        case LLVM_TYPE_VECTOR_: {
            str_append_cstr(&out, "<");
            str_append_int(&out, type.vector.size);
            str_append_cstr(&out, " x ");
            str_append(&out, llvm_generate_type(gen, *type.vector.inner));
            str_append_cstr(&out, ">");
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
        case LLVM_VALUE_FLOAT: {
            str_append_float(&out, value.float_);
        } break;
        case LLVM_VALUE_DOUBLE: {
            str_append_double(&out, value.double_);
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
            for (size_t i = 0; i < gen->functions.size; i++) {
                llvm_function_t function = gen->functions.data[i];
                if (str_eq(function.name, instruction.call.function_name)) {
                    is_vararg = function.is_vararg;
                    break;
                }
            }
            for (size_t i = 0; i < instruction.call.args.size; i++) {
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
            for (size_t i = 0; i < instruction.call.args.size; i++) {
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

str llvm_generate_linkage_type(llvm_linkage_type_t linkage) {
    if (!linkage)
        return STR("");
    switch (linkage) {
        case LLVM_LINKAGE_EXTERNAL: return STR("external ");
        case LLVM_LINKAGE_INTERNAL: return STR("internal ");
        case LLVM_LINKAGE_PRIVATE: return STR("private ");
        case LLVM_LINKAGE_LINKONCE: return STR("linkonce ");
        case LLVM_LINKAGE_WEAK: return STR("weak ");
        case LLVM_LINKAGE_COMMON: return STR("common ");
        case LLVM_LINKAGE_APPENDING: return STR("appending ");
        case LLVM_LINKAGE_EXTERN_WEAK: return STR("extern_weak ");
        case LLVM_LINKAGE_AVAILABLE_EXTERNALLY: return STR("available_externally ");
    }
    return STR("");
}