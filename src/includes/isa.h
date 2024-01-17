#ifndef __ISA_DEFINED__
#define __ISA_DEFINED__

#include <map>
#include <string>

// Rtype instructions -- register-register operations
// field: opcode, source register 1, source register 2, destination register, shift amount e.g. ADD

// Itype instructions -- register-immediate operations
// field: opcode, source register, destination register, immediate value e.g. LOAD register and an offset and loads the data from memory into the register



// Btype instructions -- branch operations
// field: opcode, source register 1, source register 2, address e.g. BEQ
// note : J-type and B-type are similar but J-type is used for unconditional jumps and B-type is used for conditional jumps unsure if we need to separate these
// RISC-V also has U-type instructions but will be ignored for now as U type deals with Unsigned Integers 
enum InstructionType {
    RType, IType, BType, JType,
};


// Registers:

// 32 general-purpose integer registers, each 32 bits wide: $x0 to $x31
// $x0 (also known as $zero) is hardwired to 0 and cannot be modified
// $x1 (also known as $ra) is the return address register (used by jal and jalr instructions)
// $x2 (also known as $sp) is the stack pointer (used by the stack-related instructions e.g. loads and stores)
// since this processor will only run a single program at a time we can assume the stack pointer will always be at the top of the stack and therefore unneeded
// unless we would like to do more complicated instructions -- we have now chosen to implement this as a feature so we can use the stack pointer needed in RV32I compiled code
// $x3 to $x8 are the argument registers for function calls
// $x9 to $x11 are the temporary registers, which may be used by the caller or callee without being preserved
// $x12 to $x17 are the saved registers, which must be preserved across function calls
// $x18 to $x27 are the temporary registers, which may be used by the callee without being preserved

// Though $x28 to $x31 are the reserved registers, which may be used by privileged software we do not intent to implement privelidged software
// therefore we will not implement these registers and will instead use $x28 to $x31 as temporary registers

// additionally we will not implement the gp register as it is used for global pointer and is not used in our implementation (I think)
// Define Registers based on RISC-V 32-bit Integer Instruction Set  

