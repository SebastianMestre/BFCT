#include "c_backend.hpp"

#include <iostream>
#include <sstream>

namespace mips {
constexpr char const* output_header = ".data\n"
                                      ".text\n"
                                      "main:\n"
									  "li $v0, 9\n"
									  "li $a0, 1000\n"
									  "syscall\n"
                                      "move $t0, $v0\n"
                                      "addi $t0, $t0, 500\n";

constexpr char const* output_footer = "li $v0, 10\n"
                                      "syscall\n";
}

/*
 * Memory pointer will be $t0
 * We will use $t1 as aux register 1
 * We will use $t2 as aux register 2
 */

std::string write_mips_code(std::vector<Op> const& code)
{
	using namespace mips;
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
			/*
            case Opcode::Lop: {
                int loopId = loopCount;
                loopStack.push_back(loopCount);
                loopCount++;
                
                result << "L" << loopId << ":\n";
                result << "lb $t1, 0($t0)\n";
                result << "beq $t1, $0, LE" << loopId << "\n";
            } break;
            case Opcode::Lcl: {
                int loopId = loopStack.back();
                loopStack.pop_back();
                
                result << "j L" << loopId << "\n";
                result << "LE" << loopId << ":\n";
            } break;

            case Opcode::Cop: result << "if(*p){\n"; break;
            case Opcode::Ccl: result << "}\n"; break;
			*/

            case Opcode::Put: {
                result << "li $v0, 11\n";
                result << "lb $t1, ($t0)\n";
                result << "move $a0, $t1\n";
                result << "syscall\n";
            } break;
            case Opcode::Get: {
                result << "li $v0, 12\n";
                result << "syscall\n";
                result << "move $t1, $v0\n";
                result << "andi $t1, $t1, 0x00ff\n";
                result << "sb $t1, ($t0)\n";
            } break;

            case Opcode::Addi: {
                result << "lb $t1, ($t0)\n";
                result << "addi $t1, $t1, " << int(op.arg1) << "\n";
                result << "andi $t1, $t1, 0x00ff\n";
                result << "sb $t1, ($t0)\n";
            } break;
            case Opcode::Seti: {
                result << "li $t1, " << int(op.arg1) << "\n";
                result << "andi $t1, $t1, 0x00ff\n";
                result << "sb $t1, ($t0)\n";
            } break;
            case Opcode::Muli: {
                result << "lb $t1, ($t0)\n";
                result << "li $t2, " << int(op.arg1) << "\n";
                result << "mult $t1, $t2\n";
                result << "mflo $t1\n";
                result << "andi $t1, $t1, 0x00ff\n";
                result << "sb $t1, ($t0)\n";
            } break;

            case Opcode::Madd: {
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
                result << "addi $t0, $t0, " << int(op.arg1) << "\n";
            } break;

            default: std::cerr << "Bad opcode: " << int(op.opcode) << '\n';
        }
    }

    result << output_footer;

    return result.str();
}
