#include <iostream>
#include <memory>

#include "cpu.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <path to program>" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  auto cpu = std::make_unique<CPU>();
  cpu->LoadProgram(argv[1]);
  cpu->Run();
}
