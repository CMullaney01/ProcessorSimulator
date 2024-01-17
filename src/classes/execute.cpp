#include <sstream> 

#include "instruction.h"
#include "fetch.h"
#include "processor.h"
#include "execute.h"

// Execute: In this stage, the processor executes the instruction.

void Execute::execute(Instruction *instrPtr) {
  // std::cout << "Executing Instruction: " << instrPtr->instrString << std::endl;
  int rs1 = processor->scoreboard->getOperandResult(instrPtr->rs1);
  int rs2 = processor->scoreboard->getOperandResult(instrPtr->rs2);
  int rd = processor->scoreboard->getOperandResult(instrPtr->rd);

  int originalMemoryAccess;
   switch (instrPtr->opcode) {
    // R-Type Instructions
    case ADD:
      instrPtr->result = rs1 + rs2;
      break;
    case SUB:
      instrPtr->result = rs1 - rs2;
      break;
    case SLL:
      instrPtr->result = rs1 << rs2;
      break;
    case SLT:
      instrPtr->result = rs1 < rs2;
      break;
    case XOR:
      instrPtr->result = rs1 ^ rs2;
      break;
    case SRL:
      instrPtr->result = rs1 >> rs2;
      break;
    case SRA:
      instrPtr->result = rs1 >> rs2;
      break;
    case OR:
      instrPtr->result = rs1 | rs2;
      break;
    case AND:
      instrPtr->result = rs1 & rs2;
      break;
    case MUL:
      instrPtr->result = rs1 * rs2;
      break;
    case DIV:
      instrPtr->result = rs1 / rs2;
      break;
    // I-Type Instructions
    case ADDI:
      instrPtr->result = rs1 + instrPtr->immediateValue;
      break;
    case SLTI:
      instrPtr->result = rs1 < instrPtr->immediateValue;
      break;
    case XORI:
      instrPtr->result = rs1 ^ instrPtr->immediateValue;
      break;
    case ORI:
      instrPtr->result = rs1 | instrPtr->immediateValue;
      break;
    case ANDI:
      instrPtr->result = rs1 & instrPtr->immediateValue;
      break;
    case SLLI:
      instrPtr->result = rs1 << instrPtr->immediateValue;
      break;
    case SRLI:
      instrPtr->result = rs1 >> instrPtr->immediateValue;
      break;
    case SRAI:
      instrPtr->result = rs1 >> instrPtr->immediateValue;
      break;
    case LW:
      originalMemoryAccess = rs1 + instrPtr->immediateValue;
      instrPtr->result = originalMemoryAccess/4;
      break;
    case SW:
      originalMemoryAccess = rs1 + instrPtr->immediateValue;
      instrPtr->result = originalMemoryAccess/4;
      break;
    
    // B-Type Instructions
    case BEQ:
      instrPtr->result = rd == rs1;
      processor->handleBranch(instrPtr->result, instrPtr);
      break;
    case BNE:
      instrPtr->result = rd != rs1;
      processor->handleBranch(instrPtr->result, instrPtr);
      break;
    case BLT:
      instrPtr->result = rd < rs1;
      processor->handleBranch(instrPtr->result, instrPtr);
      break;
    case BGE:
      instrPtr->result = rd >= rs1;
      processor->handleBranch(instrPtr->result, instrPtr);
      break;
    case BGT:
      instrPtr->result = rd > rs1;
      processor->handleBranch(instrPtr->result, instrPtr);
      break;
    case BLE:
      instrPtr->result = rd <= rs1;
      processor->handleBranch(instrPtr->result, instrPtr);
      break;
    // J-Type Instructions
    case J:
      processor->handleJump(instrPtr);
      break;
    case JR:
      processor->handleJump(instrPtr);
      break;
    default:
      break;
  }
  if (instrPtr->cyclesLeft > 0) {
    instrPtr->cyclesLeft--;
  } 
  if (instrPtr->cyclesLeft == 0) {
    instrPtr->nextStage();
  }
}

void Execute::attachExecute(Processor *proc)
{
    processor = proc;
}
