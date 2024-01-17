#include "printer.h"
#include <iostream>



void Printer::attatchPrinter(Processor *p) {
  processor = p;
}

void Printer::printImages() {
  std::cout << "Printing" << std::endl;
  // Resize the vectors to the correct size
  originalImage.resize(784);
  newImage.resize(784);
  // Get the original image
  std::cout << "Getting original image" << std::endl;
  for (int i = 0; i < 784; i++) {
    originalImage[i] = processor->memory[i];
  }
  for (int i = 6706; i < 7490; i++) {
    newImage[i - 6706] = processor->memory[i];
  }

  // Create an output file stream for the CSV file
  std::cout << "Creating output file stream" << std::endl;
  std::ofstream output_file("../images/output.csv");

  // Output the original image to the CSV file
  for (int i = 0; i < 784; i++) {
    // std::cout << "O: " << originalImage[i] << std::endl;
    output_file << originalImage[i];
    output_file << ",";
  }

  // Add a blank line between the two images
  output_file << "\n";

  // Output the new image to the CSV file
  for (int i = 0; i < 784; i++) {
    // std::cout <<"N: " << newImage[i] << std::endl;
    output_file << newImage[i];
    output_file << ",";
  }

  // Close the output file stream
  output_file.close();
}

Printer::Printer() {
}