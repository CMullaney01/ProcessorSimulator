#ifndef PRINT_H
#define PRINT_H

#include <iostream>
#include "processor.h"


class Processor;

class Printer {
  public:
    Printer();
    void printImages();
    void attatchPrinter(Processor *p);
    Processor *processor;
    std::vector<int32_t> originalImage;
    std::vector<int32_t> newImage;
};


#endif