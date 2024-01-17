#include "processor.h"

class Parser;

// create a processor and initialise the clock
Processor::Processor() {
  // initialise clock, program counter and 0 register
  this->clock = 0;
  this->pc = 0;
  this->registers[$zero] = 0;
  this->registers[$sp] = 0;
  return;
};

void Processor::getProgram(std::string filename) {
  std::cout << "Loading program" << std::endl;
  // load each instruction into instruction memory
  std::vector<std::string> program = parser->parseProgram(filename);
  for (std::string instruction : program) {
    loadInstructionIntoInstructionMemory(instruction);
  }
  std::cout << "Load instructions into instruction memory" << std::endl;
  // print instructions
  
  for (int i = 0; i < instructionMemory_size; i++) {
    std::cout << "Instruction: " << instructionMemory[i] << std::endl;
  }

  std::cout << "Load variables into data memory" << std::endl;
  // Iterate through the variableMap
  for (const auto& pair : variableMap) {
      const std::string& varName = pair.first;
      const std::array<int32_t, 784>& values = pair.second;
      // Add address to address map
      variableAddressMap["%lo("+varName+")"] = nextAddressCounter;

      // Load variable values into data memory
      if (image == 1) {
        for (int i = 0; i < values.size(); i++) {
            memory[nextAddressCounter + i] = values[i];
            // to mimic the memcpy function
            memory[nextAddressCounter + i + 6706] = values[i];
            // memory[nextAddressCounter + i] = values[i];
        }
        // Increment next address counter arrays of 5 ints are 20 bytes long
        nextAddressCounter += variableAddressMap.size() * 784;
      } else {
        for (int i = 0; i < 5; i++) {
            memory[nextAddressCounter + i] = values[i];
        }
        nextAddressCounter += variableAddressMap.size() * 5;
      }
  }


  return;
}

void Processor::loadInstructionIntoInstructionMemory(std::string instruction) {
    // if the instruction is a label, add it to the jump map so we know where to jump to in the program counter
    // std::cout << "Instruction: " << instruction << std::endl;
    if (instruction.back() == ':') {
        this->jumpMap[instruction.substr(0, instruction.size() - 1)] = instructionMemory_size;
        std::cout << "Adding label: " << instruction.substr(0, instruction.size() - 1) << " to jump map with value: " << instructionMemory_size << std::endl;
        return;
    }
    // std::cout << "Adding instruction: " << instruction << " to instruction memory at index: " << instructionMemory_size << std::endl;
    instructionMemory[instructionMemory_size] = instruction;
    // std::cout << "Instruction: " << instructionMemory[instructionMemory_size] << std::endl;
    instructionMemory_size++;
    return;
};

void Processor::buildProcessor() {
  std::cout << "Creating processor" << std::endl;

  Debugger *db = new Debugger();
  Printer *print = new Printer();
  Parser *p = new RVParser();
  Fetch *f = new Fetch();
  Decode *d = new Decode();
  Execute *e = new Execute();
  Mem *m = new Mem();
  Wb *w = new Wb();
  ReOrderBuffer *rob = new ReOrderBuffer();
  Queue *iq = new Queue();
  LoadStoreQueue *lsq = new LoadStoreQueue();

  ReservationStation *res;
  Scoreboard *sb;
  BranchPredictor *bp;
  // units which we may need to change the link depending on what we want to test  
  if (scoreboardMode == 0) {
    sb = new Scoreboard();
    res = new ReservationStationInOrder();
  } else if (scoreboardMode == 1) {
    sb = new ScoreboardWithForwarding();
    res = new ReservationStationInOrder();
  } else if (scoreboardMode == 2) {
    sb = new ScoreboardRenaming();
    if (OoO) {
      res = new ReservationStation;
    } else {
      res = new ReservationStationInOrder();
    }
  }

  if (branch_prediction_mode == 0) {
    bp = new StaticNotTakenBranchPredictor();
  } else if (branch_prediction_mode == 1) {
    bp = new StaticTakenBranchPredictor();
  } else if (branch_prediction_mode == 2) {
    bp = new DynamicBranchPredictorOneBit();
    if (debug) {
      std::cout << "Dynamic branch predictor One Bit" << std::endl;
    }
  } else if (branch_prediction_mode == 3) {
    bp = new DynamicBranchPredictorTwoBit();
  }

  std::cout << "Initialise components Finished" << std::endl;

  this->linkDebugger(db);
  this->linkParser(p);
  this->linkFetchUnit(f);
  this->linkDecodeUnit(d);
  this->linkExecuteUnit(e);
  this->linkMemUnit(m);
  this->linkWbUnit(w);
  this->linkScoreboard(sb);
  this->linkReservation(res, reservation);
  this->linkReorderBuffer(rob, 32);
  this->linkInstructionQueue(iq, 16);
  this->linkLoadStoreQueue(lsq, 8);
  this->linkBranchPredictor(bp);
  this->linkPrinter(print);

  std::cout << "Link components Finished" << std::endl;
}
void Processor::linkPrinter(Printer *p) {
  printer = p;
  printer->attatchPrinter(this);
  return;
}

