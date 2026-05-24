// src/runtime/value.cpp
#include "value.h"

namespace nova {

bool Value::isTruthy() const {
    if (type == Type::NIL) return false;
    if (type == Type::BOOL) return std::get<bool>(data);
    if (type == Type::INT) return std::get<int64_t>(data) != 0;
    if (type == Type::FLOAT) return std::get<double>(data) != 0.0;
    if (type == Type::STRING) return !std::get<std::string>(data).empty();
    return true;
}

std::string Value::toString() const {
    switch (type) {
        case Type::NIL: return "null";
        case Type::BOOL: return std::get<bool>(data) ? "true" : "false";
        case Type::INT: return std::to_string(std::get<int64_t>(data));
        case Type::FLOAT: return std::to_string(std::get<double>(data));
        case Type::STRING: return std::get<std::string>(data);
        case Type::OBJECT:
            return std::get<std::shared_ptr<Object>>(data)->toString();
        default: return "";
    }
}

} // namespace nova