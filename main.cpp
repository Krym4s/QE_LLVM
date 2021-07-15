#include "QE_LLVM.hpp"

int main(int argc, char const *argv[])
{
    SolveEquation (strtod(argv[1], NULL), strtod(argv[2], NULL), strtod(argv[3], NULL));
        return 0;

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

    if (LLVMCreateExecutionEngineForModule(&engine, mod, &error) != 0) {
        fprintf(stderr, "failed to create execution engine\n");
        abort();
    }
    if (error) {
        fprintf(stderr, "error: %s\n", error);
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }

    if (argc < 4) {
        fprintf(stderr, "usage: %s x y\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    double x = strtod(argv[1], NULL);
    double y = strtod(argv[2], NULL);
    double z = strtod(argv[3], NULL);
    double t1 = 0;
    double t2 = 0;

    LLVMInitializeNativeAsmPrinter();

    int (*lin)(double, double, double, double*, double*) = (int (*)(double, double, double, double*, double*))LLVMGetFunctionAddress(engine, "SolveQuadraticEquation");
    printf ("haha\n");
    fflush(stdout);
    printf ("%lg %lg %lg\n", x, y, z);
    lin (x,y, z, &t1, &t2);
    printf ("%d %lg %lg\n", lin (x,y, z, &t1, &t2), t1, t2);
    // Write out bitcode to file
    if (LLVMWriteBitcodeToFile(mod, "sum.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    LLVMDisposeExecutionEngine(engine);



    return 0;
}
