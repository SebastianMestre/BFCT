BFCOMPILER="./bin/bfct"
BFFLAGS="-o temp/out.c"

CCOMPILER=gcc
CFLAGS="-O3 -o temp/out.out"

$BFCOMPILER $BFFLAGS $1
$CCOMPILER $CFLAGS temp/out.c

temp/out.out
