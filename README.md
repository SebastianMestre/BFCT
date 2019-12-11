# BFCT
A BrainFuck optimizing transpiler

## Features

### Optimizations

Right now, there is a single optimization that is performed.

We take some instructions, e.g:

```bf
>>->+>-+<-+>-+-
>+<+<++++++-<><
+>+>+>-<<+>->><
```

which would compile to C code that looks like this:

```C
p += 1;
p += 1;
*p += -1;
p += 1;
*p += 1;
p += 1;
*p += -1;
*p += 1;
...
```

And sumarize the operations into the following, equivalent, code:

```C
p += 3;
*p += 8;
p += 2;
```

#### Future

An optimization I'm working on is to take a loop that runs a fixed number of
times, and turn it into a single run of the iterated version of its instructions.

Here is an example:

```bf
[-
  >>+
  >>>+
  >>>>++
]
```

Unoptimized:
```C
while(*p){
	*p += -1;
	p += 2;
	*p += 1;
	p += 1;
	*p += 1;
	p += 1;
	*p += 2;
}
```

Optimized (note how there is no loop):
```C
temp = *p;
*p += temp * -1;
p += 2;
*p += temp * 1;
p += 1;
*p += temp * 1;
p += 1;
*p += temp * 2;
```

Another interesting optmization would be to fold "move pointer, then operate on
value" into "operate on pointer index", which gives more readable C, and should
give better codegen on both MIPS and x86.

Continuing the example from before, we would get:
```C
temp = *p;
p[0] += temp * -1;
p[2] += temp * 1;
p[3] += temp * 1;
p[4] += temp * 2;
p += 4;
```

Of course, more things can be done, like eliding multiplications by 1, doing
some strength reduction, or remembering where temp comes from:

```C
temp = *p;
p[0] = 0;
p[2] += temp;
p[3] += temp;
p[4] += temp << 1;
p += 4;
```

but that's looking a bit too far into the future, I would say.

### Multiple target languages

Currently, the transpiler supports compilation to the following targets:
 - C (default)
 - MIPS Assembly (not working at the moment due to major changes to how the compiler works)

I plan on adding support for other languages eventually.

The choice of languages so far is motivated in a very simple way:

C: There exist very advanced C compilers that can optimize your code way further
than this simple program, so if you want to really run high performance BF code,
then transpiling to C to then compile it with one of those optimizing compilers
is the way to go.

MIPS: I wanted an ASM back end, and I know MIPS, so I figured I would target
that.

#### Future

I think having an LLVM backend would be pretty cool. I think GCC can also be
used in the same way, to some degree. That would also be interesting.

WASM backend, to run your high-performance brainfuck programs on the web, would
be great.

## Build

build.sh compiles the transpiler. It relies on your working directory being the root of the repo.

#### Future

I would like to use a build system, I just have to learn one first.

## Contributing

Contributions are greatly apreciated. You can take a look at the open issues on Github.

## Disclaimer

example programs taken from:
https://github.com/matslina/bfoptimization
