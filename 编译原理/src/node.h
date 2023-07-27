#ifndef _NODE_H_
#define _NODE_H_

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <llvm-10/llvm/IR/BasicBlock.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

using namespace std;

class Context;

class TreeNode {
public:
  TreeNode(int lineNo) : lineNo(lineNo) {}
  virtual llvm::Value *genCode(Context &curContext) { return nullptr; }
  virtual void genJSON(string &s) = 0;

  int lineNo;
};

class TypeNode : public TreeNode {
public:
  TypeNode(string &name, int lineNo) : TreeNode(lineNo), name(name) {}
  llvm::Value *genCode(Context &curContext) { return nullptr; }
  void genJSON(string &s);

  string name;
};

class ExpressionNode : public TreeNode {
public:
  ExpressionNode(int lineNo) : TreeNode(lineNo) {}
  virtual llvm::Value *getAddr(Context &curContext) { return nullptr; }
};

class StatementNode : public TreeNode {
public:
  StatementNode(int lineNo) : TreeNode(lineNo) {}
};

class BlockNode : public TreeNode {
public:
  BlockNode(int lineNo) : TreeNode(lineNo) {}
  BlockNode(vector<StatementNode*> statementList, int lineNo) : TreeNode(lineNo),  statementList(statementList) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  vector<StatementNode*> statementList;
};

class IntNode : public ExpressionNode {
public:
  IntNode(int value, int lineNo) : ExpressionNode(lineNo), value(value) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  int value;
};

class FloatNode : public ExpressionNode {
public:
  FloatNode(float value, int lineNo) : ExpressionNode(lineNo), value(value) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  float value;
};

class CharNode : public ExpressionNode {
public:
  CharNode(string &value, int lineNo) : ExpressionNode(lineNo), value(value) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  string &value;
};

class StringNode : public ExpressionNode {
public:
  StringNode(string &value, int lineNo) : ExpressionNode(lineNo), value(value) {}
  llvm::Value *genCode(Context &curContext);
  llvm::Value *getAddr(Context &curContext);
  void genJSON(string &s);

  string &value;
};

class IdentifierNode : public ExpressionNode {
public:
  IdentifierNode(string &name, int lineNo) : ExpressionNode(lineNo), name(name) {}
  llvm::Value *genCode(Context &curContext);
  llvm::Value *getAddr(Context &curContext);
  void genJSON(string &s);

  string name;
};

class ArrayElementNode : public ExpressionNode { 
public:
  ArrayElementNode(IdentifierNode& identifier, ExpressionNode &index, int lineNo) : ExpressionNode(lineNo), identifier(identifier), index(index) {}
  llvm::Value *genCode(Context &curContext);
  llvm::Value *getAddr(Context &curContext);
  void genJSON(string &s);

  IdentifierNode &identifier;
  ExpressionNode &index;
};

class ArrayElementAssignNode : public ExpressionNode {
public:
  ArrayElementAssignNode(IdentifierNode &identifier, ExpressionNode &index, ExpressionNode &value, int lineNo) : ExpressionNode(lineNo), identifier(identifier), index(index), value(value) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  IdentifierNode &identifier;
  ExpressionNode &index;
  ExpressionNode &value;
};

class BinaryOpNode : public ExpressionNode {
public:
  BinaryOpNode(int op, ExpressionNode &lhs, ExpressionNode &rhs, int lineNo) : ExpressionNode(lineNo), op(op), lhs(lhs), rhs(rhs) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  int op;
  ExpressionNode &lhs;
  ExpressionNode &rhs;
};

class BinaryOpEqualNode : public ExpressionNode {
public:
  BinaryOpEqualNode(int op, IdentifierNode &identifier, ExpressionNode &expression, int lineNo) : ExpressionNode(lineNo), op(op), identifier(identifier), expression(expression) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  int op;
  IdentifierNode &identifier;
  ExpressionNode &expression;
};

// i++
class PostIncreaseNode : public ExpressionNode {
public:
  PostIncreaseNode(IdentifierNode &identifier, int lineNo) : ExpressionNode(lineNo), identifier(identifier) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  IdentifierNode &identifier;
};

class PreIncreaseNode : public ExpressionNode {
public:
  PreIncreaseNode(IdentifierNode &identifier, int lineNo) : ExpressionNode(lineNo), identifier(identifier) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  IdentifierNode &identifier;
};

class getAddrNode : public ExpressionNode {
public:
  getAddrNode(IdentifierNode &identifier, int lineNo) : ExpressionNode(lineNo), identifier(identifier) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  IdentifierNode &identifier;
};

class getArrayAddrNode : public ExpressionNode {
public:
  getArrayAddrNode(IdentifierNode &identifier, ExpressionNode &index, int lineNo) : ExpressionNode(lineNo), index(index), identifier(identifier) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  ExpressionNode &index;
  IdentifierNode &identifier;
};

class AssignmentNode : public ExpressionNode {
public:
  AssignmentNode(IdentifierNode &identifier, ExpressionNode &value, int lineNo) : ExpressionNode(lineNo), identifier(identifier), value(value) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  IdentifierNode &identifier;
  ExpressionNode &value;
};

