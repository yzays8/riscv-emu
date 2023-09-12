#include <cstdint>
#include <string>
#include <memory>

#include "test.hpp"
#include "cpu.hpp"

const std::string kTestDir = "../test/";

Test::Test() {}

void Test::Run() {
  TestBin("add-addi/add-addi.bin", t6, 42);
  TestBin("lui/lui.bin", a0, 42 << 12);
  TestBin("auipc/auipc.bin", a0, kDramBaseAddr + (42 << 12));
  TestBin("jal/jal.bin", a0, kDramBaseAddr + 4, pc, kDramBaseAddr + 42);
  TestBin("store-load/store-load.bin", t1, 0, t2, 256);
  TestBin("slt/slt.bin", t2, 1, t3, 1, t4, 1);
  TestBin("xor/xor.bin", a1, 3, a2, 0);
  TestBin("or/or.bin", a1, 0b11, a2, 0b10);
  TestBin("and/and.bin", a1, 0b10, a2, 0b10);
  TestBin("sll/sll.bin", a2, 1 << 5, a3, 1 << 5, a4, 1);
  TestBin("sra-srl/sra-srl.bin", a2, -4, a3, -2, a4, -8llu >> 2, a5, -8llu >> 1);
  TestBin("op/op.bin", a2, 0x7f00002a);
  TestBin("fib/fib.bin", a0, 55);

  std::cout << "All tests passed!" << std::endl;
}
