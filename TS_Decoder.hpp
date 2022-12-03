
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
unsigned char memory[65536];

#define PRINT(x) std::cout << x << std::endl;

//Program counter (16 bit) used to hold the address of the next instruction to execute. It is initialized to zero.
uint16_t PC = 0;

//Instruction register (8 bit) used to hold the current instruction being executed
uint8_t IR = 0;

//Memory Address Register (16 bit) used to hold an address being used as a pointer, i.e., an indirect reference to data in memory
uint16_t MAR = 0;

//Accumulator (8 bit) used to operate on data
uint8_t ACC = 0;



//if MS 5 bits are 00010, its a branch/jump op. (AND should equal 0x10 or dec 16)
#define BR_OP_MASK 0b11111000

//if MSB is 1, it's a math op. (AND should equal 0x80 or dec 128)
#define MATH_OP_MASK 0b10000000 

#define MATH_FUNCTION_MASK 0b01110000
#define AND 0b00000000;
#define OR 0b00010000;
#define XOR 0b00100000;
#define ADD 0b00110000;
#define SUB 0b01000000;
#define INC 0b01010000;
#define DEC 0b01100000;
#define NOT 0b01110000;

#define MATH_DEST_MASK 0b00001100;
#define MATH_SOURCE_MASK 0b00000011;
#define M_INDIR 0b00000000;
#define M_ACC 0b00000100 ;
#define M_D_MAR 0b00001000;
#define M_MEM 0b00001100;
#define M_S_CONST 0b00001000;

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
#define MEM_M_ADR 0b00000001
#define MEM_M_ADR 0b00000010



void fetchNextInstruction(void);

void executeInstruction(void);

std::vector<uint8_t> parseFileData(std::string fileName);

void determineOp(uint8_t opcode);

