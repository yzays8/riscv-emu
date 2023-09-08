#include <iostream>
#include <memory>

#include "cpu.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <path to program>" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  auto cpu = std::make_unique<CPU>(argv[1]);

  std::cout << "Running..." << std::endl;
  for(;;) {
    cpu->PrintRegs();
    uint64_t new_pc = cpu->Execute(cpu->Fetch());
    cpu->SetPC(new_pc);
    if (new_pc == 0)
      break;
  }

  cpu->PrintRegs();
  cpu->AssertRegEq(a0, 0x37, a1, 0x15);
}
