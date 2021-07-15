#include "QE_LLVM.hpp"

const double epsilon = 1e-11;

int CreateIsZero(LLVMModuleRef module)
{
    LLVMTypeRef absParam[] = {LLVMDoubleType()};
    LLVMTypeRef absRetT    = LLVMFunctionType (LLVMDoubleType(), absParam, 1, false);
    LLVMValueRef LLVMabs   = LLVMAddFunction (module, "llvm.fabs.f64", absRetT);

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

    LLVMValueRef param1 = LLVMGetParam (IsZero, 0);
    param1 = LLVMBuildCall (builder, LLVMabs, &param1, 1, "nparam");
    
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

    LLVMValueRef IsZerof = LLVMGetNamedFunction (module, "IsZero");

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
    LLVMValueRef nRet = LLVMBuildFDiv (builder, LLVMGetParam (SolveLinEq, 1), LLVMGetParam (SolveLinEq, 0), "nRet");
    LLVMValueRef ret = LLVMBuildFNeg (builder, nRet, "ret");
    LLVMBuildStore (builder, ret, LLVMGetParam (SolveLinEq, 2));

    LLVMBuildRet (builder, one);

    LLVMDisposeBuilder (builder);

    return 0;
}

int CreateSolveQuadraticEq (LLVMModuleRef module)
{
    LLVMTypeRef sqrtParam[] = {LLVMDoubleType()};
    LLVMTypeRef sqrtRetT    = LLVMFunctionType (LLVMDoubleType(), sqrtParam, 1, false);
    LLVMValueRef LLVMsqrt   = LLVMAddFunction (module, "llvm.sqrt.f64", sqrtRetT);

    LLVMTypeRef sqrParam[]  = {LLVMDoubleType()};
    LLVMTypeRef sqrRetT     = LLVMFunctionType (LLVMDoubleType(), sqrParam, 1, false);
    LLVMValueRef LLVMsqr    = LLVMAddFunction (module, "llvm.pow.f64", sqrRetT);


    LLVMValueRef Linearf = LLVMGetNamedFunction (module, "SolveLinearEquation");
    LLVMValueRef IsZerof = LLVMGetNamedFunction (module, "IsZero");


    LLVMTypeRef QuadEqParamT[]  = {LLVMDoubleType(), LLVMDoubleType(), LLVMDoubleType(), LLVMPointerType (LLVMDoubleType(), 8), LLVMPointerType (LLVMDoubleType(), 8)};
    LLVMTypeRef QuadEqRetT      = LLVMFunctionType (LLVMInt32Type(), QuadEqParamT, 5, 0);
    LLVMValueRef SolveQuadEq    = LLVMAddFunction (module, "SolveQuadraticEquation", QuadEqRetT);

    LLVMBuilderRef builder     = LLVMCreateBuilder();

    LLVMValueRef inf    = LLVMConstInt (LLVMInt32Type(), -1, 1);
    LLVMValueRef zero   = LLVMConstInt (LLVMInt32Type(),  0, 1);
    LLVMValueRef one    = LLVMConstInt (LLVMInt32Type(),  1, 1); 
    LLVMValueRef two    = LLVMConstInt (LLVMInt32Type(),  2, 1); 


    LLVMValueRef args[5]    = {LLVMGetParam (SolveQuadEq, 0), 
                               LLVMGetParam (SolveQuadEq, 1),
                               LLVMGetParam (SolveQuadEq, 2), 
                               LLVMGetParam (SolveQuadEq, 3), 
                               LLVMGetParam (SolveQuadEq, 4), };

    LLVMBasicBlockRef blocks[7] = {};

    blocks[0] = LLVMAppendBasicBlock (SolveQuadEq, "aCond");
    blocks[1] = LLVMAppendBasicBlock (SolveQuadEq, "specialCase");
    blocks[2] = LLVMAppendBasicBlock (SolveQuadEq, "usualCase");
    blocks[3] = LLVMAppendBasicBlock (SolveQuadEq, "infR");
    blocks[4] = LLVMAppendBasicBlock (SolveQuadEq, "zeroR");
    blocks[5] = LLVMAppendBasicBlock (SolveQuadEq, "twoR");
    blocks[6] = LLVMAppendBasicBlock (SolveQuadEq, "oneR");

    LLVMPositionBuilderAtEnd (builder, blocks[0]);

    LLVMValueRef aCond = LLVMBuildCall (builder, IsZerof, args, 1, "aIf");
    LLVMBuildCondBr (builder, aCond, blocks[1], blocks[2]);

    LLVMPositionBuilderAtEnd (builder, blocks[1]);

    LLVMValueRef ret = LLVMBuildCall (builder, Linearf, (args + 1), 3, "ret1");
    LLVMBuildRet (builder, ret);

    LLVMPositionBuilderAtEnd (builder, blocks[2]);
    LLVMValueRef discr = LLVMBuildFMul (builder, *(args + 1), *(args + 1), "b2");
    LLVMValueRef discr2 = LLVMBuildFMul (builder, LLVMConstReal (LLVMDoubleType(), 4), *args, "a4");
    discr2 = LLVMBuildFMul (builder, discr2, *(args + 2), "ac4");
    discr  = LLVMBuildFSub (builder, discr, discr2, "discr");

    LLVMValueRef dCond = LLVMBuildFCmp (builder, LLVMRealOLT, discr, LLVMConstReal (LLVMDoubleType(), 0), "dcond");
    LLVMBuildCondBr (builder, dCond, blocks[3], blocks[4]);

    LLVMPositionBuilderAtEnd (builder, blocks[3]);
    LLVMBuildRet (builder, zero);

    LLVMPositionBuilderAtEnd (builder, blocks[4]);

    LLVMValueRef a2 = LLVMBuildFMul (builder, *(args), LLVMConstReal (LLVMDoubleType(), 2), "adouble");
    ret = LLVMBuildFDiv (builder, *(args + 1), a2, "bdiv2a");
    ret = LLVMBuildFNeg (builder, ret, "mbdiv2a");
    discr = LLVMBuildCall (builder, LLVMsqrt, &discr, 1, "sqrd");
    discr2 = LLVMBuildCall (builder, IsZerof, &discr, 1, "condbr");

    LLVMBuildCondBr (builder, discr2, blocks[5], blocks[6]);

    LLVMPositionBuilderAtEnd (builder, blocks[5]);

    LLVMBuildStore (builder, ret, *(args + 3));
    LLVMBuildRet (builder, one);

    LLVMPositionBuilderAtEnd (builder, blocks[6]);

    discr = LLVMBuildFDiv (builder, discr, a2, "d2a");
    LLVMValueRef x = LLVMBuildFAdd (builder, ret, discr, "x1");
    LLVMBuildStore (builder, x, *(args + 3));
    x = LLVMBuildFSub (builder, ret, discr, "x2");
    LLVMBuildStore (builder, x, *(args + 4));

    LLVMBuildRet (builder, two);
    return 0;
}

