#include "Program.h"
#include "VM.h"
#include "VortexTypes.h"

auto main(int, char **) -> int {
  auto prog = Program{};
  auto ptr = prog.createString("Hello World");
  prog.addConstant({.Type = ValueType::OBJECT, .Value = {.AsObject = ptr}});
  prog.addConstant({.Type = ValueType::OBJECT, .Value = {.AsObject = ptr}});
  prog.pushCode(PUSHC, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(PRINT, 0);
  prog.pushCode(HALT, 0);
  prog.dissassemble("bytecode");
  auto vM = VM{prog};
  vM.run();
  vM.printStack();
}
