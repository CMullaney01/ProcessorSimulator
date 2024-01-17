#ifndef INSTRUCTION_DEFINED_
#define INSTRUCTION_DEFINED_

#include <string>

#include "isa.h"
#include "stage.h"

class Instruction {
  public:
    std::string instrString;
    InstructionType type;
    Opcodes opcode;
    stage s;
    Register rs1;
    Register rs2;
    Register rd;
    int renamedRd;
    int cyclesLeft;
    int memoryCycles;
    int immediateValue;
    int result;
    int instrNum; // instruction num so we know the order of the instructions
    int pc_Fetched;
    stage getStage();
    Instruction(std::string instr);
    Instruction();
    void nextStage();
};

#endif