void Processor::linkBranchPredictor(BranchPredictor *bp) {
  branchPredictor = bp;
  branchPredictor->attachBP(this);
  return;
}

void Processor::linkDebugger(Debugger *db) {
  debugger = db;
  debugger->attatchDebugProcessor(this);
  return;
}

void Processor::linkParser(Parser *p) {
  parser = p;
  parser->attachParser(this);
  return;
}

void Processor::linkFetchUnit(Fetch *fu) {
  funit = fu;
  funit->attachFetch(this);
  return;
}

void Processor::linkDecodeUnit(Decode *du) {
  dunit = du;
  dunit->attachDecode(this);
  return;
}

void Processor::linkExecuteUnit(Execute *eu) {
  eunit = eu;
  eunit->attachExecute(this);
  return;
}

void Processor::linkMemUnit(Mem *mu) {
  munit = mu;
  munit->attachMem(this);
  return;
}

void Processor::linkWbUnit(Wb *wu) {
  wunit = wu;
  wunit->attachWb(this);
  return;
}

void Processor::linkScoreboard(Scoreboard *sb) {
  scoreboard = sb;
  scoreboard->attachScoreboard(this);
  return;
}

void Processor::linkReservation(ReservationStation *res, size_t size) {
  reservationStation = res;
  reservationStation->attachQueue(this, size);
  return;
}

void Processor::linkReorderBuffer(ReOrderBuffer *rob, size_t size) {
  reorderBuffer = rob;
  reorderBuffer->attachQueue(this, size);
  return;
}

void Processor::linkInstructionQueue(Queue *q, size_t size) {
  instrQueue = q;
  instrQueue->attachQueue(this, size);
  return;
}

void Processor::linkLoadStoreQueue(Queue *q, size_t size) {
  loadStoreQueue = q;
  loadStoreQueue->attachQueue(this, size);
  return;
}

void Processor::debugProc() {
  debugger->debugProcessor();
  return;
}

void Processor::debugInstr(Instruction *instrPtr) {
  debugger->debugInstruction(instrPtr);
  return;
}

void Processor::debugPipe(std::vector<Instruction*> pipeline) {
  debugger->debugPipeline(pipeline);
  return;
}

void Processor::fetchFunc(Instruction *instrPtr) {
  funit->fetch(instrPtr);
  return;
}

void Processor::decodeFunc(Instruction *instrPtr) {
  dunit->decode(instrPtr);
  return;
}

void Processor::executeFunc(Instruction *instrPtr) {
  eunit->execute(instrPtr);
  return;
}

void Processor::memFunc(Instruction *instrPtr) {
  munit->mem(instrPtr);
  return;
}

void Processor::wbFunc(Instruction *instrPtr) {
  wunit->wb(instrPtr);
  return;
}

void Processor::runScalar() {
  std::cout << "RUNNING PROGRAM" << std::endl;
  // start executing instructions one at a time
  while (pc < instructionMemory_size) {
    Instruction *instrPtr = new Instruction();

    fetchFunc(instrPtr);
    clock++;
    // debugProc();
    while (instrPtr->s == 1) {
      decodeFunc(instrPtr);
      clock++;
      // debugProc();
    }
    while (instrPtr->s == EXECUTE) {
      executeFunc(instrPtr);
      clock++;
      // debugProc();
    }
    while (instrPtr->s == MEM) {
      memFunc(instrPtr);
      clock++;
      // debugProc();
    }
    while (instrPtr->s == WB) {
      wbFunc(instrPtr);
      clock++;
      // debugProc();
    }
    if (debug) {
      debugProc();
      std::cout << "Instruction: " << instrPtr->instrString << std::endl; 
      this->debugger->promptToContinue();
    }
  }
  debugProc();
  this->debugger->promptToContinue();
  if (image) {
        printer->printImages();
  }
  return;
}