//NOTE: The RA register is a bit redundant at the moment as we are currently writing RISC-V code only has a single function
// 512 registers in the physical register file
enum Register: int {
    $zero, $ra, $sp, $r3, $r4, $r5, $r6, $r7, 
    $r8, $r9, $r10, $r11, $r12, $r13, $r14, $r15, 
    $r16, $r17, $r18, $r19, $r20, $r21, $r22, $r23, 
    $r24, $r25, $r26, $r27, $r28, $r29, $r30, $r31,
    // 95 registers in the physical register file
    $prf0, $prf1, $prf2, $prf3, $prf4, $prf5, $prf6, $prf7, 
    $prf8, $prf9, $prf10, $prf11, $prf12, $prf13, $prf14, $prf15, 
    $prf16, $prf17, $prf18, $prf19, $prf20, $prf21, $prf22, $prf23, 
    $prf24, $prf25, $prf26, $prf27, $prf28, $prf29, $prf30, $prf31, 
    $prf32, $prf33, $prf34, $prf35, $prf36, $prf37, $prf38, $prf39, 
    $prf40, $prf41, $prf42, $prf43, $prf44, $prf45, $prf46, $prf47, 
    $prf48, $prf49, $prf50, $prf51, $prf52, $prf53, $prf54, $prf55, 
    $prf56, $prf57, $prf58, $prf59, $prf60, $prf61, $prf62, $prf63, 
    $prf64, $prf65, $prf66, $prf67, $prf68, $prf69, $prf70, $prf71, 
    $prf72, $prf73, $prf74, $prf75, $prf76, $prf77, $prf78, $prf79, 
    $prf80, $prf81, $prf82, $prf83, $prf84, $prf85, $prf86, $prf87, 
    $prf88, $prf89, $prf90, $prf91, $prf92, $prf93, $prf94, $prf95,
    $prf96, $prf97, $prf98, $prf99, $prf100, $prf101, $prf102, $prf103,
    $prf104, $prf105, $prf106, $prf107, $prf108, $prf109, $prf110, $prf111,
    $prf112, $prf113, $prf114, $prf115, $prf116, $prf117, $prf118, $prf119,
    $prf120, $prf121, $prf122, $prf123, $prf124, $prf125, $prf126, $prf127,
    $prf128, $prf129, $prf130, $prf131, $prf132, $prf133, $prf134, $prf135,
    $prf136, $prf137, $prf138, $prf139, $prf140, $prf141, $prf142, $prf143,
    $prf144, $prf145, $prf146, $prf147, $prf148, $prf149, $prf150, $prf151,
    $prf152, $prf153, $prf154, $prf155, $prf156, $prf157, $prf158, $prf159,
    $prf160, $prf161, $prf162, $prf163, $prf164, $prf165, $prf166, $prf167,
    $prf168, $prf169, $prf170, $prf171, $prf172, $prf173, $prf174, $prf175,
    $prf176, $prf177, $prf178, $prf179, $prf180, $prf181, $prf182, $prf183,
    $prf184, $prf185, $prf186, $prf187, $prf188, $prf189, $prf190, $prf191,
    $prf192, $prf193, $prf194, $prf195, $prf196, $prf197, $prf198, $prf199,
    $prf200, $prf201, $prf202, $prf203, $prf204, $prf205, $prf206, $prf207,
    $prf208, $prf209, $prf210, $prf211, $prf212, $prf213, $prf214, $prf215,
    $prf216, $prf217, $prf218, $prf219, $prf220, $prf221, $prf222, $prf223,
    $prf224, $prf225, $prf226, $prf227, $prf228, $prf229, $prf230, $prf231,
    $prf232, $prf233, $prf234, $prf235, $prf236, $prf237, $prf238, $prf239,
    $prf240, $prf241, $prf242, $prf243, $prf244, $prf245, $prf246, $prf247,
    $prf248, $prf249, $prf250, $prf251, $prf252, $prf253, $prf254, $prf255,
    $prf256, $prf257, $prf258, $prf259, $prf260, $prf261, $prf262, $prf263,
    $prf264, $prf265, $prf266, $prf267, $prf268, $prf269, $prf270, $prf271,
    $prf272, $prf273, $prf274, $prf275, $prf276, $prf277, $prf278, $prf279,
    $prf280, $prf281, $prf282, $prf283, $prf284, $prf285, $prf286, $prf287,
    $prf288, $prf289, $prf290, $prf291, $prf292, $prf293, $prf294, $prf295,
    $prf296, $prf297, $prf298, $prf299, $prf300, $prf301, $prf302, $prf303,
    $prf304, $prf305, $prf306, $prf307, $prf308, $prf309, $prf310, $prf311,
    $prf312, $prf313, $prf314, $prf315, $prf316, $prf317, $prf318, $prf319,
    $prf320, $prf321, $prf322, $prf323, $prf324, $prf325, $prf326, $prf327,
    $prf328, $prf329, $prf330, $prf331, $prf332, $prf333, $prf334, $prf335,
    $prf336, $prf337, $prf338, $prf339, $prf340, $prf341, $prf342, $prf343,
    $prf344, $prf345, $prf346, $prf347, $prf348, $prf349, $prf350, $prf351,
    $prf352, $prf353, $prf354, $prf355, $prf356, $prf357, $prf358, $prf359,
    $prf360, $prf361, $prf362, $prf363, $prf364, $prf365, $prf366, $prf367,
    $prf368, $prf369, $prf370, $prf371, $prf372, $prf373, $prf374, $prf375,
    $prf376, $prf377, $prf378, $prf379, $prf380, $prf381, $prf382, $prf383,
    $prf384, $prf385, $prf386, $prf387, $prf388, $prf389, $prf390, $prf391,
    $prf392, $prf393, $prf394, $prf395, $prf396, $prf397, $prf398, $prf399,
    $prf400, $prf401, $prf402, $prf403, $prf404, $prf405, $prf406, $prf407,
    $prf408, $prf409, $prf410, $prf411, $prf412, $prf413, $prf414, $prf415,
    $prf416, $prf417, $prf418, $prf419, $prf420, $prf421, $prf422, $prf423,
    $prf424, $prf425, $prf426, $prf427, $prf428, $prf429, $prf430, $prf431,
    $prf432, $prf433, $prf434, $prf435, $prf436, $prf437, $prf438, $prf439,
    $prf440, $prf441, $prf442, $prf443, $prf444, $prf445, $prf446, $prf447,
    $prf448, $prf449, $prf450, $prf451, $prf452, $prf453, $prf454, $prf455,
    $prf456, $prf457, $prf458, $prf459, $prf460, $prf461, $prf462, $prf463,
    $prf464, $prf465, $prf466, $prf467, $prf468, $prf469, $prf470, $prf471,
    $prf472, $prf473, $prf474, $prf475, $prf476, $prf477, $prf478, $prf479,
    $prf480, $prf481, $prf482, $prf483, $prf484, $prf485, $prf486, $prf487,
    $prf488, $prf489, $prf490, $prf491, $prf492, $prf493, $prf494, $prf495,
    $prf496, $prf497, $prf498, $prf499, $prf500, $prf501, $prf502, $prf503,
    $prf504, $prf505, $prf506, $prf507, $prf508, $prf509, $prf510, $prf511,
  
    // extra registers
    $empty = -1,
};
// Define Opcodes based on RISC-V 32-bit Integer Instruction Set and adding Mul and Div from the RV32M extension
// https://msyksphinz-self.github.io/riscv-isadoc/html/rvi.html#addi
// https://msyksphinz-self.github.io/riscv-isadoc/html/rvm.html#mul

