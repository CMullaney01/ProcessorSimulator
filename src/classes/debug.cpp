#include "debug.h"
#include "instruction.h"
#include <iomanip>

void Debugger::attatchDebugProcessor(Processor *proc)
{
    processor = proc;
}

void Debugger::debugProcessor()
{
  std::cout << "-----------------------:Debugging processor:-------------------" << std::endl;
  std::cout << "Clock: " << processor->clock << std::endl;
  if (processor->superS) {
    std::cout << "Number of instructions: " << processor->reorderBuffer->InstructionsCount << std::endl;
    double instructionsPerCycle = static_cast<double>(processor->reorderBuffer->InstructionsCount) / processor->clock;
    std::cout << "Instructions per Cycle: " << std::fixed << std::setprecision(2) << instructionsPerCycle << std::endl;
  } else {
    std::cout << "Number of instructions: " << processor->instructionCount << std::endl;
    double instructionsPerCycle = static_cast<double>(processor->instructionCount) / processor->clock;
    std::cout << "Instructions per Cycle: " << std::fixed << std::setprecision(2) << instructionsPerCycle << std::endl;
  }
  std::cout << "Total Predictions: " << processor->branchPredictor->totalNumPredictions << std::endl;
  std::cout << "Correct Predictions: " << processor->branchPredictor->correctPredictions << std::endl;
  double branchPredictionAccuracy = static_cast<double>(processor->branchPredictor->correctPredictions) / processor->branchPredictor->totalNumPredictions;
  std::cout << "Branch Prediction Accuracy: " << std::fixed << std::setprecision(2) << branchPredictionAccuracy << std::endl;  
  std::cout << "PC: " << processor->pc << std::endl;
  std::cout << "Registers: " << std::endl;
  for (int x = 0; x < 32; x+=4)
  {
  std::cout << "Registers " << x << "-" << x+3 << ": " 
            << processor->registers[x] << " " << processor->registers[x+1] 
            << " " << processor->registers[x+2] << " " << processor->registers[x+3]
            << std::endl;
  }
  // std::cout << "Instruction memory: " << std::endl;
  // for (int x = 0; x < processor->instructionMemory_size; x++)
  // {
  //     std::cout << "Instruction " << x << ": " << processor->instructionMemory[x] << std::endl;
  // }
  std::cout << "Data memory: " << std::endl;
  for (int i = 0; i < processor->memory_size; i += 128) {
      std::cout << "Data [" << i << "-" << i+127 << "]: ";
      for (int j = 0; j < 128 && i+j < processor->memory_size; j++) {
          std::cout << processor->memory[i+j] << " ";
      }
      std::cout << std::endl;
  }
  // promptToContinue();
  return;
  std::cout << "-----------------------:Debugging processor:-------------------" << std::endl;

}

void Debugger::debugInstruction(Instruction *instrptr) {

    std::cout << "Debugging instruction" << std::endl;
    std::cout << "Instruction string: " << instrptr->instrString << std::endl;
    std::cout << "Instruction type: " << typeToString(instrptr->type) << std::endl;
    std::cout << "Instruction opcode: " << opcodeToString(instrptr->opcode) << std::endl;
    std::cout << "Instruction stage: " << stageToString(instrptr->s) << std::endl;
    std::cout << "Instruction rs1: " << instrptr->rs1 << std::endl;
    std::cout << "rs1 value: " << processor->registers[instrptr->rs1] << std::endl;
    std::cout << "Instruction rs2: " << instrptr->rs2 << std::endl;
    std::cout << "rs2 value: " << processor->registers[instrptr->rs2] << std::endl;
    std::cout << "Instruction rd: " << instrptr->rd << std::endl;
    std::cout << "rd value: " << processor->registers[instrptr->rd] << std::endl;
    std::cout << "Instruction cycles: " << instrptr->cyclesLeft << std::endl;
    std::cout << "Instruction immediateValue: " << instrptr->immediateValue << std::endl;
    std::cout << "Instruction result: " << instrptr->result << std::endl;
    std::cout << "Instruction result value: " << processor->memory[instrptr->result] << std::endl;
    promptToContinue();
  return;
}

