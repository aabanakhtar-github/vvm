#include "VM.h"
#include "VortexTypes.h"
#include <cassert>
#include <iostream>

#define DO_BINARY_OP

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
    // TODO: add support for proper error handling between both the front and
    // the back
    std::cerr << "TODO: addd support for proper error handling.\n";
    break;
  case VMState::STACK_OVERFLOW:
    std::cerr << "Stack overflow!\n";
    break;
  case VMState::STACK_UNDERFLOW:
    std::cerr << "Stack underflow!\n";
    break;
  case VMState::HALTED:
    std::cout << "Program finished with code: "
              << stack_[stack_top_ - 1].asString() << "\n";
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
  case PUSH_TRUE:
    pushBool(true);
    break;
  case PUSH_FALSE:
    pushBool(false);
    break;
  case PUSH_NIL:
    pushNil();
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
  case NOT:
    knot();
    break;
  case EQ:
    eq();
    break;
  case LESS:
    less();
    break;
  case LESS_EQ:
    lessThanOrEqual();
    break;
  case GREATER:
    greater();
    break;
  case GREATER_EQ:
    greaterThanOrEqual();
    break;
  case PRINT:
    print(pop());
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

auto VM::print(VortexValue value) -> void {
  switch (value.Type) {
  default:
    abort();
  }
}

auto VM::pushc() -> void {
  assert((PC_ + 3 < bytecode_.Bytecode.size()) &&
         "Operand of invalid or "
         "non-existent size for "
         "instruction PUSHC!"); // need a 24bit
                                // operand

  auto b1 = static_cast<std::int32_t>(
      bytecode_.Bytecode[PC_ + 1]); // the three bytes of the instruction
  auto b2 = static_cast<std::int32_t>(bytecode_.Bytecode[PC_ + 2]);
  auto b3 = static_cast<std::int32_t>(bytecode_.Bytecode[PC_ + 3]);
  auto index = (b1 << 16) | (b2 << 8) | (b3); // magic stuff
  auto constant = bytecode_.getConstant(static_cast<std::size_t>(index));
  push(constant);
  PC_ += 3; // the 4th gets removed in the main executeOp func
}

auto VM::pushBool(bool val) -> void {
  auto bval = VortexValue{.Type = ValueType::BOOL, .Value = {.AsBool = val}};
  push(bval);
}

auto VM::pushNil() -> void {
  auto val = VortexValue{.Type = ValueType::NIL, .Value = {.AsBool = false}};
  push(val);
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
  // TODO: debug assertions in the vortex error format see trello for more
  if (!check(VMState::STACK_UNDERFLOW, 2)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto b = pop();
  auto a = pop();
  switch (a.Type) {
  case ValueType::OBJECT: {
    /* HANDLE ADDING OBJECTS LIKE STRINGS */
    switch (a.Value.AsObject->Type) {
    case ObjectType::STR: {
      // Convert these to string objects type
      auto string_object1 = dynamic_cast<StringObject *>(a.Value.AsObject);
      auto string_object2 = dynamic_cast<StringObject *>(b.Value.AsObject);
      // add em all up
      auto result =
          bytecode_.createString(string_object1->Str + string_object2->Str);
      push(VortexValue{.Type = ValueType::OBJECT, .Value{.AsObject = result}});
      break;
    }
    default: { // will be unreachable after semantic analyzer
      error_ = "Cannot handle adding objects of non-string type!";
      state_ = VMState::RUNTIME_ERR;
      break;
    }
    }
  }
  case ValueType::DOUBLE: {
    // handle regular addition here
    auto result = a.Value.AsDouble + b.Value.AsDouble;
    push(VortexValue{.Type = ValueType::DOUBLE, .Value{.AsDouble = result}});
    break;
  }
  default: // will be unreachable after semantic analzyer
    error_ = "Cannot add two non addable types!";
    state_ = VMState::RUNTIME_ERR;
    break;
  }
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
  if (b.Value.AsDouble == 0.0) {
    // division by zero is not good for the vm
    error_ = "Division by zero!";
    state_ = VMState::RUNTIME_ERR;
  }
  // TODO: type checking
  push(VortexValue{.Type = ValueType::DOUBLE,
                   .Value = {a.Value.AsDouble / b.Value.AsDouble}});
}

auto VM::knot() -> void {
  if (!check(VMState::STACK_UNDERFLOW, 1)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto rhs = pop();
  auto val = VortexValue{};
  val.Type = ValueType::BOOL;
  val.Value.AsBool = !isTrue(rhs);
  push(val);
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
  // TODO: typechecking  using assert and then a dedicated static analysis
  if (!check(VMState::STACK_UNDERFLOW, 2)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto b = pop();
  auto a = pop();
  auto val = VortexValue{};
  val.Type = ValueType::BOOL;
  val.Value.AsBool = a.Value.AsDouble == b.Value.AsDouble;
  val.Type = ValueType::BOOL;
  push(val);
}

auto VM::lessThanOrEqual() -> void {
  if (!check(VMState::STACK_UNDERFLOW, 2)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto b = pop();
  auto a = pop();
  auto val = VortexValue{};
  val.Type = ValueType::BOOL;
  val.Value.AsBool = a.Value.AsDouble <= b.Value.AsDouble;
  push(val);
}

auto VM::greaterThanOrEqual() -> void {
  // TODO: typechecking  using assert and then a dedicated static analysis
  if (!check(VMState::STACK_UNDERFLOW, 2)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto b = pop();
  auto a = pop();
  auto val = VortexValue{};
  val.Type = ValueType::BOOL;
  val.Value.AsBool = a.Value.AsDouble >= b.Value.AsDouble;
  val.Type = ValueType::BOOL;
  push(val);
}

auto VM::greater() -> void {
  // TODO: typechecking  using assert and then a dedicated static analysis
  if (!check(VMState::STACK_UNDERFLOW, 2)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto b = pop();
  auto a = pop();
  auto val = VortexValue{};
  val.Type = ValueType::BOOL;
  val.Value.AsBool = a.Value.AsDouble > b.Value.AsDouble;
  val.Type = ValueType::BOOL;
  push(val);
}

auto VM::less() -> void {
  // TODO: typechecking  using assert and then a dedicated static analysis
  if (!check(VMState::STACK_UNDERFLOW, 2)) {
    state_ = VMState::STACK_UNDERFLOW;
    return;
  }
  auto b = pop();
  auto a = pop();
  auto val = VortexValue{};
  val.Type = ValueType::BOOL;
  val.Value.AsBool = a.Value.AsDouble < b.Value.AsDouble;
  val.Type = ValueType::BOOL;
  push(val);
}

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
    std::cout << stack_[i].asString() << "\n";
  }
}
