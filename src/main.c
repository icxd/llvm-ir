#include <llvm.h>
#include <windows.h>

int main(void) {
    llvm_generator_t gen;
    llvm_init(&gen);
    
    array(llvm_attribute_t) attrs = array_new(llvm_attribute_t)();
    array_push(llvm_attribute_t)(&attrs, LLVM_ATTR_INTERNAL);
    array_push(llvm_attribute_t)(&attrs, LLVM_ATTR_CONSTANT);
    llvm_add_global(&gen, LLVM_GLOBAL("str_0", attrs, LLVM_TYPE_ARRAY(LLVM_TYPE_CHAR(), 13), LLVM_VALUE_CSTRING("Hello world!")));
    
    array(llvm_type_t) print_args = array_new(llvm_type_t)();
    array_push(llvm_type_t)(&print_args, LLVM_TYPE_STRING());
    llvm_add_function(&gen, LLVM_NATIVE_FUNCTION("printf", LLVM_TYPE_INT(32), print_args, true));
    
    array(llvm_type_t) main_args = array_new(llvm_type_t)();
    array(llvm_basic_block_t) main_basic_blocks = array_new(llvm_basic_block_t)();
    array(llvm_basic_block_instruction_t) main_instructions = array_new(llvm_basic_block_instruction_t)();

    array_push(llvm_basic_block_instruction_t)(&main_instructions,
        LLVM_BASIC_BLOCK_INSTRUCTION_LOCAL(LLVM_LOCAL(0,
            LLVM_VALUE_GETELEMENTPTR("str_0", LLVM_TYPE_ARRAY(LLVM_TYPE_CHAR(), 13), LLVM_VALUE_INT(0), LLVM_VALUE_INT(0)))));
    array(llvm_function_arg_t) args = array_new(llvm_function_arg_t)();
    array_push(llvm_function_arg_t)(&args, (llvm_function_arg_t){
        LLVM_TYPE_STRING(),
        LLVM_VALUE_LOCAL(0),
    });
    array_push(llvm_basic_block_instruction_t)(&main_instructions,
        LLVM_BASIC_BLOCK_INSTRUCTION_INSTRUCTION(LLVM_INSTR_CALL(LLVM_TYPE_INT(32), "printf", args)));
    array_push(llvm_basic_block_instruction_t)(&main_instructions,
        LLVM_BASIC_BLOCK_INSTRUCTION_INSTRUCTION(LLVM_INSTR_RETURN(LLVM_TYPE_INT(32), LLVM_VALUE_INT(0))));
    array_push(llvm_basic_block_t)(&main_basic_blocks, LLVM_BASIC_BLOCK("entry", main_instructions));
    
    llvm_function_body_t main_body = {main_basic_blocks};
    llvm_add_function(&gen, LLVM_FUNCTION("main", LLVM_TYPE_INT(32), main_args, main_body, false));
    
    str output = llvm_generate(&gen);
    file_write(STR("out.ll"), output);
    
    array_free(llvm_attribute_t)(&attrs);
    array_free(llvm_type_t)(&main_args);
    array_free(llvm_function_arg_t)(&args);
    llvm_free(&gen);

    system("llc out.ll -o out.s");
    system("gcc out.s -o out.exe");
    system("out.exe");
    
    return 0;
}