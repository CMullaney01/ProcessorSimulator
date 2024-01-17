#include <sstream> 
#include <limits>
#include "instruction.h"
#include "decode.h"
// Decode: In this stage, the processor decodes the instruction and determines the operands.

void Decode::decode(Instruction *instrPtr) {
    // std::cout << "Decoding instruction: " << instrPtr->instrString << std::endl;
    std::vector<std::string> splitInstr = splitString(instrPtr->instrString);
    // check if instruction is a pseudo instruction
    if (splitInstr.front() == "li" || splitInstr.front() == "mv") {
        splitInstr = handlePseudoInstr(splitInstr);
    }
    InstructionInfo InsInfo = InstructionMap.at(splitInstr.front());
    uint32_t lenInstr = splitInstr.size();
    std::string jumpLabel;
    std::string varLabel;
    instrPtr->opcode = InsInfo.opcode;
    instrPtr->type = InsInfo.type;
    instrPtr->cyclesLeft = InsInfo.executeCycles;
    int x;
    switch(instrPtr->type)
    {
        case RType:
            instrPtr->rs2 = register_map.at(splitInstr[lenInstr - 1]); // src2
            instrPtr->rs1 = register_map.at(splitInstr[lenInstr - 2]); // src1
            instrPtr->rd = register_map.at(splitInstr[lenInstr - 3]); // dest
            break;
        case IType:
            // check if immediate value is a variable label
            if (splitInstr[lenInstr - 1].front() == '%') {
                varLabel = splitInstr[lenInstr - 1]; // label to jump to
                instrPtr->immediateValue = processor->variableAddressMap.at(varLabel); // PC to jump to
            } else {
                x = std::stoi(splitInstr[lenInstr - 1]);
                instrPtr->immediateValue = x; // src1
            }
            instrPtr->rs1 = register_map.at(splitInstr[lenInstr - 2]); // src2
            instrPtr->rd = register_map.at(splitInstr[lenInstr - 3]); // dest
            break;
        case BType:
            jumpLabel = splitInstr[lenInstr - 1]; // label to jump to
            instrPtr->immediateValue = processor->jumpMap.at(jumpLabel); // PC to jump to
            instrPtr->rs1 = register_map.at(splitInstr[lenInstr - 2]); // src2
            instrPtr->rd = register_map.at(splitInstr[lenInstr - 3]); // dest
            // add to branch target buffer
            processor->branchPredictor->insert(instrPtr->instrString, instrPtr->pc_Fetched, instrPtr->immediateValue);
            // store rename Table State etc
            processor->scoreboard->storeBoard(instrPtr);
            break;
        case JType:
            if (splitInstr.front() == "j") {
                jumpLabel = splitInstr[lenInstr - 1]; // label to jump to
                instrPtr->immediateValue = processor->jumpMap.at(jumpLabel); // PC to jump to
                // add to branch target buffer
                processor->branchPredictor->insert(instrPtr->instrString, instrPtr->pc_Fetched, instrPtr->immediateValue);
            } else if (splitInstr.front() == "jr") {
                // std::cout << "jr" << std::endl;
                // of teh form jr ra therefore we need to get the value of ra
                instrPtr->rs1 = register_map.at(splitInstr[lenInstr - 1]); // PC to jump to
                // std::cout << "rs1: " << instrPtr->rs1 << std::endl;
            }
            // store rename Table State etc
            processor->scoreboard->storeBoard(instrPtr);
            break;
    };
    
    // once decode is finished set the next stage it needs to execute
    instrPtr->nextStage();
    
}

std::vector<std::string> Decode::splitString(const std::string &str) {
	std::vector<std::string> result;
	std::istringstream iss(str);
	for (std::string s; iss >> s; )
		result.push_back(s);
	return result;
}

void Decode::attachDecode(Processor *proc)
{
    processor = proc;
}

std::vector<std::string> Decode::handlePseudoInstr(std::vector<std::string> splitInstr) {
    if (splitInstr.front() == "li") {
        // li $rd, imm
        // addi $rd, $zero, imm
        splitInstr.front() = "addi";
        splitInstr.insert(splitInstr.begin() + 2, "zero");
    } else if (splitInstr.front() == "mv") {
        // mv $rd, $rs
        // addi $rd, $rs, $zero
        splitInstr.front() = "addi";
        splitInstr.insert(splitInstr.begin() + 3, "0");
    } else {
        std::cout << "Error: Pseudo instruction not recognized" << std::endl;
    }
    return splitInstr;
}