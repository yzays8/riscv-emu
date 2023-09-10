#include <cstdint>
#include <string>
#include <memory>

#include "test.hpp"
#include "cpu.hpp"

Test::Test() {}

void Test::Run() {
  TestBin("test/add-addi/add-addi.bin", t6, 42);
  TestBin("test/fib/fib.bin", a0, 55);

  std::cout << "All tests passed!" << std::endl;
}