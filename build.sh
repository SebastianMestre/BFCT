FLAGS=-O3

COMPILER=g++

SOURCE="src/main.cpp src/Op.cpp src/frontend.cpp src/optimizer.cpp src/c_backend.cpp"

OUTPUT=bin/bfct

$COMPILER $FLAGS -o $OUTPUT $SOURCE 
