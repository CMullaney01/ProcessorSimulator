#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <numeric>

#include "parser.h"
#include "processor.h"

void Parser::attachParser(Processor *proc)
{
    processor = proc;
}

// create a vector of strings from the program file
std::vector<std::string> Parser::parseProgram(std::string filename)
{
    std::string line;
    std::ifstream file;
    std::vector<std::string> program;
    file.open(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return program;
    }
    while (!file.eof())
    {
        std::getline(file, line);
        program.push_back(line);
    }
    return program;
};

std::vector<std::string> RVParser::parseProgram(std::string filename)
{
    std::string line;
    std::ifstream file;
    std::vector<std::string> program;
    file.open(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return program;
    }
    program.push_back("addi sp sp 30000"); // add sp instruction to the beginning of the program
    // this sp instruction needs to be 4 times the size of the memory as later we do a conversion of /4 for memory addresses
    bool inFunction = false;
    bool inData = false;
    std::array<int32_t,784> tempArray;
    tempArray.fill(999);
    while (std::getline(file, line))
    {   
        // remove leading whitespace characters
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) {
            return !std::isspace(ch);
        }));
            // skip empty lines and lines with only whitespace characters
        if (line.empty() || std::all_of(line.begin(), line.end(), [](char c) {
                return std::isspace(static_cast<unsigned char>(c));
            })) {
            continue;
        }

        // skip lines that start with a hashtag
        if (line.front() == '#') {
            continue;
        }
        
        if (line.find(".LC0:") != std::string::npos) // found start of function
        {
            inData = true;
            continue;
        }
        if (line.find("main:") != std::string::npos) // found start of function
        {
            inData = false;
            processor->variableMap[".LC0"] = tempArray;
            std::cout << "Added Array to Map" << std::endl;
            std::cout << "Array0: " << processor->variableMap[".LC0"][0] << std::endl;
            std::cout << "Array1: " << processor->variableMap[".LC0"][1] << std::endl;
            std::cout << "Array2: " << processor->variableMap[".LC0"][2] << std::endl;
            std::cout << "Array3: " << processor->variableMap[".LC0"][3] << std::endl;
            std::cout << "Array4: " << processor->variableMap[".LC0"][4] << std::endl;
            continue;
        }


        if (line.find(".LFB0:") != std::string::npos) // found start of function
        {
            inFunction = true;
            continue;
        }
        else if (line.find(".LFE0:") != std::string::npos) // found end of function
        {
            inFunction = false;
            break; // exit the loop once end of function is found
        }

        // // skip CFI directives
        if (line.find(".cfi_") != std::string::npos || line.find("jr") != std::string::npos || line.find("call") != std::string::npos) {
            continue;
        }

        if (line.find("lui") != std::string::npos) {
            continue;
        }
        //skip CFI directives bug with jr should be fixed
        // if (line.find(".cfi_") != std::string::npos) {
            // continue;
        // }

        if (inFunction) // record lines only if inside the function
        {
            // Replace commas with spaces and split the line into tokens
            std::replace(line.begin(), line.end(), ',', ' ');
            std::istringstream iss(line);
            std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                             std::istream_iterator<std::string>());

            // Convert "lw" and "sw" instructions into the required format
            if (tokens.front() == "lw" || tokens.front() == "sw") {
                std::string rd = tokens[1].substr(0, tokens[1].find(','));
                std::string imm = tokens[2].substr(0, tokens[2].find('('));

                std::string rs1 = tokens[2].substr(tokens[2].find('(')+1);
                rs1.erase(std::remove(rs1.begin(), rs1.end(), ')'), rs1.end());
                tokens = {tokens.front(), rd, rs1, imm}; // rearrange the tokens
            }

            // Join the tokens back into a single line with spaces
            std::string instr = std::accumulate(std::next(tokens.begin()), tokens.end(),
                                                tokens.front(), [](std::string a, std::string b) {
                return a + " " + b;
            });

            program.push_back(instr);
        }

        if (inData) // record lines only if inside DATA
        {
            // if the line begins with .word get the number
            if (line.find(".word") != std::string::npos) {
                std::string numString = line.substr(line.find(".word")+6); // extract the number string
                int32_t num = std::stoi(numString); // convert the number string to integer
                for (int i = 0; i < 784; i++) { // loop through the array to find the first empty place
                    if (tempArray[i] == 999) {
                        tempArray[i] = num;
                        break;
                    }
                }
            }
        }
    }
    return program;
}
