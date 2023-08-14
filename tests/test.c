#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif // defined(_WIN32) || defined(_WIN64)

#include <lib/llvm.h>

int main(void) {
    llvm_generator_t gen;
    llvm_init(&gen);

    llvm_add_type_declaration(&gen, LLVM_TYPE_DECLARATION("T1", LLVM_TYPE_STRUCTURE(array_new_with_values(llvm_type_ptr_t)(2, &LLVM_TYPE_INT(32), &LLVM_TYPE_INT(32)), false)));
    llvm_add_type_declaration(&gen, LLVM_TYPE_DECLARATION("T2", LLVM_TYPE_STRUCTURE(array_new_with_values(llvm_type_ptr_t)(2, &LLVM_TYPE_INT(32), &LLVM_TYPE_INT(32)), true)));

    llvm_add_global(&gen, (llvm_global_t){
        .name = STR("msg"),
        .linkage = LLVM_LINKAGE_INTERNAL, 
        .is_constant = true,
        .type = &LLVM_TYPE_ARRAY(LLVM_TYPE_CHAR(), 13),
        .value = LLVM_VALUE_CSTRING("Hello world!"),
    });

    llvm_add_global(&gen, (llvm_global_t){
        .name = STR("G"),
        .address_space = 5,
        // .dll_storage_class = LLVM_DLL_STORAGE_CLASS_DLLIMPORT,
        .visibility = LLVM_VISIBILITY_HIDDEN,
        .is_constant = true,
        .type = &LLVM_TYPE_FLOAT(),
        .value = LLVM_VALUE_FLOAT(1.0f),
        .alignment = 4,
    });

    array(llvm_basic_block_instruction_t) f_instructions = array_new_with_values(llvm_basic_block_instruction_t)(1, LLVM_BASIC_BLOCK_INSTRUCTION_INSTRUCTION(LLVM_INSTR_RETURN(LLVM_TYPE_INT(32), LLVM_VALUE_INT(0))));
    llvm_function_body_t f_body = {array_new_with_values(llvm_basic_block_t)(1, LLVM_BASIC_BLOCK("entry", f_instructions))};
    llvm_add_function(&gen, (llvm_function_t){
        .name = STR("f"),
        .linkage = LLVM_LINKAGE_PRIVATE,
        .visibility = LLVM_VISIBILITY_PROTECTED,
        .call_convention = LLVM_CALL_CONVENTION_COLD,
        // .dll_storage_class = LLVM_DLL_STORAGE_CLASS_DLLEXPORT,
        .return_type = LLVM_TYPE_INT(32),
        .args = array_new_with_values(llvm_type_t)(3, LLVM_TYPE_INT(32), LLVM_TYPE_INT(32), LLVM_TYPE_INT(32)),
        .body = &f_body,
        .address_space = 5,
        .alignment = 4,
    });
    
    llvm_add_function(&gen, (llvm_function_t){
        .name = STR("printf"),
        .return_type = LLVM_TYPE_INT(32),
        .args = array_new_with_values(llvm_type_t)(1, LLVM_TYPE_STRING()),
        .is_vararg = true,
        .is_native = true,
    });
    
    array(llvm_basic_block_instruction_t) main_instructions = array_new(llvm_basic_block_instruction_t)();
    array(llvm_type_t) main_args = array_new(llvm_type_t)();

    array_push(llvm_basic_block_instruction_t)(&main_instructions,
        LLVM_BASIC_BLOCK_INSTRUCTION_LOCAL(LLVM_LOCAL(0,
            LLVM_LOCAL_INSTRUCTION(
                LLVM_INSTR_GETELEMENTPTR("msg", LLVM_TYPE_ARRAY(LLVM_TYPE_CHAR(), 13), LLVM_VALUE_INT(0), LLVM_VALUE_INT(0))))));
    
    array(llvm_function_arg_t) args = array_new_with_values(llvm_function_arg_t)(1, (llvm_function_arg_t){ LLVM_TYPE_STRING(), LLVM_VALUE_LOCAL(0), });

    array_push(llvm_basic_block_instruction_t)(&main_instructions,
        LLVM_BASIC_BLOCK_INSTRUCTION_LOCAL(LLVM_LOCAL(1, LLVM_LOCAL_INSTRUCTION(
            LLVM_INSTR_CALL(LLVM_TYPE_INT(32), "printf", args)))));
    array_push(llvm_basic_block_instruction_t)(&main_instructions,
        LLVM_BASIC_BLOCK_INSTRUCTION_INSTRUCTION(LLVM_INSTR_RETURN(LLVM_TYPE_INT(32), LLVM_VALUE_INT(0))));
    
    llvm_function_body_t main_body = {array_new_with_values(llvm_basic_block_t)(1, LLVM_BASIC_BLOCK("entry", main_instructions))};
    llvm_add_function(&gen, (llvm_function_t){
        .name = STR("main"),
        .return_type = LLVM_TYPE_INT(32),
        .args = main_args,
        .body = &main_body,
    });
    
    str output = llvm_generate(&gen);
    file_write(STR("out.ll"), output);
    
    array_free(llvm_type_t)(&main_args);
    array_free(llvm_function_arg_t)(&args);
    llvm_free(&gen);

#if defined(_WIN32) || defined(_WIN64)
    system("llc out.ll -o out.s");
    system("gcc out.s -o out.exe");
    system("out.exe");
#elif defined(__linux__)
    system("llc out.ll -o out.s");
    system("gcc out.s -o out");
    system("./out");
#endif // defined(_WIN32) || defined(_WIN64)
    
    return 0;
}