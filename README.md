# 🧠 Assembly Compiler 

Assembler project in ANSI C, built as part of the Open University's "MAMAN 14" assignment.  
The assembler reads `.as` files written in a custom assembly language, handles macro expansion, processes instructions and symbols in two passes, and outputs machine code in a Hexadecimal presentation of custom 24-bit binary format to `.ob` file + outputs for extern and entry symbols in `.ext` and `.ent` files.

---

## 📁 Project Structure

| File / Module        | Description                                                                 |
|----------------------|-----------------------------------------------------------------------------|
| `assembler.c/h`      | Main driver – handles macro expansion, first pass, and second pass          |
| `mcros.c/h`          | Expands user-defined macros (`mcro` / `mcroend`)                            |
| `as_to_am.c/h`       | Handles `.as` to `.am` conversion (macro-expanded version of source)        |
| `am_first.c/h`       | First pass: parses lines, identifies labels, directives and builds symbols  |
| `am_sec.c/h`         | Second pass: resolves addresses, manages `.entry` / `.extern`               |
| `binary.c/h`         | Constructs the binary (24-bit) memory image, encodes instruction formats     |
| `symbols.c/h`        | Manages the symbol table: add, query, tag symbols                           |
| `analyser.c/h`       | Helper validation functions: check for registers, symbols, syntax, etc.     |

---

## 🛠 Features

- ✅ **Macro Expansion** – Replace user-defined macros (`mcro`, `mcroend`)

- ✅ **Two-Pass Compilation**:
  - **First Pass** – Parses source, builds symbol table, validates syntax
  - **Second Pass** – Resolves addresses and generates binary code

- ✅ **Symbol Table Support** – Handles `.entry`, `.extern`, labels

- ✅ **Instruction Parsing** – Supports immediate, direct, relative, and register addressing

- ✅ **Machine Code Output** – Custom 24-bit format with memory translation

- ✅ **Robust Error Detection** – Detects:
  - Illegal/missing operands  
  - Duplicate/undefined symbols  
  - Invalid macro usage  
  - Invalid addressing modes  
  - Conflicts between `.entry` and `.extern`


## 🚀 How to Compile & Run

1. Compile the project:
```bash
make
```
2. Run the project:

Run the assembler on one or more input files:

```bash
./assembler file1 file2 ...

./assembler file1 file2 ...
```

## 📌 Requirements
* Language: ANSI C
* Compiler: gcc
* OS: Ubuntu (VMware environment)
* Flags: -ansi -Wall -pedantic -g

## 🧪 Testing
To test the assembler’s error detection, use the provided fpfail.as file for first pass error presentation and spfail.as file for second pass error presentation.

## 🔍 What I Learned
* Writing an assembler from scratch in ANSI C
* Deep understanding of instruction formats and memory models
* Symbol table construction and management
* Resolving relative vs direct memory references
* Managing segmentation faults and memory safety
* Designing modular and testable C code

📬 Contact
Feel free to reach out if you’d like to learn more or want to collaborate!
📎 LinkedIn: https://www.linkedin.com/in/ran-yohanan/
