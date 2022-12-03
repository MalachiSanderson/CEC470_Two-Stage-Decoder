#include "TS_Decoder.hpp"

#define INPUT_FILE "mem_in.txt"

int main()
{
    std::vector<uint8_t> readProgram;
    readProgram = parseFileData(INPUT_FILE);
    for(int i = 0; i < readProgram.size(); i++)
    {
        memory[i] = readProgram.at(i);
    }
    
}

void fetchNextInstruction(void)
{
   IR = memory[PC++];
}

void executeInstruction(void)
{
    //check ir to determine op then execute op

}

void determineOp(uint8_t opcode)
{
    



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
