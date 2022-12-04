#include "TS_Decoder.hpp"
#include "assert.h"

unsigned char* makeArr(std::vector<uint8_t> prog);

int main()
{
    std::vector<uint8_t> readProgram;
    PRINT("\n\n\t\tWELCOME!\n"); 
    
    readProgram = parseFileData(INPUT_FILE);
    //printProgCommands(readProgram);
    try
    {
        for(int i = 0; i < readProgram.size(); i++)
        {
            memory[i] = readProgram.at(i);
        }

        while(memory[PC] != HALT)
        {
            fetchNextInstruction();
            executeInstruction();
        }

        writeMemoryToFile(memory);
        PRINT("\n\n\t\tDONE!\n"); 
        return 1;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -2;
    }
    return 0;
}

void fetchNextInstruction(void)
{
    assert(PC >= 0 && PC < 256);//fail
    IR = memory[PC];
    prev_PC = PC;
    //PRINT("PC: " << PC << "->" << (int)IR);
    PC += 1+ determineIncrement(IR);
    assert(PC >= 0 && PC < 256);//fail
}

void executeInstruction(void)
{
    //check ir to determine op then execute op
    switch(determineOp(IR))
    {
        case operation_types::Math :
            mathOps(IR);
        break;

        case operation_types::Memory :
            memoryOps(IR);
        break;

        case operation_types::Branch :
            branchOps(IR);
        break;

        default:
        PRINT("\n\n\t\t[ERROR IN SELECTING OPERATION TYPE!!!!]\n"); 
        break;
    }
}

operation_types determineOp(uint8_t opcode)  
{
    //MATH OPS
    if((opcode & MATH_OP_MASK) == 0x80)
        return operation_types::Math;

    //MEMORY OPS
    else if((opcode & MEM_OP_MASK) == 0x00)
        return operation_types::Memory;

    //BRANCH OPS
    else if((opcode & BR_OP_MASK) == 0x10) 
        return operation_types::Branch;

    exit(-69);
    return operation_types::Math;
}


void mathOps(uint8_t opcode)
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
        //TODO: no clue...see [1 ln 210]
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
        //TODO: no clue...see [1 ln 221]
        if((IR & 0x0c) == 0x8) 
            source = (memory[PC - 2] << 8) + memory[PC -1];
        else source = memory[PC - 1];
        break;
        
    case M_S_MEM:
        //TODO: no clue...see [1 ln 227]
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

    switch(dest_ID) //TODO: no clue...see [1 ln 266]
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


void memoryOps(uint8_t opcode)
{
    int func = opcode & MEM_FUNCTION_MASK;
    int reg = opcode & MEM_REG_MASK;
    int meth = opcode & MEM_METH_MASK;

    if(func == MEM_STORE)
    {
        switch (reg)
        {
        case MEM_R_ACC:
            switch (meth)
            {
            case MEM_M_ADR: //operand is used as address
                //TODO: no clue...see [1 ln 287]
                memory[ (memory[prev_PC+1] << 8) + memory[prev_PC+2] ] = ACC;
                break;

            case MEM_M_CON: //operand is used as a constant var...do nothing here.
                break;
                
            case MEM_M_IND: //indirect, MAR is used as a ptr.
                memory[MAR] = ACC;
                break;

            default: 
                break;
            }
            break;

        case MEM_R_INDEX:
            switch (meth)
            {
            case MEM_M_ADR: //operand is used as address
                //TODO: no clue...see [1 ln 300]
                memory[ (memory[prev_PC + 1] << 8) + memory[prev_PC + 2] ] = (MAR >> 8) & 0xff; 
                memory[((memory[prev_PC + 1] << 8) + memory[prev_PC + 2]) + 1] = MAR & 0xff;
                break;

            case MEM_M_CON: //operand is used as a constant var...do nothing here
                break;
                
            case MEM_M_IND: //indirect, MAR is used as a ptr.
                //TODO: no clue...see [1 ln 307]
                memory[MAR] = (MAR >> 8) & 0xff;
                memory[MAR + 1] = MAR & 0xff;
                break;

            default: 
                break;
            }
            break;
        }
    }

    else if(func == MEM_LOAD)
    {
        switch (reg)
        {
        case MEM_R_ACC:
            switch (meth)
            {
            case MEM_M_ADR: //operand is used as address
                //TODO: no clue...see [1 ln 318]
                ACC = memory[ (memory[prev_PC+1]<<8) + memory[prev_PC+2]  ];
                break;

            case MEM_M_CON: //operand is used as a constant var...
                ACC = memory[prev_PC+1];
                break;
                
            case MEM_M_IND: //indirect, MAR is used as a ptr.
                ACC = memory[MAR];
                break;

            default: 
                break;
            }
            break;

        case MEM_R_INDEX:
            switch (meth)
            {
            case MEM_M_ADR: //operand is used as address
                //TODO: no clue...see [1 ln 336]
                MAR = memory[((memory[prev_PC + 1] << 8) + memory[prev_PC + 2])];
                MAR <<=8;
                MAR += memory[((memory[prev_PC + 1] << 8) + memory[prev_PC + 2]) + 1];
                break;

            case MEM_M_CON: //operand is used as a constant var...
                //TODO: no clue...see [1 ln 341]
                MAR = memory[prev_PC + 1];
                MAR <<= 8;
                MAR += memory[prev_PC + 2];
                break;
                
            case MEM_M_IND: //indirect, MAR is used as a ptr.
                //TODO: no clue...see [1 ln 346]
                int prev_MAR = MAR;
                MAR = memory[prev_MAR];
                MAR <<= 8;
                MAR += memory[prev_MAR + 1];
                break;

            }
            break;
        }
    }
        
}


