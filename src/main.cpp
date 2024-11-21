#include <iostream>
#include <fstream> // for file I/O
#include <sstream> // for string streams
#include <cstring> // for `strcmp`
#include <string>
#include <string_view>
#include <boost/program_options.hpp> // for command line options

#include "mathlangconfig.hpp"

#include "vm/vm.hpp"
#include "global/config.hpp"
#include "debug/verbose.hpp"

#define EXTENSION ".mthl"
#define EXTENSION_LENGTH 5

namespace po = boost::program_options;

void define_options(po::options_description & desc);
void parse_options(int argc, const char ** argv, po::options_description & desc);

void print_usage(void);
void print_version(void);

void repl(void);
void get_continuation(std::string & source);

bool version_printed = false;

int main(int argc, const char ** argv)
{
	po::options_description desc("Options");

	define_options(desc);
	parse_options(argc, argv, desc);

	if (config::repl_mode)
	{
		repl();
	}

	return 0;
}

void repl(void)
{
	print_version(); // Print version info
	std::cout << "- Type `quit` or `exit` to terminate the interpreter\n"
			  << "- Type `\\` at the end of a line to continue on the next line\n"
			  << "- To end continuation, press Enter\n";
	
	if (config::verbose)
	{
		log_v("Verbose mode enabled\n");
	}

	VM vm;
	while (true)
	{
		std::cout << "MathLang> ";
		std::string source;
		std::getline(std::cin, source);

		// Strip leading and trailing whitespaces
		source.erase(source.find_last_not_of(" \t\n\r\f\v") + 1);

		if (source == "quit" || source == "exit")
			break;

		if (!source.empty() && source.back() == '\\')
		{
			// Strip the trailing backslash
			source.pop_back();
			source += '\n';
			get_continuation(source);
		}
		
		// Flag to check if the source has been interpreted at least once
		// This is just to reduce repeated code
		bool interpreted = false;
		InterpretResult result;
		while (!interpreted || result == InterpretResult::INCOMPLETE_CODE)
		{
			if (interpreted)
				get_continuation(source);

			result = vm.interpret_source(source);
			interpreted = true;
		}
	}
}

void get_continuation(std::string & source)
{
	std::string continuation;
	while (true)
	{
		std::cout << "  ... >>> ";
		std::string line;
		std::getline(std::cin, line);

		// Strip leading and trailing whitespaces
		line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

		if (line.empty())
			break;

		if (line.back() == '\\')
		{
			// Strip the trailing backslash
			line.pop_back();
			line += '\n';
			continuation += line;
		}
		else
		{
			continuation += line;
			break;
		}
	}

	source += continuation;
}

void define_options(po::options_description & desc)
{
	desc.add_options()
		("help,h", "Display this help message")
		("version,v", "Display interpreter version and additional information")
		("file,f", po::value<std::string>()->value_name("<file>"), "Read from a file. <file> must have the `.mthl` extension")
		("dev,D", "Enable debug mode")
		("verbose,V", "Enable verbose output")
		;
}

void parse_options(int argc, const char ** argv, po::options_description & desc)
{
	po::variables_map vmap;

	try {
		po::store(po::parse_command_line(argc, argv, desc), vmap);
		po::notify(vmap);
	}
	catch (const po::error & e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		print_usage();
		std::cout << desc;
		exit(1);
	}

	bool repl_mode = true;

	if (vmap.count("help"))
	{
		print_usage();
		std::cout << desc << '\n';
	}
	else if (vmap.count("version"))
	{
		print_version();
	}

	//! For later use
	else if (vmap.count("file"))
	{
		repl_mode = false;
		std::string file_name = vmap["file"].as<std::string>();
		//open_file(file_name);
		//interpret_file(file_name);
	}

	else if (vmap.count("dev"))
	{
		// enable debug output
		std::cout << "\033[1;31m* Developer mode enabled\033[0m\n";
		config::dev = true;
	}
	else if (vmap.count("verbose"))
	{
		// enable verbose output
		std::cout << "\033[1;33m* Verbose mode enabled\033[0m\n";
		config::verbose = true;
	}

	config::repl_mode = repl_mode;
}

void print_usage(void)
{
	std::cout << "Usage: mathlang [<options>]\n";
}

void print_version(void)
{
	if (version_printed)
		return;

	version_printed = true;
	std::cout << "MathLang Interpreter v";
	std::cout << MathLang_VERSION_MAJOR << '.' << MathLang_VERSION_MINOR << '.' << MathLang_VERSION_PATCH;
	if (MathLang_PRE_RELEASE)
	{
		std::cout << "-" << MathLang_VERSION_STATUS << "." << MathLang_VERSION_STATUS_VERSION;
	}
	std::cout << "\nBuilt on " << __DATE__ << " at " << __TIME__ << "\n";
}
