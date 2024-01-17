#include <string>
#include "stage.h"
#include "instruction.h"
#include "stage.h"


Instruction::Instruction(std::string instr)
{
    instrString = instr;
    cyclesLeft = 0;
    memoryCycles = -1;
    s = stage::FETCH;
    rd = $empty;
    rs1 = $empty;
    rs2 = $empty;
    renamedRd = -1;
};

// empty instruction to initialise
Instruction::Instruction()
{
    instrString = "";
    s = stage::FETCH;
    rd = $empty;
    rs1 = $empty;
    rs2 = $empty;
};

stage Instruction::getStage() {
    return s;
};

void Instruction::nextStage() {
    if (s == stage::FETCH) {
        s = stage::DECODE;
        return;
    }
    if (s == stage::DECODE) {
        s = stage::EXECUTE;
        return;
    }
    if (s == stage::EXECUTE) {
        switch (type)
        {
        case InstructionType::RType:
            s = stage::WB;
            break;
        case InstructionType::IType:
            if (opcode == Opcodes::LW || opcode == Opcodes::SW) {
                s = stage::MEM;
            } else {
                s = stage::WB;
            }
            break;
        case InstructionType::BType:
            s = stage::DONE;
            break;
        case InstructionType::JType:
            s = stage::DONE;
            break;
        default:
            break;
        }
        return;
    }
    if (s == stage::MEM) {
        if (opcode == Opcodes::LW) {
            s = stage::WB;
        } else {
            // we need to store in the writeback stage to prevent having to snapshot our memory as well
            s = stage::WB;
        }
        return;
    }
    if (s == stage::WB) {
        s = stage::DONE;
        return;
    }
};