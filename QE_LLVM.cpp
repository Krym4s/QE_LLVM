#include "QE_LLVM.hpp"

const double epsilon = 1e-5;

int CreateIsZero(LLVMModuleRef module)
{
    LLVMTypeRef IsZeroParamT[]  = {LLVMDoubleType()};
    LLVMTypeRef IsZeroRetT      = LLVMFunctionType (LLVMInt1Type(), IsZeroParamT, 1, 0);
    LLVMValueRef IsZero         = LLVMAddFunction (module, "IsZero", IsZeroRetT);

    LLVMBuilderRef builder      = LLVMCreateBuilder();

    LLVMValueRef eps        = LLVMConstReal (LLVMDoubleType(), epsilon);
    LLVMValueRef trueBool   = LLVMConstInt  (LLVMInt1Type(), 1, 0);
    LLVMValueRef falseBool  = LLVMConstInt  (LLVMInt1Type(), 0, 0);

    LLVMBasicBlockRef blocks[3] = {};

    blocks[0] = LLVMAppendBasicBlock (IsZero, "condition");
    blocks[1] = LLVMAppendBasicBlock (IsZero, "trueB");
    blocks[2] = LLVMAppendBasicBlock (IsZero, "falseB");

    LLVMPositionBuilderAtEnd (builder, blocks[0]);

    LLVMValueRef condition = LLVMBuildFCmp (builder, LLVMRealOLE, LLVMGetParam (IsZero, 0) , eps, "if");
    LLVMBuildCondBr (builder, condition, blocks[1], blocks[2]);

    LLVMPositionBuilderAtEnd (builder, blocks[1]);
    LLVMBuildRet (builder, trueBool);

    LLVMPositionBuilderAtEnd (builder, blocks[2]);
    LLVMBuildRet (builder, falseBool);

    LLVMDisposeBuilder (builder);

    return 0;    
}


int CreateSolveLinearEq (LLVMModuleRef module)
{
    LLVMTypeRef LinEqParamT[]  = {LLVMDoubleType(), LLVMDoubleType(), LLVMPointerType (LLVMDoubleType(), 8)};
    LLVMTypeRef LinEqRetT      = LLVMFunctionType (LLVMInt32Type(), LinEqParamT, 3, 0);
    LLVMValueRef SolveLinEq    = LLVMAddFunction (module, "SolveLinearEquation", LinEqRetT);

    LLVMBuilderRef builder     = LLVMCreateBuilder();

    LLVMValueRef inf    = LLVMConstInt (LLVMInt32Type(), -1, 1);
    LLVMValueRef zero   = LLVMConstInt (LLVMInt32Type(),  0, 1);
    LLVMValueRef one    = LLVMConstInt (LLVMInt32Type(),  1, 1); 

    LLVMBasicBlockRef blocks[5] = {};

    blocks[0] = LLVMAppendBasicBlock (SolveLinEq, "aCond");
    blocks[1] = LLVMAppendBasicBlock (SolveLinEq, "specialCase");
    blocks[2] = LLVMAppendBasicBlock (SolveLinEq, "usualCase");
    blocks[3] = LLVMAppendBasicBlock (SolveLinEq, "infR");
    blocks[4] = LLVMAppendBasicBlock (SolveLinEq, "zeroR");

    LLVMPositionBuilderAtEnd (builder, blocks[0]);

    LLVMValueRef IsZerof = LLVMGetFirstFunction(module);

    LLVMValueRef arg1 = LLVMGetParam (SolveLinEq, 0);
    LLVMValueRef aCond = LLVMBuildCall (builder, IsZerof, &(arg1), 1, "aIf");
    LLVMBuildCondBr (builder, aCond, blocks[1], blocks[2]);
    
    LLVMPositionBuilderAtEnd (builder, blocks[1]);
    LLVMValueRef arg2 = LLVMGetParam (SolveLinEq, 1);
    LLVMValueRef bCond = LLVMBuildCall (builder, IsZerof, &(arg2), 1, "bIf");
    LLVMBuildCondBr (builder, bCond, blocks[3], blocks[4]);

    LLVMPositionBuilderAtEnd (builder, blocks[3]);
    LLVMBuildRet (builder, inf);

    LLVMPositionBuilderAtEnd (builder, blocks[4]);
    LLVMBuildRet (builder, zero);

    LLVMPositionBuilderAtEnd (builder, blocks[2]);
    LLVMValueRef nRet = LLVMBuildFDiv (builder, LLVMGetParam (SolveLinEq, 0), LLVMGetParam (SolveLinEq, 1), "nRet");
    LLVMValueRef ret = LLVMBuildFNeg (builder, nRet, "ret");
    LLVMBuildStore (builder, ret, LLVMGetParam (SolveLinEq, 2));

    LLVMBuildRet (builder, one);

    LLVMDisposeBuilder (builder);

    return 0;
}