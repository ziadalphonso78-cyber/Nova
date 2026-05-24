// src/runtime/vm.cpp
#include "vm.h"
#include <iostream>
#include <cmath>

namespace nova {

VM::VM() {
    defineNativeFunctions();
}

void VM::defineNativeFunctions() {
    auto printFunc = std::make_shared<FunctionObject>();
    printFunc->name = "print";
    globals["print"] = Value(printFunc);
}

Value VM::evaluateExpression(Expr* expr, std::unordered_map<std::string, Value>* env) {
    switch (expr->type) {
        case ExprType::BINARY:
            return evaluateBinary(static_cast<BinaryExpr*>(expr), env);
        case ExprType::UNARY:
            return evaluateUnary(static_cast<UnaryExpr*>(expr), env);
        case ExprType::LITERAL:
            return evaluateLiteral(static_cast<LiteralExpr*>(expr));
        case ExprType::VARIABLE:
            return evaluateVariable(static_cast<VariableExpr*>(expr), env);
        case ExprType::ASSIGN:
            return evaluateAssign(static_cast<AssignExpr*>(expr), env);
        case ExprType::CALL:
            return evaluateCall(static_cast<CallExpr*>(expr), env);
        default:
            return Value();
    }
}

Value VM::evaluateBinary(BinaryExpr* expr, std::unordered_map<std::string, Value>* env) {
    Value left = evaluateExpression(expr->left.get(), env);
    Value right = evaluateExpression(expr->right.get(), env);
    
    if (left.type == Value::Type::INT && right.type == Value::Type::INT) {
        int64_t l = std::get<int64_t>(left.data);
        int64_t r = std::get<int64_t>(right.data);
        
        switch (expr->op) {
            case TokenType::PLUS: return Value(l + r);
            case TokenType::MINUS: return Value(l - r);
            case TokenType::STAR: return Value(l * r);
            case TokenType::SLASH: return Value(l / r);
            case TokenType::PERCENT: return Value(l % r);
            case TokenType::EQ: return Value(l == r);
            case TokenType::NE: return Value(l != r);
            case TokenType::LT: return Value(l < r);
            case TokenType::LE: return Value(l <= r);
            case TokenType::GT: return Value(l > r);
            case TokenType::GE: return Value(l >= r);
            default: break;
        }
    }
    
    if (left.type == Value::Type::FLOAT && right.type == Value::Type::FLOAT) {
        double l = std::get<double>(left.data);
        double r = std::get<double>(right.data);
        
        switch (expr->op) {
            case TokenType::PLUS: return Value(l + r);
            case TokenType::MINUS: return Value(l - r);
            case TokenType::STAR: return Value(l * r);
            case TokenType::SLASH: return Value(l / r);
            case TokenType::EQ: return Value(l == r);
            case TokenType::NE: return Value(l != r);
            case TokenType::LT: return Value(l < r);
            case TokenType::LE: return Value(l <= r);
            case TokenType::GT: return Value(l > r);
            case TokenType::GE: return Value(l >= r);
            default: break;
        }
    }
    
    if (expr->op == TokenType::PLUS && 
        left.type == Value::Type::STRING && 
        right.type == Value::Type::STRING) {
        return Value(std::get<std::string>(left.data) + std::get<std::string>(right.data));
    }
    
    throw std::runtime_error("Invalid binary operation");
}

Value VM::evaluateUnary(UnaryExpr* expr, std::unordered_map<std::string, Value>* env) {
    Value operand = evaluateExpression(expr->operand.get(), env);
    
    switch (expr->op) {
        case TokenType::MINUS:
            if (operand.type == Value::Type::INT) {
                return Value(-std::get<int64_t>(operand.data));
            }
            if (operand.type == Value::Type::FLOAT) {
                return Value(-std::get<double>(operand.data));
            }
            break;
        case TokenType::NOT:
            return Value(!operand.isTruthy());
        default:
            break;
    }
    
    throw std::runtime_error("Invalid unary operation");
}

Value VM::evaluateLiteral(LiteralExpr* expr) {
    if (std::holds_alternative<int64_t>(expr->value)) {
        return Value(std::get<int64_t>(expr->value));
    }
    if (std::holds_alternative<double>(expr->value)) {
        return Value(std::get<double>(expr->value));
    }
    if (std::holds_alternative<std::string>(expr->value)) {
        return Value(std::get<std::string>(expr->value));
    }
    if (std::holds_alternative<bool>(expr->value)) {
        return Value(std::get<bool>(expr->value));
    }
    return Value();
}

Value VM::evaluateVariable(VariableExpr* expr, std::unordered_map<std::string, Value>* env) {
    auto it = env->find(expr->name);
    if (it != env->end()) {
        return it->second;
    }
    
    auto git = globals.find(expr->name);
    if (git != globals.end()) {
        return git->second;
    }
    
    throw std::runtime_error("Undefined variable: " + expr->name);
}

Value VM::evaluateAssign(AssignExpr* expr, std::unordered_map<std::string, Value>* env) {
    Value value = evaluateExpression(expr->value.get(), env);
    
    auto it = env->find(expr->name);
    if (it != env->end()) {
        it->second = value;
        return value;
    }
    
    auto git = globals.find(expr->name);
    if (git != globals.end()) {
        git->second = value;
        return value;
    }
    
    throw std::runtime_error("Undefined variable in assignment: " + expr->name);
}

Value VM::evaluateCall(CallExpr* expr, std::unordered_map<std::string, Value>* env) {
    Value callee = evaluateExpression(expr->callee.get(), env);
    
    if (callee.type != Value::Type::OBJECT) {
        throw std::runtime_error("Can only call functions");
    }
    
    auto* funcObj = dynamic_cast<FunctionObject*>(std::get<std::shared_ptr<Object>>(callee.data).get());
    if (!funcObj) {
        throw std::runtime_error("Value is not callable");
    }
    
    if (funcObj->name == "print") {
        for (const auto& arg : expr->arguments) {
            Value argVal = evaluateExpression(arg.get(), env);
            std::cout << argVal.toString();
        }
        std::cout << std::endl;
        return Value();
    }
    
    return Value();
}

void VM::executeStatement(Stmt* stmt, std::unordered_map<std::string, Value>* env) {
    switch (stmt->type) {
        case StmtType::EXPRESSION:
            executeExpressionStmt(static_cast<ExpressionStmt*>(stmt), env);
            break;
        case StmtType::VARIABLE_DECL:
            executeVariableDecl(static_cast<VariableDeclStmt*>(stmt), env);
            break;
        case StmtType::IF:
            executeIfStmt(static_cast<IfStmt*>(stmt), env);
            break;
        case StmtType::WHILE:
            executeWhileStmt(static_cast<WhileStmt*>(stmt), env);
            break;
        case StmtType::BLOCK:
            executeBlockStmt(static_cast<BlockStmt*>(stmt), env);
            break;
        case StmtType::FUNC_DECL:
            executeFuncDecl(static_cast<FuncDeclStmt*>(stmt), env);
            break;
        case StmtType::RETURN:
            executeReturnStmt(static_cast<ReturnStmt*>(stmt), env);
            break;
        default:
            break;
    }
}

void VM::executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, 
                      std::unordered_map<std::string, Value>* env) {
    for (const auto& stmt : statements) {
        executeStatement(stmt.get(), env);
    }
}

