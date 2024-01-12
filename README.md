# MathLang Compiler

MathLang is a programming language designed for mathematical expressions and computations. This repository contains the source code for the MathLang compiler, which translates MathLang code into bytecode for execution on a virtual machine.

_This project is still a work in progress. If you are interested in contributing or collaborating, please feel free to reach out or submit a pull request._

# Current features
1. **Bytecode Compiler**: MathLang features a bytecode compiler that translates MathLang code into bytecode for execution on a virtual machine.

2. **Virtual Machine**: The bytecode generated by the compiler is executed on the MathLang Virtual Machine (MLVM), providing a runtime environment for MathLang programs.

3. **Arithmetic Expressions**: MathLang supports the basic arithmetic operations, including addition, subtraction, multiplication, division, and exponentiation.

4. **Supported Literals**: MathLang currently only supports the following literals:

	- Integer Literals: Whole numbers without decimal points
	- Real Literals: Numbers with decimal points

5. **Built-in Operators**: The language comes with built-in operators for arithmetic operations:
	- Addition: `+` (binary and unary),
	- Subtraction: `-` (binary and unary),
	- Multiplication: `*`  (binary),
	- Division: `/`  (binary),
	- Exponentiation: `^`  (binary),
	- Print: `print`  (unary)

	and more. These operators can be overloaded or extended (a planned features).

6. **Variable Declaration**: MathLang allows you to declare variables using the `let` keyword followed by the variable type and identifier. Optionally, you can initialize the variable with an expression using the `:=` operator.

	Syntax:

		let <type> <identifier>;
	or,

		let <type> <identifier> := <expression>;

_Note: Scopes are not supported yet. All variables are, currently, global variables._
	
# Future Features
## More literal types
Future versions of MathLang will introduce more literal types, expanding the language's support for various data types.

- Strings
- Boolean

## Conditional Statements
Conditional statements, such as `If` and `Else`, will be introduced to enable branching based on logical conditions.

Syntax:

	If [<condition1>] {...}
	Else If [<condition2>] {...}
	...
	Else {...}

## User-Defined Functions
Users will be able to define their own functions with custom logic.

Syntax:

	define <identifier>(<type> param1, ...) -> <type>;

## User-Defined Operators
MathLang will also allow users to define their own operators, that are at most four characters long, with custom behavior. This feature will provide flexibility and extensibility to the language, allowing developers to create domain-specific operators or redefine existing operators.

## Symbolic Expressions
Symbolic Expressions is an upcoming feature in MathLang that allows users to manipulate mathematical expressions symbolically, rather than numerically. With this feature, users will be able to work with variables, constants, and mathematical operations to create and manipulate complex mathematical expressions.


# How to Contribute
1. **Fork the Repository**: Fork the MathLang repository to your GitHub account. This will create a copy of the project under your account.

2. **Clone your Fork**: Clone the forked repository to your local machine using the following command:

		git clone https://github.com/<your-username>/mathlang.git
	
3. **Create a Branch**: Create a new branch for your changes. Use a descriptive name for your branch to indicate the purpose of your contribution:

		git checkout -b feature/<new-feature>

4. **Commit Changes**: Commit your changes with a clear and concise commit message:

		git commit -m "add new feature" -s

The `-s` flag adds a "Signed-off-by" line to the commit, indicating that you agree to the [Developer Certificate of Origin (DCO)](https://developercertificate.org).

5. **Push Changes**: Push your changes to your forked repository on GitHub:

		git push origin feature/new-feature

6. **Submit a Pull Request (PR)**: Open a pull request on the MathLang repository. Provide a detailed description of your changes and explain the purpose of the pull request.

_Make sure to follow the existing code style and formatting conventions used in the project. Consistent code style makes it easier for collaborators to understand and maintain the codebase._

# Collaborators
MathLang Compiler is an open-source project, and collaboration is encouraged. If you are interested in becoming a collaborator, please reach out to [@PhoenXHO](https://github.com/PhoenXHO) via GitHub or email. Collaborators have direct write access to the repository and play a vital role in shaping the future of MathLang.
