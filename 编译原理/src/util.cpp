#include "util.h"

void balanceType(llvm::Value* &left, llvm::Value* &right) {
    if (left->getType() != right->getType()) {
        if (left->getType() == llvm::Type::getFloatTy(GlobalContext)) {
            right = typeCast(right, llvm::Type::getFloatTy(GlobalContext));
        } else {
            if (right->getType() == llvm::Type::getFloatTy(GlobalContext)) {
                left = typeCast(left, llvm::Type::getFloatTy(GlobalContext));
            } else {
                if (left->getType() == llvm::Type::getInt32Ty(GlobalContext)) {
                    right = typeCast(right, llvm::Type::getInt32Ty(GlobalContext));
                } else if(right->getType() == llvm::Type::getInt32Ty(GlobalContext)) {
                    left = typeCast(left, llvm::Type::getInt32Ty(GlobalContext));
                } else {
                    throw logic_error("cann't use bool in +-*/");
                }
            }
        }
    }
    return;
}

// 对字符串中的所有换行符进行转义
void escapeLineBreak(string &s) {
    string lineBreak = string(1, '\n');
    int pos = s.find("\\n");
    
    while(pos != string::npos) {
        s = s.replace(pos, 2, lineBreak);
        pos = s.find("\\n");
    }
}

// 获取 identifer 对应变量的指针类型
llvm::Type* getVarPtrType(llvm::Value* var) {
    return var->getType()->getPointerElementType();
}

// 判断变量是否为指针类型
bool isPtr(llvm::Value* var) {
    return var->getType()->getPointerElementType()->isPointerTy();
}

// 根据传入的 type 字符串，返回对应的 LLVM 类型
llvm::Type* getLLvmType(string type) {
    if (!type.compare("int")) {
        return llvm::Type::getInt32Ty(GlobalContext);
    } else if (!type.compare("float")) {
        return llvm::Type::getFloatTy(GlobalContext);
    } else if (!type.compare("char")) {
        return llvm::Type::getInt8Ty(GlobalContext);
    } else {
        return llvm::Type::getVoidTy(GlobalContext);
    }
}

// 根据传入的 type 字符串，返回对应的 LLVM (指针) 类型
llvm::Type* getLLvmPtrType(string type) {
    if (!type.compare("int")) {
        return llvm::Type::getInt32PtrTy(GlobalContext);
    } else if (!type.compare("float")) {
        return llvm::Type::getFloatPtrTy(GlobalContext);
    } else if (!type.compare("char")) {
        return llvm::Type::getInt8PtrTy(GlobalContext);
    } else {
        return llvm::Type::getVoidTy(GlobalContext);
    }
}

// 根据传入的 type 字符串，返回对应的 LLVM (数组) 类型
llvm::Type* getArrayLLvmType(string type, int size) {
    if (!type.compare("int")) {
        return llvm::ArrayType::get(llvm::Type::getInt32Ty(GlobalContext), size);
    } else if (!type.compare("float")) {
        return llvm::ArrayType::get(llvm::Type::getFloatTy(GlobalContext), size);
    } else if (!type.compare("char")) {
        return llvm::ArrayType::get(llvm::Type::getInt8Ty(GlobalContext), size);
    } else {
        return nullptr;
    }
}

// 类型转换
llvm::Instruction::CastOps getCastedInt(llvm::Type* src, llvm::Type* dst) {
    if (src == llvm::Type::getFloatTy(GlobalContext) && dst == llvm::Type::getInt32Ty(GlobalContext)) {
        return llvm::Instruction::FPToSI;  // float -> int
    } else if (src == llvm::Type::getInt32Ty(GlobalContext) && dst == llvm::Type::getFloatTy(GlobalContext)) { 
        return llvm::Instruction::SIToFP;  // int -> float
    } else if (src == llvm::Type::getInt8Ty(GlobalContext) && dst == llvm::Type::getFloatTy(GlobalContext)) {
        return llvm::Instruction::UIToFP;  // char -> float
    } else if (src == llvm::Type::getInt8Ty(GlobalContext) && dst == llvm::Type::getInt32Ty(GlobalContext)) {
        return llvm::Instruction::ZExt;    // char -> int
    } else if (src == llvm::Type::getInt32Ty(GlobalContext) && dst == llvm::Type::getInt8Ty(GlobalContext)) {
        return llvm::Instruction::Trunc;   // int -> char
    } else if (src == llvm::Type::getFloatTy(GlobalContext) && dst == llvm::Type::getInt8Ty(GlobalContext)) {
        return llvm::Instruction::FPToUI;  // float -> char
    } else {
        throw logic_error("[ERROR] Wrong typecast");
    }
}

llvm::Value* typeCast(llvm::Value* src, llvm::Type* dst) {
    llvm::Instruction::CastOps op = getCastedInt(src->getType(), dst);

    return Builder.CreateCast(op, src, dst, "tmptypecast");
}