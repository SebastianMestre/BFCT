#include <fstream>
#include <iostream>
#include <string>

#include <cstring>

#include "c_backend.hpp"
#include "frontend.hpp"
#include "optimizer.hpp"

constexpr char const* usage_string = " -- BrainFuck C Transpiler --\n"
                                     " -- usage: bfct [-o <name>] <file>\n";
void bad_usage()
{
    std::cerr << usage_string;
    std::terminate();
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
        out_file << write_c_code(optimize(compile(file_contents)));
    }

    return 0;
}
