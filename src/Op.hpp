#pragma once

#include <iostream>

#include <cassert>
#include <cstdint>

enum class Opcode : uint8_t
{
    // Base brainfuck ops
    Inc, // Increment at data pointer
    Dec, // Decrement at data pointer
    Mvr, // Increment data pointer (move right)
    Mvl, // Decrement data pointer (move left)
    Lop, // Loop open
    Lcl, // Loop close
    Put, // Write to stdin
    Get, // Read from stdin
    // Extended opset for optimization
    // Arithmetic with memory values
    Add,
    Sub,
    Set,
    Mul,
    // Arithmetic with immediate values (constants)
    Addi,
    Subi,
    Seti,
    Muli,
    // Data pointer manipulation
    Jmp,
    // Enum trick to keep count of size of instruction set
    Count
};

struct Op
{
    Opcode opcode;
    // Some ops have signed arguments, some have unsigned arguments
    union {
        uint16_t uval;
        int16_t sval;
    };

    bool operator==(Op const& o) const { return opcode == o.opcode and uval == o.uval; }
};

static_assert(sizeof(Op) == 4);

Op multi_op(Opcode opcode, int count);

bool has_multi (Opcode opcode);
