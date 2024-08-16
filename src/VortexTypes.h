#ifndef VORTEX_TYPES_H
#define VORTEX_TYPES_H

#include <cstdint>
#include <string>

enum OpCode : std::uint8_t {
  PUSHC = 0, // load constant
  PUSH_TRUE,
  PUSH_FALSE,
  PUSH_NIL,
  POP,
  ADD,
  SUB,
  MUL,
  DIV,
  NOT,        // !
  NEGATE,     // - (pop())
  EQ,         // ==
  LESS_EQ,    // <=
  GREATER_EQ, // >=
  GREATER,    // >
  LESS,       // <
  HALT,
  INVALID_OP
};

enum class ValueType : std::uint8_t { DOUBLE, BOOL, NIL, OBJECT };
enum class ObjectType : std::uint8_t { STR };

struct Object {
  ObjectType Type;

  auto is(ObjectType type) -> bool { return Type == type; }
  virtual auto asString() -> std::string { return "object"; }
};

struct StringObject : Object {
  explicit StringObject(std::string_view s) : Str{s} { Type = ObjectType::STR; }

  std::string Str;
  virtual auto asString() -> std::string override { return "\"" + Str + "\""; }
};

struct VortexValue {
  union Value {
    double AsDouble;
    bool AsBool;
    Object *AsObject;
  };

  ValueType Type;
  Value Value;

  auto asString() -> std::string {
    switch (Type) {
    case ValueType::DOUBLE:
      return std::to_string(Value.AsDouble);
      break;
    case ValueType::BOOL:
      return Value.AsBool == true ? "true" : "false";
      break;
    case ValueType::NIL:
      return "nil";
      break;
    case ValueType::OBJECT:
      return Value.AsObject->asString();
    }
  }
};

#endif // !VORTEX_TYPES_H
