// src/runtime/vm.h
#pragma once
#include "value.h"
#include "../core/ast.h"
#include <vector>
#include <stack>
#include <unordered_map>
#include <memory>

namespace nova {

class VM {
public:
    VM();
    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);
    
private:
    std::unordered_map<std::string, Value> globals;
    
    Value evaluateExpression(Expr* expr, std::unordered_map<std::string, Value>* env);
    void executeStatement(Stmt* stmt, std::unordered_map<std::string, Value>* env);
    void executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements,
                      std::unordered_map<std::string, Value>* env);
    
    Value evaluateBinary(BinaryExpr* expr, std::unordered_map<std::string, Value>* env);
    Value evaluateUnary(UnaryExpr* expr, std::unordered_map<std::string, Value>* env);
    Value evaluateLiteral(LiteralExpr* expr);
    Value evaluateVariable(VariableExpr* expr, std::unordered_map<std::string, Value>* env);
    Value evaluateAssign(AssignExpr* expr, std::unordered_map<std::string, Value>* env);
    Value evaluateCall(CallExpr* expr, std::unordered_map<std::string, Value>* env);
    
    void executeExpressionStmt(ExpressionStmt* stmt, std::unordered_map<std::string, Value>* env);
    void executeVariableDecl(VariableDeclStmt* stmt, std::unordered_map<std::string, Value>* env);
    void executeIfStmt(IfStmt* stmt, std::unordered_map<std::string, Value>* env);
    void executeWhileStmt(WhileStmt* stmt, std::unordered_map<std::string, Value>* env);
    void executeBlockStmt(BlockStmt* stmt, std::unordered_map<std::string, Value>* env);
    void executeFuncDecl(FuncDeclStmt* stmt, std::unordered_map<std::string, Value>* env);
    void executeReturnStmt(ReturnStmt* stmt, std::unordered_map<std::string, Value>* env);
    
    void defineNativeFunctions();
};

} // namespace nova