#include <cstdint>
#include <string>
#include <memory>

#include "bus.hpp"

Bus::Bus(const std::string& prog_path) : dram_{std::make_unique<DRAM>(prog_path)} {}

uint64_t Bus::Load(uint64_t addr, int size) {
  return dram_->Load(addr, size);
}

void Bus::Store(uint64_t addr, int size, uint64_t data) {
  dram_->Store(addr, size, data);
}
