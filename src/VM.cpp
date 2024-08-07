#include "VM.h"
#include "VortexTypes.h"
#include <cassert>
#include <iostream>

static constexpr bool debug_stack = false;

VM::VM(Program &bytecode) : bytecode_{bytecode} {}

auto VM::run() -> VMState {
  while (state_ == VMState::OK) {
    state_ = executeOp();
    if constexpr (debug_stack) {
      std::cout << "======\n";
      printStack();
    }
  }
  switch (state_) {
  case VMState::COMPILE_ERR:
    break;
  case VMState::RUNTIME_ERR:
    std::cerr << error_ << "\n";
    break;
  case VMState::STACK_OVERFLOW:
    std::cerr << "Stack overflow!\n";
    break;
  case VMState::STACK_UNDERFLOW:
    std::cerr << "Stack underflow!\n";
    break;
  case VMState::HALTED:
    std::cout << "Program finished with code: "
              << stack_[stack_top_ - 1].Value.AsDouble << "\n";
    break;
  default:
    break;
  }

  if (state_ != VMState::HALTED) {
    std::cout << "Would you like to print the stack? (y/n)";
    auto c = char{};
    std::cin >> c;
    if (c == 'y') {
      printStack();
    }
  }

  return state_;
}

auto VM::executeOp() -> VMState {
  switch (bytecode_.Bytecode[PC_]) {
  case POP:
    pop();
    break;
  case PUSHC:
    pushc();
    break;
  case ADD:
    add();
    break;
  case SUB:
    sub();
    break;
  case MUL:
    mul();
    break;
  case DIV:
    div();
    break;
  case NEGATE:
    negate();
    break;
  case HALT:
    state_ = VMState::HALTED;
    break;
  default:
    state_ = VMState::RUNTIME_ERR;
    error_ = "Invalid instruction!";
    break;
  }
  // we do this so that the right instruction prints out when we
  // debug the stack. The PC_ stays on the last consumed instruction.
  if (state_ == VMState::OK) {
    ++PC_;
  }
  return state_;
}

auto VM::pushc() -> void {
  assert((PC_ + 1 < bytecode_.Bytecode.size()) &&
         "No operand for instruction PUSHC!"); // need an operand
  PC_++;
  auto lookup_index = static_cast<std::size_t>(bytecode_.Bytecode[PC_]);
  auto constant = bytecode_.getConstant(lookup_index);
  push(constant);
}

auto VM::pop() -> VortexValue {
  if (!check(VMState::STACK_UNDERFLOW, 1)) {
    state_ = VMState::STACK_UNDERFLOW;
    return {};
  }
  --stack_top_;              // shrink the stack
  return stack_[stack_top_]; // return the element
}

auto VM::push(VortexValue value) -> void {
  if (!check(VMState::STACK_OVERFLOW)) {
    state_ = VMState::HALTED;
    return;
  }
  stack_[stack_top_] = value;
  ++stack_top_;
}

auto VM::add() -> void {
  if (!check(VMState::STACK_UNDERFLOW, 2)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto b = pop();
  auto a = pop();
  // TODO: type checking
  push(VortexValue{.Type = ValueType::DOUBLE,
                   .Value = {a.Value.AsDouble + b.Value.AsDouble}});
}

auto VM::sub() -> void {
  if (!check(VMState::STACK_UNDERFLOW, 2)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto b = pop();
  auto a = pop();
  // TODO: type checking
  push(VortexValue{.Type = ValueType::DOUBLE,
                   .Value = {a.Value.AsDouble - b.Value.AsDouble}});
}

auto VM::mul() -> void {
  if (!check(VMState::STACK_UNDERFLOW, 2)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto b = pop();
  auto a = pop();
  // TODO: type checking
  push(VortexValue{.Type = ValueType::DOUBLE,
                   .Value = {a.Value.AsDouble * b.Value.AsDouble}});
}

auto VM::div() -> void {
  if (!check(VMState::STACK_UNDERFLOW, 2)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto b = pop();
  auto a = pop();
  // TODO: type checking
  push(VortexValue{.Type = ValueType::DOUBLE,
                   .Value = {a.Value.AsDouble / b.Value.AsDouble}});
}

auto VM::negate() -> void {
  if (!check(VMState::STACK_UNDERFLOW, 1)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto rhs = pop();
  push(VortexValue{.Type = ValueType::DOUBLE, .Value = {-rhs.Value.AsDouble}});
}

auto VM::eq() -> void {
  // TODO: typechecking first.
}

auto VM::lessThanOrEqual() -> void {}
auto VM::greaterThanOrEqual() -> void {}

auto VM::greater() -> void {}

auto VM::less() -> void {}

auto VM::check(VMState for_state, std::size_t expected_size) -> bool {
  assert((for_state == VMState::STACK_OVERFLOW ||
          for_state == VMState::STACK_UNDERFLOW) &&
         "Cannot check for unknowable states.");
  switch (for_state) {
  case VMState::STACK_OVERFLOW:
    if (stack_top_ >= STACK_SIZE_) {
      return false;
    }
    return true;
  case VMState::STACK_UNDERFLOW:
    if (stack_top_ < expected_size) {
      return false;
    }
    return true;
  default:
    // you're never getting here (hopefully)
    return true;
  }
}

auto VM::printStack() -> void {
  std::cout << "STACK BOTTOM is here. \n";
  std::cout << "Program counter: " << PC_ << "\n";
  for (std::size_t i = 0; i < stack_top_; ++i) {
    switch (stack_[i].Type) {
    case ValueType::DOUBLE:
      std::cout << "DOUBLE: " << stack_[i].Value.AsDouble << "\n";
    }
  }
}