void Processor::handleJump(Instruction *instrPtr) {
  // update the pc
  if (instrPtr->opcode == J) {
    pc = instrPtr->immediateValue;
  } else if (instrPtr->opcode == JR) {
    pc = registers[instrPtr->rs1];
  }
  // need to uncomment this for the pipeline to work
  if (superS) {
    flushPipe(instrPtr);
  }
  return;
}

void Processor::handleBranch(int didTakeBranch, Instruction *instrPtr) {
  // if branch is taken, update PC
  if (debug) {
    // branchPredictor->logBTB();
  }
  // std::cout << "Branch Predicted taken: " << branchPredictor->takenMap[instrPtr->instrNum] << std::endl;
  // std::cout << "Branch Actually: " << didTakeBranch << std::endl;
  if (didTakeBranch == branchPredictor->takenMap[instrPtr->instrNum]) {
    // Nothing to be done here we were correct
    bool correct = true;
    branchPredictor->update(instrPtr->instrString, instrPtr->pc_Fetched, correct);   
    return;
  }
  // we predicted not taken but branch was in fact taken
  else if (!branchPredictor->takenMap[instrPtr->instrNum]) {
    // update the pc
    pc = instrPtr->immediateValue;
    bool correct = false;
    branchPredictor->update(instrPtr->instrString, instrPtr->pc_Fetched, correct);
  } else {
    // we predicted taken but branch was not taken
    // update the pc
    pc = instrPtr->pc_Fetched + 1;
    bool correct = false;
    branchPredictor->update(instrPtr->instrString, instrPtr->pc_Fetched, correct);
  }
  // std::cout << "Branch mispredicted" << std::endl;
  // need to uncomment this for the pipeline to work
  if (superS) {
    flushPipe(instrPtr);
  }
  return;
}

// going to need some pretty hefty flushes in the future
void Processor::flushPipe(Instruction *instr) {
  // will also need to flush the instruction queue
  for (int i = 0; i < 5; i++) {
    // std::cout << "Flushing pipeline register: " << i << std::endl;
    // we need a better way to flush pipeline registers since the updated version
    if (pipelineRegs[i] != nullptr && pipelineRegs[i]->instrNum > instr->instrNum) {
      scoreboard->flushFlightInstr(pipelineRegs[i]);
      pipelineRegs[i] = nullptr;
    }
  }

  for (std::vector<Instruction*>::size_type i = 0; i < executeRegs.size(); i++) {
    // std::cout << "Flushing execute register: " << i << std::endl;
    if (executeRegs[i] != nullptr && executeRegs[i]->instrNum > instr->instrNum) {
      scoreboard->flushFlightInstr(executeRegs[i]);
      executeRegs[i] = nullptr;
    }
  }

  for (std::vector<Instruction*>::size_type i = 0; i < memRegs.size(); i++) {
    // std::cout << "Flushing mem register: " << i << std::endl;
    if (memRegs[i] != nullptr && memRegs[i]->instrNum > instr->instrNum) {
      scoreboard->flushFlightInstr(memRegs[i]);
      scoreboard->loadStore.erase(memRegs[i]->result);
      memRegs[i] = nullptr;
    }
  }

  // Flushing board
  scoreboard->flushFlightInstructions(instr);
  scoreboard->flushIssuedInstructions(instr);
  


  instrQueue->flushQueue(instr);
  loadStoreQueue->flushQueue(instr);
  // std::cout << "Flushing Instr finished" << std::endl;
  reservationStation->flushQueue(instr);
  // std::cout << "Flushing Reserve finished" << std::endl;
  reorderBuffer->flushQueueRO(instr);

  scoreboard->flushFlightInstr(instr);

  // std::cout << pc << std::endl;
  // rebuild our scoreboard
  scoreboard->rebuildBoard(instr);
  // Things to flush 
  // 1. pipeline registers

  return;
}


void Processor::fetchTick() {
  // Pipeline stage 1: Instruction fetch
  // if we dont have an instruction in the pipeline, fetch one
  // fetchFunc handles if there is no more to fetch
  if (!instrQueue -> full()) {
    if (pipelineRegs[0] == nullptr || pipelineRegs[0]->instrString == "") {
      pipelineRegs[0] = new Instruction();
      fetchFunc(pipelineRegs[0]);
    } 
    if (pipelineRegs[0] != nullptr && pipelineRegs[0]->instrString != "") {
      instrQueue->pushToQueue(pipelineRegs[0], clock);
      pipelineRegs[0] = nullptr;
    }
    if (pipelineRegs[0] != nullptr) {
      // std::cout << pipelineRegs[0]->instrString << std::endl;
    }
  }
}

