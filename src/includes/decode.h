#ifndef DECODE_INCLUDED_
#define DECODE_INCLUDED_


#include <iostream> 
#include <vector> 
#include <string> 
#include <sstream> 
#include <fstream>

#include "instruction.h"
#include "processor.h"

//Decode: In this stage, the processor determines the meaning 
// of the fetched instruction, which involves decoding the instruction's 
// opcode and any operand or addressing mode information. 
// The processor may also read any necessary data from memory or registers to execute the instruction.

class Processor;

class Decode {
  public:
    Processor *processor;
    std::vector<std::string> splitString(const std::string& str);
    void decode(Instruction *instrPtr);
    void attachDecode(Processor *proc);
    std::vector<std::string> handlePseudoInstr(std::vector<std::string> splitInstr);
};

#endif