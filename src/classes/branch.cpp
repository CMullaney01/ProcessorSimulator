#include "branch.h"


void BranchPredictor::attachBP(Processor *p) {
  processor = p;
}


void BranchPredictor::insert(std::string instrString, int pc, int target) {
    std::string btb_str = instrString + "-" + std::to_string(pc);
    if (BTargetbuffer.find(btb_str) == BTargetbuffer.end())
    { 
        std::pair<int, int> targetPred = std::make_pair(target, pc + 1);
        BTargetbuffer.insert(std::pair<std::string, std::pair<int, int>>(btb_str, targetPred));
    }
}

BranchPredictor::BranchPredictor() {
}

void BranchPredictor::update(std::string instrString, int pc, bool correct) {
  totalNumPredictions++;
  correct ? correctPredictions++ : correctPredictions += 0;
  return;
}

int BranchPredictor::predict(std::string instrString, int pc, int instrNum) {
  return -1;
}

// static not taken
int StaticNotTakenBranchPredictor::predict(std::string instrString, int pc, int instrNum) {
  // this is forwards branch prediction so we just return the pc
  takenMap[instrNum] = false;
  return pc + 1;
}

// static taken
int StaticTakenBranchPredictor::predict(std::string instrString, int pc, int instrNum) {
  // std::cout << "Predicting Instruction: " << instr << std::endl;
  std::string btb_str = instrString + "-" + std::to_string(pc);
  for (auto it = BTargetbuffer.begin(); it != BTargetbuffer.end(); ++it) {
    // If the instruction is in the BTargetbuffer, then return the target
    // address.
    if (it->first == instrString) {
      takenMap[instrNum] = true;
      // std::cout << "Predicting Taken" << std::endl;
      return it->second.first;
    }
  }
  // If the instruction is not in the buffer, then predict the next
  // instruction.
  // std::cout << "Predicting Not Taken" << std::endl;
  takenMap[instrNum] = false;
  return pc + 1;
}


// Dynamic 1 bit predictor
void DynamicBranchPredictorOneBit::insert(std::string instrString, int pc, int target)
{   
    std::string btb_str = instrString + "-" + std::to_string(pc);
    if (BTargetbuffer.find(btb_str) == BTargetbuffer.end())
    {
        std::pair<int, int> targetPred = std::make_pair(target, pc + 1);
        BTargetbuffer.insert(std::pair<std::string, std::pair<int, int>>(btb_str, targetPred));
        find_Prediction.insert(std::pair<std::string, BranchState>(btb_str, DynamicBranchPredictorOneBit::BranchState::DNT_TAKE));
    }
};

int DynamicBranchPredictorOneBit::predict(std::string instrString, int pc, int instrNum)
{ 
  // std::cout <<"FAILED AFTER HERE PREDICT START" << std::endl;
  std::string btb_str = instrString + "-" + std::to_string(pc);
    for (auto it = BTargetbuffer.begin(); it != BTargetbuffer.end(); ++it) {
      if (it->first == instrString) {
        int target = it->second.first;
        // std::cout << "FOUND IN BTargetbuffer" << std::endl;
        BranchState state = find_Prediction.at(instrString);
        // std::cout << "Predicting Not Taken" << std::endl;
        if (state == DynamicBranchPredictorOneBit::BranchState::D_TAKE)
        {   
            takenMap[instrNum] = true;
            return target;
        }
        takenMap[instrNum] = false;
        return pc + 1;
      }
    }
    // std::cout << "Predicting Not Taken" << std::endl;
    takenMap[instrNum] = false;
    return pc + 1;

}

void DynamicBranchPredictorOneBit::update(std::string instrString, int pc, bool correct)
{    
    totalNumPredictions++;
    correct ? correctPredictions++ : correctPredictions += 0;
    std::string btb_str = instrString + "-" + std::to_string(pc);


    for (auto it = find_Prediction.begin(); it != find_Prediction.end(); ++it) {
      if (it->first == btb_str) {
        BranchState updatedState = state(it->second, correct);
        it->second = updatedState;
        return;
      }
    }
    return;
    
};

