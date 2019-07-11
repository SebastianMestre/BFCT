#include "Op.hpp"

bool can_merge(Opcode opcode)
{
    switch (opcode)
    {
        case Opcode::Addi: return true;
        case Opcode::Madd: return true;
        case Opcode::Jmp: return true;
        default: return false;
    }
}