// Design Decision questions: 
// MUL and DIV are not dedicated instructions in RV32I, but are implemented as a sequence of instructions.
// Should we implement these as a sequence of instructions or should we implement them as a single instruction? aka what is easier to implement?
// Similarly NOR is not a dedicated instruction in RV32I, but is implemented as a sequence of instructions. same question as above

// add Mul

// Ommitted instructions:
// All unsigned instructions have been ommitted for ease of use -- i didnt need larger values
// All floating point instructions have been ommitted for ease of use
// All CSR instructions have been ommitted for ease of use -- CSR instructions are used to access the control and status registers
// WFI aka wait for interrupt has been ommitted  should we add this?
// ECALL and EBREAK have been ommitted
// FENCE calls have been ommitted -- they are used to ensure that all memory accesses before the fence are completed before any memory accesses after the fence are started, unsure if we need this
// lb lh lw and sb sh sw have been ommitted -- unsure if we need these

// CYCLE FORMAT?
// Instruction, fetch stage cycles, decode stage cycles, execute stage cycles, memory stage cycles, writeback stage cycles
// since we only get the num cycles left in the decode stage we just assume 1 less cycle and that it went through decode stage
// hence why map below will show 3 cycles for 1,1,1,0,1 instructions
// // R-type
// ADD, 1, 1, 1, 0, 1
// SUB, 1, 1, 1, 0, 1
// SLL, 1, 1, 1, 0, 1
// SLT, 1, 1, 1, 0, 1
// XOR, 1, 1, 1, 0, 1
// SRL, 1, 1, 1, 0, 1
// SRA, 1, 1, 1, 0, 1
// OR, 1, 1, 1, 0, 1
// AND, 1, 1, 1, 0, 1
// MUL, 1, 1, 3-5, 0, 1
// DIV, 1, 1, 10-40, 0, 1

// // I-type + sw for cheekiness
// ADDI, 1, 1, 1, 0, 1
// SLTI, 1, 1, 1, 0, 1
// XORI, 1, 1, 1, 0, 1
// ORI, 1, 1, 1, 0, 1
// ANDI, 1, 1, 1, 0, 1
// SLLI, 1, 1, 1, 0, 1
// SRLI, 1, 1, 1, 0, 1
// SRAI, 1, 1, 1, 0, 1
// LW, 1, 1, 1, 5-20, 1
// SW, 1, 1, 1, 5-20, 0

// // B-type
// BEQ, 1, 1, 1, 0, 0
// BNE, 1, 1, 1, 0, 0
// BLT, 1, 1, 1, 0, 0
// BGE, 1, 1, 1, 0, 0

// for the ranges of cycles we will assume the lower bound
enum Opcodes {
    // R-type
    ADD, // Add
    SUB, // Subtract
    SLL, // Shift Left Logical
    SLT, // Set Less Than
    XOR, // Exclusive OR
    SRL, // Shift Right Logical
    SRA, // Shift Right Arithmetic
    OR, // OR
    AND, // AND
    MUL, // Multiply
    DIV, // Divide

    // I-type + sw for cheekiness
    ADDI, // Add Immediate
    SLTI, // Set Less Than Immediate
    XORI, // Exclusive OR Immediate
    ORI, // OR Immediate
    ANDI, // AND Immediate
    SLLI, // Shift Left Logical Immediate
    SRLI, // Shift Right Logical Immediate
    SRAI, // Shift Right Arithmetic Immediate
    LW, // Load Word
    SW, // Store Word

    // B-type
    BEQ, // Branch Equal
    BNE, // Branch Not Equal
    BLT, // Branch Less Than
    BGE, // Branch Greater Than or Equal  
    // pseudo instruciton
    BGT, // Branch Greater Than -- combination of BGE and Bne
    BLE, // Branch Less Than or Equal -- combination of BLT and Bne

    // The following instructions have been added  to provide support for RV32I compiled code as they are pseudo code instructions
    //J-type
    J, // Jump
    JR, // Jump Register
    LI, // Load Immediate
    MV, // Move

    // There are also some pseudo code instructions which we have decided to handle differently
    // Pseudo code instructions
    // li, load immediate -- we will handle this as an addi instructions, this wont cause problems as long as we dont have to handle the case where the immediate is too large i.e. immediate has to be -2048 to 2047
    // mv, move -- we will handle this as an addi instruction, this wont cause problems as long as we dont have to handle the case where the immediate is too large i.e. immediate has to be -2048 to 2047
    // For mv e.g. mv $t0, $t1 will be handled as addi $t0, $t1, 0
    // For li e.g. li $t0, 5 will be handled as addi $t0, $zero, 5

};

