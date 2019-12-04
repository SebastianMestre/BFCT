#include <fstream>
#include <iostream>
#include <string>

#include <cstring>

#include "mips_backend.hpp"
#include "c_backend.hpp"
#include "frontend.hpp"
#include "optimizer.hpp"

constexpr char const* usage_string = " -- BrainFuck C Transpiler --\n"
                                     " -- usage: bfct [-o <name>] [-t <target>] <file>\n"
                                     " -- target can be one of: c mips\n";
void bad_usage()
{
    std::cerr << usage_string;
    std::terminate();
}

enum class Target {
	C, MIPS
};

template<typename Value>
void report_command_error (int argument_index, char const* expected_description, Value value) {
	std::cerr << "On argument " << argument_index
	          << " expected " << expected_description
	          << " but got: " << value << '\n';
	bad_usage();
}

int main(int argc, char** argv)
{

	if (argc < 2)
		bad_usage();

	std::string output_path{ "out.c" };
	std::string input_path{};
	Target target = Target::C;

	{
		bool has_input = false;

		for (int i = 1; i < argc; ++i)
		{
			if (argv[i][0] == '-')
			{
				if (strcmp(argv[i], "-o") == 0)
				{
					if (i + 1 >= argc)
					{
						report_command_error(i + 1, "an output file name", "nothing");
					}

					output_path = argv[++i];
				}
				else if (strcmp(argv[i], "-t") == 0)
				{
					if (i + 1 >= argc)
					{
						report_command_error(i + 1, "a valid target", "nothing");
					}

					if (strcmp(argv[i + 1], "c") == 0)
					{
						target = Target::C;
					}
					else if (strcmp(argv[i + 1], "mips") == 0)
					{
						target = Target::MIPS;
					}
					else
					{
						report_command_error(i + 1, "a valid target", argv[i + 1]);
					}

					++i;
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
		{
			bad_usage();
		}
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
		auto compilation_result = compile(file_contents);

		switch (target)
		{
		case Target::C:
			out_file << write_c_code(compilation_result);
			break;
// 		case Target::MIPS:
// 			out_file << write_mips_code(compilation_result);
// 			break;
		default:
			std::cerr << "Could not compile.\n";
		}
	}

	return 0;
}
