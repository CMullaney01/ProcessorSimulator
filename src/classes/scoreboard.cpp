#include "scoreboard.h"
#include <iostream>
#include <iomanip>

Scoreboard::Scoreboard()
{
  // initialise the board to true as all registers are free at the start of execution
  board.fill(true);
  regStatus.fill(-1);
}

void Scoreboard::attachScoreboard(Processor *p){
    processor = p;
}

void Scoreboard::setValid(int reg)
{
  board[reg] = true;
}

void Scoreboard::setBusy(int reg)
{
  board[reg] = false;
}

bool Scoreboard::checkAvailable(Register reg, int instrNum)
{
  // case where register isnt used and is therefore -1 e.g. in I-Type instructions rs2 is -1 we also want to bypass the $zero register as 
  // it cannot be written to and therefore we have no data hazards
  // std::cout << "Checking if reg: " << reg << " is available" << std::endl;
  // std::cout << "RegStatus: " << regStatus[reg] << std::endl;
  if (reg == -1 || reg == 0 || regStatus[reg] == instrNum)
  {
    return true;
  }
  else
  {
    // std::cout << "Returning Board Reg State: " << board[reg] << std::endl;
    return board[reg];
  }
}


void Scoreboard::addIssuedInstr(Instruction *instrPtr)
{
  issuedInstructions.push_back(instrPtr);
}

void Scoreboard::completeFetchInstr(Instruction *instrPtr)
{
  inFlightInstructions.push_back(instrPtr);
}

void Scoreboard::setInstrBusy(Instruction *instrPtr)
{
  // std::cout << "Setting instr busy " << instrPtr->instrString << std::endl;
  setBusy(instrPtr->rd);
  setRegStatus(instrPtr->rd, instrPtr->instrNum);
  setBusy(instrPtr->rs1);
  setRegStatus(instrPtr->rs1, instrPtr->instrNum);
  if (instrPtr->type == RType) {
      setBusy(instrPtr->rs2);
      setRegStatus(instrPtr->rs2, instrPtr->instrNum);
  }
}

void Scoreboard::completeDecodeInstr(Instruction *instrPtr)
{
  addIssuedInstr(instrPtr);
}

// Basically in Simple scoreboarding we write back and set things to be available in the Writeback stage
// In Scoreboarding with Forwarding we write back and set things to be available in the Execute stage
void Scoreboard::completeExecutedInstr(Instruction *instrPtr)
{
  // without result forwarding we only need to validate for branch instructions here
  // as they never enter the writeback stage so we need to release them here
  int instrNumCheck = instrPtr->instrNum;
  if (instrPtr->type != BType && instrPtr->type != JType) {
    // std::cout << "Not a BType or JType instruction" << std::endl;
    return;
  }
  // we are a btype function so we need to release the registers
  for (std::array<int, 512>::size_type i = 0; i < 32; i++) {
    if (regStatus[i] == instrNumCheck) {
      regStatus[i] = -1;
      board[i] = true;
    }
  }
  for (std::vector<Instruction*>::size_type i = 0; i < issuedInstructions.size(); i++) {
    if (issuedInstructions[i]->instrNum == instrPtr->instrNum) {
      issuedInstructions.erase(issuedInstructions.begin() + i);
    }
  }

  // remove from inflight instructions
  flushFlightInstr(instrPtr);
  completedInstructions.push_back(instrPtr);
  return;
}

void Scoreboard::completeMemoryStageInstr([[maybe_unused]]Instruction *instrPtr)
{
  // Dont need to do anything here as we do it in the writeback stage for no result forwarding
  return;
}

