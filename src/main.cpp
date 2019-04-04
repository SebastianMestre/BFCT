#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

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

int collapse_repetitions(std::ostream& out, int i, std::string const& line, char rep,
    const char* single, const char* multi)
{
    int count = 1;
    while (i + 1 < line.size() && line[i + 1] == rep)
    {
        count++;
        i++;
    }

    if (count == 1)
        out << single;
    else
        out << multi << count << ";\n";

    return i;
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
        std::string line;
        std::ifstream in_file{ input_path };
        std::ofstream out_file{ output_path };

        out_file << output_header;
        while (std::getline(in_file, line))
        {
            for (int i = 0; i < line.size(); ++i)
            {
                char c = line[i];
                switch (c)
                {
                case '+':
                    i = collapse_repetitions(out_file, i, line, '+', "(*p)++;", "*p += ");
                    break;
                case '-':
                    i = collapse_repetitions(out_file, i, line, '-', "(*p)--;", "*p -= ");
                    break;
                case '>':
                    i = collapse_repetitions(out_file, i, line, '>', "p++;", "p += ");
                    break;
                case '<':
                    i = collapse_repetitions(out_file, i, line, '<', "p--;", "p -= ");
                    break;
                case '[':
                {
                    if (i + 2 < line.size() && line[i + 1] == '-' && line[i + 2] == ']')
                    {
                        out_file << "*p = 0;\n";
                        i += 2;
                    }
                    else
                    {
                        out_file << "while (*p) {\n";
                    }
                }
                break;
                case ']':
                    out_file << "}\n";
                    break;
                case '{':
                    out_file << "while (*p) { p++;\n";
                    break;
                case '}':
                    out_file << "p--; (*p)--; }\n";
                    break;
                case '(':
                    out_file << "while (*p) { uint8_t* old_p = p; p++;\n";
                    break;
                case ')':
                    out_file << "p = old_p; (*p)--; }\n";
                    break;
                case '.':
                    out_file << "putchar(*p);\n";
                    break;
                case ',':
                    out_file << "*p = getchar();\n";
                    break;
                case ' ':
                case '\t':
                    break;
                default:
                    std::clog << "Invalid Instruction: " << c << '\n';
                    break;
                }
            }
        }
        out_file << output_footer;
    }

    return 0;
}
