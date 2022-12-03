
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

unsigned char memory[65536];

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



