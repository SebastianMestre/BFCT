#include "c_backend.hpp"

#include <iostream>
#include <sstream>

namespace {
constexpr char const* output_header = ".data\n"
                                      "buffer: .space 1000\n"
                                      ".text\n"
                                      "main:\n"
                                      "la $t0, buffer\n"
                                      "addi $t0, $t0, 500\n";

constexpr char const* output_footer = "li $v0, 10\n"
                                      "syscall\n";
}

/*
 * Memory pointer will be $t0
 * We will use $t1 as aux register 1
 * We will use $t2 as aux register 2

            case Opcode::Lop: result << "while(*p){\n"; break;
            case Opcode::Lcl: result << "}\n"; break;

            case Opcode::Cop: result << "if(*p){\n"; break;
            case Opcode::Ccl: result << "}\n"; break;

            case Opcode::Put: result << "putchar(*p);\n"; break;



 */

std::string write_mips_code(std::vector<Op> const& code)
{
    std::stringstream result;
    result << output_header;

    int loopCount = 0;
    int condCount = 0;

    std::vector<int> loopStack;
    std::vector<int> condStack;

    for (Op op : code)
    {
        switch (op.opcode)
        {
            case Opcode::Lop: {
                result << "#Loop Start\n";
                int loopId = loopCount;
                loopStack.push_back(loopCount);
                loopCount++;
                
                result << "L" << loopId << ":\n";
                result << "lb $t1, ($t0)\n";
                result << "beq $t1, 0, LE" << loopId << "\n";
            } break;
            case Opcode::Lcl: {
                result << "#Loop End\n";
                int loopId = loopStack.back();
                loopStack.pop_back();
                
                result << "j L" << loopId << "\n";
                result << "LE" << loopId << ":\n";
            } break;

            case Opcode::Cop: result << "if(*p){\n"; break;
            case Opcode::Ccl: result << "}\n"; break;

            case Opcode::Put: {
                result << "#Put\n";
                result << "li $v0, 11\n";
                result << "lb $t1, ($t0)\n";
                result << "move $a0, $t1\n";
                result << "syscall\n";
            } break;
            case Opcode::Get: {
                result << "#Get\n";
                result << "li $v0, 12\n";
                result << "syscall\n";
                result << "move $t1, $v0\n";
                result << "andi $t1, $t1, 0x00ff\n";
                result << "sb $t1, ($t0)\n";
            } break;

            case Opcode::Addi: {
                result << "#*p += " << int(op.arg1) << ";\n";
                result << "lb $t1, ($t0)\n";
                result << "addi $t1, $t1, " << int(op.arg1) << "\n";
                result << "andi $t1, $t1, 0x00ff\n";
                result << "sb $t1, ($t0)\n";
            } break;
            case Opcode::Seti: {
                result << "#*p = getchar();\n";
                result << "li $t1, " << int(op.arg1) << "\n";
                result << "andi $t1, $t1, 0x00ff\n";
                result << "sb $t1, ($t0)\n";
            } break;
            case Opcode::Muli: {
                result << "#*p *= " << int(op.arg1) << ";\n";
                result << "lb $t1, ($t0)\n";
                result << "li $t2, " << int(op.arg1) << "\n";
                result << "mult $t1, $t2\n";
                result << "mflo $t1\n";
                result << "andi $t1, $t1, 0x00ff\n";
                result << "sb $t1, ($t0)\n";
            } break;

            case Opcode::Madd: {
                result << "#*p += p[" << int(op.arg2) << "] * " << int(op.arg1) << ";\n";
                result << "lb $t1, " << int(op.arg2) << "($t0)\n";
                result << "li $t2, " << int(op.arg1) << "\n";
                result << "mult $t2, $t1\n";
                result << "mflo $t2\n";
                result << "lb $t1, ($t0)\n";
                result << "add $t1, $t1, $t2\n";
                result << "andi $t1, $t1, 0x00ff\n";
                result << "sb $t1, ($t0)\n";
            } break;

            case Opcode::Jmp: {
            	result << "#p += " << int(op.arg1) << ";\n";
                result << "addi $t0, $t0, " << int(op.arg1) << "\n";
            } break;

            default: std::cerr << "Bad opcode: " << int(op.opcode) << '\n';
        }
    }

    result << output_footer;

    return result.str();
}
