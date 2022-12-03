#include "TS_Decoder.hpp"

#define INPUT_FILE "mem_in.txt"

uint16_t prev_PC;

int main()
{
    std::vector<uint8_t> readProgram;
    readProgram = parseFileData(INPUT_FILE);
    for(int i = 0; i < readProgram.size(); i++)
    {
        memory[i] = readProgram.at(i);
    }

    for(int i = 0; i < MEMORY_SIZE; i++)
    {

    }
    
}

void fetchNextInstruction(void)
{
   IR = memory[PC];
   prev_PC = PC;
   PC += 1+ determineIncrement(PC);
}

void executeInstruction(void)
{
    //check ir to determine op then execute op

}

void determineOp(uint8_t opcode)
{
    //MATH OPS
    if((opcode & MATH_OP_MASK) == 0x80)
    {
        int funct = opcode & MATH_FUNCTION_MASK;
        int dest_ID = opcode & MATH_DEST_MASK;
        int source_ID = opcode & MATH_SOURCE_MASK;
        
        int destination;
        int source;

        switch (dest_ID)
        {
        case M_INDIR:
            destination = memory[MAR];
            break;

        case M_D_ACC:
            destination = ACC;
            break;
            
        case M_D_MAR:
            destination = MAR;
            break;
            
        case M_D_MEM:
            //TODO: no clue...
            destination = memory[(  (memory[prev_PC+1] << 8)  + memory[prev_PC+2]  )] ;
            break;
        }
        
        switch (source_ID)
        {
        case M_INDIR:
            source = memory[MAR];
            break;

        case M_S_ACC:
            source = ACC;
            break;
            
        case M_S_CONST:
            //TODO: no clue...
            if((IR & 0x0c) == 0x8) 
                source = (memory[PC - 2] << 8) + memory[PC -1];
            else source = memory[PC - 1];
            break;
            
        case M_S_MEM:
            //TODO: no clue...
            if((IR & 0x0c) == 0x8)
            { 
                int address = ((memory[prev_PC + 1] << 8) + memory[prev_PC + 2]);
                source = (memory[address] << 8) + memory[address + 1];
			}
            else source = memory[((memory[prev_PC + 1] << 8) + memory[prev_PC + 2])];
            break;
        
        }

        switch (funct)
        {
        case AND:
            destination &= source;
            break;

        case OR:
            destination |= source;
            break;

        case XOR:
            destination ^= source; 
            break;

        case ADD:
            destination += source;
            break;

        case SUB:
            destination -= source; 
            break;

        case INC:
            ++destination; 
            break;
            
        case DEC:
            --destination; 
            break;
            
        case NOT:
            destination = !destination;
            break;
        }

		switch(dest_ID) //TODO: no clue...
        { 
			case M_INDIR:
				memory[MAR] = destination & 0xff; 
				break;
			case M_D_ACC:
				ACC = destination & 0xff; 
				break;
			case M_D_MAR:
				MAR = destination & 0xffff; 
				break;
			case M_D_MEM:
				memory[((memory[prev_PC + 1] << 8) + memory[prev_PC + 2])] = destination & 0xff; 
				break;
		}
    
    }

    //MEMORY OPS
    else if((opcode & MEM_OP_MASK) == 0x00)
    {
        int func = opcode & MEM_FUNCTION_MASK;
        int reg = opcode & MEM_REG_MASK;
        int meth = opcode & MEM_METH_MASK;
        
        if(func == MEM_STORE)
        {

        }
        else if(func == MEM_LOAD)
        {

        }
        
        
        switch (reg)
        {
        case MEM_R_ACC:
            //TODO:
            break;

        case MEM_R_INDEX:
            //TODO:
            break;
        }
        
        switch (meth)
        {
        case MEM_M_ADR:
            //TODO:
            break;

        case MEM_M_CON:
            //TODO:
            break;
            

        case MEM_M_IND:
            //TODO:
            break;
        }
    }

    //BRANCH
    if((opcode & BR_OP_MASK) == 0x10)
    {
        int type = opcode & BR_TYPE_MASK;
        switch (type)
        {
        case BRA:
            //TODO:
            break;

        case BRZ:
            //TODO:
            break;

        case BNE:
            //TODO:
            break;

        case BLT:
            //TODO:
            break;

        case BLE:
            //TODO:
            break;

        case BGT:
            //TODO:
            break;

        case BGE:
            //TODO:
            break;
            
        }
    }




}

int determineIncrement(uint8_t opcode)
{
    //MATH OPS
    if((opcode & MATH_OP_MASK) == 0x80)
    {
        int dest = opcode & MATH_DEST_MASK;
        int source = opcode & MATH_SOURCE_MASK;
        switch (dest)
        {
        case M_INDIR:
            if(source == M_INDIR) return 0;
            if(source == M_S_ACC) return 0;
            if(source == M_S_CONST) return 1;
            if(source == M_S_MEM) return 2;
            break;

        case M_D_ACC:
            if(source == M_INDIR) return 0;
            if(source == M_S_ACC) return 0;
            if(source == M_S_CONST) return 1;
            if(source == M_S_MEM) return 2;
            break;
            
        case M_D_MAR:
            if(source == M_INDIR) return 0;
            if(source == M_S_ACC) return 0;
            if(source == M_S_CONST) return 2;
            if(source == M_S_MEM) return 2;
            break;
            
        case M_D_MEM:
            if(source == M_INDIR) return 2;
            if(source == M_S_ACC) return 2;
            if(source == M_S_CONST) return 3;
            if(source == M_S_MEM) return 4;
            break;
        }
        
    }

    //MEMORY OPS
    if((opcode & MEM_OP_MASK) == 0x00)
    {
        int reg = opcode & MEM_REG_MASK;
        int meth = opcode & MEM_METH_MASK;
        switch (reg)
        {
        case MEM_R_ACC:
            if(meth == MEM_M_ADR) return 2;
            if(meth == MEM_M_CON) return 1;
            if(meth == MEM_M_IND) return 0;
            break;

        case MEM_R_INDEX:
            if(meth == MEM_M_ADR) return 2;
            if(meth == MEM_M_CON) return 2;
            if(meth == MEM_M_IND) return 0;
            break;
        }
    }

    //BRANCH
    if((opcode & BR_OP_MASK) == 0x10) return 2;

    return -1000000;
}

std::vector<uint8_t> parseFileData(std::string fileName)
{
    std::vector<uint8_t> readProgram;
    std::ifstream inFile;
    try
    {
        inFile.open(fileName);
        unsigned int temp;
        while(!inFile.eof()) 
        {
            inFile >> std::hex >> temp;
            readProgram.push_back(temp);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return readProgram;
}

void printProgCommands(std::vector<uint8_t> prog)
{
    // Print one element per row
    std::vector<uint8_t>::iterator i;
    int c = 0;
    for (i = prog.begin(); i != prog.end(); ++i) 
    {
        //if(static_cast<unsigned> (*i) != 0)
        std::cout << static_cast<unsigned> (*i) << ",";
        c++;
        if (c == 10)
        {
            std::cout << std::endl;
            c = 0;
        }
    }
}
