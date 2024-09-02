#include "Program.h"
#include "VM.h"
#include "VortexTypes.h"

auto main(int, char **) -> int {
  auto prog = Program{};
  auto ptr = prog.createString("Hello World");
  prog.addConstant({.Type = ValueType::DOUBLE, .Value = {.AsDouble = 0.0}});
  prog.addConstant({.Type = ValueType::DOUBLE, .Value = {.AsDouble = 67.0}});
  auto index = prog.createGlobal(
      "hi", {.Type = ValueType::DOUBLE, .Value = {.AsDouble = 5.0}});
  prog.pushCode(PUSHC, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(PUSHC, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(1, 1);
  prog.pushCode(SAVE_GLOB, 0);
  prog.pushCode(PUSHC, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(0, 0);
  prog.pushCode(LOAD_GLOB, 0);
  prog.pushCode(PRINT, 0);
  prog.pushCode(HALT, 0);
  prog.dissassemble("bytecode");
  auto vM = VM{prog};
  vM.run();
  vM.printStack();
}
