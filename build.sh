FLAGS="-O3 -std=c++11"

COMPILER=g++

SOURCE="src/main.cpp src/Op.cpp src/frontend.cpp src/optimizer.cpp src/c_backend.cpp"

OUTPUTDIR=bin
OUTPUT=$OUTPUTDIR/bfct

mkdir $OUTPUTDIR

$COMPILER $FLAGS -o $OUTPUT $SOURCE
