#ifndef VM_H
#define VM_H

#include "Program.h"
#include "VortexTypes.h"
#include <array>

enum class VMState {
  OK,
  HALTED,
  STACK_OVERFLOW,
  STACK_UNDERFLOW,
  COMPILE_ERR,
  RUNTIME_ERR
};

class VM {
public:
  explicit VM(Program &bytecode);
  auto run() -> VMState;
  auto printStack() -> void;

private:
  auto executeOp() -> VMState;
  // instructions implementations
  auto pushc() -> void; // loads a constant
  auto push(VortexValue value) -> void;
  auto pop() -> VortexValue;
  auto add() -> void;
  auto sub() -> void;
  auto mul() -> void;
  auto div() -> void;
  // apparently C++ has a not keyword
  auto knot() -> void;
  auto negate() -> void;
  auto eq() -> void;
  auto lessThanOrEqual() -> void;
  auto greaterThanOrEqual() -> void;
  auto greater() -> void;
  auto less() -> void;
  auto pushNil() -> void;
  auto pushBool(bool val) -> void;
  auto print(VortexValue val) -> void;
  auto loadGlob() -> void;
  auto saveGlob() -> void;
  auto getLocal() -> void;
  auto addLocal() -> void;
  auto setLocal() -> void;
  // safety
  // returns true if the state is ok.
  auto check(VMState for_state, std::size_t expected_size = 0) -> bool;
  // global variables stuff
  auto loadGlobal(std::size_t index) -> void;
  auto updateGlobal(std::size_t index) -> void; // Give the global a new value
  // util
  auto isTrue(VortexValue val) -> bool {
    if (val.Type == ValueType::BOOL) {
      return val.Value.AsBool;
    } else if (val.Type == ValueType::NIL) {
      return false;
    }
    return true;
  }

private:
  static constexpr std::size_t STACK_SIZE_ = 2048;
  std::size_t PC_ = 0;
  std::size_t stack_top_ = 0;
  VMState state_ = VMState::OK;
  Program &bytecode_;
  std::array<VortexValue, STACK_SIZE_> stack_;
  std::string error_;
};

#endif // !VM_H