DynamicBranchPredictorOneBit::BranchState DynamicBranchPredictorOneBit::state(BranchState current, bool result)
{
    switch(current) {
        case DynamicBranchPredictorOneBit::BranchState::D_TAKE:
            return result ? DynamicBranchPredictorOneBit::BranchState::D_TAKE : DynamicBranchPredictorOneBit::BranchState::DNT_TAKE;
        case DynamicBranchPredictorOneBit::BranchState::DNT_TAKE:
            return result ? DynamicBranchPredictorOneBit::BranchState::DNT_TAKE : DynamicBranchPredictorOneBit::BranchState::D_TAKE;
    }
    return DynamicBranchPredictorOneBit::BranchState::DNT_TAKE;
}



void DynamicBranchPredictorTwoBit::insert(std::string instrString, int pc, int target)
{
    std::string btb_str = instrString + "-" + std::to_string(pc);
    if (BTargetbuffer.find(btb_str) == BTargetbuffer.end())
    {
        std::pair<int, int> targetPred = std::make_pair(target, pc + 1);
        BTargetbuffer.insert(std::pair<std::string, std::pair<int, int>>(btb_str, targetPred));
        find_Prediction.insert(std::pair<std::string, BranchState>(btb_str, DynamicBranchPredictorTwoBit::BranchState::WD_TAKE));
    }
};

int DynamicBranchPredictorTwoBit::predict(std::string instrString, int pc, int instrNum)
{ 
  // std::cout <<"FAILED AFTER HERE PREDICT START" << std::endl;
  std::string btb_str = instrString + "-" + std::to_string(pc);
    for (auto it = BTargetbuffer.begin(); it != BTargetbuffer.end(); ++it) {
      if (it->first == instrString) {
        int target = it->second.first;
        // std::cout << "FOUND IN BTargetbuffer" << std::endl;
        BranchState state = find_Prediction.at(instrString);
        // std::cout << "Predicting Not Taken" << std::endl;
        if (state == DynamicBranchPredictorTwoBit::BranchState::S_TAKE || state == DynamicBranchPredictorTwoBit::BranchState::W_TAKE)
        {   
            takenMap[instrNum] = true;
            return target;
        }
        takenMap[instrNum] = false;
        return pc + 1;
      }
    }
    // std::cout << "Predicting Not Taken" << std::endl;
    takenMap[instrNum] = false;
    return pc + 1;

}

void DynamicBranchPredictorTwoBit::update(std::string instrString, int pc, bool correct)
{    
    totalNumPredictions++;
    correct ? correctPredictions++ : correctPredictions += 0;
    std::string btb_str = instrString + "-" + std::to_string(pc);
    for (auto it = find_Prediction.begin(); it != find_Prediction.end(); ++it) {
      if (it->first == btb_str) {
        BranchState updatedState = state(it->second, correct);
        it->second = updatedState;
        return;
      }
    }
    return;
};

DynamicBranchPredictorTwoBit::BranchState DynamicBranchPredictorTwoBit::state(BranchState current, bool result)
{
    switch(current) {
        case DynamicBranchPredictorTwoBit::BranchState::S_TAKE:
            return result ? DynamicBranchPredictorTwoBit::BranchState::S_TAKE : DynamicBranchPredictorTwoBit::BranchState::W_TAKE;
        case DynamicBranchPredictorTwoBit::BranchState::W_TAKE:
            return result ? DynamicBranchPredictorTwoBit::BranchState::S_TAKE : DynamicBranchPredictorTwoBit::BranchState::SD_TAKE;
        case DynamicBranchPredictorTwoBit::BranchState::WD_TAKE:
            return result ? DynamicBranchPredictorTwoBit::BranchState::SD_TAKE : DynamicBranchPredictorTwoBit::BranchState::S_TAKE;
        case DynamicBranchPredictorTwoBit::BranchState::SD_TAKE:
            return result ? DynamicBranchPredictorTwoBit::BranchState::SD_TAKE : DynamicBranchPredictorTwoBit::BranchState::WD_TAKE;
    }
    return DynamicBranchPredictorTwoBit::BranchState::WD_TAKE;
}




////////////////////// Logging Functions //////////////////////

void BranchPredictor::logBTB() {
  std::cout << "BTB: " << std::endl;
  for (auto it = BTargetbuffer.begin(); it != BTargetbuffer.end(); ++it) {
    std::cout << it->first << " => Target " << it->second.first << " => Prediction " << it->second.second << '\n';
  }
  std::cout << "Jump Map: " << std::endl;
  for (auto it = processor->jumpMap.begin(); it != processor->jumpMap.end(); ++it) {
    std::cout << it->first << " => " << it->second << '\n';
  }
  return;
}


