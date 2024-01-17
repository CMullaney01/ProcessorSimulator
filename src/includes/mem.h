#ifndef MEM_INCLUDED_
#define MEM_INCLUDED_

#include <iostream> 
#include <vector> 
#include <string> 
#include <sstream> 
#include <fstream>

#include "instruction.h"
#include "processor.h"

// Memory access: In the memory stage, the processor accesses memory if necessary. 
// This may involve reading or writing data from or to memory, depending on the instruction being executed. 
// Some instructions may not require memory access, in which case this stage may be skipped.

class Processor;

class Mem {
  public:
    Processor *processor;
    void mem(Instruction *instrPtr);
    void attachMem(Processor *proc);
};

#endif