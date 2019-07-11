#include "c_backend.hpp"

#include <iostream>
#include <sstream>

namespace {
constexpr char const* output_header = "#include <stdio.h>\n"
                                      "#include <stdint.h>\n"
                                      "#define BUFFER_SIZE 1000\n"
                                      "int main(){\n"
                                      "uint8_t buffer[BUFFER_SIZE] = {};\n"
                                      "uint8_t* p = buffer + BUFFER_SIZE / 2;\n";

constexpr char const* output_footer = "return 0;\n"
                                      "}\n";
}

std::string write_c_code(std::vector<Op> const& code)
{
    std::stringstream result;
    result << output_header;

    for (Op op : code)
    {
        switch (op.opcode)
        {
            case Opcode::Lop: result << "while(*p){\n"; break;
            case Opcode::Lcl: result << "}\n"; break;
            case Opcode::Put: result << "putchar(*p);\n"; break;
            case Opcode::Get: result << "*p = getchar();\n"; break;

            case Opcode::Addi: result << "*p += " << int(op.arg1) << ";\n"; break;
            case Opcode::Seti: result << "*p = "  << int(op.arg1) << ";\n"; break;
            case Opcode::Muli: result << "*p *= " << int(op.arg1) << ";\n"; break;

            case Opcode::Madd: result << "*p += p[" << int(op.arg2) << "] * " << int(op.arg1) << ";\n"; break;

            case Opcode::Jmp: result << "p += " << int(op.arg1) << ";\n"; break;

            default: std::cerr << "Bad opcode: " << int(op.opcode) << '\n';
        }
    }

    result << output_footer;

    return result.str();
}
