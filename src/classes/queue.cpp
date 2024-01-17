#include "queue.h"
#include <algorithm>
#include <limits>

void Queue::attachQueue(Processor *p, std::size_t size) {
  processor = p;
  queueSize = size;
}

void Queue::pushToQueue(Instruction *instr, int issuedCycle) {
  QueueEntry entry;
  entry.instr = instr;
  entry.issuedCycle = issuedCycle;
  queue.push(entry);
}

void Queue::popFromQueue() {
  queue.pop();
}

QueueEntry Queue::frontOfQueue() {
  return queue.front();
}

bool Queue::empty() {
  return queue.empty();
}

int Queue::size() {
  return queue.size();
}

bool Queue::full() {
  return queue.size() == queueSize;
}

Instruction* Queue::getNextInstr() {
  // std::cout << "Getting next instruction" << std::endl;
  if (empty()) {
    return nullptr;
  } else {
    QueueEntry entry = frontOfQueue();
    popFromQueue();
    return entry.instr;
  }
}

void Queue::removeInstr(Instruction *instr) {
  std::queue<QueueEntry> tempQueue;
  while (!queue.empty()) {
    QueueEntry entry = queue.front();
    queue.pop();
    if (entry.instr != instr) {
      tempQueue.push(entry);
    }
  }
  queue = tempQueue;
}

void Queue::flushQueue(Instruction *instr) {
  std::queue<QueueEntry> tempQueue;
    
    // Pop all elements from the original queue to the temporary queue until we find the given instruction
    while (!queue.empty()) {
        QueueEntry entry = queue.front();
        queue.pop();
        if (entry.instr->instrNum < instr->instrNum) {
          tempQueue.push(entry);
        }
    }
    
    // Clear the original queue
    queue = std::queue<QueueEntry>();
    
    // Move all elements back from the temporary queue to the original queue
    while (!tempQueue.empty()) {
        QueueEntry entry = tempQueue.front();
        tempQueue.pop();
        queue.push(entry);
    }
}

void Queue::printQueue() {
  std::cout << "--------------------Queue--------------------- " << std::endl;
  std::queue<QueueEntry> tempQueue = queue;
  while (!tempQueue.empty()) {
    QueueEntry entry = tempQueue.front();
    tempQueue.pop();
    std::cout << entry.instr->instrString << " " << entry.instr->instrNum << std::endl;
  }
  std::cout << "---------------------------------------------- " << std::endl;
}
//// begin Load store queue class instructions ////

Instruction* LoadStoreQueue::getNextInstr() {

  // create a map of all memory locations that are currently being used
  std::set<int> memoryLocations;

  std::queue<QueueEntry> tempQueue = processor->reorderBuffer->queue;
  while (!tempQueue.empty()) {
    QueueEntry entry = tempQueue.front();
    // std::cout << "Checking entry: " << entry.instr->instrNum << std::endl;
    if (entry.instr->opcode == LW || entry.instr->opcode == SW) {
      if (entry.instr->s < MEM) {
        // case where we have an unknown memory location so no one can access just in case
        // std::cout << "Unknown memory Location in LSQ" << std::endl;
        return nullptr;
      }
      // std::cout << "Checking memory location: " << entry.instr->result << std::endl;
      if (processor->scoreboard->loadStore.count(entry.instr->result) > 0) {
        // We have an instruction in memory that has a result not being used by an instruction ahead of it
        // std::cout << "Memory location in use" << std::endl;
      }
      if (entry.instr->s == MEM && memoryLocations.find(entry.instr->result) == memoryLocations.end() &&  !(processor->scoreboard->loadStore.count(entry.instr->result) > 0)) {
        // We have an instruction in memory that has a result not being used by an instruction ahead of it
        processor->scoreboard->loadStore.insert(entry.instr->result);
        removeInstr(entry.instr);
        return entry.instr;
      }
      if (entry.instr->opcode == SW || entry.instr->s == MEM) {
        // if an SW is in WB it still needs to reserve memory location, we dont care about LWs in WBs as they already have their value
        memoryLocations.insert(entry.instr->result);
      }
    }
    
    tempQueue.pop();
  }
  // std::cout << "No instruction found in LSQ" << std::endl;
  return nullptr;
}


