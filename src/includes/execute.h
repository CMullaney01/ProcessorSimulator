#ifndef EXECUTE_INCLUDED_
#define EXECUTE_INCLUDED_

#include <iostream> 
#include <vector> 
#include <string> 
#include <sstream> 
#include <fstream>

#include "instruction.h"
#include "processor.h"

// Execute: Finally, the CPU executes the instruction by performing the operation 
// specified by the opcode on the operands. This can involve performing arithmetic or 
// logical operations on data, loading or storing data in memory, 
// or branching to a different part of the program based on the result of a comparison or other condition.
// The exact nature of the execute stage will depend on the type of instruction being executed.

class Processor;

class Execute {
  public:
    Processor *processor;
    void execute(Instruction *instrPtr);
    void attachExecute(Processor *proc);
};

#endif