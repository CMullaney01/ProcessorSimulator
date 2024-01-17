#ifndef _PARSER_INCLUDED_
#define _PARSER_INCLUDED_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>

class Processor;

class Parser
{
    public:
        Processor *processor;
        virtual std::vector<std::string> parseProgram(std::string filename);
        void attachParser(Processor *process);
};

class RVParser : public Parser
{
    public:
        std::vector<std::string> parseProgram(std::string filename) override;
};

#endif