void Debugger::debugPipeline(std::vector<Instruction*> pipeline) {
  std::cout << "--------------------Debugging pipeline-----------------" << std::endl;
  for (std::vector<Instruction*>::size_type x = 0; x < pipeline.size(); x++) {
    if (pipeline[x] == NULL) {
      std::cout << "Instruction " << x << ": NULL" << std::endl;
      continue;
    }
    std::cout << "Instruction " << x << ": " << std::endl;
    std::cout << "Instruction string: " << pipeline[x]->instrString << std::endl;
    std::cout << "Instruction type: " << typeToString(pipeline[x]->type) << std::endl;
    std::cout << "Instruction opcode: " << opcodeToString(pipeline[x]->opcode) << std::endl;
    std::cout << "Instruction stage: " << stageToString(pipeline[x]->s) << std::endl;
    std::cout << "Instruction rs1: " << pipeline[x]->rs1 << std::endl;
    std::cout << "rs1 value: " << processor->registers[pipeline[x]->rs1] << std::endl;
    std::cout << "Instruction rs2: " << pipeline[x]->rs2 << std::endl;
    std::cout << "rs2 value: " << processor->registers[pipeline[x]->rs2] << std::endl;
    std::cout << "Instruction rd: " << pipeline[x]->rd << std::endl;
    std::cout << "rd value: " << processor->registers[pipeline[x]->rd] << std::endl;
    std::cout << "Instruction Execute cycles left " << pipeline[x]->cyclesLeft << std::endl;
    std::cout << "Instruction immediateValue: " << pipeline[x]->immediateValue << std::endl;
    std::cout << "Instruction result: " << pipeline[x]->result << std::endl;
    std::cout << "Instruction result value: " << processor->memory[pipeline[x]->result] << std::endl;
  }
  std::cout << "-------------------------------------------------------" << std::endl;
  promptToContinue();
  return;
}

std::string Debugger::stageToString(stage s) {
  // stages: FETCH, DECODE, EXECUTE, MEM, WB, DONE,
  switch (s) {
    case FETCH:
      return "FETCH";
    case DECODE:
      return "DECODE";
    case EXECUTE:
      return "EXECUTE";
    case MEM:
      return "MEM";
    case WB:
      return "WB";
    case DONE:
      return "DONE";
    default:
      return "unknown";
  }
}
std::string Debugger::opcodeToString(Opcodes op) {
   switch (op) {
        case ADD:
            return "add";
        case SUB:
            return "sub";
        case SLL:
            return "ssl";
        case SLT:
            return "slt";
        case XOR:
            return "xor";
        case SRL:
            return "srl";
        case SRA:
            return "sra";
        case OR:
            return "or";
        case AND:
            return "and";
        case ADDI:
            return "addi";
        case SLTI:
            return "slti";
        case XORI:
            return "xori";
        case ORI:
            return "ori";
        case ANDI:
            return "andi";
        case SLLI:
            return "ssli";
        case SRLI:
            return "srli";
        case SRAI:
            return "srai";
        case LW:
            return "lw";
        case SW:
            return "sw";
        case BEQ:
            return "beq";
        case BNE:
            return "bne";
        case BLT:
            return "blt";
        case BGE:
            return "bge";
        default:
            return "unknown";
    }
}

std::string Debugger::typeToString(InstructionType t) {
  // types: RType, IType, JType, BType, End
  switch (t) {
    case RType:
      return "RType";
    case IType:
      return "IType";
    case BType:
      return "BType";
    default:
      return "unknown";
  }
}

void Debugger::promptToContinue() {
    std::string input;
    do {
        std::cout << "Do you want to continue? (y/n): ";
        std::getline(std::cin, input);
    } while (input != "y" && input != "n");

    if (input == "n") {
        std::cout << "Exiting...\n";
        exit(0); // or return, or throw an exception, etc.
    }
}