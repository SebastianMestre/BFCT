# BFCT
A BrainFuck optimizing transpiler

## Features

### Optimizations

The transpiler does a few different optimizations:

A big one is that "trivial loops" -that is: those that contain no loops, the memory pointer ends at the same point where it starts, and the value at that position goes down by one after an iteration- are reduced to a constant time construct (made up mostly of additions and multiplications).

As a consequence of that, the `[-]` and `[+]` expressions are reduced to a single "set to zero" operation. This could be generalized for any amount of increment or decrement, but I just haven't gotten around to doing that.


Succesive operations of the same type on the same value are reduced to a single operation. This could be seen as a special case of the first optimization, but some changes should be done before implementing it that way is feasible.


### Multiple target languages

Currently, the transpiler supports compilation to the following targets:
 - C (default)
 - MIPS Assembly

I plan on adding support for other languages eventually.

The choice of languages so far is motivated in a very simple way:

C: There exist very advanced C compilers that can optimize your code way further than this simple program, so if you want to really run high performance BF code, then transpiling to C to then compile it with one of those optimizing compilers is the way to go.

MIPS: I wanted an Assembler back end, and I know MIPS, so I figured I would do just that.

## Build

build.sh compiles the transpiler. It relies on your working directory being the root of the repo.

## Contributing

Contributions are greatly apreciated. You can take a look at the open issues on Github.

## Disclaimer

example programs taken from:
https://github.com/matslina/bfoptimization
