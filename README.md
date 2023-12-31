# OpenUni-20465-Assembler

# Project Overview
This is the final assignment for the System Programming Lab course in the Open University.
This repository contains an assembler tool designed to process assembly code written in a specific format and generate corresponding machine code. The tool operates in four main phases: preprocessing, first pass, second pass, and translation.


# Input Format
The input for the assembler is a text file with a ".as" extension. The file contains assembly code divided into two main parts: instructions and declarations

# Output Format
A ".am" file after preprocessing.
A ".ob" file containing the translated machine code in base 64 encoding.
An ".ext" file tracking lines with external label usage.
An ".ent" file listing locations with entry label declarations.

# Phase 1: Preprocessing
The assembler begins by processing any macros defined between "mcro" and "endmcro" markers.
Comments and empty lines are ignored during preprocessing.
The remaining code is saved in a new ".am" file for further processing.

# Phase 2: First Pass
Initial errors are detected and reported.
Labels defined in both instructions and data sections are recorded along with their addresses.
A symbol table is constructed to store label-address pairs.

# Phase 3: Second Pass
More complex errors involving labels are identified and handled.
Initial translation of assembly instructions to machine code representation (12-character binary words) is performed.

# Phase 4: Translation
The binary encoding of instructions is converted into base 64 encoding.
The resulting base 64 encoded machine code is written to a new ".ob" file.
The ".ob" file begins with a header indicating the number of instruction lines and data lines.


# Configuration
A word in the machine language is represented by 12 bits, and data is also represented using 12 bits.
The machine supports assembly instruction using registers, labels and integers.
The machine's memory starts at address 100 and has a total of 1023 cells.

# Project Structure
main-c is the entry point to the program.
parser.c is responsible for the overall management of preproccessor, first phase and second phase.
lexer.c and translator.c are helper files responsible for ensuring proper syntax of source code and translation to machine code.
writer.c and reader.c are responsible for the proper reading of input files and writing output files.
image.c , macro_table.c, symbol_table.c are data structures files.
utils.c and errors.c are helper functions used throughout the program.
constants.h and globals.h are shared public variables.

# Getting Started
Clone this repository to your local machine.
Ensure you have a C compiler.
Place customized assembly code in root folder with the ".as" extension  (testfiles provided in repository).
Run the assembler program to process your code and generate output files.

# Running program
Execute the assembler using the command in commandline: ./assembler <input_file>
The processed code will be saved as "<input_file>.am".
Translated machine code will be saved in "<input_file>.ob".
External label usage will be tracked in "<input_file>.ext".
Entry label declarations will be listed in "<input_file>.ent".

# Contributors
Dor Varsulker


# Additional Info
Inside the repository/tests you can see various examples of assembly files, either valid or incorrect.