void Processor::decodeTick() {
  // Pipeline stage 2: Instruction decode
  if (pipelineRegs[1] == nullptr) {
    pipelineRegs[1] = instrQueue->getNextInstr();
  }
  if (pipelineRegs[1] != nullptr) {
    if (pipelineRegs[1]->s == stage::DECODE) {
      decodeFunc(pipelineRegs[1]);
    }
    if (pipelineRegs[1]->s == stage::EXECUTE) {
      // send to reorder buffer and reservation station (if there is space else stall)
      if (!reservationStation->full() && !reorderBuffer->full()) {
        reorderBuffer->completeDecodeROBuff(pipelineRegs[1]);
        reservationStation->pushToQueue(pipelineRegs[1], clock);
        pipelineRegs[1] = nullptr;
      }
    }
  } 
}

void Processor::executeTick(int executeRegIndex) {
  // Pipeline stage 3: Execution
  // std::cout << "Execute stage for execute register: " << executeRegIndex << std::endl;
  // if we have an instruction in execute pipeline register and needs executing, execute it
  // if we are using general purpose ALUs use this one
  if (general) {
    if (executeRegs[executeRegIndex] == nullptr) {
      executeRegs[executeRegIndex] = reservationStation->getNextInstr();
    }
    if (executeRegs[executeRegIndex] != nullptr) {
      if (executeRegs[executeRegIndex]->s == stage::EXECUTE) {
        executeFunc(executeRegs[executeRegIndex]);
      }
      // std::cout << "Execute stage: " << executeRegs[executeRegIndex]->instrString << std::endl;
      // if we have an instruction in execute pipeline register and needs memory access, send it to memory
      // else Set it to null and let the reorderbuffer and writeback handle it
      if (executeRegs[executeRegIndex]->s == stage::MEM && !loadStoreQueue->full()) {
          // std::cout << "Sending to memory" << std::endl;
          reorderBuffer->completeExecuteROBuff(executeRegs[executeRegIndex]);
          loadStoreQueue->pushToQueue(executeRegs[executeRegIndex], clock);
          executeRegs[executeRegIndex] = nullptr;
      } else {
        reorderBuffer->completeExecuteROBuff(executeRegs[executeRegIndex]);
        executeRegs[executeRegIndex] = nullptr;
      }
    }
  } else {
    // std::cout << "Non-GeneralPurposeALU" << std::endl;
    // Case where we are not using general purpose ALU -- only works for 8-way
    if (executeRegs[executeRegIndex] == nullptr) {
      if (executeRegIndex == 0 || executeRegIndex == 1) {
        // std::cout << "Trying To get Branch Instruction" << std::endl;
        executeRegs[executeRegIndex] = reservationStation->getNextBranchInstr();
      } else if (executeRegIndex == 2 || executeRegIndex == 3) {
        executeRegs[executeRegIndex] = reservationStation->getNextLoadInstr();
      } else {
        executeRegs[executeRegIndex] = reservationStation->getNextArithmeticInstr();
      }
    }

    if (executeRegs[executeRegIndex] != nullptr) {
      // std::cout << "Register: " << executeRegIndex << " is free" << std::endl;
      if (executeRegs[executeRegIndex]->s == stage::EXECUTE) {
        executeFunc(executeRegs[executeRegIndex]);
      }
      // std::cout << "Execute stage: " << executeRegs[executeRegIndex]->instrString << std::endl;
      // if we have an instruction in execute pipeline register and needs memory access, send it to memory
      // else Set it to null and let the reorderbuffer and writeback handle it
      if (executeRegs[executeRegIndex]->s == stage::MEM && !loadStoreQueue->full()) {
          // std::cout << "Sending to memory" << std::endl;
          reorderBuffer->completeExecuteROBuff(executeRegs[executeRegIndex]);
          loadStoreQueue->pushToQueue(executeRegs[executeRegIndex], clock);
          executeRegs[executeRegIndex] = nullptr;
      } else {
        reorderBuffer->completeExecuteROBuff(executeRegs[executeRegIndex]);
        executeRegs[executeRegIndex] = nullptr;
      }
    }
  }
}

