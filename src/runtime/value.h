// src/runtime/value.h
#pragma once
#include <variant>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace nova {

struct Object {
    virtual ~Object() = default;
    virtual std::string toString() const = 0;
};

struct FunctionObject : Object {
    std::string name;
    std::vector<std::string> paramNames;
    bool isAsync = false;
    
    std::string toString() const override {
        return "<func " + name + ">";
    }
};

struct Value {
    enum class Type {
        NIL, BOOL, INT, FLOAT, STRING, OBJECT
    };
    
    Type type;
    std::variant<std::monostate, bool, int64_t, double, std::string, std::shared_ptr<Object>> data;
    
    Value() : type(Type::NIL), data(std::monostate()) {}
    Value(bool b) : type(Type::BOOL), data(b) {}
    Value(int64_t i) : type(Type::INT), data(i) {}
    Value(double d) : type(Type::FLOAT), data(d) {}
    Value(const std::string& s) : type(Type::STRING), data(s) {}
    Value(const char* s) : type(Type::STRING), data(std::string(s)) {}
    Value(std::shared_ptr<Object> obj) : type(Type::OBJECT), data(obj) {}
    
    bool isTruthy() const;
    std::string toString() const;
};

} // namespace nova