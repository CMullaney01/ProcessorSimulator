#include <iostream>
#include <string>

#include "isa.h"
#include "processor.h"
#include "parser.h"
#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "instruction.h"
#include "stage.h"
#include <string>
#include <cstring>



// int main(int argc, char* argv[]) {
//     if (argc != 2) {
//         std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
//         return 1;
//     }
//     std::string filename = argv[1];

//     std::cout <<  "Starting" << std::endl;
//     Processor *processor = new Processor();
//     processor->debug = true;
//     processor->buildProcessor();
//     processor->getProgram("../"+filename);
//     std::cout << "ProgramSize: " << processor->instructionMemory_size <<std::endl;
//     // processor->runScalar();
//     // processor->runPipelined();
//     processor->runSuperScalar(1, 1, 1, 1, 1);
//     // processor->runSuperScalar(2, 2, 2, 2, 2);
//     // processor->runSuperScalar(3, 3, 3, 3, 3);
//     // processor->runSuperScalar(4, 4, 4, 4, 4);
//     // processor->runSuperScalar(8, 8, 8, 8, 8);

//     return 0;
// }

int main(int argc, char* argv[]) {
    // Default values for command line arguments
    int superscalar = 1;
    int pipeline_width = 1;
    int scoreboardMode = 0;
    int branch_prediction_mode = 0;
    int OoO = 0;
    int image = 0;
    int verbose = 0;
    int general = 1;
    int reservationWidth = 8;
    std::string filename;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "-ss") == 0 && i + 1 < argc) {
            superscalar = std::stoi(argv[i + 1]);
            i++;
        } else if (std::strcmp(argv[i], "-pw") == 0 && i + 1 < argc) {
            pipeline_width = std::stoi(argv[i + 1]);
            i++;
        } else if (std::strcmp(argv[i], "-fm") == 0 && i + 1 < argc) {
            scoreboardMode = std::stoi(argv[i + 1]);
            i++;
        } else if (std::strcmp(argv[i], "-bm") == 0 && i + 1 < argc) {
            branch_prediction_mode = std::stoi(argv[i + 1]);
            i++;
        } else if (std::strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            image = std::stoi(argv[i + 1]);
            i++;
        } else if (std::strcmp(argv[i], "-g") == 0 && i + 1 < argc) {
            general = std::stoi(argv[i + 1]);
            i++;
        } else if (std::strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            reservationWidth = std::stoi(argv[i + 1]);
            i++;
        } 
        else if (std::strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            OoO = std::stoi(argv[i + 1]);
            i++;
        } else if (std::strcmp(argv[i], "-v") == 0) {
            verbose = std::stoi(argv[i + 1]);
        } else {
            filename = argv[i];
        }
    }

    // Check if filename is provided
    if (filename.empty()) {
        std::cerr << "Usage: " << argv[0] << " [-ss <super-scalar>] [-pw <pipeline-width>] [-fm <forwarding-mode>] [-bm <branch-prediction-mode>] [-i <image>] [-g <general purpose ALU>] [-r <reservationSize>] [-o <out-of-order>] [-v] <filename>" << std::endl;
        return 1;
    }

    // Run the processor
    std::cout << "Starting" << std::endl;
    Processor* processor = new Processor();
    processor->debug = verbose;
    // Default values for command line arguments
    processor->superS = superscalar;
    processor->scoreboardMode = scoreboardMode;
    processor->OoO = OoO;
    processor->branch_prediction_mode = branch_prediction_mode;
    processor->image = image;
    processor->general = general;
    processor->width = pipeline_width;
    processor->reservation = reservationWidth;
    processor->buildProcessor();
    processor->getProgram("../" + filename);
    std::cout << "ProgramSize: " << processor->instructionMemory_size << std::endl;
    if (superscalar) {
        std::cout << "Running Superscalar" << std::endl;
        processor->runSuperScalar(pipeline_width, pipeline_width, pipeline_width, pipeline_width, pipeline_width);
    } else {
        std::cout << "Running Scalar" << std::endl;
        processor->runScalar();
    }

    return 0;
}