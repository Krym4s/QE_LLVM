CC=clang
CFLAGS=-g `llvm-config --cflags`
LD=clang++
LDFLAGS=`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all: quadtratic_equation

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

quadtratic_equation: main.o QE_LLVM.o
	$(LD) main.o QE_LLVM.o $(LDFLAGS) -o quadratic_equation

QE_LLVM.o: QE_LLVM.cpp
	$(CC) $(CFLAGS) -c QE_LLVM.cpp

clean:
	-rm -f sum.o sum sum.bc sum.ll