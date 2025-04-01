#include "Instruction.h"
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <cstdint>
#include <iomanip>

using namespace std;

void executeInstruction(const string &instruction)
{
    // Basic parsing of the instruction
    istringstream iss(instruction);
    string opcode, rd, rs1, rs2, immediate, Label;
    int imm;

    iss >> opcode;
    opcode_vec.push_back(opcode);

    // R-format instructions
    if (opcode == "add" || opcode == "sub" || opcode == "and" ||opcode == "or" || opcode == "xor" || opcode == "sll" ||
        opcode == "srl" || opcode == "sra" || opcode == "slt" || opcode == "sltu")
    {

        getline(iss, rd, ',');
        getline(iss, rs1, ',');
        getline(iss, rs2);

        rd = trim(rd);
        rs1 = trim(rs1);
        rs2 = trim(rs2); 

        if (opcode == "add")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] + registers[getRegisterIndex(rs2)];
        else if (opcode == "sub")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] - registers[getRegisterIndex(rs2)];
        else if (opcode == "or")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] | registers[getRegisterIndex(rs2)];
        else if (opcode == "xor")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] ^ registers[getRegisterIndex(rs2)];
        else if (opcode == "and")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] & registers[getRegisterIndex(rs2)];
        else if (opcode == "sll")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] << registers[getRegisterIndex(rs2)];
        else if (opcode == "srl")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] >> registers[getRegisterIndex(rs2)];
        else if (opcode == "slt") 
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] < registers[getRegisterIndex(rs2)] ? 1 : 0; // Set less than
        else if (opcode == "sltu") 
            registers[getRegisterIndex(rd)] = (uint64_t)registers[getRegisterIndex(rs1)] < (uint64_t)registers[getRegisterIndex(rs2)] ? 1 : 0; // Set less than unsigned
        else if (opcode == "sra"){
            if (registers[getRegisterIndex(rs1)] < 0)
            {
                registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] >> registers[getRegisterIndex(rs2)];
                int64_t num = uint64_t(-1) << (64 - registers[getRegisterIndex(rs2)]);
                registers[getRegisterIndex(rd)] |= num;
            }
            else
                registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] >> registers[getRegisterIndex(rs2)];
        }

        currentLine = PC/4+1 + lineNum;
    }

    // I format instruction
    else if (opcode == "addi" || opcode == "andi" || opcode == "ori" || opcode == "xori" ||
             opcode == "slli" || opcode == "srli" || opcode == "srai" || opcode == "slti" || opcode == "sltiu")
    {
        getline(iss, rd, ',');
        getline(iss, rs1, ',');
        getline(iss, immediate);

        rd = trim(rd);
        rs1 = trim(rs1);
        immediate = trim(immediate);

        if(immediate[0] == '0' && immediate[1] == 'x'){
            immediate = immediate.substr(2);
            imm = stoi(immediate, nullptr, 16);
        }
        else
            imm = stoi(immediate);

        if (opcode == "addi")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] + imm;
        else if (opcode == "xori")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] ^ imm;
        else if (opcode == "ori")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] | imm;
        else if (opcode == "andi")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] & imm;
        else if (opcode == "slli")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] << imm;
        else if (opcode == "srli")
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] >> imm;
        else if (opcode == "slti") 
            registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] < imm ? 1 : 0; // Set less than
        else if (opcode == "sltiu") 
            registers[getRegisterIndex(rd)] = (uint64_t)registers[getRegisterIndex(rs1)] < (uint64_t)imm ? 1 : 0; // Set less than unsigned
        else if (opcode == "srai") {
            if (registers[getRegisterIndex(rs1)] < 0)
            {
                registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] >> imm;
                int64_t num = uint64_t(-1) << (64 - imm);
                registers[getRegisterIndex(rd)] |= num;
            }
            else
                registers[getRegisterIndex(rd)] = registers[getRegisterIndex(rs1)] >> imm;
        }

        currentLine =PC/4 + 1 + lineNum; 
    }
    // load instructions
    else if (opcode == "ld" || opcode == "lw" || opcode == "lh" || opcode == "lb" ||
             opcode == "lhu" || opcode == "lbu" || opcode == "lwu" || opcode == "jalr")
    {
        getline(iss, rd, ',');
        getline(iss, immediate, '(');
        getline(iss, rs1, ')');

        rd = trim(rd);
        rs1 = trim(rs1);
        immediate = trim(immediate);

        if(immediate[0] == '0' && immediate[1] == 'x'){
            immediate = immediate.substr(2);
            imm = stoi(immediate, nullptr, 16);
        }
        else
                imm = stoi(immediate);

        uint64_t addr = registers[getRegisterIndex(rs1)] + imm;

        int loadSize = 0; // This will be set according to the instruction type

        // Determine the size of the load operation based on the opcode
        if (opcode == "lb" || opcode == "lbu")
                    loadSize = 1;
        else if (opcode == "lh" || opcode == "lhu")
                    loadSize = 2;
        else if (opcode == "lw" || opcode == "lwu")
                    loadSize = 4;
        else if (opcode == "ld")
                    loadSize = 8;

        // Make sure the address is valid within memory bounds
        if (addr + loadSize > MEMORY_SIZE)
        {
            cout << "Memory access out of bounds!" << endl;
            return;
        }

        // Handle different load instructions based on the size of the data
        if (opcode == "ld")
        {
            // Load 8 bytes (64-bit) from memory into register
            uint64_t value = 0;
            memcpy(&value, &memory[addr], sizeof(uint64_t));
            registers[getRegisterIndex(rd)] = value;
        }
        else if (opcode == "lw")
        {
            // Load 4 bytes (32-bit) from memory into register
            uint32_t value = 0;
            memcpy(&value, &memory[addr], sizeof(uint32_t));
            registers[getRegisterIndex(rd)] = static_cast<uint64_t>(value); // Zero-extend to 64-bit
        }
        else if (opcode == "lh")
        {
            // Load 2 bytes (16-bit) from memory into register
            uint16_t value = 0;
            memcpy(&value, &memory[addr], sizeof(uint16_t));
            registers[getRegisterIndex(rd)] = static_cast<uint64_t>(value); // Zero-extend to 64-bit
        }
        else if (opcode == "lb")
        {
            // Load 1 byte (8-bit) from memory into register
            uint8_t value = 0;
            memcpy(&value, &memory[addr], sizeof(uint8_t));
            registers[getRegisterIndex(rd)] = static_cast<uint64_t>(value); // Zero-extend to 64-bit
        }
        else if (opcode == "lbu")
        {
            // Load 1 byte (unsigned) from memory into register
            uint8_t value = 0;
            memcpy(&value, &memory[addr], sizeof(uint8_t));
            registers[getRegisterIndex(rd)] = static_cast<uint64_t>(value); // Zero-extend to 64-bit
        }
        else if (opcode == "lhu")
        {
            // Load 2 bytes (unsigned) from memory into register
            uint16_t value = 0;
            memcpy(&value, &memory[addr], sizeof(uint16_t));
            registers[getRegisterIndex(rd)] = static_cast<uint64_t>(value); // Zero-extend to 64-bit
        }
        else if (opcode == "lwu")
        {
            // Load 4 bytes (unsigned) from memory into register
            uint32_t value = 0;
            memcpy(&value, &memory[addr], sizeof(uint32_t));
            registers[getRegisterIndex(rd)] = static_cast<uint64_t>(value); // Zero-extend to 64-bit
        }

         else if (opcode == "jalr"){
                registers[getRegisterIndex(rd)] = PC + 4;
                PC = (registers[getRegisterIndex(rs1)] - 4 + imm) & ~1;

                if (!callStack.empty()) 
                callStack.pop(); // Remove the top function from the stack
                dq.pop_back();
                currentFunction = dq.back();
            }

            currentLine = PC/4 +1 + lineNum;
        }

    // S-format instructions (sd, sw, sh, sb)
    if (opcode == "sd" || opcode == "sw" || opcode == "sh" || opcode == "sb")
    {
        getline(iss, rs2, ','); // Source register
        getline(iss, immediate, '(');
        getline(iss, rs1, ')'); // Base register

        rs1 = trim(rs1);
        rs2 = trim(rs2);
        immediate = trim(immediate);

        if(immediate[0] == '0' && immediate[1] == 'x'){
            immediate = immediate.substr(2);
            imm = stoi(immediate, nullptr, 16);
        }
        else
                imm = stoi(immediate);

        // Calculate the effective memory address
        uint64_t addr = registers[getRegisterIndex(rs1)] + imm;

        // Make sure the address is valid within memory bounds
        if (addr >= MEMORY_SIZE)
        {
            cout << "Memory access out of bounds!" << endl;
            return;
        }

        // Handle different store instructions based on the size of the data
        if (opcode == "sd")
        {
            // Store 8 bytes (64-bit) from register to memory
            uint64_t value = registers[getRegisterIndex(rs2)];
            if (addr + 8 > MEMORY_SIZE)
            {
                cout << "Memory access out of bounds!" << endl;
                return;
            }

            memcpy(memory + addr, &value, sizeof(uint64_t));
        }
        else if (opcode == "sw")
        {
            // Store 4 bytes (32-bit) from register to memory
            uint32_t value = static_cast<uint32_t>(registers[getRegisterIndex(rs2)]);
            if (addr + 4 > MEMORY_SIZE)
            {
                cout << "Memory access out of bounds!" << endl;
                return;
            }
            memcpy(memory + addr, &value, sizeof(uint32_t));
        }
        else if (opcode == "sh")
        {
            // Store 2 bytes (16-bit) from register to memory
            uint16_t value = static_cast<uint16_t>(registers[getRegisterIndex(rs2)]);
            if (addr + 2 > MEMORY_SIZE)
            {
                cout << "Memory access out of bounds!" << endl;
                return;
            }
            memcpy(memory + addr, &value, sizeof(uint16_t));
        }
        else if (opcode == "sb")
        {
            // Store 1 byte (8-bit) from register to memory
            uint8_t value = static_cast<uint8_t>(registers[getRegisterIndex(rs2)]);
            memcpy(memory + addr, &value, sizeof(uint8_t));
        }
        currentLine = PC/4 + 1 + lineNum;
    }

    // B-format instructions
    else if (opcode == "beq" || opcode == "bne" || opcode == "blt" || opcode == "bge" || opcode == "bltu" || opcode == "bgeu")
    {
        getline(iss, rs1, ',');
        getline(iss, rs2, ',');
        getline(iss, Label);

        rs1 = trim(rs1);
        rs2 = trim(rs2);
        Label = trim(Label);

        if(isdigit(Label[0]) || Label[0] == '-' )
                imm = stoi(Label);
        else{
            if((labels.find(Label) != labels.end()))
                    imm = labels[Label] - PC;
            else
                cout << "Undefined Label for instruction" << instruction << endl;
        }
        
        currentLine =PC/4 + 1 + lineNum;

        if (opcode == "beq")
        {
            if (registers[getRegisterIndex(rs1)] == registers[getRegisterIndex(rs2)])
                PC += imm - 4; // Adjusting PC since it will be incremented by 4 after the execution
        }
        else if (opcode == "bne")
        {
            if (registers[getRegisterIndex(rs1)] != registers[getRegisterIndex(rs2)])
                PC += imm - 4;
        }
        else if (opcode == "blt")
        {
            if ((int64_t)registers[getRegisterIndex(rs1)] < registers[getRegisterIndex(rs2)])
                PC += imm - 4;
        }
        else if (opcode == "bge")
        {
            if (registers[getRegisterIndex(rs1)] >= registers[getRegisterIndex(rs2)])
                PC += imm - 4;
        }
        else if (opcode == "bltu")
        {
            if ((uint64_t)registers[getRegisterIndex(rs1)] < (uint64_t)registers[getRegisterIndex(rs2)])
                PC += imm - 4;
        }
        else if (opcode == "bgeu")
        {
            if ((uint64_t)registers[getRegisterIndex(rs1)] >= (uint64_t)registers[getRegisterIndex(rs2)])
                PC += imm - 4;
        }
    }

    // J-format instructions
    else if (opcode == "jal")
    {
        getline(iss, rd, ',');
        getline(iss, Label);

        rd = trim(rd);
        Label = trim(Label);

        registers[getRegisterIndex(rd)] = PC + 4;

        if(isdigit(Label[0]) || Label[0] == '-' )
                imm = stoi(Label);

        else{
            if((labels.find(Label) != labels.end()))
                        imm =labels[Label] - PC;
            else
                        cout << "Undefined Label for instruction" << instruction << endl;
        }
        
        pushFunction(currentFunction, PC/4+1);   // Push current function (main) and line onto the stack
        currentFunction = Label;
        dq.push_back(Label);
        currentLine = PC/4 + 1 + lineNum;

        PC += imm - 4; 
    }

    // U-format instructions
    else if (opcode == "lui" || opcode == "auipc")
    {
        getline(iss, rd, ',');
        getline(iss, immediate);

        rd = trim(rd);
        immediate = trim(immediate);
        
        immediate = immediate.substr(2);
        imm = stoi(immediate, nullptr, 16);

        if (opcode == "lui")
                    registers[getRegisterIndex(rd)] = imm << 12;
        else if (opcode == "auipc")
                    registers[getRegisterIndex(rd)] = PC + (imm << 12);

        currentLine = PC/4 + 1 + lineNum;
    }

    registers[0] = 0;

    PC += 4;   // Move PC by 4 after each instruction (each instruction is 4 bytes)
}
