#ifndef _PROCESSOR_INCLUDED_
#define _PROCESSOR_INCLUDED_
#include <map>

#include "debug.h"
#include "parser.h"
#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "mem.h"
#include "wb.h"
#include "scoreboard.h"
#include "instruction.h"
#include "queue.h"
#include "branch.h"
#include "printer.h"

class Parser;
class Fetch;
class Decode;
class Execute;
class Mem;
class Wb;
class Scoreboard;
class Debugger;
class Queue;
class ReOrderBuffer;
class ReservationStation;
class BranchPredictor;
class Printer;

class Processor
{
  public:
    // debugger
    Debugger *debugger;
    void linkDebugger(Debugger *d);
    // printer
    Printer *printer;
    void linkPrinter(Printer *p);
    // processor components
    Parser *parser;
    Fetch *funit;
    Decode *dunit;
    Execute *eunit;
    Mem *munit;
    Wb *wunit;
    Scoreboard *scoreboard;
    ReservationStation *reservationStation;
    ReOrderBuffer *reorderBuffer;
    Queue *instrQueue;
    Queue *loadStoreQueue;
    BranchPredictor *branchPredictor;
    
    //debugger
    void debugProc();
    void debugInstr(Instruction *instrPtr);
    void debugPipe(std::vector<Instruction*> pipeline);
    // Scalar unPipelined Instructions
    void fetchFunc(Instruction *instrPtr);
    void decodeFunc(Instruction *instrPtr);
    void executeFunc(Instruction *instrPtr);
    void memFunc(Instruction *instrPtr);
    void wbFunc(Instruction *instrPtr);
    
    // create the processor
    void buildProcessor(); 
    Processor();
    void linkParser(Parser *p);
    void linkFetchUnit(Fetch *fu);
    void linkDecodeUnit(Decode *du);
    void linkExecuteUnit(Execute *eu);
    void linkMemUnit(Mem *mu);
    void linkWbUnit(Wb *wu);
    void linkScoreboard(Scoreboard *s);
    void linkReorderBuffer(ReOrderBuffer *rob, size_t size);
    void linkReservation(ReservationStation *res, size_t size);
    void linkInstructionQueue(Queue *iq, size_t size);
    void linkLoadStoreQueue(Queue *lsq, size_t size);
    void linkBranchPredictor(BranchPredictor *bp);

    // make processor
    // registers and program counter
    // initialise array for 32 general purpose registers 
    // 1 for the program counter register and 1 $zero register and set all of them to 0
    // std::array <int32_t, 33> registers;
    // we have the 33 registers from before plus the 95 regisister representing the Physical register file
    std::array <int32_t, 65536> registers;

    // int32_t registers[33];
    int clock = 0;
    int instructionCount = 0;
    int pc = 0;

    //initialise Data memory and set everything to 0
    int32_t memory[8192] = {};
    int memory_size = 8192;

    //initialise Instruction memory
    std::string instructionMemory[1024];
    int32_t instructionMemory_size = 0;

    // initiate a jump maps so that our branches and jump instructions can jump to the correct location
    std::map<std::string, int> jumpMap;
    // initialise variable map
    std::map<std::string, std::array <int32_t, 784>> variableMap;
    std::map<std::string, int> variableAddressMap;
    int nextAddressCounter = 0;
    // pipe stuff
    std::vector<Instruction*> pipelineRegs;
    std::vector<Instruction*> executeRegs;
    std::vector<Instruction*> memRegs;


    //Processor Functions
    void getProgram(std::string filename);
    void loadInstructionIntoInstructionMemory(std::string instruction);
    void fetchTick();
    void decodeTick();
    void executeTick(int executeRegIndex);
    void memTick(int memRegIndex);
    void wbTick();
    void runScalar();
    void flushPipe(Instruction *instr);
    void handleBranch(int didTakeBranch, Instruction *instr);
    void handleJump(Instruction *instr);
    void runSuperScalar(int FetchWidth, int DecodeWidth, int ExecuteWidth, int MemWidth, int WbWidth);


    /// Command line stuff
    int debug = 0;
    int superS = 0;
    // Default values for command line arguments
    int scoreboardMode = 0;
    int OoO = 0;
    int branch_prediction_mode = 0;
    int image = 0;
    int general = 0;
    int width = 0;
    int reservation = 0;
};

#endif