void branchOps(uint8_t opcode)
{
    int type = opcode & BR_TYPE_MASK;
    //TODO: no clue...see [1 ln 358]
    int address = (memory[prev_PC + 1] << 8) + memory[prev_PC + 2];

    switch (type)
    {
    case BRA:
        PC = address;
        break;

    case BRZ:
        if(ACC == 0) PC = address;
        break;

    case BNE:
        if(ACC != 0) PC = address;
        break;

    case BLT:
        if(ACC < 0) PC = address;
        break;

    case BLE:
        if(ACC <= 0) PC = address;
        break;

    case BGT:
        if(ACC > 0) PC = address;
        break;

    case BGE:
        if(ACC >= 0) PC = address;
        break;
        
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
    else assert("[FAILURE TO DETERMINE PROPER INCREMENTS]");
    return -1000000;
}

std::vector<uint8_t> parseFileData(std::string fileName)
{
    std::vector<uint8_t> readProgram;
    std::ifstream inFile;
    try
    {
        inFile.open(fileName);
        std::string line;
        int i = 0;
        unsigned int temp;
        // while(!inFile.eof()) 
        // {
        //     inFile >> std::hex >> temp;

        //     readProgram.push_back(temp);
        // }
        //THE ABOVE WORKS TOO BUT FOR SOME REASON HAS ONE EXTRA "ec" that's missing from the expected output so 
        //...gonna just use the below method bec it's totally accurate
        while (std::getline(inFile, line))
        {
	        std::istringstream stream_in((line)); // Hex to binary conversion
	        for (unsigned hex_to_bin; stream_in >> std::hex >> hex_to_bin;)
            {
	            //memory[i] = hex_to_bin;
	            //i++;
                readProgram.push_back(hex_to_bin);
                //PRINT(hex_to_bin);
	        }
	    }

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return readProgram;
}


void writeMemoryToFile(unsigned char mem[])
{
    try
    {
        std::ofstream outFile;
        outFile.open(OUTPUT_FILE);
        for(int i = 0; i < MEMORY_SIZE; i++)
        {
            if((i % 16) == 0 && i != 0) outFile << "\n";
            std::ostringstream ss;
            ss << std::hex << (int)mem[i];
            std::string result = ss.str();
            if((int)mem[i] <= 15) 
            {
                result.insert(0,"0");
            }
            outFile << result ;
            outFile << " ";
        }
            
        outFile.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n\n\t[ERR: FAILED TO WRITE MEMORY TO FILE!!!]\n" << e.what() << '\n';
    }
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
    std::cout << std::endl;
}
