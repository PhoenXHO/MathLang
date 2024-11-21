# MathLang Interpreter

MathLang is a custom programming language designed for mathematical computations and symbolic expressions. This repository contains the source code for the MathLang interpreter, which is currently in the alpha stage of development.

_Note: This project is a complete rewrite of the MathLang interpreter, with cleaner code and more organized structure. The interpreter will be able to run code in a REPL, and will have a better error handling system._

## Features

### Current Features (0.2.0-alpha.1)
- Basic expressions with numbers, parentheses, and built-in operators
- Precise operations with integers and floating-point numbers using GMP and MPFR libraries
- Type checking
- Variables and constants
- Function definitions and calls
- Built-in functions
- REPL (Read-Eval-Print Loop) for interactive programming

### Planned Features
- Class definitions and object creation
- Built-in classes
- Control flow statements
- More built-in functions and classes
- Operator overloading and type conversions
- Symbolic expressions and mappings

## Getting Started

### Prerequisites
- CMake 3.29.7
- Boost 1.74.0
- GMP
- MPFR

### Building the Project
1. Clone the repository:
	```sh
	git clone https://github.com/PhoenXHO/MathLang.git
	cd MathLang
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
./MathLang
```

<!-- Not yet implemented -->
<!--To run a MathLang script:
```sh
./MathLang -f <script.mthl>
```-->

## Usage

### REPL Commands
- `quit` or `exit`: Terminate the interpreter
- `\` at the end of a line: Continue on the next line (note that if the line is incomplete, the interpreter will wait for more input)

### Command Line Options
- `-h, --help`: Display help message
- `-v, --version`: Display interpreter version
- `-f, --file <file>`: Read from a file with `.mthl` extension
<!--- `-D, --dev`: Enable debug mode-->
- `-V, --verbose`: Enable verbose output

## Contributing
Contributions are welcome! For major changes, please open an issue first to discuss what you would like to change.

### License
This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.