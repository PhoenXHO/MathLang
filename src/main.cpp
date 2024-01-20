#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <string_view>

#include "vm.h"
#include "globals.h"

#include "mathlangconfig.h"

#define EXTENSION ".mthl"
#define EXTENSION_LENGTH 5

void print_usage(void);
void print_version(void);
void print_help(void);

void open_file(std::string path);

bool read_arguments(int argc, const char ** argv);
size_t find_dot(std::string_view path);
void check_file_extension(std::string_view path);
std::string_view extract_file_name(std::string_view path);
void tabs_to_spaces(std::string & source);

void repl(void);

std::string_view file_name;

int main(int argc, const char ** argv)
{
	// Initialize init_repl to true to start the REPL by default
	bool init_repl = true;

	// Try to parse command line arguments
	try
	{
		// Read arguments and set init_repl to false if a file is passed 
		// or if the -v and -h flags are passed
		init_repl = read_arguments(argc, argv);
	}
	catch (const std::exception & e)
	{
		std::cerr << "\nerror: " << e.what() << " (use `mathlang -h` for help)" << '\n';
		return 1;
	}

	// If init_repl is still true, start the REPL
	if (init_repl)
	{
		try
		{
			repl();
		}
		catch (const std::exception & e)
		{
			std::cerr << "error: " << e.what() << '\n';
			return 1;
		}
	}

	return 0;
}

void repl(void)
{
	print_version(); // print version info
	std::cout << "Type `quit` to terminate the interpreter\n";

	VM vm;
	// loop continuously to read user input
	while (true)
	{
		std::cout << "> ";

		std::string line;
		std::getline(std::cin, line);

		if (line == "quit")
			break;

		if (line == "debug")
		{
			// enable debug output
			config::print_lexer_output = true;
			config::print_parser_output = true;
			config::print_compiler_output = true;
			std::cout << "debug mode enabled\n";
			continue;
		}

		vm.interpret_source(line);
		std::cout << std::endl;
	}
}

bool read_arguments(int argc, const char **argv)
{
#define IS_SHORT_FLAG(character, flag) \
	(std::strlen(flag) == 2) && (flag[1] == character)
#define IS_LONG_FLAG(string, flag) \
	(std::strlen(flag) > 2) && (flag[1] == '-' && strcmp(flag + 2, string) == 0)

	// Loop through all command line arguments
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-') // Check if current argument starts with '-'
		{
			// Handle -v/--version flags
			if (IS_SHORT_FLAG('v', argv[i]) || IS_LONG_FLAG("version", argv[i]))
			{
				print_version();
				return false; // Exit early
			}

			// Handle -h/--help flags
			if (IS_SHORT_FLAG('h', argv[i]) || IS_LONG_FLAG("help", argv[i]))
			{
				print_help();
				return false; // Exit early
			}

			// Handle -f flag
			if (IS_SHORT_FLAG('f', argv[i]))
			{
				if (i + 1 > argc - 1) // Check that a file was specified
					throw std::invalid_argument("no file was specified");

				open_file(argv[i + 1]);
				return false; // Exit early
			}

			// Handle -l flag
			if (IS_SHORT_FLAG('l', argv[i]))
			{
				config::print_lexer_output = true;
				config::print_parser_output = true;
				config::print_compiler_output = true;
				continue;
			}
		}
	}

	return true; // If no flags matched, start REPL
}

void open_file(std::string path)
{
	check_file_extension(path);
	file_name = extract_file_name(path);

	std::ifstream file { path };

	if (!file.is_open())
		throw std::ios_base::failure("unable to open file `" + path + '`');

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::string source = buffer.str();
	tabs_to_spaces(source);

	VM vm;
	vm.interpret_source(source);

	file.close();
}

std::string_view extract_file_name(std::string_view path)
{
	size_t i = path.length() - 1;
	// loop through the path backwards to find the last '/' or '\'
	while ((path[i] != '/' && path[i] != '\\') && i >= 0)
		i--;

	// return the substring after the last '/' or '\'
	// this is the file name
	return path.substr(i + 1);
}

void check_file_extension(std::string_view path)
{
	size_t extension_index = find_dot(path);
	// if no '.' was found, throw an error
	if (extension_index == std::string::npos)
		throw std::invalid_argument("file format not recognized");

	// get the extension substring (including the dot)
	std::string_view extension = path.substr(extension_index);

	// check if the extension is invalid
	if (extension.length() == 1)
		throw std::invalid_argument("invalid file format");

	// check if the extension matches the expected one
	if (extension.length() != EXTENSION_LENGTH || extension != EXTENSION)
		throw std::invalid_argument("file format not recognized");
}

size_t find_dot(std::string_view path)
{
	for (size_t i = path.length() - 1; (path[i] != '/' && path[i] != '\\') && i >= 0; i--)
		if (path[i] == '.')
			return i;

	return std::string::npos;
}

void tabs_to_spaces(std::string & source)
{
	size_t tab_index = source.find('\t');
	while (tab_index != std::string::npos)
	{
		source.replace(tab_index, 1, "    ");
		tab_index = source.find('\t');
	}
}


void print_usage(void)
{
	std::cout << "usage: mathlang [<options> -f <file>]\n";
}

void print_version(void)
{
    std::cout << "MathLang Interpreter v"
              << MathLang_VERSION_MAJOR << '.' << MathLang_VERSION_MINOR
              << '\n';
}

void print_help(void)
{
    print_usage();
    std::cout << "options:\n"
			  << "    --help (or -h)\t: Display this help message\n"
    		  << "    --version (or -v)\t: Display interpreter version and additional information\n"
			  << "    -f <file>\t\t: Read from a file. <file> must have the `.mthl` extension\n"
			  << "    -l\t\t\t: Print the stream of tokens generated by the lexer\n";
}
