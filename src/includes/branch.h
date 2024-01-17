#ifndef BRANCH_H
#define BRANCH_H

#include "instruction.h"
#include "processor.h"
#include <cstdint>
#include <unordered_map>


class Processor;

// Base Branch prediction
class BranchPredictor {
  public:
    std::map<std::string, std::pair<int, int>> BTargetbuffer;
    Processor *processor;
    void attachBP(Processor *p);
    BranchPredictor();
    // a map of integers to booleans for each branch allows us to tell what was predicted for a specific instruction
    std::map<int, bool> takenMap;
    virtual int predict(std::string instrString, int pc, int instrNum);
    virtual void update(std::string instrString, int pc, bool correct);
    virtual void insert(std::string instrString, int pc, int target);
    virtual void logBTB();
    int totalNumPredictions;
    int correctPredictions;
};

// Static Taken Branch prediction
class StaticTakenBranchPredictor : public BranchPredictor {
  public:
    int predict(std::string instrString, int pc, int instrNum) override;
};

// Static Not Taken Branch prediction
class StaticNotTakenBranchPredictor : public BranchPredictor {
  public:
    int predict(std::string instrString, int pc, int instrNum) override;
};

// Dynamic Branch prediction
class DynamicBranchPredictorOneBit : public BranchPredictor {
public:
    enum class BranchState { D_TAKE = 1, DNT_TAKE = -1 };
    std::map<std::string, BranchState> find_Prediction;
    BranchState state(BranchState current, bool result);
    void insert(std::string instrString, int pc, int target);
    int predict(std::string instrString, int pc, int instrNum);
    void update(std::string instrString, int pc, bool correct);
};

class DynamicBranchPredictorTwoBit : public BranchPredictor {
public:
    enum class BranchState { S_TAKE = 2, W_TAKE = 1, SD_TAKE = -1, WD_TAKE = -2 };
    std::map<std::string, BranchState> find_Prediction;
    BranchState state(BranchState current, bool result);
    void insert(std::string instrString, int pc, int target);
    int predict(std::string instrString, int pc, int instrNum);
    void update(std::string instrString, int pc, bool correct);
};

#endif