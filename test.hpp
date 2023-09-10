#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "cpu.hpp"

class Test {
 public:
  Test();
  void Run();

 private:
  template <class... Args> void TestBin(const std::string& file_path, Args... args);
};

template <class... Args>
void Test::TestBin(const std::string& file_path, Args... args) {
  auto cpu = std::make_unique<CPU>(file_path);

  std::cout << "Testing " << file_path << " ..." << std::endl;
  for(;;) {
    uint32_t instr = cpu->Fetch();
    if (instr == 0)
      break;
    uint64_t new_pc = cpu->Execute(instr);
    cpu->SetPC(new_pc);
    if (new_pc == 0)
      break;
  }

  cpu->AssertRegEq(args...);
  std::cout << "Passed!" << std::endl;
}