void Scoreboard::completeWritebackInstr(Instruction *instrPtr)
{
  int instrNumCheck = instrPtr->instrNum;
  for (std::array<int, 32>::size_type i = 0; i < regStatus.size(); i++) {
    if (regStatus[i] == instrNumCheck) {
      regStatus[i] = -1;
      board[i] = true;
    }
  }
  //remove from issued instructions and add to completed instructions
  for (std::vector<Instruction*>::size_type i = 0; i < issuedInstructions.size(); i++) {
    if (issuedInstructions[i]->instrNum == instrPtr->instrNum) {
      issuedInstructions.erase(issuedInstructions.begin() + i);
      completedInstructions.push_back(instrPtr);
    }
  }
  // remove inflight instructions
  flushFlightInstr(instrPtr);
}


bool Scoreboard::isInstrComplete(int instrNum)
{
for (std::vector<Instruction*>::size_type i = 0; i < completedInstructions.size(); i++) {    if (completedInstructions[i]->instrNum == instrNum) {
      return true;
    }
  }
  return false;
}

int Scoreboard::getInstrResult(Instruction *instrPtr)
{
  int instrNumCheck = instrPtr->instrNum;
for (std::vector<Instruction*>::size_type i = 0; i < completedInstructions.size(); i++) {    if (completedInstructions[i]->instrNum == instrNumCheck) {
      return completedInstructions[i]->result;
    }
  }
  return -1;
}

bool Scoreboard::isInstrReady(Instruction *instrPtr)
{
  // std::cout << "Checking if instruction is ready" << std::endl;
  Register rs1 = instrPtr->rs1;
  Register rs2 = instrPtr->rs2;
  Register rd = instrPtr->rd;

  // we dont need to check the destination register as it is not used until the writeback stage
  // and we will use a reorder buffer to ensure the correct order of execution
  if (instrPtr->type == BType || instrPtr->type == IType) {
    // maybe poor design but we need to check the destination register for branch instructions
    // and rs2 is not used in branch instr so we can use it to check the destination register
    rs2 = rd;
  }
  // std::cout << "checking registers: " << rs1 << " and " << rs2 << " for instruction: " << instrPtr->instrString << std::endl;
  if (checkAvailable(rs1, instrPtr->instrNum) && checkAvailable(rs2, instrPtr->instrNum)) {
    // std::cout << "Instruction ready returning true" << std::endl;
    return true;
  }
  else {
    // std::cout << "Instruction not ready returning false" << std::endl;
    return false;
  }
}

int Scoreboard::getOperandResult(Register reg) {
  return processor->registers[reg];
}

void Scoreboard::setRegStatus(int reg, int instrNum)
{
  regStatus[reg] = instrNum;
}

void Scoreboard::logBoard()
{ 
  std::cout << "-:Scoreboard Start:-" << std::endl;
  // for (std::array<bool, 32>::size_type i = 0; i < board.size(); i++)
  for (std::array<bool, 32>::size_type i = 0; i < 64; i++)
  {
    std::cout << "Register " << i << " is " << board[i] << " held by Instruction " << regStatus[i] << std::endl;
  }
  std::cout << "-:Scoreboard End:- " << std::endl;
}

void Scoreboard::flushFlightInstr(Instruction *instrPtr) {
  for (std::vector<Instruction*>::size_type i = 0; i < inFlightInstructions.size(); i++) {    
    if (inFlightInstructions[i]->instrNum == instrPtr->instrNum) {
      inFlightInstructions.erase(inFlightInstructions.begin() + i);
    }
  }
}