/////// End of Queue class instructions
/// In order reservation station ////
Instruction* ReservationStationInOrder::getNextInstr() {
  // std::cout << "Getting next instruction" << std::endl;
  std::queue<QueueEntry> tempQueue = queue;
  while (!tempQueue.empty()) {
    QueueEntry entry = tempQueue.front();
    // std::cout << "Checking instruction: " << entry.instr->instrString << std::endl;
    if (processor->scoreboard->isInstrReady(entry.instr)) {
      // std::cout << "Instruction is ready" << std::endl;
      // remove the instruction from the queue sets the operands to be busy and return it
      processor->scoreboard->setInstrBusy(entry.instr);
      removeInstr(entry.instr);
      return entry.instr;
    }
    tempQueue.pop();
    // Uncomment this to make reservation station work like in order
    return nullptr;
  }
  // return nullptr;
}
Instruction* ReservationStationInOrder::getNextBranchInstr() {
  // std::cout << "Getting next instruction" << std::endl;
  std::queue<QueueEntry> tempQueue = queue;
  while (!tempQueue.empty()) {
    QueueEntry entry = tempQueue.front();
    if (entry.instr->type == BType) {
      // std::cout << "Checking instruction: " << entry.instr->instrString << std::endl;
      if (processor->scoreboard->isInstrReady(entry.instr)) {
        // std::cout << "Instruction is ready" << std::endl;
        // remove the instruction from the queue sets the operands to be busy and return it
        processor->scoreboard->setInstrBusy(entry.instr);
        removeInstr(entry.instr);
        return entry.instr;
      }
      // Uncomment this to make reservation station work like in order
      return nullptr;
    }
    tempQueue.pop();
  }
  return nullptr;
}

Instruction* ReservationStationInOrder::getNextLoadInstr() {
  // std::cout << "Getting next instruction" << std::endl;
  std::queue<QueueEntry> tempQueue = queue;
  while (!tempQueue.empty()) {
    QueueEntry entry = tempQueue.front();
    if (entry.instr->opcode == SW || entry.instr->opcode == LW) {
      // std::cout << "Checking instruction: " << entry.instr->instrString << std::endl;
      if (processor->scoreboard->isInstrReady(entry.instr)) {
        // std::cout << "Instruction is ready" << std::endl;
        // remove the instruction from the queue sets the operands to be busy and return it
        processor->scoreboard->setInstrBusy(entry.instr);
        removeInstr(entry.instr);
        return entry.instr;
      }
      // Uncomment this to make reservation station work like in order
      return nullptr;
    }
    tempQueue.pop();
  }
  return nullptr;
}

Instruction* ReservationStationInOrder::getNextArithmeticInstr() {
  // std::cout << "Getting next instruction" << std::endl;
  std::queue<QueueEntry> tempQueue = queue;
  while (!tempQueue.empty()) {
    QueueEntry entry = tempQueue.front();
    if (entry.instr->type != BType && entry.instr->opcode != SW && entry.instr->opcode != LW) {
      // std::cout << "Checking instruction: " << entry.instr->instrString << std::endl;
      if (processor->scoreboard->isInstrReady(entry.instr)) {
        // std::cout << "Instruction is ready" << std::endl;
        // remove the instruction from the queue sets the operands to be busy and return it
        processor->scoreboard->setInstrBusy(entry.instr);
        removeInstr(entry.instr);
        return entry.instr;
      }
      // Uncomment this to make reservation station work like in order
      return nullptr;
    }
    tempQueue.pop();
  }
  return nullptr;
}

/////// Begin of ReservationStation class instructions
Instruction* ReservationStation::getNextInstr() {
  // std::cout << "Getting next instruction" << std::endl;
  std::queue<QueueEntry> tempQueue = queue;
  while (!tempQueue.empty()) {
    QueueEntry entry = tempQueue.front();
    // std::cout << "Checking instruction: " << entry.instr->instrString << std::endl;
    if (processor->scoreboard->isInstrReady(entry.instr)) {
      // std::cout << "Instruction is ready" << std::endl;
      // remove the instruction from the queue sets the operands to be busy and return it
      processor->scoreboard->setInstrBusy(entry.instr);
      removeInstr(entry.instr);
      return entry.instr;
    }
    tempQueue.pop();
    // Uncomment this to make reservation station work like in order
    // return nullptr;
  }
  return nullptr;
}

Instruction* ReservationStation::getNextBranchInstr() {
  // std::cout << "Getting next instruction" << std::endl;
  std::queue<QueueEntry> tempQueue = queue;
  while (!tempQueue.empty()) {
    QueueEntry entry = tempQueue.front();
    // std::cout << "Checking instruction: " << entry.instr->instrString << std::endl;
    if (entry.instr->type == BType) {
      if (processor->scoreboard->isInstrReady(entry.instr)) {
        // std::cout << "Instruction is ready" << std::endl;
        // remove the instruction from the queue sets the operands to be busy and return it
        processor->scoreboard->setInstrBusy(entry.instr);
        removeInstr(entry.instr);
        return entry.instr;
      }
      // Uncomment this to make reservation station work like in order
      // return nullptr;
    }
    tempQueue.pop();
  }
  return nullptr;
}

