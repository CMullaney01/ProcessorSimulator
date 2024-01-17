#include "instruction.h"
#include "fetch.h"
#include "processor.h"

// Fetch: In this stage, the processor reads the next instruction from memory.
// The address of the instruction to be fetched is typically stored in the program counter (PC) register.
// Once the instruction has been fetched, the PC is incremented to point to the next instruction.

void Fetch::fetch(Instruction *instrPtr) {
  if (processor->pc >= processor->instructionMemory_size) {
    // std::cout << "Reached end of program" << std::endl;
    instrPtr = nullptr;
    return;
  }
  std::string instr = processor->instructionMemory[processor->pc];
  // std::cout << "Fetching instruction: " << instr << std::endl;
  instrPtr->instrString = instr;
  instrPtr->instrNum = processor->instructionCount;
  instrPtr->memoryCycles = -1;
  instrPtr->pc_Fetched = processor->pc;
  instrPtr->nextStage();
  processor->instructionCount++;
  processor->pc = processor->pc + 1;
  processor->scoreboard->completeFetchInstr(instrPtr);

  /// Handling Branches //////////////////////////////////////////////
  std::string btb_str = instr + "-" + std::to_string(instrPtr->pc_Fetched);
  if(processor->branchPredictor->BTargetbuffer.find(btb_str) != processor->branchPredictor->BTargetbuffer.end())
  {
    // We have a hit in the BTB So now we can predict the next PC
    int prediction = processor->branchPredictor->predict(btb_str, instrPtr->pc_Fetched, instrPtr->instrNum);
    processor->pc = prediction;
  }
}


void Fetch::attachFetch(Processor *proc)
{
    processor = proc;
}