int SolveEquation (double x, double y, double z)
{
    LLVMModuleRef mod = LLVMModuleCreateWithName("quadratic_equation");

    CreateIsZero (mod);
    CreateSolveLinearEq (mod);
    CreateSolveQuadraticEq (mod);

    char *error = NULL;
    LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    LLVMExecutionEngineRef engine;
    error = NULL;
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();

    LLVMWriteBitcodeToFile(mod, "sum.bc");

    LLVMCreateExecutionEngineForModule(&engine, mod, &error);
    if (error)
    {
        return 1;
    }
    
    double t1 = 0;
    double t2 = 0;

    LLVMInitializeNativeAsmPrinter();

    int (*CreateSolveQuadraticEq)(double, double, double, double*, double*) = (int (*)(double, double, double, double*, double*))LLVMGetFunctionAddress(engine, "SolveQuadraticEquation");
    int nRoots = CreateSolveQuadraticEq (x, y, z, &t1, &t2);
    CreateOutput (nRoots, t1, t2);

    if (LLVMWriteBitcodeToFile(mod, "sum.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    LLVMDisposeExecutionEngine(engine);

    return 0;

}

int CreateOutput (int nRoots, double x, double y)
{
    switch (nRoots)
    {
    case -1:
        printf ("infinite roots");
        break;

    case 0:
        printf ("no roots");
        break;
    
    case 1:
        printf ("roots: %lg", x);
        break;

    
    case 2:
        printf ("roots: %lg %lg", x, y);
        break;
    
    default:
        printf ("error");
        break;
    }
    return 0;
}
