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

  for(;;) {
    uint64_t new_pc = cpu->Execute(cpu->Fetch());
    cpu->SetPC(new_pc);
    if (new_pc == 0)
      break;
  }

  cpu->AssertRegEq(args...);
}