void VM::executeExpressionStmt(ExpressionStmt* stmt, std::unordered_map<std::string, Value>* env) {
    if (stmt->expr) {
        evaluateExpression(stmt->expr.get(), env);
    }
}

void VM::executeVariableDecl(VariableDeclStmt* stmt, std::unordered_map<std::string, Value>* env) {
    Value value;
    if (stmt->initializer) {
        value = evaluateExpression(stmt->initializer.get(), env);
    }
    (*env)[stmt->name] = value;
}

void VM::executeIfStmt(IfStmt* stmt, std::unordered_map<std::string, Value>* env) {
    Value condition = evaluateExpression(stmt->condition.get(), env);
    
    if (condition.isTruthy()) {
        executeStatement(stmt->thenBranch.get(), env);
    } else if (stmt->elseBranch) {
        executeStatement(stmt->elseBranch.get(), env);
    }
}

void VM::executeWhileStmt(WhileStmt* stmt, std::unordered_map<std::string, Value>* env) {
    while (true) {
        Value condition = evaluateExpression(stmt->condition.get(), env);
        if (!condition.isTruthy()) break;
        executeStatement(stmt->body.get(), env);
    }
}

void VM::executeBlockStmt(BlockStmt* stmt, std::unordered_map<std::string, Value>* env) {
    std::unordered_map<std::string, Value> blockEnv = *env;
    for (const auto& statement : stmt->statements) {
        executeStatement(statement.get(), &blockEnv);
    }
}

void VM::executeFuncDecl(FuncDeclStmt* stmt, std::unordered_map<std::string, Value>* env) {
    auto func = std::make_shared<FunctionObject>();
    func->name = stmt->name;
    func->isAsync = stmt->isAsync;
    
    for (const auto& param : stmt->params) {
        func->paramNames.push_back(param.first);
    }
    
    (*env)[stmt->name] = Value(func);
}

void VM::executeReturnStmt(ReturnStmt* stmt, std::unordered_map<std::string, Value>* env) {
    if (stmt->value) {
        throw evaluateExpression(stmt->value.get(), env);
    }
    throw Value();
}

void VM::interpret(const std::vector<std::unique_ptr<Stmt>>& statements) {
    std::unordered_map<std::string, Value> globalEnv = globals;
    
    try {
        for (const auto& stmt : statements) {
            executeStatement(stmt.get(), &globalEnv);
        }
    } catch (const Value& returnValue) {
        // Function return - ignore at top level
    } catch (const std::exception& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
    }
}

} // namespace nova