#ifndef _UTIL_H
#define _UTIL_H

#include "context.h"

void escapeLineBreak(string &s);
bool isPtr(llvm::Value* var);
void balanceType(llvm::Value* &left, llvm::Value* &right);

llvm::Type* getVarPtrType(llvm::Value* var);
llvm::Type* getLLvmType(string type);
llvm::Type* getLLvmPtrType(string type);
llvm::Type* getArrayLLvmType(string type, int size);
llvm::Instruction::CastOps getCastedInt(llvm::Type* src, llvm::Type* dst);
llvm::Value* typeCast(llvm::Value* src, llvm::Type* dst);

#endif // _UTIL_H_