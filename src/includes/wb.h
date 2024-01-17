#ifndef WB_INCLUDED_
#define WB_INCLUDED_

#include "instruction.h"
#include "processor.h"


// Write back: In the write back stage, the results of the 
// executed instruction are written back to the appropriate registers. 
// This stage updates the register file with the results of the 
// operation performed in the execute stage, so that 
// these values can be used in subsequent instructions.

class Processor;

class Wb {
  public:
    Processor *processor;
    void wb(Instruction *instrPtr);
    void attachWb(Processor *proc);
};

#endif