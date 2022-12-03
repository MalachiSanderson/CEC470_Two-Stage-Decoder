/*
    CEC470
    Project 2: Two-Stage Instruction Decoder
    @note: Project Objectives: "Design a simple processor based on the simplified 
    2-step instruction cycle shown in Figure 3.3 of our textbook. 
    You will be creating two functions in software, one to simulate the fetch next
    instruction cycle, fetchNextInstruction(), and one to simulate the execute 
    instruction cycle, executeInstruction(). Neither of these functions will have 
    parameters nor will they return values. They will operation on global data meant
    to simulate the registers and memory of the processor."
    
    
    @since 12-3-2022

    @author Malachi Sanderson
    @author Miles Tucker
    @author Jakob Haehre
    @author CODE REFERENCED: {@link https://github.com/AndrewDaws/Two-Stage_Instruction_Decoder}
*/
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>

#define MEMORY_SIZE 65536
unsigned char memory[MEMORY_SIZE];

#define PRINT(x) std::cout << x << std::endl;

//Program counter (16 bit) used to hold the address of the next instruction to execute. It is initialized to zero.
uint16_t PC = 0;

//Instruction register (8 bit) used to hold the current instruction being executed
uint8_t IR = 0;

//Memory Address Register (16 bit) used to hold an address being used as a pointer, i.e., an indirect reference to data in memory
uint16_t MAR = 0;

//Accumulator (8 bit) used to operate on data
uint8_t ACC = 0;

void fetchNextInstruction(void);

void executeInstruction(void);

std::vector<uint8_t> parseFileData(std::string fileName);

void determineOp(uint8_t opcode);

int determineIncrement(uint8_t opcode);



#pragma region MASKS

#pragma region MATH
//if MSB is 1, it's a math op. (AND should equal 0x80 or dec 128)
#define MATH_OP_MASK 0b10000000 

#define MATH_FUNCTION_MASK 0b01110000
#define AND 0b00000000
#define OR 0b00010000
#define XOR 0b00100000
#define ADD 0b00110000
#define SUB 0b01000000
#define INC 0b01010000
#define DEC 0b01100000
#define NOT 0b01110000

#define MATH_DEST_MASK 0b00001100
#define MATH_SOURCE_MASK 0b00000011

#define M_INDIR 0b00000000

#define M_D_ACC 0b00000100 
#define M_D_MAR 0b00001000
#define M_D_MEM 0b00001100

#define M_S_ACC 0b00000001
#define M_S_CONST 0b00000010
#define M_S_MEM 0b00000011

#pragma endregion

#pragma region MEMORY
//if MS 4 bits are 0, its a mem op. (AND should equal 0x0)
#define MEM_OP_MASK 0b11110000
#define MEM_FUNCTION_MASK 0b00001000
#define MEM_REG_MASK 0b00000100
#define MEM_METH_MASK 0b00000011
#define MEM_STORE 0b00000000
#define MEM_LOAD 0b00001000
#define MEM_R_ACC 0b00000000
#define MEM_R_INDEX 0b00000100
#define MEM_M_ADR 0b00000000
#define MEM_M_CON 0b00000001
#define MEM_M_IND 0b00000010
#pragma endregion

#pragma region BRANCH
//if MS 5 bits are 00010, its a branch/jump op. (AND should equal 0x10 or dec 16)
#define BR_OP_MASK 0b11111000
#define BR_TYPE_MASK 0b00000111
#define BRA 0b00000000 //unconditional branch
#define BRZ 0b00000001 
#define BNE 0b00000010
#define BLT 0b00000011
#define BLE 0b00000100
#define BGT 0b00000101
#define BGE 0b00000110
#pragma endregion

#pragma region SPECIAL OPS
#define NOP 0b00011000
#define HALT 0b00011001
#pragma endregion


#pragma endregion

