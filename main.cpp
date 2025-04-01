#include "Instruction.h"
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <cstdint>
#include <iomanip>

using namespace std;

int64_t registers[NUM_REGISTERS]; // RISC-V 64-bit registers
uint8_t memory[MEMORY_SIZE];
unordered_map<string, uint64_t> labels; 
set<uint64_t> breakpoints;
stack<StackFrame> callStack;
vector<string> instructions;
deque<string>dq;
vector<string>opcode_vec;


uint64_t dataPointer = DATA_START;
uint64_t instructionPointer = TEXT_START;
uint64_t SP = STACK_START;

string currentFunction = "main";  // Start in main function
int currentLine = 0;
uint64_t PC =0;
int lineNum = 0;

unordered_map<string, int> register_map = {
    {"x0", 0}, {"x1", 1}, {"x2", 2}, {"x3", 3}, {"x4", 4}, {"x5", 5}, {"x6", 6}, {"x7", 7}, {"x8", 8}, {"x9", 9}, {"x10", 10},{"x11", 11},
    {"x12", 12}, {"x13", 13}, {"x14", 14}, {"x15", 15}, {"x16", 16}, {"x17", 17}, {"x18", 18}, {"x19", 19}, {"x20", 20},{"x21", 21},
    {"x22", 22}, {"x23", 23}, {"x24", 24}, {"x25", 25}, {"x26", 26}, {"x27", 27}, {"x28", 28}, {"x29", 29}, {"x30", 30}, {"x31", 31},
    {"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4}, {"t0", 5}, {"t1", 6}, {"t2", 7}, {"s0", 8}, {"fp", 8}, {"s1", 9}, {"a0", 10},
    {"a1", 11}, {"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15}, {"a6", 16}, {"a7", 17}, {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21},
    {"s6", 22}, {"s7", 23}, {"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27}, {"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31}
};

// Function to parse and load the .data section
void handleDataSection(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file." << endl;
        return;
    }

    string line;
    //int memoryAddress = 0x100000; // Start of the .data section

    while (getline(file, line))

    {
        if (line.find(';') != string::npos)
            line = line.substr(0, line.find(';'));  // Skip comments

        line = trim(line);
        if (line.empty())
                continue;    // Skip empty lines
            

        // Check if the line is part of the .data section
        if (line.find(".data") != string::npos)
        {
            // Start reading the data section
            while (getline(file, line))
            {   
                line = trim(line);
                if (line.empty())
                    continue;

                if (line.find(".text") != string::npos){
                    lineNum += 1;
                    break;
                }

                // Parse and store variables based on data type
                if (line.find(".word") != string::npos) {
                    // .word (4 bytes)
                    lineNum += 1;
                    size_t pos = line.find(".word");
                    string values = line.substr(pos + 5); // Get the values
                    vector<string> tokens = split(values, ','); // Split by commas
                    for (const string &token : tokens) {
                        int32_t wordValue = stoi(token);
                        memcpy(memory + dataPointer, &wordValue, sizeof(int32_t));
                        dataPointer += sizeof(int32_t); // Move the pointer by 4 bytes
                    }
                } 
                else if (line.find(".byte") != string::npos) {
                    // .byte (1 byte)
                    lineNum += 1;
                    size_t pos = line.find(".byte");
                    string values = line.substr(pos + 5); // Get the values
                    vector<string> tokens = split(values, ','); // Split by commas
                    for (const string &token : tokens) {
                        int8_t byteValue = stoi(token);
                        memcpy(memory + dataPointer, &byteValue, sizeof(int8_t));
                        dataPointer += sizeof(int8_t); // Move the pointer by 1 byte
                    }
                } 
                else if (line.find(".half") != string::npos) {
                    // .half (2 bytes)
                    lineNum += 1;
                    size_t pos = line.find(".half");

                    string values = line.substr(pos + 5); // Get the values

                    vector<string> tokens = split(values, ','); // Split by commas
                    for (const string &token : tokens) {
                        int16_t halfValue = stoi(token);

                        memcpy(memory + dataPointer, &halfValue, sizeof(int16_t));

                        dataPointer += sizeof(int16_t); // Move the pointer by 2 bytes
                    }
                } 
                else if (line.find(".dword") != string::npos) {
                    lineNum += 1;
                    // .dword (8 bytes)
                    size_t pos = line.find(".dword");
                    string values = line.substr(pos + 6); // Get the values
                    vector<string> tokens = split(values, ','); // Split by commas

                    for (const string &token : tokens) {
                        int64_t dwordValue = stoll(token);
                        memcpy(memory + dataPointer, &dwordValue, sizeof(int64_t));
                        dataPointer += sizeof(int64_t); // Move the pointer by 8 bytes
                    }
                }
            }  
        }
        if (line.find(".text") != string::npos)
                break;
    }
}

void load(const string &filename)
{
    ifstream file(filename);
    string line;
    
    int line_num = 0;

    instructions.clear();
    labels.clear();
    breakpoints.clear();
    
    PC = 0;
    SP = STACK_START;
    fill(begin(registers), end(registers), 0);
    fill(begin(memory), end(memory), 0);

    while (getline(file, line))
    {   
        stringstream ss(line);
        line = trim(line);
        if (line.empty())
            continue;  // Skip empty lines

        if (line.find(".data") != string::npos) {
            // Handle .data section
            lineNum += 1;
            handleDataSection(filename);
            continue;
        } 
        else{
            if (line.find(".text") != string::npos)
                continue;            
        }
         
         string com;
         ss >> com;
         if(com == ".dword" || com == ".word" || com == ".text" || com == ".half" || com == ".byte"){
            continue;
         }

        if (line.find(';') != string::npos)
                line = line.substr(0, line.find(';'));  // skip comments

        line = trim(line);
        if(line.empty())
                continue;    // skip empty lines

        size_t colonPos =  line.find(':');
        string label;
        if (colonPos != string::npos) {
                   
                string restOfLine = line.substr(colonPos + 1);   // Skip the label and get the rest of the line
                restOfLine=trim(restOfLine);

                    label = line.substr(0,colonPos);
                    if (labels.find(label) != labels.end()){
                            cerr << "Error: Duplicate label '" << label << "' at line " << line_num + 1 << endl;
                    }
                    labels[label] = line_num*4;

                    if(restOfLine.empty())
                            continue;

                        line = restOfLine;  // Set line to the part after the label
                            
                    }
                    instructions.push_back(line);
        line_num++;
    }
}

void commandLoop()
{
    string command;
    while (true)
    {
        cout << "> ";
        cin >> command;

        if (command == "load")
        {
            string filename;
            cin >> filename;
            load(filename);
        }
        else if (command == "run")
        {
            run();
        }
        else if (command == "regs")
        {
            regs();
        }
        else if (command == "mem")
        {
            uint64_t addr;
            int count;
            cin >> hex >> addr >> dec >> count;
            mem(addr, count);
        }
        else if (command == "step")
        {
            step();
        }
        else if (command == "show-stack")
        {
            showStack();
        }
        else if (command == "break")
        {
            uint64_t line;
            cin >> line;
            setBreakpoint(line);
        }
        else if(command == "del"){
            string brk;
            uint64_t line;
            cin >> brk;
            cin >> line;
            deleteBreakpoint(line);
        }
        else if (command == "exit")
        {
            cout << "Exited the simulator" << endl
                 << endl;
            break;
        }
        cout << endl;
    }
}

int main()
{
    dq.push_back("main");
    opcode_vec.push_back("main");
    commandLoop();

    return 0;
}