void Scoreboard::flushFlightInstructions(Instruction *instrPtr) {
    // Find the set instruction with the given instrNum
    Instruction* setInstruction = nullptr;
    for (Instruction* instr : inFlightInstructions) {
        if (instr->instrNum == instrPtr->instrNum) {
            setInstruction = instr;
            break;
        }
    }

    // If the set instruction was found, remove all instructions after it
    if (setInstruction != nullptr) {
        auto it = inFlightInstructions.begin();
        while (it != inFlightInstructions.end()) {
            if ((*it)->instrNum > instrPtr->instrNum) {
                it = inFlightInstructions.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void Scoreboard::flushIssuedInstructions(Instruction *instrPtr) {
    // Find the instruction with the given instrNum
    Instruction* targetInstruction = nullptr;
    for (Instruction* instr : issuedInstructions) {
        if (instr->instrNum == instrPtr->instrNum) {
            targetInstruction = instr;
            break;
        }
    }

    // If the instruction was found, remove all instructions after it
    if (targetInstruction != nullptr) {
        auto it = issuedInstructions.begin();
        while (it != issuedInstructions.end()) {
            if ((*it)->instrNum > instrPtr->instrNum) {
                it = issuedInstructions.erase(it);
            } else {
                ++it;
            }
        }
    }
}


int Scoreboard::lowestFlightInstrToWriteBack() {
  int lowest = inFlightInstructions.front()->instrNum;
  for (std::vector<Instruction*>::size_type i = 0; i < inFlightInstructions.size(); i++) {      if (inFlightInstructions[i]->s > 0) {
        if (inFlightInstructions[i]->instrNum < lowest && inFlightInstructions[i]->type != BType && inFlightInstructions[i]->opcode != SW) {
          lowest = inFlightInstructions[i]->instrNum;
        }
    }
  }
  return lowest;
}


void Scoreboard::logFlightInstrs() {
    // Define the column headers for the table
    std::cout << "--------------------:In flight Instructions Start:----------------------" << std::endl;
    std::cout << std::left << std::setw(10) << "InstrNum"
              << std::setw(24) << "InstrString"
              << std::setw(8) << "Fetch"
              << std::setw(8) << "Decode"
              << std::setw(8) << "Execute"
              << std::setw(8) << "Memory"
              << std::setw(8) << "Writeback"
              << std::endl;

    // Loop through each instruction in the vector and print out its data in a row
    for (auto instr : inFlightInstructions) {
        std::cout << std::left << std::setw(10) << instr->instrNum
                  << std::setw(24) << instr->instrString;
        for (int stage = 0; stage < 5; ++stage) {
            std::cout << std::setw(8) << (instr->s >= stage ? "X" : "-"); // Fetch, Decode, Execute, Memory, Writeback columns
        }
        std::cout << std::endl; // End the row
    }

    std::cout << "--------------------:In flight Instructions End:----------------------" << std::endl;
}

///// End of Scoreboard Class /////

///// Start of ScoreboardWithForwarding Class /////
ScoreboardWithForwarding::ScoreboardWithForwarding()
{
  // initialise the board to true as all registers are free at the start of execution
  board.fill(true);
  regStatus.fill(-1);
  regResult.fill(std::make_pair(false, -1));
}


void ScoreboardWithForwarding::completeExecutedInstr(Instruction *instrPtr)
{
  int instrNumCheck = instrPtr->instrNum;
  if (instrPtr->opcode == LW || instrPtr->opcode == SW) {
    // Dont do anything we need to wait for the memory stage to complete
    // in actuality we can let sw happen now but lw needs to wait for memory stage
    // we have grouped together for now, if optimise in the future we can change this
    return;
  }
  if (instrPtr->type == BType || instrPtr->type == JType) {
    // we are a btype function so we need to release the registers
    for (std::array<int, 32>::size_type i = 0; i < regStatus.size(); i++) {
      if (regStatus[i] == instrNumCheck) {
        regStatus[i] = -1;
        board[i] = true;
      }
      for (std::vector<Instruction*>::size_type i = 0; i < issuedInstructions.size(); i++) {
        if (issuedInstructions[i]->instrNum == instrPtr->instrNum) {
          issuedInstructions.erase(issuedInstructions.begin() + i);
        }
      }
    }
    flushFlightInstr(instrPtr);
    completedInstructions.push_back(instrPtr);
    return;
  }

  for (std::array<int, 32>::size_type i = 0; i < regStatus.size(); i++) {
    if (regStatus[i] == instrNumCheck) {
      regStatus[i] = -1;
      board[i] = true;
      if (i == static_cast<std::array<int, 32>::size_type>(instrPtr->rd)) {
        regResult[i] = std::make_pair(true, instrPtr->result);
      }
    }
  }
}


void ScoreboardWithForwarding::completeMemoryStageInstr(Instruction *instrPtr)
{
  int instrNumCheck = instrPtr->instrNum;
  if (instrPtr->opcode == LW) {
    for (std::array<int, 32>::size_type i = 0; i < regStatus.size(); i++) {
      if (regStatus[i] == instrNumCheck) {
        // regStatus[i] = -1;
        if (i == instrPtr->rd) {
          regResult[i] = std::make_pair(true, instrPtr->result);
        }
        board[i] = true;
      }
    }
  }
  for (std::vector<Instruction*>::size_type i = 0; i < issuedInstructions.size(); i++) {
    if (issuedInstructions[i]->instrNum == instrPtr->instrNum) {
      issuedInstructions.erase(issuedInstructions.begin() + i);
      completedInstructions.push_back(instrPtr);
    }
  }
}

void ScoreboardWithForwarding::completeWritebackInstr(Instruction *instrPtr)
{
  // remove inflight instructions
  int instrNumCheck = instrPtr->instrNum;
  if (instrPtr->opcode == SW) {
    for (int i = 0; i < regStatus.size(); i++) {
      if (regStatus[i] == instrNumCheck) {
        regStatus[i] = -1;
        board[i] = true;
      }
    }
  }
  flushFlightInstr(instrPtr);
  for (std::vector<Instruction*>::size_type i = 0; i < issuedInstructions.size(); i++) {
    if (issuedInstructions[i]->instrNum == instrPtr->instrNum) {
      issuedInstructions.erase(issuedInstructions.begin() + i);
      completedInstructions.push_back(instrPtr);
    }
  }
  return;
}


int ScoreboardWithForwarding::getOperandResult(Register reg) {
  // if there is a return value from the execution stage set operand result to that
  if (regResult[reg].first) {
    // std::cout << "Forwarding register " << reg << " with value " << regResult[reg].second << std::endl;
    // return the result from the execution stage
    int result = regResult[reg].second;
    return result;
  }
  else 
  {
    return processor->registers[reg];
  }
}

void ScoreboardWithForwarding::logBoard()
{
  std::cout << "-:Scoreboard Start:-" << std::endl;
  for (std::array<bool, 32>::size_type i = 0; i < 32; i++)
  {
    std::cout << "Register " << i << " is " << board[i] << " held by Instruction " << regStatus[i] << " with value " << regResult[i].second << std::endl;
  }
  std::cout << "-:Scoreboard End:- " << std::endl;
}

void ScoreboardRenaming::logBoard()
{
  std::cout << "-:Scoreboard Start:-" << std::endl;
  for (std::array<bool, 32>::size_type i = 0; i < 128; i++)
  {
    std::cout << "Register " << i << " is " << board[i] << " held by Instruction " << regStatus[i] << " with value " << regResult[i].second << std::endl;
  }
  std::cout << "-:Scoreboard End:- " << std::endl;
}

///// End of ScoreboardWithForwarding Class /////

///// Start of ScoreboardRenaming Class /////
// IT WORKS HURRAY! we can easily move this functionality to the ROBUFF as the Scoreboard with forwarding has exactly the same logic
// if we remove the renaming functionality from the scoreboard and move it to the ROBUFF we can use the same scoreboard for both
ScoreboardRenaming::ScoreboardRenaming()
{
  // initialise the board to true as all registers are free at the start of execution
    board.fill(true);
    regStatus.fill(-1);
    regResult.fill(std::make_pair(false, -1));
    regCount = $prf0; // ARF 32 registers + 1 for $pc therefore our prf starts from 33
    // initialse rename map to -1 ($empty)
    for (int i = 0; i < 32; ++i) {
    renameMap[static_cast<Register>(i)] = Register::$empty;
    }
}

int ScoreboardRenaming::getOperandResult(Register reg) {
  // std::cout << "Getting operand result for register " << reg << std::endl;
  // if there is a return value from the execution stage set operand result to that
  if (regResult[reg].first) {
    // std::cout << "Forwarding register " << reg << " with value " << regResult[reg].second << std::endl;
    // return the result from the execution stage
    int result = regResult[reg].second;
    return result;
  }
  else 
  {
    return processor->registers[reg];
  }
}

void ScoreboardRenaming::completeDecodeInstr(Instruction *instrPtr)
{
  // Here we need to rename the instruction, and rename only, do not set reg status or update any boards etc
  // std::cout << "Renaming instruction " << instrPtr->instrNum << std::endl;
  // std::cout << "Original instruction: " << instrPtr->rd << " " << instrPtr->rs1 << " " << instrPtr->rs2 << std::endl;

  destinationMap[instrPtr->instrNum] = instrPtr->rd;
  switch (instrPtr->type)
  {
  case RType:
    renameRType(instrPtr);
    break;
  case IType:
    renameIType(instrPtr);
    break;
  case BType:
    renameBType(instrPtr);
    break;
  default:
    break;
  }
  // std::cout << "Renamed instruction: " << instrPtr->rd << " " << instrPtr->rs1 << " " << instrPtr->rs2 << std::endl;
  // / going to need some sort of print here to check correct renaming
  addIssuedInstr(instrPtr);
  setInstrBusy(instrPtr);
  return;
}

void ScoreboardRenaming::completeExecutedInstr(Instruction *instrPtr)
{
  int instrNumCheck = instrPtr->instrNum;
  if (instrPtr->opcode == LW || instrPtr->opcode == SW) {
    // Dont do anything we need to wait for the memory stage to complete
    return;
  }
  if (instrPtr->type == BType || instrPtr->type == JType) {
    // we are a btype function so we need to release the registers
    for (int i = 0; i < regStatus.size(); i++) {
      if (regStatus[i] == instrNumCheck) {
        regStatus[i] = -1;
        board[i] = true;
      }
      for (std::vector<Instruction*>::size_type i = 0; i < issuedInstructions.size(); i++) {
          if (issuedInstructions[i]->instrNum == instrPtr->instrNum) {
            issuedInstructions.erase(issuedInstructions.begin() + i);
          }
      }
    }
    flushFlightInstr(instrPtr);
    completedInstructions.push_back(instrPtr);
    return;
  }

  for (int i = 0; i < regStatus.size(); i++) {
    if (regStatus[i] == instrNumCheck) {
      regStatus[i] = -1;
      board[i] = true;
      if (i == instrPtr->rd) {
        regResult[i] = std::make_pair(true, instrPtr->result);
      }
    }
  }

  for (int i = 0; i < issuedInstructions.size(); i++) {
    if (issuedInstructions[i]->instrNum == instrPtr->instrNum) {
      issuedInstructions.erase(issuedInstructions.begin() + i);
      completedInstructions.push_back(instrPtr);
    }
  }
}

void ScoreboardRenaming::completeMemoryStageInstr(Instruction *instrPtr)
{
  int instrNumCheck = instrPtr->instrNum;
  if (instrPtr->opcode == LW) {
    // std::cout << "LW instruction " << instrPtr->instrNum << " has completed memory stage: "  << instrPtr->instrString << std::endl;
    for (int i = 0; i < regStatus.size(); i++) {
      if (regStatus[i] == instrNumCheck) {
        // std::cout << "Releasing register " << i << " from instruction " << instrNumCheck << std::endl;
        regStatus[i] = -1;
        if (i == instrPtr->rd) {
          regResult[i] = std::make_pair(true, instrPtr->result);
        }
        board[i] = true;
      }
    }
    for (std::vector<Instruction*>::size_type i = 0; i < issuedInstructions.size(); i++) {
      if (issuedInstructions[i]->instrNum == instrPtr->instrNum) {
        issuedInstructions.erase(issuedInstructions.begin() + i);
        completedInstructions.push_back(instrPtr);
      }
    }
  }
}

void ScoreboardRenaming::completeWritebackInstr(Instruction *instrPtr)
{
  // remove inflight instructions
  // essentially at this point we need to write the result to the architectural register file
  // and then release the renamed registers
  int instrNumCheck = instrPtr->instrNum;
  if (instrPtr->opcode == SW) {
    for (int i = 0; i < regStatus.size(); i++) {
      if (regStatus[i] == instrNumCheck) {
        regStatus[i] = -1;
        board[i] = true;
      }
    }
    flushFlightInstr(instrPtr);
    for (std::vector<Instruction*>::size_type i = 0; i < issuedInstructions.size(); i++) {
      if (issuedInstructions[i]->instrNum == instrPtr->instrNum) {
        issuedInstructions.erase(issuedInstructions.begin() + i);
        completedInstructions.push_back(instrPtr);
      }
    }
    return;
  }
  // write the result to the architectural register file
  // std::cout << "Writing result " << instrPtr->result << " to register " << destinationMap[instrPtr->instrNum] << std::endl;
  Register OriginalDest = destinationMap[instrPtr->instrNum];
  processor->registers[OriginalDest] = instrPtr->result;
  // release the renamed registers
  flushFlightInstr(instrPtr);
  return;
}

void ScoreboardRenaming::renameRType(Instruction *instrPtr)
{
  // Get the source and destination registers from the instruction
  Register rs1 = instrPtr->rs1;
  Register rs2 = instrPtr->rs2;
  Register rd = instrPtr->rd;

  // std::cout << "rs1: " << rs1 << std::endl;
  // std::cout << "rs2: " << rs2 << std::endl;
  // std::cout << "rd: " << rd << std::endl;
  // check the rename map to see if the register has been renamed
  // if it has been renamed then use the renamed register
  // if it has not been renamed then use the original register
  if (renameMap[rs1] != $empty) {
    // std::cout << "rs1 has been renamed" << std::endl;
    // std::cout << renameMap[rs1] << std::endl;
    // if the source operand has already been renamed then use the renamed register
    rs1 = renameMap[rs1];
    instrPtr->rs1 = rs1;
  } else {
    // if the source operand has not been renamed then rename it
    if (rs1 != $zero) {
      Register renameRs1 = getNextRenameReg();
      renameMap[rs1] = renameRs1;
      instrPtr->rs1 = renameRs1;
    }
  }
  if (renameMap[rs2] != $empty) {
    // std::cout << "rs2 has been renamed" << std::endl;
    // std::cout << renameMap[rs2] << std::endl;
    // if the source operand has already been renamed then use the renamed register
    rs2 = renameMap[rs2];
    instrPtr->rs2 = rs2;
  } else {
    // if the source operand has not been renamed then rename it
    if (rs1 != $zero) {
      Register renameRs2 = getNextRenameReg();
      renameMap[rs2] = renameRs2;
      instrPtr->rs2 = renameRs2;
    }
  }
  // We always rename the destination register
  if (rd != $zero) {
    Register renameRd = getNextRenameReg();
    renameMap[rd] = renameRd;
    instrPtr->rd = renameRd;
  }
}

void ScoreboardRenaming::renameIType(Instruction *instrPtr)
{
  // std::cout << "Renaming IType" << std::endl;
  Register rs1 = instrPtr->rs1;
  Register rd = instrPtr->rd;

  // check the rename map to see if the register has been renamed
  // if it has been renamed then use the renamed register
  // if it has not been renamed then use the original register
  if (renameMap[rs1] != $empty) {
    // if the source operand has already been renamed then use the renamed register
    rs1 = renameMap[rs1];
    instrPtr->rs1 = rs1;
  } else {
    // if the source operand has not been renamed then rename it
    if (rs1 != $zero) {
      Register renameRs1 = getNextRenameReg();
      renameMap[rs1] = renameRs1;
      instrPtr->rs1 = renameRs1;
    }
  }
  // due to poor code from before we need to handle SW as a separate case
  // the problems comes with SW doesnt have a destination register and therefore
  // should not be renamed automatically
  if (instrPtr->opcode == SW) {
    if (renameMap[rd] != $empty) {
    // if the source operand has already been renamed then use the renamed register
    rd = renameMap[rd];
    instrPtr->rd = rd;
    } else {
      // if the source operand has not been renamed then rename it
      if (rd != $zero) {
        Register renameRd = getNextRenameReg();
        renameMap[rd] = renameRd;
        instrPtr->rd = renameRd;
      }
    }
  } else {
    // We always rename the destination register
    if (rd != $zero) {
      Register renameRd = getNextRenameReg();
      renameMap[rd] = renameRd;
      instrPtr->rd = renameRd;
    }
  }
}

void ScoreboardRenaming::renameBType(Instruction *instrPtr)
{
  Register rs1 = instrPtr->rs1;
  Register rd = instrPtr->rd;

  // check the rename map to see if the register has been renamed
  // if it has been renamed then use the renamed register
  // if it has not been renamed then use the original register

  if (renameMap[rs1] != $empty) {
    // if the source operand has already been renamed then use the renamed register
    rs1 = renameMap[rs1];
    instrPtr->rs1 = rs1;
  } else {
    // if the source operand has not been renamed then rename it
    if (rs1 != $zero) {
      Register renameRs1 = getNextRenameReg();
      renameMap[rs1] = renameRs1;
      instrPtr->rs1 = renameRs1;
    }
  }
  if (renameMap[rd] != $empty) {
    // if the source operand has already been renamed then use the renamed register
    rd = renameMap[rd];
    instrPtr->rd = rd;
  } else {
    // if the source operand has not been renamed then rename it
    if (rd != $zero) {
      Register renameRd = getNextRenameReg();
      renameMap[rd] = renameRd;
      instrPtr->rd = renameRd;
    }
  }
}

Register ScoreboardRenaming::getNextRenameReg()
{
  // get the next free register

  Register nextReg = static_cast<Register>(regCount);
  regCount++;
  if (regCount > regResult.size() - 1) {
    std::cout << "Error: No more registers available" << std::endl;
    exit(1);
  }
  return nextReg;
}

void ScoreboardRenaming::logRenameMap()
{
  std::cout << "Rename Map" << std::endl;
  for (int i = 0; i < renameMap.size(); i++) {
    Register reg = static_cast<Register>(i);
    std::cout << "Register " << i << " renamed to " << renameMap[reg] << std::endl;
  }
}

void Scoreboard::logRenameMap()
{
  return;
}

// only need to set the destination register to be busy
void ScoreboardRenaming::setInstrBusy(Instruction *instrPtr)
{
  // std::cout << "Setting instr busy " << instrPtr->instrString << std::endl;
  if (instrPtr->type == BType || instrPtr->opcode == SW) {
    return;
  }
  setBusy(instrPtr->rd);
  setRegStatus(instrPtr->rd, instrPtr->instrNum);
}

///// End of ScoreboardRenaming Class /////


/// branch predictor stuff ///
// In branch prediction, since we only writeback when an instruction is at the top of ROB
// The ONLY thing we need to store is the renaming map
// The edge case where this is necessary is when we have a branch that is not taken
// ANd one of the past instructions has renamed a register
// we call this Storeboard in Decode so we can ensure that all registers ahead of the branch
// are renamed correctly

/////////////////////// we need to rebuild the rename table from the storeboard
//remove the instruction properly from the scoreboard
void Scoreboard::rebuildBoard(Instruction *instrPtr) {
  return;
}

void ScoreboardRenaming::rebuildBoard(Instruction *instrPtr) {
  restoreBoard(instrPtr);
  return;
}
// currently doesnt work and not used
void ScoreboardRenaming::remakeRenameTable() {
  // reset rename map
  for (int i = 0; i < 32; ++i) {
    renameMap[static_cast<Register>(i)] = Register::$empty;
  }
  regCount = 0;

  std::queue<QueueEntry> rob = processor->reorderBuffer->queue;

  // calculating regcount
  while (!rob.empty()) {
    QueueEntry entry = rob.front();
    rob.pop();
    if (entry.instr->type == BType || entry.instr->opcode == SW || entry.instr->opcode == JType || entry.instr->s < EXECUTE) {
      continue;
    }
    Register rd = entry.instr->rd;
    if (regCount < rd) {
      regCount = rd;
    }
    renameMap[destinationMap[entry.instr->instrNum]] = rd;
    break;
  }
  regCount = regCount + 1;
  return;
}

void Scoreboard::removeInstr(Instruction *instrPtr) {
  // std::cout << "Removing instr " << instrPtr->instrString << std::endl;
  int instrNumCheck = instrPtr->instrNum;
  for (std::array<int, 512>::size_type i = 0; i < 32; i++) {
    if (regStatus[i] == instrNumCheck) {
      // std::cout << "Releasing reg: " << i << std::endl;
      // std::cout << "RegStatus: " << regStatus[i] << std::endl;
      regStatus[i] = -1;
      board[i] = true;
    }
  }
  return;
}

void ScoreboardWithForwarding::removeInstr(Instruction *instrPtr) {
  // std::cout << "Removing instr " << instrPtr->instrString << std::endl;
  int instrNumCheck = instrPtr->instrNum;
  for (std::array<int, 512>::size_type i = 0; i < 32; i++) {
    if (regStatus[i] == instrNumCheck) {
      // std::cout << "Releasing reg: " << i << std::endl;
      // std::cout << "RegStatus: " << regStatus[i] << std::endl;
      regStatus[i] = -1;
      board[i] = true;
      if (i == instrPtr->rd && instrPtr->type != BType && instrPtr->opcode != SW && instrPtr->type != JType) {
          regResult[i] = std::make_pair(false, -1);
      }
    }
  }
  return;
}

void ScoreboardRenaming::removeInstr(Instruction *instrPtr) {
//  std::cout << "Removing instr " << instrPtr->instrString << std::endl;
  int instrNumCheck = instrPtr->instrNum;
  for (std::array<int, 512>::size_type i = 0; i < 32; i++) {
    if (regStatus[i] == instrNumCheck) {
      // std::cout << "Releasing reg: " << i << std::endl;
      // std::cout << "RegStatus: " << regStatus[i] << std::endl;
      regStatus[i] = -1;
      board[i] = true;
      if (i == instrPtr->rd && instrPtr->type != BType && instrPtr->opcode != SW && instrPtr->type != JType) {
          regResult[i] = std::make_pair(false, -1);
      }
    }
  }
  return;
}

OldBoard::OldBoard()
{
  // initialize the rename map
  for (int i = 0; i < renameMap.size(); i++) {
    Register reg = static_cast<Register>(i);
    renameMap[reg] = $empty;
  }
  regCount = 0;
}

void Scoreboard::storeBoard(Instruction *instrPtr)
{
  return;
}

void Scoreboard::restoreBoard(Instruction *instrPtr)
{
  return;
}
// JUICYNESS
void ScoreboardRenaming::storeBoard(Instruction *instrPtr)
{
  // store the rename map
  OldBoard* oldBoard = new OldBoard();
  oldBoard->renameMap = renameMap;
  oldBoard->regCount = regCount;

  // add to old board map
  oldBoardMap[instrPtr] = oldBoard;


}

void ScoreboardRenaming::restoreBoard(Instruction *instrPtr)
{
  // restore the rename map
  OldBoard* oldBoard = oldBoardMap[instrPtr];
  renameMap = oldBoard->renameMap;
  regCount = oldBoard->regCount;
}