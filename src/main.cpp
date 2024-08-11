#include "Program.h"
#include "VM.h"
#include "VortexTypes.h"

auto main(int, char **) -> int {
  auto prog = Program{};
  prog.addConstant({.Type = ValueType::DOUBLE, .Value = {5}});
  prog.addConstant({.Type = ValueType::DOUBLE, .Value = {2.0}});
  prog.pushCode(PUSHC, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(PUSHC, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(1, 0);
  prog.pushCode(DIV, 0);
  prog.pushCode(HALT, 0);
  prog.dissassemble("bytecode");
  auto vM = VM{prog};
  vM.run();
}
