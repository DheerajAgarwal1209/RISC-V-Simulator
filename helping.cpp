#include "Instruction.h"
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <cstdint>
#include <iomanip>

// Function to trim spaces from start and end of a string
string trim(const string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

int getRegisterIndex(const string &reg)
{
    if(register_map.find(reg) == register_map.end())
            cerr << "Invalid register :" << reg << endl;
            
    return register_map[reg];

}

//Function to split a line by commas
vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

// Function to push the current function and its last executed line onto the stack
void pushFunction(const string& funcName, int line) {
    callStack.push({funcName, line}); 
    currentFunction = funcName;        // Update to the new function
}

// Function to pop the current function from the stack (on `jalr`)
void popFunction() {
    if (!callStack.empty()) {
        auto top = callStack.top();
        currentFunction = top.functionName;  // Restore the previous function
        currentLine = top.lastExecutedLine;  // Restore the last executed line of that function
        callStack.pop();
    }
}

void step()
{
    if (PC / 4 < instructions.size())
    {
        cout << "Executed: " << instructions[PC / 4] << " ;   PC=0x" << hex << setw(8) << setfill('0') << PC << endl;
        executeInstruction(instructions[PC / 4]);
        
    }
    else
            cout << "Nothing to step" << endl;
}

void run()
{
    while (PC / 4 < instructions.size())
    {
        if (find(breakpoints.begin(), breakpoints.end(), PC) != breakpoints.end())
        {
            cout << "Execution stopped at breakpoint" << endl;
            return;
        }
        step();
    }
}

void regs()
{
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        cout << "x" << dec << i << ": 0x" << hex << registers[i] << endl;
    }
}

void mem(uint64_t addr, int count)
{
    if (addr + count > MEMORY_SIZE)
    {
        cout << "Memory access out of bounds!" << endl;
        return;
    }
    for (int i = 0; i < count; i++)
    {
        cout << "Memory[0x" << hex << addr + i << "] = 0x" << hex << setw(2) << setfill('0') << static_cast<int>(memory[addr + i]) << endl;
    }
}

// Function to display the current call stack
void showStack() {
    if(currentLine==0){
        currentLine=lineNum;
    }
    
    if (PC / 4 >= instructions.size()) {
        cout << "Empty Call Stack: Execution complete" << endl;
    } else {
        stack<StackFrame> tempStack = callStack;
        stack<StackFrame> tempStack2;
        while(!tempStack.empty()){
            tempStack2.push(tempStack.top());
            tempStack.pop();
        }

        cout << "Call Stack:" << endl;
        while (!tempStack2.empty()) {
            auto frame = tempStack2.top();
            cout << frame.functionName << ":" <<dec << frame.lastExecutedLine << endl;
            tempStack2.pop();
        }

        if(opcode_vec.back() != "jal")
            cout << currentFunction << ":" << dec << currentLine<< endl;  // Show current function and line
    }
}

void setBreakpoint(uint64_t line)
{
    breakpoints.insert((line-1) * 4);
    cout << "Breakpoint set at line " << dec << line << endl;
}

void deleteBreakpoint(uint64_t line){
        if (breakpoints.find((line-1)*4) == breakpoints.end())
                cout << "No breakpoint set at line " << line << endl;
        else{
                breakpoints.erase((line-1)*4);
                cout << "Breakpoint at line " << line << " is deleted." << endl;
            }
}
