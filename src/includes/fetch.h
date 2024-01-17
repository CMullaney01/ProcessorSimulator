#ifndef FETCH_INCLUDED_
#define FETCH_INCLUDED_

#include "instruction.h"
#include "processor.h"

// Fetch: In this stage, the processor reads the next instruction from memory. 
// The address of the instruction to be fetched is typically stored in the program counter (PC) register. 
// Once the instruction has been fetched, the PC is incremented to point to the next instruction.
class Processor;

class Fetch {
    public:
    Processor *processor;
    void fetch(Instruction *instruction);
    void attachFetch(Processor *proc);
};

#endif