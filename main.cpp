#include "QE_LLVM.hpp"

int main(int argc, char const *argv[])
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

    if (LLVMCreateExecutionEngineForModule(&engine, mod, &error) != 0) {
        fprintf(stderr, "failed to create execution engine\n");
        abort();
    }
    if (error) {
        fprintf(stderr, "error: %s\n", error);
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }
    double x = strtod(argv[1], NULL);
    double y = strtod(argv[2], NULL);
    double z = strtod(argv[3], NULL);
    double t1 = 0;
    double t2 = 0;

    LLVMInitializeNativeAsmPrinter();

    LLVMWriteBitcodeToFile(mod, "sum.bc");

    int (*sv)(double) = (int (*) (double))LLVMGetFunctionAddress(engine, "IsZero");
    printf ("test1:%d\n\n", sv(x));

    int (*line)(double, double, double*) = (int (*)(double, double, double*))LLVMGetFunctionAddress(engine, "SolveLinearEquation");
    printf ("haha\n");
    fflush(stdout);
    printf ("%lg %lg\n", x, y);
    //line (x,y, &z);
    //printf ("%d %lg\n", line(x,y, &z), z);

    //int (*sum_func)(double) = (int (*)(double))LLVMGetFunctionAddress(engine, "IsZero");
    //printf("%d\n", sum_func(x));
    int (*lin)(double, double, double, double*, double*) = (int (*)(double, double, double, double*, double*))LLVMGetFunctionAddress(engine, "SolveQuadraticEquation");
    printf ("haha\n");
    fflush(stdout);
    printf ("%lg %lg %lg\n", x, y, z);
    lin (x,y, z, &t1, &t2);
    printf ("%d \n%lg %lg\n", lin (x,y, z, &t1, &t2), t1, t2);
    // Write out bitcode to file
    if (LLVMWriteBitcodeToFile(mod, "sum.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    LLVMDisposeExecutionEngine(engine);



    return 0;
}
