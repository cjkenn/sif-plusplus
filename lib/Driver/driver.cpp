#include "sif/Driver/driver.h"
#include "sif/Parser/lexer.h"

using namespace sif;

void Driver::run() { Lexer l = Lexer(filename_); }
