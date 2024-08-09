#pragma once

#include <string>

namespace sif {
class Driver {
public:
  Driver(std::string filename) { filename_ = filename; };
  ~Driver(){};

  void run();

private:
  std::string filename_;
};
} // namespace sif
