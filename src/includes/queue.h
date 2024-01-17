#ifndef QUEUE_DEFINED_
#define QUEUE_DEFINED_

#include "instruction.h"
#include "processor.h"
#include <queue>
#include <utility>

class Processor;

class QueueEntry {
  public:
    Instruction *instr;
    int issuedCycle;
};

class Queue {
  public:
    Processor *processor;
    std::size_t queueSize;
    void attachQueue(Processor *p, std::size_t size);
    std::queue<QueueEntry> queue;
    void popFromQueue();
    QueueEntry frontOfQueue();
    bool empty();
    bool full();
    int size();
    void removeInstr(Instruction *instr);
    void printQueue();
    // returns the next instruction that is ready to execute
    virtual Instruction* getNextInstr();
    virtual void flushQueue(Instruction *instr);
    virtual void pushToQueue(Instruction *instr, int issuedCycle);
};

class LoadStoreQueue : public Queue {
  public:
    Instruction* getNextInstr() override;
};


class ReservationStation : public Queue{
  public:
    Instruction* getNextInstr() override;
    virtual Instruction* getNextBranchInstr();
    virtual Instruction* getNextLoadInstr();
    virtual Instruction* getNextArithmeticInstr();

};

class ReservationStationInOrder : public ReservationStation {
  public:
    Instruction* getNextInstr() override;
    Instruction* getNextBranchInstr() override;
    Instruction* getNextLoadInstr() override;
    Instruction* getNextArithmeticInstr() override;
};

class ReOrderBuffer : public Queue {
  public:
    int InstructionsCount = 0;
    Instruction* getNextInstr() override;
    void flushQueueRO(Instruction *instr);

    //override pushToQueue 
    void completeDecodeROBuff(Instruction *instr);
    void completeExecuteROBuff(Instruction *instr);
    void completeMemROBuff(Instruction *instr);
    void completeWbROBuff(Instruction *instr);
};



#endif