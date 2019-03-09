# BFCT
A BrainFuck to C transpiler

example programs taken from:
https://github.com/matslina/bfoptimization

There are two shell scripts on the root directory of the repo.

build.sh compiles the transpiler.

bfrun.sh takes a filename and transpiles it to c, then compiles it.

---

This is actually not a plain implementation of brainfuck. It has two extensions that I deemed useful for simplifiying the way you can write programs in brainfuck.

The extensions are the following:
`{}` for automatic shifting when declaring a loop

usage (printing an A):

    ++++ ++++
    {
        ++++ ++++
        {
            +
        }
    }
    >>+.

instead of:

    ++++ ++++
    [>
        ++++ ++++
        [>
            +
        <-]
    <-]
    >>+.
    
and `()`, which does something similar but also remembers where the memory pointer was at the beggining of the block, and puts it back on every iteration instead of just shifting it by one.

usage:

    ++++ ++++
    (
        ++++ ++++
        (
            +>
            ++>
            +++
        )
    )

instead of:

    ++++ ++++
    {
        ++++ ++++
        {
            +>
            ++>
            +++
            <<
        }
    }