// basic stack to track recursive function calls -- Call stack search
// create register mapping 
// https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
///// OLD REGISTER MAP /////
// static std::map<std::string, Register> register_map = {
//     {"$zero", $zero}, {"$ra", $ra}, {"$a0", $r2}, {"$a1", $r3}, {"$a2", $r4}, {"$a3", $r5}, {"$a4", $r6}, {"$a5", $r7}, 
//     {"$t0", $r8}, {"$t1", $r9}, {"$t2", $r10}, {"$t3", $r11}, {"$s0", $r12}, {"$s1", $r13}, {"$s2", $r14}, {"$s3", $r15}, 
//     {"$s4", $r16}, {"$s5", $r17}, {"$t4", $r18}, {"$t5", $r19}, {"$t6", $r20}, {"$t7", $r21}, {"$t8", $r22}, {"$t9", $r23}, 
//     {"$t10", $r24}, {"$t11", $r25}, {"$t12", $r26}, {"$t13", $r27}, {"$t14", $r28}, {"$t15", $r29}, {"$t16", $r30}, {"$t17", $r31},
//     {"$empty", $empty},
// };
///// NEW REGISTER MAP ///// for RV32I compiled code
static std::map<std::string, Register> register_map = {
    {"zero", $zero}, {"ra", $ra}, {"sp", $sp}, {"a0", $r3}, {"a1", $r4}, {"a2", $r5}, {"a3", $r6}, {"a4", $r7}, {"a5", $r8}, 
    {"t0", $r9}, {"t1", $r10}, {"t2", $r11}, {"t3", $r12}, {"t4", $r13}, {"t5", $r14}, {"t6", $r15}, {"t7", $r16}, 
    {"s0", $r17}, {"s1", $r18}, {"s2", $r19}, {"s3", $r20}, {"s4", $r21}, {"s5", $r22}, {"s6", $r23}, {"s7", $r24}, 
    {"s8", $r25}, {"s9", $r26}, {"s10", $r27}, {"s11", $r28}, {"t8", $r29}, {"t9", $r30}, {"t10", $r31},
    {"empty", $empty},
};

// create opcode/instruction mapping
struct InstructionInfo {
  Opcodes opcode;
  InstructionType type;
  int executeCycles;
};
static std::map<std::string, InstructionInfo> InstructionMap = {
  // R-type
  {"add", {ADD, RType, 1}}, // Add
  {"sub", {SUB, RType, 1}}, // Subtract
  {"sll", {SLL, RType, 1}}, // Shift Left Logical
  {"slt", {SLT, RType, 1}}, // Set Less Than
  {"xor", {XOR, RType, 1}}, // Exclusive OR
  {"srl", {SRL, RType, 1}}, // Shift Right Logical
  {"sra", {SRA, RType, 1}}, // Shift Right Arithmetic
  {"or", {OR, RType, 1}}, // OR
  {"and", {AND, RType, 1}}, // AND
  {"mul", {MUL, RType, 3}}, // Multiply
  {"div", {DIV, RType, 10}}, // Divide

  // I-type
  {"addi", {ADDI, IType, 1}}, // Add Immediate
  {"slti", {SLTI, IType, 1}}, // Set Less Than Immediate
  {"xori", {XORI, IType, 1}}, // Exclusive OR Immediate
  {"ori", {ORI, IType, 1}}, // OR Immediate
  {"andi", {ANDI, IType, 1}}, // AND Immediate
  {"slli", {SLLI, IType, 1}}, // Shift Left Logical Immediate
  {"srli", {SRLI, IType, 1}}, // Shift Right Logical Immediate
  {"srai", {SRAI, IType, 1}}, // Shift Right Arithmetic Immediate
  {"lw", {LW, IType, 1}}, // Load Word
  {"sw", {SW, IType, 1}}, // Store Word

  // B-type
  {"beq", {BEQ, BType, 1}}, // Branch Equal
  {"bne", {BNE, BType, 1}}, // Branch Not Equal
  {"blt", {BLT, BType, 1}}, // Branch Less Than
  {"bge", {BGE, BType, 1}}, // Branch Greater Than or Equal
  //pseudo instruction
  {"bgt", {BGT, BType, 1}}, // Branch Greater Than
  {"ble", {BLE, BType, 1}}, // Branch Less Than or Equal

  // The following instructions have been added  to provide support for RV32I compiled code
  // apparantly li is a pseudo instruction that is used to load a constant into a register
  // the same goes for j and jr etc.
  //J-type
  {"j", {J, JType, 1}}, // Jump
  {"jr", {JR, JType, 1}}, // Jump Register
  // As stated before we have handled "li" and "mv" as "addi" instructions which are handled by the "handlePseudoInstructions" function in decode.cpp
};

// why no jump? -- jump is a special case of branch, just jump to function address
#endif