void Processor::memTick(int memRegIndex) {
  // std::cout << "MEM TICK" << std::endl;
  // Pipeline stage 4: Memory access
  // std::cout << "Currently used addresses: " << std::endl;
  // for (auto it = this->scoreboard->loadStore.begin(); it != this->scoreboard->loadStore.end(); ++it) {
  //   std::cout << *it << std::endl;
  // }
  if (memRegs[memRegIndex] == nullptr) {
    memRegs[memRegIndex] = loadStoreQueue->getNextInstr();
  }

  if (memRegs[memRegIndex] != nullptr) {
    if (memRegs[memRegIndex]->s == stage::MEM) {
      memFunc(memRegs[memRegIndex]);
    } 
    if (memRegs[memRegIndex]->s != stage::MEM) {
      // Memory stage finished, set instruction to null and let the reorderbuffer and writeback handle it
      reorderBuffer->completeMemROBuff(memRegs[memRegIndex]);
      memRegs[memRegIndex] = nullptr;
    }
  }
}

void Processor::wbTick() {
  // Pipeline stage 5: Write back
  // pull instruction from reorderbuffer if we are able to, then write back
  if (pipelineRegs[4] == nullptr) {
    pipelineRegs[4] = reorderBuffer->getNextInstr();
  }
  // if we sucessfully get an instruction from the reorder buffer, write back
  if (pipelineRegs[4] != nullptr) {
    wbFunc(pipelineRegs[4]);
    // std::cout << "Finished Writeback" << std::endl;
    reorderBuffer->completeWbROBuff(pipelineRegs[4]);
    pipelineRegs[4] = nullptr;
  } 
}

// to run pipelined, just run super scalar with 1s for width
void Processor::runSuperScalar(int FetchWidth, int DecodeWidth, int ExecuteWidth, int MemWidth, int WbWidth) {
  std::cout << "RUNNING PROGRAM" << std::endl;
  // Initialize pipeline registers
  this->pipelineRegs = std::vector<Instruction*>(5, nullptr);
  this->executeRegs = std::vector<Instruction*>(ExecuteWidth, nullptr);
  this->memRegs = std::vector<Instruction*>(MemWidth, nullptr);

  // start executing instructions one at a time
  while (true) {
    // backward loop to move the tail forward first // writeback doesnt need anything to change as we just look at the front of the reorder buffer
    for (int i = WbWidth - 1; i >= 0; i--) {
      // std::cout << "PIPELINE STAGE WRITEBACK" << std::endl;
      wbTick();
    }

    for (int i = MemWidth - 1; i >= 0; i--) {
      // std::cout << "PIPELINE STAGE MEMORY" << std::endl;
      memTick(i);
    }
    for (int i = ExecuteWidth - 1; i >= 0; i--) {
      // std::cout << "PIPELINE STAGE EXECUTE" << std::endl;
      executeTick(i);
    }
    for (int i = DecodeWidth - 1; i >= 0; i--) {
      // std::cout << "PIPELINE STAGE DECODE" << std::endl;
      decodeTick();
    }
    for (int i = FetchWidth - 1; i >= 0; i--) {
      // std::cout << "PIPELINE STAGE FETCH" << std::endl;
      fetchTick();
    }
    // std::cout << clock << std::endl;
   // since we are stalling the pipeline in each stage if it takes multiple cycles to execute or memory access, we need to increment the clock cycle here
    clock++;

    // scoreboard->logRenameMap();
    if (debug) {
      scoreboard->logFlightInstrs();
      this->debugger->debugProcessor();
      this->debugger->promptToContinue();
      // debugPipe(pipelineRegs);
      // instrQueue->printQueue();
      // loadStoreQueue->printQueue();
      scoreboard->logBoard();
      reorderBuffer->printQueue();
      // reservationStation->printQueue();
    }
    if (reservationStation->empty() && reorderBuffer->empty() && instrQueue->empty() && loadStoreQueue->empty()) {
      debugProc();
      this->debugger->promptToContinue();
      if (image) {
        printer->printImages();
      }
      break;
    }
   }
}

//// NOTES /////

// notes from TA
// internal pipeline in execute stage for clock cycle management

// OoO without renaming and speculation -- 60



// stage 4: 1 - 4
// from stage 5 --> being 70 

// experiment tips: quantifiable results IPC,