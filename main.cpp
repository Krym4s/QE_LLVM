#include "QE_LLVM.hpp"

int main(int argc, char const *argv[])
{

    LLVMModuleRef mod = LLVMModuleCreateWithName("quadratic_equation");

    CreateIsZero (mod);
    CreateSolveLinearEq (mod);

    char *error = NULL;
    LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    LLVMExecutionEngineRef engine;
    error = NULL;
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();

    if (LLVMCreateExecutionEngineForModule(&engine, mod, &error) != 0) {
        fprintf(stderr, "failed to create execution engine\n");
        abort();
    }
    if (error) {
        fprintf(stderr, "error: %s\n", error);
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }

    if (argc < 3) {
        fprintf(stderr, "usage: %s x y\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    double x = strtod(argv[1], NULL);
    double y = strtod(argv[2], NULL);
    double z = 0;

    LLVMInitializeNativeAsmPrinter();

    //int (*sum_func)(double) = (int (*)(double))LLVMGetFunctionAddress(engine, "IsZero");
    //printf("%d\n", sum_func(x));
    int (*lin)(double, double, double*) = (int (*)(double, double, double*))LLVMGetFunctionAddress(engine, "SolveLinearEquation");
    printf ("haha\n");
    fflush(stdout);
    printf ("%lg %lg\n", x, y);
    lin (x,y, &z);
    printf ("%d %lg\n", lin(x,y, &z), z);
    // Write out bitcode to file
    if (LLVMWriteBitcodeToFile(mod, "sum.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    LLVMDisposeExecutionEngine(engine);



    return 0;
}
