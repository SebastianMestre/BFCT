#pragma once

#include <iostream>

#include <cassert>
#include <cstdint>

enum class Opcode : uint8_t
{
    // Base brainfuck ops
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
    int16_t val;

    bool operator==(Op const& o) const { return opcode == o.opcode and val == o.val; }
};

static_assert(sizeof(Op) == 4);

bool can_merge (Opcode opcode);
