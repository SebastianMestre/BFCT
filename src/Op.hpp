#pragma once

#include <iostream>

#include <cassert>
#include <cstdint>

enum class Opcode : uint8_t
{
    Lop, // Loop open
    Lcl, // Loop close
    Cop, // Condition open
    Ccl, // Condition close
    Put, // Write to stdin
    Get, // Read from stdin
    // Arithmetic with immediate values (constants)
    Addi,
    Seti,
    Muli,
    // Add value at offset times immediate
    Madd,
    // Data pointer manipulation
    Jmp,
    // Enum trick to track size of instruction set
    Count
};

struct Op
{
    Opcode opcode;
    int16_t arg1;
    int16_t arg2;

    bool operator == (Op const& o) const {
		return opcode == o.opcode and arg1 == o.arg1 and arg2 == o.arg2;
	}
};

bool can_merge (Opcode opcode);
