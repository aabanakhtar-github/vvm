#include "Program.h"
#include "VM.h"
#include "VortexTypes.h"

auto main(int, char **) -> int {
  auto prog = Program{};
  prog.addConstant({.Type = ValueType::DOUBLE, .Value = {4.13}});
  prog.addConstant({.Type = ValueType::DOUBLE, .Value = {10.0}});
  prog.pushCode(PUSHC, 0);
  // prog.pushCode(PUSHC, 0);
  // prog.pushCode(1, 0);
  prog.dissassemble("bytecode");
  VM vm(prog);
  vm.run();
}
