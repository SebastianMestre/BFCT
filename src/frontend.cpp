#include "frontend.hpp"

std::vector<Op> compile(std::string const& code)
{
    std::vector<Op> result;
    for (char c : code)
    {
        switch (c)
        {
            case '+': result.push_back({ Opcode::Addi, 1 }); break;
            case '-': result.push_back({ Opcode::Subi, 1 }); break;
            case '>': result.push_back({ Opcode::Jmp,  1 }); break;
            case '<': result.push_back({ Opcode::Jmp, -1 }); break;
            case '[': result.push_back({ Opcode::Lop }); break;
            case ']': result.push_back({ Opcode::Lcl }); break;
            case '.': result.push_back({ Opcode::Put }); break;
            case ',': result.push_back({ Opcode::Get }); break;
        }
    }
    return result;
}
