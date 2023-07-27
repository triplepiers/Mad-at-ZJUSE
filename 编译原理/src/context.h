#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <map>
#include <stack>
#include <string>
#include <vector>
#include <typeinfo>

#include <llvm-10/llvm/IR/GlobalVariable.h>
#include <llvm-10/llvm/IR/Type.h>
#include <llvm/ADT/Twine.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/raw_ostream.h>

#include "node.h"

using namespace std;

extern llvm::LLVMContext GlobalContext;          // 全局 context
extern llvm::IRBuilder<> Builder;                // 全局 IRbuilder
extern stack<llvm::BasicBlock *> AfterBBStack;   // afterloop 标签构成的栈 -> 记录 break 返回位置
extern stack<llvm::BasicBlock *> ContBBStack;    // cond/loopinc 标签构成的栈 -> 记录 continue 返回位置

llvm::BasicBlock* getCondBlock();

class symbolTable{ 
public:
    map<string, llvm::Value*> local_var;        //  局部符号表 id -> Value
    map<string, llvm::Type*> local_var_type;    //  局部符号表 id -> Type

    bool hasVariable(string varName);

    void setVarValue(string varName, llvm::Value* varValue);
    llvm::Value* getVarValue(string varName);

    void setVarType(string varName, llvm::Type* varType);
};


class Context{
public:
    bool isArgs;
    bool hasReturn;
    llvm::Module *module; 
    llvm::Function *printf,*scanf, *gets;       // 默认引入 printf, scanf, gets 库函数
    Context();
    llvm::Function* getPrintf();
    llvm::Function* getScanf(); 
    llvm::Function* getGets();

    vector<symbolTable *> symbolTable_stack;    //  符号表
    symbolTable* getTopSymbolTable();
    void pushSymbolTable();
    void popSymbolTable();
    llvm::Value* getVariable(string variableName);
    llvm::Type* getVariableType(string variableName);

    // 当前正在处理的函数信息
    llvm::Function* currentFunc;            
    llvm::BasicBlock* returnBB;                 // return tag 所在的 BasicBlock
    llvm::Value* returnVal;                     // 返回值
    llvm::Function* getCurFunction();
    void pushFunction(llvm::Function* func);
    void popFunction();

    void genCode(BlockNode* Root);              // 入口函数

};

#endif // _CONTEXT_H_