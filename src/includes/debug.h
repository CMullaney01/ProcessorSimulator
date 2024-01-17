#ifndef DEBUG_H
#define DEBUG_H

#include "processor.h"
#include "instruction.h"
#include <vector>  // add this line

// Debugger class which prints our things for debugging
class Processor;

class Debugger {
  public:
    Processor *processor;
    void attatchDebugProcessor(Processor *proc);
    void debugProcessor();
    void debugInstruction(Instruction *instrPtr);
    void debugPipeline(std::vector<Instruction*> pipeline);
    std::string opcodeToString(Opcodes op);
    std::string typeToString(InstructionType t);
    std::string stageToString(stage s);
    void promptToContinue();
};

#endif