Instruction* ReservationStation::getNextLoadInstr() {
  // std::cout << "Getting next instruction" << std::endl;
  std::queue<QueueEntry> tempQueue = queue;
  while (!tempQueue.empty()) {
    QueueEntry entry = tempQueue.front();
    if (entry.instr->opcode == SW || entry.instr->opcode == LW) {
      // std::cout << "Checking instruction: " << entry.instr->instrString << std::endl;
      if (processor->scoreboard->isInstrReady(entry.instr)) {
        // std::cout << "Instruction is ready" << std::endl;
        // remove the instruction from the queue sets the operands to be busy and return it
        processor->scoreboard->setInstrBusy(entry.instr);
        removeInstr(entry.instr);
        return entry.instr;
      }
      // Uncomment this to make reservation station work like in order
      // return nullptr;
    }
    tempQueue.pop();
  }
  return nullptr;
}

Instruction* ReservationStation::getNextArithmeticInstr() {
  // std::cout << "Getting next instruction" << std::endl;
  std::queue<QueueEntry> tempQueue = queue;
  while (!tempQueue.empty()) {
    QueueEntry entry = tempQueue.front();
    if (entry.instr->type != BType && entry.instr->opcode != SW && entry.instr->opcode != LW) {
      // std::cout << "Checking instruction: " << entry.instr->instrString << std::endl;
      if (processor->scoreboard->isInstrReady(entry.instr)) {
        // std::cout << "Instruction is ready" << std::endl;
        // remove the instruction from the queue sets the operands to be busy and return it
        processor->scoreboard->setInstrBusy(entry.instr);
        removeInstr(entry.instr);
        return entry.instr;
      }
      // Uncomment this to make reservation station work like in order
      // return nullptr;
    }
    tempQueue.pop();
  }
  return nullptr;
}

/////// Begin of ReOrderBuffer class instructions

Instruction* ReOrderBuffer::getNextInstr() {
  // std::cout << "Getting next instruction" << std::endl;
  if (queue.empty()) {
    // std::cout << "Queue is empty" << std::endl;
    return nullptr;
  }
  std::queue<QueueEntry> tempQueue = queue;
  QueueEntry entry = tempQueue.front();
  while(entry.instr->s == stage::DONE) {
    InstructionsCount++;
    removeInstr(entry.instr);
    tempQueue.pop();
    entry = tempQueue.front();
  }
  if (entry.instr->s == stage::WB) {
    // remove the instruction from the queue and return it
    removeInstr(entry.instr);
    InstructionsCount++;
    return entry.instr;
  }
  return nullptr;
}

void ReOrderBuffer::completeDecodeROBuff(Instruction *instr) {
  // push the instruction to the ReOrder Buffer
  pushToQueue(instr, processor->clock);
  // call the scoreboard complete stages which are different based on implementation
  processor->scoreboard->completeDecodeInstr(instr);
  return;
}

void ReOrderBuffer::completeExecuteROBuff(Instruction *instr) {
  processor->scoreboard->completeExecutedInstr(instr);
  return;
}

void ReOrderBuffer::completeMemROBuff(Instruction *instr) {
  processor->scoreboard->completeMemoryStageInstr(instr);
  return;
}

void ReOrderBuffer::completeWbROBuff(Instruction *instr) {
  // std::cout << "Completing writeback for instruction: " << instr->instrString << std::endl;
  processor->scoreboard->completeWritebackInstr(instr);
  return;
}



void ReOrderBuffer::flushQueueRO(Instruction *instr) {
    std::queue<QueueEntry> tempQueue;
    // Pop all elements from the original queue to the temporary queue until we find the given instruction
    while (!queue.empty()) {
        QueueEntry entry = queue.front();
        queue.pop();
        if (entry.instr->instrNum < instr->instrNum) {
          tempQueue.push(entry);
        }
        else {
          // Remove the instruction from the scoreboard
          if (entry.instr->opcode == SW && entry.instr->s >= MEM) {
            if (processor->scoreboard->loadStore.count(entry.instr->result) > 0) {
              processor->scoreboard->loadStore.erase(entry.instr->result);
            }
          }
          // this may be unneeded but we are being extra safe
          if (entry.instr->opcode == LW && entry.instr->s == MEM) {
            if (processor->scoreboard->loadStore.count(entry.instr->result) > 0) {
              processor->scoreboard->loadStore.erase(entry.instr->result);
            }
          }
          processor->scoreboard->removeInstr(entry.instr);
        }
    }
    
    // Clear the original queue
    queue = std::queue<QueueEntry>();
    
    // Move all elements back from the temporary queue to the original queue
    while (!tempQueue.empty()) {
        QueueEntry entry = tempQueue.front();
        tempQueue.pop();
        queue.push(entry);
    }
}
