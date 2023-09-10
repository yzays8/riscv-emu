#include <unistd.h>
#include <iostream>
#include <memory>

#include "cpu.hpp"
#include "test.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " [-t]\n"
              << "       " << argv[0] << " <file_path>" << std::endl;
    return 1;
  }

  bool do_test = false;
  int opt;
  while ((opt = getopt(argc, argv, "t")) != -1) {
    switch (opt) {
    case 't':
      do_test = true;
      break;
    default:
      std::cerr << "Usage: " << argv[0] << " [-t]\n"
                << "       " << argv[0] << " <file_path>" << std::endl;
      return 1;
    }
  }

  if (do_test) {
    auto test = std::make_unique<Test>();
    test->Run();
    return 0;
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
}
