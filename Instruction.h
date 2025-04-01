#ifndef RISC_V_INSTRUCTION_H
#define RISC_V_INSTRUCTION_H

#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <cstdint>
#include <iomanip>

using namespace std;

const int NUM_REGISTERS = 32;
const int MEMORY_SIZE = 0x50000; // Size of memory
const uint64_t DATA_START = 0x10000;   // .data section starts at this memory address
const uint64_t TEXT_START = 0x0;
const uint64_t STACK_START = 0x50000;

// Register and memory storages
extern int64_t registers[NUM_REGISTERS]; // RISC-V 64-bit registers
extern uint8_t memory[MEMORY_SIZE];      // Memory storage
extern unordered_map<string, uint64_t> labels; // To store label addresses
extern unordered_map<string, int> register_map;
extern set<uint64_t> breakpoints;

extern vector<string> instructions;  // Instruction memory (text section)
extern deque<string> dq;
extern vector<string>opcode_vec;
extern uint64_t PC;                        // Program Counter
extern int lineNum;
struct StackFrame {
    string functionName;
    int lastExecutedLine;
};

extern stack<StackFrame> callStack;   // Stack configuration
extern uint64_t SP; // Stack Pointer
extern string currentFunction; 
extern int currentLine;

void step();
void run();
void regs();
void mem(uint64_t addr, int count);
void showStack();
void setBreakpoint(uint64_t line);
void deleteBreakpoint(uint64_t line);

string trim(const string &str);
int getRegisterIndex(const string &reg);
vector<string> split(const string &s, char delimiter);
void pushFunction(const string& funcName, int line);
void popFunction();
void executeInstruction(const string &instruction);

#endif
