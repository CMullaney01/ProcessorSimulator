#include "mem.h"

void Mem::mem(Instruction *InstrPtr) {
    // std::cout << "Memory Stage for instruction: " << InstrPtr->instrString  << "InstructionNum: " << InstrPtr->instrNum << std::endl;
    if (InstrPtr->memoryCycles == -1) {
        // InstrPtr->memoryCycles = 5;
        InstrPtr->memoryCycles = 3;
    }
    // std::cout << "Memory Cycles: " << InstrPtr->memoryCycles << std::endl;
    // std::cout << "Getting address: " << InstrPtr->result << std::endl;
    int x = 0;
    if (InstrPtr->memoryCycles == 1) {
        switch (InstrPtr->opcode)
        {
        case LW:
            // std::cout << "Reading from memory: " << InstrPtr->result << std::endl;
            // std::cout << "Value: " << processor->memory[InstrPtr->result] << std::endl;
            x = processor->memory[InstrPtr->result];
            // this is the least run through the pipeline, so we can update the scoreboard and result here
            if (InstrPtr->memoryCycles == 1) {
                processor->scoreboard->loadStore.erase(InstrPtr->result);
                InstrPtr->result = x;
            }
            break;
        case SW:
            // do nothing
            break;
        default:
            break; 
        }
    }
    InstrPtr->memoryCycles--;
    if (InstrPtr->memoryCycles == 0) {
        InstrPtr->nextStage();
        // std::cout << "Next Stage: " << InstrPtr->s << std::endl;
    }
}

void Mem::attachMem(Processor *p) {
    processor = p;
}