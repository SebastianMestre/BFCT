#include "Op.hpp"

bool has_multi (Opcode opcode)
{
    switch (opcode)
    {
        case Opcode::Inc: return true;
        case Opcode::Dec: return true;
        case Opcode::Mvr: return true;
        case Opcode::Mvl: return true;
        default: return false;
    }
}

Op multi_op(Opcode opcode, int count)
{
    assert(count < 65536 && "The optimizer cannot handle more than 65535 for a multi op.");
    assert(has_multi(opcode) && "Instruction must have a multi version.");

    Op result;
    switch (opcode)
    {
        case Opcode::Inc: result.opcode = Opcode::Addi; result.uval = uint16_t(count); break;
        case Opcode::Dec: result.opcode = Opcode::Subi; result.uval = uint16_t(count); break;
        case Opcode::Mvr: result.opcode = Opcode::Jmp; result.sval =  int16_t(count); break;
        case Opcode::Mvl: result.opcode = Opcode::Jmp; result.sval = -int16_t(count); break;
        default: std::cerr << "Op " << int(opcode) << " has no multi.\n";
    }
    return result;
}
