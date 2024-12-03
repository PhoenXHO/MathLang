# Mathium Interpreter (0.2.0-alpha.1)

Mathium is a custom programming language designed for mathematical computations and symbolic expressions. This repository contains the source code for the Mathium interpreter, which is currently in the alpha stage of development.

_Note: This project is a complete rewrite of the Mathium interpreter, with cleaner code and more organized structure. The interpreter will be able to run code in a REPL, and will have a better error handling system._

## Features

### Current Features
- Basic expressions with numbers, parentheses, and built-in operators
	- Currently, only the addition operator (`+`) is supported (more operators will be added for this version)
- REPL (Read-Eval-Print Loop) for interactive programming
- Variable declarations and retrievals
	- `let <variable>` to declare a variable
	- `let <variable> := <expression>` to initialize a variable

### Planned Features for 0.2.0-alpha.1
- Read from a file with `.mthx` extension
- More built-in operators
- Precise operations with integers and floating-point numbers using GMP and MPFR libraries
- Variables and constants (with different scopes)
- Function definitions and calls
- Built-in functions

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
- GMP
- MPFR

### Building the Project
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
./Mathium -f <script.mthl>
```-->

## Usage

### REPL Commands
- `quit` or `exit`: Terminate the interpreter
- `\` at the end of a line: Continue on the next line (note that if the line is incomplete, the interpreter will wait for more input)

### Command Line Options
- `-h, --help`: Display help message
- `-v, --version`: Display interpreter version
- `-f, --file <file>`: Read from a file with `.mthx` extension
<!--- `-D, --dev`: Enable debug mode-->
- `-V, --verbose`: Enable verbose output

## Contributing
Contributions are welcome! For major changes, please open an issue first to discuss what you would like to change.

### License
This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.