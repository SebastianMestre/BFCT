
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++14

OUTDIR = ../bin
OUTFILE = $(OUTDIR)/bfct 

all: bfct clean

unity: unity_build.cpp
	$(CXX) $(CXXFLAGS) -o $(OUTFILE) $^

bfct: \
	main.o \
	Op.o \
	frontend.o \
	optimizer.o \
	c_backend.o \
	mips_backend.o \
	ast.o
	$(CXX) $(CXXFLAGS) -o $(OUTFILE) $^

main.o: main.cpp

Op.o: Op.cpp

frontend.o: frontend.cpp

optimizer.o: optimizer.cpp

c_backend.o: c_backend.cpp

mips_backend.o: mips_backend.cpp

ast.o: ast.cpp

clean:
	rm *.o

cleanest: clean
	rm bfct
