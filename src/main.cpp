#include <iostream>
#include <fstream> // for file I/O
#include <sstream> // for string streams
#include <cstring> // for `strcmp`
#include <string>
#include <string_view>
#include <boost/program_options.hpp> // for command line options

#include "mathium_config.hpp"

#include "vm/vm.hpp"
#include "util/config.hpp"
#include "debug/verbose.hpp"


#define EXTENSION ".mthx"
#define EXTENSION_LENGTH 5


namespace po = boost::program_options;


void define_options(po::options_description & desc);
void parse_options(int argc, const char ** argv, po::options_description & desc);

void print_usage(void);
void print_version(void);

void repl(void);
bool get_continuation(std::string & source);
void check_exit(std::string_view source);


namespace globals
{
	std::string_view source;
}

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
		std::cout << "Mathium> ";
		std::string source;
		std::getline(std::cin, source);

		// Strip leading and trailing whitespaces
		source.erase(source.find_last_not_of(" \t\n\r\f\v") + 1);

		check_exit(source);
		if (!source.empty() && source.back() == '\\')
		{
			// Strip the trailing backslash
			source.pop_back();
			//source += '\n';
			get_continuation(source);
		}
		if (source.empty())
			continue;
		
		// Flag to check if the source has been interpreted at least once
		// This is just to reduce repeated code
		bool interpreted = false;
		bool interrupted = false;
		InterpretResult result;
		while (!interrupted && (!interpreted || result == InterpretResult::INCOMPLETE_CODE))
		{
			if (interpreted)
				interrupted = get_continuation(source);

			globals::source = source;
			result = vm.interpret_source(interrupted);
			interpreted = true;
		}
	}
}

/// @brief Get the continuation of a source code snippet
/// @param source: The source code snippet
/// @return `true` if the user wants to interrupt the continuation, `false` otherwise
bool get_continuation(std::string & source)
{
	std::string continuation;
	while (true)
	{
		std::cout << " ... >>> ";
		std::string line;
		std::getline(std::cin, line);

		// Strip leading and trailing whitespaces
		line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

		if (line.empty())
			return true;
		check_exit(line);

		if (line.back() == '\\')
		{
			// Strip the trailing backslash
			line.pop_back();
			line += '\n';
			continuation += line;
		}
		else
		{
			continuation += '\n';
			continuation += line;
			break;
		}
	}

	source += continuation;
	return false;
}

/// @brief Check if the user wants to exit the interpreter
/// @param source: The source code snippet
/// @return `true` if the user wants to exit, `false` otherwise
void check_exit(std::string_view source)
{
	// Strip leading and trailing whitespaces
	source.remove_prefix(source.find_first_not_of(" \t\n\r\f\v"));
	source.remove_suffix(source.size() - source.find_last_not_of(" \t\n\r\f\v") - 1);
	if (source == "quit" || source == "exit")
	{
		exit(0);
	}
}

void define_options(po::options_description & desc)
{
	desc.add_options()
		("help,h", "Display this help message")
		("version,v", "Display interpreter version and additional information")
		("file,f", po::value<std::string>()->value_name("<file>"), "Read from a file. <file> must have the `" EXTENSION "` extension")
		("dev,D", "Enable debug mode")
		("verbose,V", "Enable verbose output")
		("print-all,P", "Print all expressions that do not have a semicolon at the end (if this option is not enabled, only the last expression will be printed)")
		("benchmark,b", "Print the time taken to execute the program at the end of execution (in microseconds)")
		("no-warn,W", "Disable warnings")
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

	if (vmap.count("help"))
	{
		print_usage();
		std::cout << desc << '\n';
	}
	if (vmap.count("version"))
	{
		print_version();
	}

	//! For later use
	if (vmap.count("file"))
	{
		config::repl_mode = false;
		std::string file_name = vmap["file"].as<std::string>();
		//open_file(file_name);
		//interpret_file(file_name);
	}

	if (vmap.count("dev"))
	{
		// enable debug output
		std::cout << "\033[1;31m* Developer mode enabled\033[0m\n";
		config::dev = true;
	}
	if (vmap.count("verbose"))
	{
		// enable verbose output
		std::cout << "\033[1;33m* Verbose mode enabled\033[0m\n";
		config::verbose = true;
	}
	if (vmap.count("print-all"))
	{
		// print all expressions that do not have a semicolon at the end
		std::cout << "\033[1;32m* Print-all enabled\033[0m\n";
		config::print_all = true;
	}

	if (vmap.count("benchmark"))
	{
		// enable benchmark mode
		std::cout << "\033[1;34m* Benchmark mode enabled\033[0m\n";
		config::benchmark = true;
	}

	if (vmap.count("no-warn"))
	{
		// disable warnings
		std::cout << "\033[1;35m* Warnings disabled\033[0m\n";
		config::warnings = false;
	}
}

void print_usage(void)
{
	std::cout << "Usage: Mathium [<options>]\n";
}

void print_version(void)
{
	if (version_printed)
		return;

	version_printed = true;
	std::cout << "Mathium Interpreter v";
	std::cout << Mathium_VERSION_MAJOR << '.' << Mathium_VERSION_MINOR << '.' << Mathium_VERSION_PATCH;
	if (Mathium_PRE_RELEASE)
	{
		std::cout << "-" << Mathium_VERSION_STATUS << "." << Mathium_VERSION_STATUS_VERSION;
	}
	std::cout << "\nBuilt on " << __DATE__ << " at " << __TIME__ << "\n";
}
