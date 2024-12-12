# Mathium Interpreter (0.2.0-alpha.1)

Mathium is a custom programming language designed for mathematical computations and symbolic expressions. This repository contains the source code for the Mathium interpreter, which is currently in the alpha stage of development.

_Note: This project is a complete rewrite of the Mathium interpreter, with cleaner code and more organized structure. The interpreter will be able to run code in a REPL, and will have a better error handling system._

## Table of Contents
- [Features](#features)
	- [Current Features](#current-features)
	- [Planned Features for 0.2.0-alpha.1](#planned-features-for-020-alpha1)
	- [Future Features](#future-features)
- [Getting Started](#getting-started)
	- [Prerequisites](#prerequisites)
	- [Building the Project](#building-the-project)
	- [Running the Interpreter](#running-the-interpreter)
- [Usage](#usage)
	- [REPL Commands](#repl-commands)
	- [Command Line Options](#command-line-options)
- [Contributing](#contributing)
	- [License](#license)

## Features

### Current Features
- Basic expressions with numbers, parentheses, and built-in operators
	- Currently, only the addition operator (`+`) is supported (more operators will be added for this version)
	- Precise operations with integers and floating-point numbers using GMP and MPFR libraries
- REPL (Read-Eval-Print Loop) for interactive programming
- Variable declarations and retrievals
	- `let <variable>` to declare a variable
	- `let <variable> := <expression>` to initialize a variable
	- `let <type> <variable> := <expression>` to declare a variable with a specific type
- Function calls (built-in functions only)

### Planned Features for 0.2.0-alpha.1
- Read from a file with `.mthx` extension
- More built-in operators
- Variables and constants (with different scopes)
- Function definitions
- Built-in functions
- Classes and objects

### Future Features
- Class definitions and object creation
- Built-in classes
- Type checking and type casting (implicit and explicit)
- Control flow statements
- More built-in functions and classes
- Operator overloading and type cast overloading
- Symbolic expressions and mappings

## Getting Started

### Prerequisites
- CMake 3.29.7
- Boost 1.74.0
- GMP (GNU Multiple Precision Arithmetic Library)
- MPFR (Multiple Precision Floating-Point Reliable Library)

### Building the Project

To build the Mathium interpreter, follow these steps:

1. **Install the Prerequisites**:
	- On Ubuntu:
		```sh
		sudo apt-get install cmake libboost-all-dev libgmp-dev libmpfr-dev
		```
	- On Fedora:
		```sh
		sudo dnf install cmake boost-devel gmp-devel mpfr-devel
		```
	- On macOS (using Homebrew):
		```sh
		brew install cmake boost gmp mpfr
		```
	- On Windows:
		- Download and install CMake from [cmake.org](https://cmake.org/download/)
		- Download and install Boost from [boost.org](https://www.boost.org/users/download/)
		- Download and install GMP from [gmplib.org](https://gmplib.org/)
		- Download and install MPFR from [mpfr.org](https://www.mpfr.org/)
		- _Add the paths to the libraries to the system environment variables_

2. **Build the Project**:
	1. Clone the repository:
		```sh
		git clone https://github.com/PhoenXHO/MathLang.git
		cd Mathium
		```

	2. Create a build directory and navigate to it:
		```sh
		mkdir build
		cd build
		```

	3. Configure the project using CMake:
		```sh
		cmake ..
		```

	4. Build the project:
		```sh
		make
		```

### Running the Interpreter
To start the REPL:
```sh
./Mathium
```

<!-- Not yet implemented -->
<!--To run a Mathium script:
```sh
./Mathium -f <script.mthx>
```-->

## Usage

### REPL Commands
- `quit` or `exit`: Terminate the interpreter
- `\` at the end of a line: Continue on the next line. Note that if the line is incomplete, the interpreter will wait for more input, and if you want to cancel continuing on the next line, hit `Enter` without typing anything.

### Command Line Options
- `-h, --help`: Display help message
- `-v, --version`: Display interpreter version
- `-f, --file <file>`: Read from a file with `.mthx` extension
<!--- `-D, --dev`: Enable debug mode-->
- `-V, --verbose`: Enable verbose output
- `-P, --print-all`: Print all expressions without a semicolon at the end (if not enabled, only the last expression will be printed)
- `-b, --benchmark`: Enable benchmark mode (print execution time at the end of the program)

## Contributing
Contributions are welcome! For major changes, please open an issue first to discuss what you would like to change.

### License
This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.