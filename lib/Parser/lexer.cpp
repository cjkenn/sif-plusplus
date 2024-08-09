#include "sif/Parser/lexer.h"
#include "sif/Parser/token.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace sif;

Lexer::Lexer(std::string filename) {
  std::ifstream infile;
  infile.open(filename);

  assert(infile.is_open() && "Failed to open file!");

  std::vector<std::string> buffer;
  std::string line;
  while (getline(infile, line)) {
    buffer.push_back(line);
  }

  infile.close();

  buffer_ = buffer;
  currChar_ = std::nullopt;
  currPos_ = 0;
  currLine_ = 0;
};
