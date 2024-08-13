#include "Program.h"
#include "VM.h"
#include "VortexTypes.h"

auto main(int, char **) -> int {
  auto prog = Program{};
  prog.addConstant({.Type = ValueType::DOUBLE, .Value = {.AsBool = false}});
  prog.addConstant({.Type = ValueType::DOUBLE, .Value = {5.0}});
  prog.pushCode(PUSHC, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(NOT, 0);
  prog.pushCode(HALT, 0);
  prog.dissassemble("bytecode");
  auto vM = VM{prog};
  vM.run();
}
