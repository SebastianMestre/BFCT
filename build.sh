FLAGS="-O2 -std=c++14"

COMPILER=g++

SOURCE="
src/main.cpp
src/Op.cpp
src/frontend.cpp
src/optimizer.cpp
src/c_backend.cpp
src/mips_backend.cpp
src/ast.cpp
"

OUTPUTDIR=bin
OUTPUT=$OUTPUTDIR/bfct

mkdir -p $OUTPUTDIR

$COMPILER $FLAGS -o $OUTPUT $SOURCE
