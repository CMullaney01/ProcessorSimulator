#ifndef SCOREBOARD_H
#define SCOREBOARD_H


#include "isa.h"
#include "instruction.h"
#include "processor.h"
#include <vector>
#include <set>
#include <unordered_map>


// create an array of bools to represent the scoreboard This should be 32 long as there are 32 registers
// Additionally in architecture this is represented by a valid bit within the register file
// If we have implemented register renaming this will be done on the Physical Register File (PRF)
// and the scoreboard will be implemented on the Physical Register File (PRF)
// else it will be implemented on the Architecture Register File (ARF) -- We currently implement this on the ARF

// In RISC-V Architectures the Result forwarder is also coupled with the scoreboard
class Processor;
class OldBoard {
  public:
  std::unordered_map<Register, Register> renameMap;
  int regCount;
  OldBoard();
};

class Scoreboard
{ 
  public:
  Processor *processor;
  void attachScoreboard(Processor *p);
  // Note to self, maybe we should have our issued and completed instructions in the processor.h
  // leave for now
  std::array <bool, 65536> board;
  // who holds the register
  std::array<int, 65536> regStatus;
  std::vector<Instruction*> issuedInstructions;
  std::vector<Instruction*> completedInstructions;
  std::vector<Instruction*> inFlightInstructions;
  
  // A map which maps an instruction to an old board
  Scoreboard();
  // virtual funcs
  virtual void completeDecodeInstr(Instruction *instrPtr);
  virtual void completeExecutedInstr(Instruction *instrPtr);
  virtual void completeMemoryStageInstr(Instruction *instrPtr);
  virtual void completeWritebackInstr(Instruction *instrPtr);
  virtual int getOperandResult(Register reg);
  virtual bool checkAvailable(Register reg, int instrNum);
  virtual void setBusy(int reg);
  virtual void setRegStatus(int reg, int instrNum);
  virtual void logRenameMap();
  virtual void setInstrBusy(Instruction *instrPtr);

  //Branch thingy stuff;
  std::set<int> loadStore;
  virtual void removeInstr(Instruction *instrPtr);
  virtual void rebuildBoard(Instruction *instrPtr);
  std::unordered_map<Instruction*, OldBoard*> oldBoardMap;
  virtual void storeBoard(Instruction *instrPtr);
  virtual void restoreBoard(Instruction *instrPtr);
  // this function rebuilds the board based off what is still in the reorder buffer

  // non-virtual funcs
  bool isInstrReady(Instruction *instrPtr);
  void completeFetchInstr(Instruction *instrPtr);
  void setValid(int reg);
  virtual void logBoard();
  void addIssuedInstr(Instruction *instrPtr);
  bool isInstrComplete(int instrNum);
  int getInstrResult(Instruction *instrPtr);
  void logFlightInstrs();
  void flushFlightInstr(Instruction *instrPtr);
  void flushFlightInstructions(Instruction *instrPtr);
  void flushIssuedInstructions(Instruction *instrPtr);
  int lowestFlightInstrToWriteBack();
  // Note to self: since the scoreboard doesnt use the processor
  // we dont need to link both ways and no attatchScoreboard function is needed
};

class ScoreboardWithForwarding : public Scoreboard
{ 
  public:
  std::array<std::pair<bool, int>, 32> regResult;

  ScoreboardWithForwarding();
  void completeExecutedInstr(Instruction *instrPtr) override;
  void completeMemoryStageInstr(Instruction *instrPtr) override;
  void completeWritebackInstr(Instruction *instrPtr) override;
  int getOperandResult(Register reg) override;

  //new Instructions
  void logBoard() override;
  void removeInstr(Instruction *instrPtr) override;
};

class ScoreboardRenaming : public Scoreboard {
  public:
  int regCount;
  std::unordered_map<int, Register> destinationMap; // maps instrNum to destination register for writeback
  std::array<std::pair<bool, int>, 65536> regResult;
  std::unordered_map<Register, Register> renameMap;
  ScoreboardRenaming();
  void completeDecodeInstr(Instruction *instrPtr) override;
  void completeExecutedInstr(Instruction *instrPtr) override;
  void completeMemoryStageInstr(Instruction *instrPtr) override;
  void completeWritebackInstr(Instruction *instrPtr) override;
  int getOperandResult(Register reg) override;
  void logRenameMap() override;
  void setInstrBusy(Instruction *instrPtr) override;
  void logBoard() override;
  void removeInstr(Instruction *instrPtr) override;
  void rebuildBoard(Instruction *instrPtr) override;
  void storeBoard(Instruction *instrPtr) override;
  void restoreBoard(Instruction *instrPtr) override;
  
  void renameRType(Instruction *instrPtr);
  void renameIType(Instruction *instrPtr);
  void renameBType(Instruction *instrPtr);
  void remakeRenameTable();

  Register getNextRenameReg();
};

#endif