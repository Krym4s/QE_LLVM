#include <llvm-c-11/llvm-c/Core.h>
#include <llvm-c-11/llvm-c/ExecutionEngine.h>
#include <llvm-c-11/llvm-c/Target.h>
#include <llvm-c-11/llvm-c/Analysis.h>
#include <llvm-c-11/llvm-c/BitWriter.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

 //!-------------------------------------------------
 //! @brief Create a IsZero function in LLVM IR 
 //!        function requires one real param and checks if it is zero 
 //! 
 //! @param module - LLVM module 
 //! @return int - returns if creation was successful 
 //!-------------------------------------------------

int CreateIsZero (LLVMModuleRef module);

 //!-------------------------------------------------
 //! @brief Create a function that solves linear equations
 //!        function requires two real params, that are coefs of ax + b = 0 equation
 //!        and one pointer to real parametr, which is used to return root
 //! 
 //! @param module - LLVM module
 //! @return int - returns if creation was successful
 //!-------------------------------------------------

int CreateSolveLinearEq (LLVMModuleRef module);
