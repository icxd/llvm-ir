#ifndef LLVM_COMMON_H
#define LLVM_COMMON_H

#include <lib/base.h>
#include "value.h"
#include "type.h"

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

typedef struct llvm_function_arg_t {
    llvm_type_t arg_type;
    llvm_value_t arg_value;
} llvm_function_arg_t;
array_proto(llvm_function_arg_t); array_impl(llvm_function_arg_t);

#endif // LLVM_COMMON_H