class ExpressionStatementNode : public StatementNode {  //增加语句
public:
	ExpressionStatementNode(ExpressionNode& expression, int lineNo) : StatementNode(lineNo),  expression(expression) {}
	virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);

  ExpressionNode &expression;
};

class IfElseStatementNode : public StatementNode {
public:
  IfElseStatementNode(ExpressionNode &expression, BlockNode &ifBlock, BlockNode &elseBlock, int lineNo)
    : StatementNode(lineNo), expression(expression), ifBlock(ifBlock), elseBlock(elseBlock) {}
  virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);

  ExpressionNode &expression;
  BlockNode &ifBlock;
  BlockNode &elseBlock;
};

class IfStatementNode : public StatementNode {
public:
  IfStatementNode(ExpressionNode &expression, BlockNode &ifBlock, int lineNo)
    : StatementNode(lineNo), expression(expression), ifBlock(ifBlock) {}
  virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);

  ExpressionNode &expression;
  BlockNode &ifBlock;
};

class WhileStatementNode : public StatementNode {
public:
  WhileStatementNode(ExpressionNode &expression, BlockNode &block, int lineNo)
    : StatementNode(lineNo), expression(expression), block(block) {}
  virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);

  ExpressionNode &expression;
  BlockNode &block;
};

class ForStatementNode : public StatementNode {
public:
  ForStatementNode(ExpressionNode &initiation, ExpressionNode &condition, ExpressionNode &increment, BlockNode &body, int lineNo) : StatementNode(lineNo), initiation(initiation), condition(condition), increment(increment), body(body) {}
  virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);

  ExpressionNode &initiation;
  ExpressionNode &condition;
  ExpressionNode &increment;
  BlockNode &body;
};

class ReturnStatementNode : public StatementNode {
public:
  ReturnStatementNode(ExpressionNode &expression, int lineNo) : StatementNode(lineNo), expression(expression) {}
  virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);

  ExpressionNode &expression;
};

class ReturnVoidNode : public StatementNode {
public:
  ReturnVoidNode(int lineNo) : StatementNode(lineNo) {}
  virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);
};

class BreakStatementNode : public StatementNode {
public:
	BreakStatementNode(int lineNo) : StatementNode(lineNo) {}
	virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);
};

class ContinueStatementNode: public StatementNode {
public:
  ContinueStatementNode(int lineNo) : StatementNode(lineNo) {}
  virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);
};

class VariableDeclarationNode : public StatementNode {
public:
  VariableDeclarationNode(TypeNode &type, IdentifierNode &identifier, int lineNo) : StatementNode(lineNo), type(type), identifier(identifier), size(0), assignmentExpression(nullptr) {}

  VariableDeclarationNode(TypeNode &type, IdentifierNode &identifier, int size, int lineNo) : StatementNode(lineNo), type(type), identifier(identifier), size(size), assignmentExpression(nullptr) {}

  VariableDeclarationNode(TypeNode &type, IdentifierNode& identifier, ExpressionNode *assignmentExpression, int lineNo) : StatementNode(lineNo), type(type), identifier(identifier), assignmentExpression(assignmentExpression) {}

  virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);

  int size; // size != 0 means this is an array
  TypeNode &type;
  IdentifierNode &identifier;
  ExpressionNode *assignmentExpression;
};

// 函数声明
class FunctionDeclarationNode : public StatementNode {
public:
  FunctionDeclarationNode(TypeNode &type, IdentifierNode &identifier, vector<VariableDeclarationNode*> args, int lineNo) : StatementNode(lineNo), type(type), identifier(identifier), args(args) {}
  virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);

  TypeNode &type;
  IdentifierNode &identifier;
  vector<VariableDeclarationNode*> args;
};

// 函数定义
class FunctionDefinitionNode : public StatementNode {
public:
  FunctionDefinitionNode(TypeNode &type, IdentifierNode &identifier, vector<VariableDeclarationNode*> args, BlockNode& block, int lineNo) : StatementNode(lineNo), type(type), identifier(identifier), args(args), block(block) {}
  virtual llvm::Value* genCode(Context &curContext);
  void genJSON(string &s);

  TypeNode &type;
  IdentifierNode &identifier;
  BlockNode &block;
  vector<VariableDeclarationNode*> args;
};

class FunctionCallNode : public ExpressionNode {
public:
  FunctionCallNode(IdentifierNode &identifier, vector<ExpressionNode*> &args, int lineNo) : ExpressionNode(lineNo), identifier(identifier), args(args) {}
  FunctionCallNode(IdentifierNode &identifier, int lineNo) : ExpressionNode(lineNo), identifier(identifier) {}
  llvm::Value *genCode(Context &curContext);
  void genJSON(string &s);

  IdentifierNode &identifier;
  vector<ExpressionNode*> args;
};

class EmptyStatementNode : public StatementNode {
public:
  EmptyStatementNode(int lineNo) : StatementNode(lineNo) {}
  virtual llvm::Value* genCode(Context &curContext) { return nullptr; }
  void genJSON(string &s);
};


#endif // _NODE_H_