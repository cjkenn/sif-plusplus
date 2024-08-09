#include "sif/Driver/driver.h"

using namespace sif;

int main(int argc, char *argv[]) {
  std::string filename = argv[1];
  Driver driver = Driver(filename);
  driver.run();
  return 0;
}
