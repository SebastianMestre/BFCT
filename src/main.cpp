#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include <cstring>
#include <cassert>

constexpr char const* usage_string = " -- BrainFuck C Transpiler --\n"
                                     " -- usage: bfct [-o <name>] <file>\n";

constexpr char const* output_header = "#include <stdio.h>\n"
                                      "#include <stdint.h>\n"
                                      "#define BUFFER_SIZE 1000\n"
                                      "int main(){\n"
                                      "uint8_t buffer[BUFFER_SIZE] = {};\n"
                                      "uint8_t* p = buffer + BUFFER_SIZE / 2;\n";

constexpr char const* output_footer = "return 0;\n"
                                      "}\n";

void bad_usage()
{
    std::cerr << usage_string;
    std::terminate();
}

enum class Opcode : uint8_t{
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

std::vector<Op> compile(std::string const& code)
{
    std::vector<Op> result;
    for (char c : code)
    {
        switch (c)
        {
            case '+': result.push_back({ Opcode::Inc }); break;
            case '-': result.push_back({ Opcode::Dec }); break;
            case '>': result.push_back({ Opcode::Mvr }); break;
            case '<': result.push_back({ Opcode::Mvl }); break;
            case '[': result.push_back({ Opcode::Lop }); break;
            case ']': result.push_back({ Opcode::Lcl }); break;
            case '.': result.push_back({ Opcode::Put }); break;
            case ',': result.push_back({ Opcode::Get }); break;
        }
    }
    return result;
}

Op multi_op (Opcode opcode, int count) {
    assert(count < 65536 && "The optimizer cannot handle more than 65535 for a multi op.");

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

std::vector<Op> collapse_consecutive(std::vector<Op> const& code)
{
    std::vector<Op> result;

    int count = 0;
    Opcode opcode = Opcode::Count; // dummy value
    for (Op op : code)
    {
        if (op.opcode != opcode and count)
        {
            result.push_back(multi_op(opcode, count));
        }

        if (not has_multi(op.opcode))
        {
            result.push_back(op);
            opcode = Opcode::Count;
            count = 0;
        }
        else
        {
            if (op.opcode == opcode)
            {
                count++;
            }
            else
            {
                opcode = op.opcode;
                count = 1;
            }
        }
    }

    return result;
}

std::vector<Op> constant_folding(std::vector<Op> const& code)
{
    std::vector<Op> result;

    int i = 0;
    for (; i < code.size() - 2; ++i)
    {
        if (code[i].opcode == Opcode::Lop)
        {
            if (code[i + 1].opcode == Opcode::Inc || code[i + 1].opcode == Opcode::Dec
                || code[i + 1] == Op{ Opcode::Addi, 1 } || code[i + 1] == Op{ Opcode::Subi, 1 })
            {
                if (code[i + 2].opcode == Opcode::Lcl)
                {
                    result.push_back(Op{Opcode::Seti, 0});
                    i += 2;
                    continue;
                }
            }
        }

        // this only runs if the check above fails somewhere
        result.push_back(code[i]);
    }
    
    for (; i < code.size(); ++i) {
        result.push_back(code[i]);
    }

    return result;
}

std::vector<Op> optimize(std::vector<Op> const& code)
{
    std::vector<Op> result = code;
    
    result = collapse_consecutive(result);
    result = constant_folding(result);

    return result;
}

std::string write_c_code(std::vector<Op> const& code)
{
    std::stringstream result;

    for (Op op : code)
    {
        switch (op.opcode)
        {
            case Opcode::Inc: result << "(*p)++;\n"; break;
            case Opcode::Dec: result << "(*p)--;\n"; break;
            case Opcode::Mvr: result << "p++;\n"; break;
            case Opcode::Mvl: result << "p--;\n"; break;
            case Opcode::Lop: result << "while(*p){\n"; break;
            case Opcode::Lcl: result << "}\n"; break;
            case Opcode::Put: result << "putchar(*p);\n"; break;
            case Opcode::Get: result << "*p = getchar();\n"; break;

            case Opcode::Add: result << "*p += p[" << int(op.uval) << "];\n"; break;
            case Opcode::Sub: result << "*p -= p[" << int(op.uval) << "];\n"; break;
            case Opcode::Set: result << "*p = p["  << int(op.uval) << "];\n"; break;
            case Opcode::Mul: result << "*p *= p[" << int(op.uval) << "];\n"; break;

            case Opcode::Addi: result << "*p += " << int(op.uval) << ";\n"; break;
            case Opcode::Subi: result << "*p -= " << int(op.uval) << ";\n"; break;
            case Opcode::Seti: result << "*p = "  << int(op.uval) << ";\n"; break;
            case Opcode::Muli: result << "*p *= " << int(op.uval) << ";\n"; break;

            case Opcode::Jmp: result << "p += " << int(op.sval) << ";\n"; break;

            default: std::cerr << "Bad opcode: " << int(op.opcode) << '\n';
        }
    }

    return result.str();
}

int main(int argc, char** argv)
{

    if (argc < 2)
        bad_usage();

    std::string output_path{ "out.c" };
    std::string input_path{};

    {
        bool has_input = false;

        for (int i = 1; i < argc; ++i)
        {
            if (argv[i][0] == '-')
            {
                if (strcmp(argv[i], "-o") == 0)
                {
                    output_path = argv[++i];
                }
                else
                {
                    bad_usage();
                }
            }
            else
            {
                if (not has_input)
                {
                    input_path = argv[i];
                    has_input = true;
                }
                else
                {
                    bad_usage();
                }
            }
        }

        if (not has_input)
            bad_usage();
    }

    {
        std::string file_contents;

        {
            std::ifstream in_file{ input_path };
            std::string line;
            while (std::getline(in_file, line))
            {
                file_contents.append(line);
            }
        }

        std::ofstream out_file{ output_path };
        out_file << output_header;
        out_file << write_c_code(optimize(compile(file_contents)));
        out_file << output_footer;
    }

    return 0;
}

