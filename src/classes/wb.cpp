#include "wb.h"

void Wb::wb(Instruction *instrPtr) {
    // std::cout << "Write Back Stage for instruction: " << instrPtr->instrString << std::endl;
    if (instrPtr->opcode == Opcodes::SW) {
        int rd = processor->scoreboard->getOperandResult(instrPtr->rd);
        // std::cout << "Writing to memory: " << instrPtr->result << std::endl;
        // std::cout << "Value: " << rd << std::endl;
        processor->memory[instrPtr->result] = rd;
        processor->scoreboard->loadStore.erase(instrPtr->result);
        instrPtr->nextStage();
        return;
    }
    // std::cout << "Writing to register: " << instrPtr->rd << std::endl;
    // std::cout << "Value: " << instrPtr->result << std::endl;
    processor->registers[instrPtr->rd] = instrPtr->result;
    instrPtr->nextStage();
    return;
}

void Wb::attachWb(Processor *p) {
    processor = p;
}