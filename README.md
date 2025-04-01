# RISC-V Simulator

## Directory Structure
The project directory contains the following files:

- `Instruction.h`: Header file declaring functions for mapping of parsing and executing RISC-V instructions.
- `helping.cpp` : contains the Helping function use in execution of input file.
- `Main.cpp`: The main file that loading input.t handles instruction execution and coordinates between different instruction types.
- `Execution.cpp`: It handles the instruction and execute them and modify the value of registers and memory.
- `Makefile`: The Makefile used to build the project.

## Usage Instructions

### Compiling the Project

To compile the project, just run the command in terminal

```bash
make

./riscv_sim
```

To clean the temporaries/executables, run the command
```bash
make clean
```



