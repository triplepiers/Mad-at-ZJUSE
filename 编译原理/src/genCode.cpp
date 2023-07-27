#include "node.h"
#include "parse.hpp"
#include "context.h"
#include "util.h"

llvm::Value* IntNode::genCode(Context &curContext) {
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(GlobalContext), this->value, true);
}

llvm::Value* FloatNode::genCode(Context &curContext) {
    return llvm::ConstantFP::get(llvm::Type::getFloatTy(GlobalContext), this->value);
}

llvm::Value* CharNode::genCode(Context &curContext) {
    if (this->value.size() == 3) // '?'
        return Builder.getInt8(this->value.at(1));
    else {                       // '\?' input escape
        switch (this->value[2]) {
            case 'n' : return Builder.getInt8('\n'); break;
            case '\\': return Builder.getInt8('\\'); break;
            case 'a' : return Builder.getInt8('\a'); break;
            case 'b' : return Builder.getInt8('\b'); break;
            case 'f' : return Builder.getInt8('\f'); break;
            case 't' : return Builder.getInt8('\t'); break;
            case 'v' : return Builder.getInt8('\v'); break;
            case '\'': return Builder.getInt8('\''); break;
            case '\"': return Builder.getInt8('\"'); break;
            case '0' : return Builder.getInt8('\0'); break;
            default  : throw logic_error("[ERROR] Illegal char [" + this->value + "]");
        }
    }
}

llvm::Value* StringNode::genCode(Context &curContext) {
    string str = value.substr(1, value.length() - 2); // "???" 去掉两边的引号
    escapeLineBreak(str);

    // 为需要引用的字符串创建对应的全局变量
    llvm::Constant *strConst = llvm::ConstantDataArray::getString(GlobalContext, str);
    llvm::Value *globalVar = new llvm::GlobalVariable(*(curContext.module), strConst->getType(), true, llvm::GlobalValue::PrivateLinkage, strConst, "_Const_String_");
    
    // 用于将类型识别为 i8* 并指向字符数组的起始位置
    vector<llvm::Value*> indexList;
    indexList.push_back(Builder.getInt32(0));
    indexList.push_back(Builder.getInt32(0));

    return Builder.CreateInBoundsGEP(globalVar, llvm::ArrayRef<llvm::Value*>(indexList), "tmpstring");
}

// （返回的也是数组的起始位置）
llvm::Value* StringNode::getAddr(Context &curContext) {
    string str = value.substr(1, value.length() - 2);
    escapeLineBreak(str);

    llvm::Constant *strConst = llvm::ConstantDataArray::getString(GlobalContext, str);
    llvm::Value *globalVar = new llvm::GlobalVariable(*(curContext.module), strConst->getType(), true, llvm::GlobalValue::PrivateLinkage, strConst, "_Const_String_");
    
    vector<llvm::Value*> indexList;
    indexList.push_back(Builder.getInt32(0));
    indexList.push_back(Builder.getInt32(0));

    return Builder.CreateInBoundsGEP(globalVar, llvm::ArrayRef<llvm::Value*>(indexList), "tmpstring");
}

llvm::Value* IdentifierNode::genCode(Context &curContext) {
    // 查找变量
    llvm::Value* var = curContext.getVariable(name);
    if (!var) {
        throw logic_error("undeclared variable [" + name + "]");
    }
    
    // 获取变量类型
    llvm::Type* type = getVarPtrType(var);

    // 返回变量值
    llvm::Value* varVal = nullptr;
    if (type->isArrayTy()) { // 数组 -> 返回首地址
        vector<llvm::Value*> indexList;
        indexList.push_back(Builder.getInt32(0));
        indexList.push_back(Builder.getInt32(0));
        varVal = Builder.CreateInBoundsGEP(var, indexList, "arrayPtr");
    } else {
        varVal = new llvm::LoadInst(type, var, "LoadInst", false, Builder.GetInsertBlock());
    }

    return varVal;
}

llvm::Value* IdentifierNode::getAddr(Context &curContext) {
    llvm::Value* var = curContext.getVariable(name);
    if (!var) {
        throw logic_error("undeclared variable [" + name + "]");
    }
    return var;
}

llvm::Value* getArrayAddrNode::genCode(Context &curContext) {
    llvm::Value* var = curContext.getVariable(identifier.name);
    if (!var) {
        throw logic_error("undeclared array [" + identifier.name + "]");
    }
    
    llvm::Value* indexValue = index.genCode(curContext);
    vector<llvm::Value*> indexList;

    if (isPtr(var)) {
        var = Builder.CreateLoad(getVarPtrType(var), var);
        indexList.push_back(indexValue);    
    } else {
        indexList.push_back(Builder.getInt32(0));
        indexList.push_back(indexValue);    
    }
    // 返回对应 idx 地址
    return Builder.CreateInBoundsGEP(var, llvm::ArrayRef<llvm::Value*>(indexList), "elePtr");
}

llvm::Value* ArrayElementNode::genCode(Context &curContext) {
    llvm::Value* var = curContext.getVariable(identifier.name);
    if (!var) {
        throw logic_error("undeclared array [" + identifier.name + "]");
    }

    llvm::Value* indexValue = index.genCode(curContext);
    vector<llvm::Value*> indexList;

    // 如果是一个指针
    if (isPtr(var)) {
        var = Builder.CreateLoad(getVarPtrType(var), var);
        indexList.push_back(indexValue);    
    } else {
        indexList.push_back(Builder.getInt32(0));
        indexList.push_back(indexValue);    
    }

    llvm::Value* elePtr =  Builder.CreateInBoundsGEP(var, llvm::ArrayRef<llvm::Value*>(indexList), "tmparray");

    // 返回值
    return Builder.CreateLoad(getVarPtrType(elePtr), elePtr, "tmpvar");
}

llvm::Value* ArrayElementNode::getAddr(Context &curContext) {
    llvm::Value* var = curContext.getVariable(identifier.name);
    if (!var) {
        throw logic_error("undeclared array [" + identifier.name + "]");
    }

    llvm::Value* indexValue = index.genCode(curContext);
    vector<llvm::Value*> indexList;

    // 如果是一个指针
    if (isPtr(var)) {
        var = Builder.CreateLoad(getVarPtrType(var), var);
        indexList.push_back(indexValue);    
    } else {
        indexList.push_back(Builder.getInt32(0));
        indexList.push_back(indexValue);    
    }

    // 返回地址
    return Builder.CreateInBoundsGEP(var, llvm::ArrayRef<llvm::Value*>(indexList), "tmparray");
}

llvm::Value* ArrayElementAssignNode::genCode(Context &curContext) {
    llvm::Value* var = curContext.getVariable(identifier.name);
    if (!var) {
        throw logic_error("undeclared array [" + identifier.name + "]");
    } 

    llvm::Value* indexValue = index.genCode(curContext);
    vector<llvm::Value*> indexList;

    // 如果是一个指针
    if (isPtr(var)) {
        var = Builder.CreateLoad(getVarPtrType(var), var);
        indexList.push_back(indexValue);    
    } else {
        indexList.push_back(Builder.getInt32(0));
        indexList.push_back(indexValue);    
    }

    llvm::Value* arrEle =  Builder.CreateInBoundsGEP(var, llvm::ArrayRef<llvm::Value*>(indexList), "tmpvar");
    llvm::Value* val = value.genCode(curContext);

    if (val->getType() != getVarPtrType(arrEle)) {
        val = typeCast(val, getVarPtrType(arrEle));
    }

    return Builder.CreateStore(val, arrEle);
}

llvm::Value* BinaryOpNode::genCode(Context &curContext) {

    llvm::Value* left = lhs.genCode(curContext);
    llvm::Value* right = rhs.genCode(curContext);
    llvm::Instruction::BinaryOps bi_op;

    switch (op) {
        case PLUS :
        case MINUS:
        case MUL  :
        case DIV  :
            balanceType(left, right);
            switch (op) {
                case PLUS : bi_op = left->getType()->isFloatTy() ? llvm::Instruction::FAdd : llvm::Instruction::Add;  break;
                case MINUS: bi_op = left->getType()->isFloatTy() ? llvm::Instruction::FSub : llvm::Instruction::Sub;  break;
                case MUL  : bi_op = left->getType()->isFloatTy() ? llvm::Instruction::FMul : llvm::Instruction::Mul;  break;
                case DIV  : bi_op = left->getType()->isFloatTy() ? llvm::Instruction::FDiv : llvm::Instruction::SDiv; break;
            }
            return llvm::BinaryOperator::Create(bi_op,left,right,"", Builder.GetInsertBlock());
        case AND   :
            if (left->getType() != llvm::Type::getInt1Ty(GlobalContext) || right->getType() != llvm::Type::getInt1Ty(GlobalContext)) {
                throw logic_error("cannot use types other than bool in AND");
            }
            return Builder.CreateAnd(left, right, "tmpAnd");
        case OR    :
            if (left->getType() != llvm::Type::getInt1Ty(GlobalContext) || right->getType() != llvm::Type::getInt1Ty(GlobalContext)) {
                throw logic_error("cannot use types other than bool in OR");
            }
            return Builder.CreateOr(left, right, "tmpOR");
        default    : //LT、GT、EQ、NEQ、LE、GE
            balanceType(left, right);
            switch (op) {
                case EQU      :
                    return (left->getType() == llvm::Type::getFloatTy(GlobalContext)) ? Builder.CreateFCmpOEQ(left, right, "fcmptmp") : Builder.CreateICmpEQ(left, right, "icmptmp");
                case GEQ      :
                    return (left->getType() == llvm::Type::getFloatTy(GlobalContext)) ? Builder.CreateFCmpOGE(left, right, "fcmptmp") : Builder.CreateICmpSGE(left, right, "icmptmp");
                case LEQ      :
                    return (left->getType() == llvm::Type::getFloatTy(GlobalContext)) ? Builder.CreateFCmpOLE(left, right, "fcmptmp") : Builder.CreateICmpSLE(left, right, "icmptmp");
                case GREATERT :
                    return (left->getType() == llvm::Type::getFloatTy(GlobalContext)) ? Builder.CreateFCmpOGT(left, right, "fcmptmp") : Builder.CreateICmpSGT(left, right, "icmptmp");
                case LESST    :
                    return (left->getType() == llvm::Type::getFloatTy(GlobalContext)) ? Builder.CreateFCmpOLT(left, right, "fcmptmp") : Builder.CreateICmpSLT(left, right, "icmptmp");
                case NEQ      :
                    return (left->getType() == llvm::Type::getFloatTy(GlobalContext)) ? Builder.CreateFCmpONE(left, right, "fcmptmp") : Builder.CreateICmpNE(left, right, "icmptmp");
            }
    }

    return nullptr;
}

llvm::Value* BinaryOpEqualNode::genCode(Context &curContext) {
    llvm::Value* left = identifier.genCode(curContext);
    llvm::Value* right = expression.genCode(curContext);
    llvm::Instruction::BinaryOps bi_op;

    // 左右值类型不等
    balanceType(left, right);
    // 分类处理四种算数运算
    switch (op) {
        case PLUS_EQ  : bi_op = left->getType()->isFloatTy() ? llvm::Instruction::FAdd : llvm::Instruction::Add;  break;
        case MINUS_EQ : bi_op = left->getType()->isFloatTy() ? llvm::Instruction::FSub : llvm::Instruction::Sub;  break;
        case MUL_EQ   : bi_op = left->getType()->isFloatTy() ? llvm::Instruction::FMul : llvm::Instruction::Mul;  break;
        case DIV_EQ   : bi_op = left->getType()->isFloatTy() ? llvm::Instruction::FDiv : llvm::Instruction::SDiv; break;
    }

    // 赋值
    llvm::Value* res = llvm::BinaryOperator::Create(bi_op, left, right, "", Builder.GetInsertBlock());
    llvm::Value* var = curContext.getVariable(identifier.name);     
    // 函数外单独赋值的全局变量
    if (!curContext.currentFunc) {
        throw logic_error("Variable [" + identifier.name + "] can't be assigned out of function");
    }

    auto CurrentBlock = Builder.GetInsertBlock();
    if (res->getType() != getVarPtrType(var))
        res = typeCast(right, getVarPtrType(var));

    return new llvm::StoreInst(res, var, false, CurrentBlock);
}

// ++i
llvm::Value* PreIncreaseNode::genCode(Context &curContext) {
    llvm::Value* left = identifier.genCode(curContext);
    llvm::Value* right = llvm::ConstantInt::get(llvm::Type::getInt32Ty(GlobalContext), 1, true); // 操作数是1
    llvm::Instruction::BinaryOps bi_op;

    // 左右值类型不等
    balanceType(left, right);

    bi_op = left->getType()->isFloatTy() ? llvm::Instruction::FAdd : llvm::Instruction::Add;

    // 赋值
    llvm::Value* res = llvm::BinaryOperator::Create(bi_op, left, right, "", Builder.GetInsertBlock());
    llvm::Value* var = curContext.getVariable(identifier.name);     
    // 函数外单独赋值的全局变量
    if (!curContext.currentFunc) {
        throw logic_error("Variable [" + identifier.name + "] can't be assigned out of function");
    }

    auto CurrentBlock = Builder.GetInsertBlock();
    if (res->getType() != getVarPtrType(var))
        res = typeCast(right, getVarPtrType(var));

    return new llvm::StoreInst(res, var, false, CurrentBlock);
}

// i++
llvm::Value* PostIncreaseNode::genCode(Context &curContext) {
    llvm::Value* left = identifier.genCode(curContext);
    llvm::Value* right = llvm::ConstantInt::get(llvm::Type::getInt32Ty(GlobalContext), 1, true); // 操作数是1
    llvm::Instruction::BinaryOps bi_op;

    // 左右值类型不等
    balanceType(left, right);

    bi_op = left->getType()->isFloatTy() ? llvm::Instruction::FAdd : llvm::Instruction::Add;

    // 赋值
    llvm::Value* res = llvm::BinaryOperator::Create(bi_op, left, right, "", Builder.GetInsertBlock());
    llvm::Value* var = curContext.getVariable(identifier.name);     
    // 函数外单独赋值的全局变量
    if (!curContext.currentFunc) {
        throw logic_error("Variable [" + identifier.name + "] can't be assigned out of function");
    }

    auto CurrentBlock = Builder.GetInsertBlock();
    if (res->getType() != getVarPtrType(var))
        res = typeCast(right, getVarPtrType(var));

    // 不太妙(好像不能返回前面的值)
    return new llvm::StoreInst(res, var, false, CurrentBlock);
}

llvm::Value* getAddrNode::genCode(Context &curContext) {
    // 在符号表和全局变量中查找
    llvm::Value* var = curContext.getVariable(identifier.name);
    if (!var) {
        throw logic_error("undeclared variable [" + identifier.name + "]");
    }
    return var;
}

llvm::Value* AssignmentNode::genCode(Context &curContext) {
    // 在符号表和全局变量中查找
    llvm::Value* var = curContext.getVariable(identifier.name);
    if (!var) {
        throw logic_error("undeclared variable [" + identifier.name + "]");
    }

    llvm::Value* val = value.genCode(curContext);
    
    // 函数外单独赋值的全局变量
    if (!curContext.currentFunc) {
        throw logic_error("Variable [" + identifier.name + "] can't be assigned out of function");
    }

    auto CurrentBlock = Builder.GetInsertBlock();
    if (val->getType() != getVarPtrType(var))
        val = typeCast(val, getVarPtrType(var));

    return new llvm::StoreInst(val, var, false, CurrentBlock);
}

llvm::Value* BlockNode::genCode(Context &curContext) {
    // 处理 block 内的每一条 statement
    for (auto stmt : statementList) {
        // tmp = 
        (*stmt).genCode(curContext);
        if (curContext.hasReturn == true) {
            break;
        }
    }
    return nullptr;
}

llvm::Value* ExpressionStatementNode::genCode(Context &curContext) {
	expression.genCode(curContext);
    return nullptr;
}

llvm::Value* IfStatementNode::genCode(Context &curContext) {
    
    // initiation
    llvm::Function *curFunc = curContext.currentFunc;
    llvm::BasicBlock *IfBB = llvm::BasicBlock::Create(GlobalContext, "if", curFunc);
    llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(GlobalContext, "afterif", curFunc); // 被 if 分割的语句

    // 跳转判断（false 跳到 后续操作）
    llvm::Value *condValue = expression.genCode(curContext);
    condValue = Builder.CreateICmpNE(condValue, llvm::ConstantInt::get(llvm::Type::getInt1Ty(GlobalContext), 0, true), "ifCond");
    auto branch = Builder.CreateCondBr(condValue, IfBB, ThenBB);

    // 处理 condition == true 的情况
    Builder.SetInsertPoint(IfBB);
    curContext.pushSymbolTable();
    ifBlock.genCode(curContext);
    curContext.popSymbolTable();

    // 处理后续操作
    if (curContext.hasReturn) {
        curContext.hasReturn = false;
    } else {
        // 中间没有跳出的话，执行 afterIfOnly
        Builder.CreateBr(ThenBB);
    }

    Builder.SetInsertPoint(ThenBB);    
    
    return nullptr;
}

llvm::Value* IfElseStatementNode::genCode(Context &curContext) {
    // initiation
    llvm::Function *curFunc = curContext.currentFunc;
    llvm::BasicBlock *IfBB = llvm::BasicBlock::Create(GlobalContext, "if", curFunc);
    llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(GlobalContext, "else", curFunc);
    llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(GlobalContext, "afterifelse", curFunc);

    // 跳转判断（false 跳到 else）
    llvm::Value *condValue = expression.genCode(curContext);
    condValue = Builder.CreateICmpNE(condValue, llvm::ConstantInt::get(llvm::Type::getInt1Ty(GlobalContext), 0, true), "ifCond");
    auto branch = Builder.CreateCondBr(condValue, IfBB, ElseBB);

    // cond = true
    Builder.SetInsertPoint(IfBB);
    curContext.pushSymbolTable();
    ifBlock.genCode(curContext);
    curContext.popSymbolTable();

    if (curContext.hasReturn) {
        curContext.hasReturn = false;
    } else {
        Builder.CreateBr(ThenBB);
    }

    // cond = false
    Builder.SetInsertPoint(ElseBB);
    curContext.pushSymbolTable();
    elseBlock.genCode(curContext);
    curContext.popSymbolTable();

    if (curContext.hasReturn) {
        curContext.hasReturn = false;
    } else {
        Builder.CreateBr(ThenBB);
    }

    // 返回后续操作
    Builder.SetInsertPoint(ThenBB);

    return nullptr;
}

llvm::Value* WhileStatementNode::genCode(Context &curContext){
    // initiation
    llvm::Function *curFunc = curContext.currentFunc;
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(GlobalContext, "cond", curFunc);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(GlobalContext, "loop", curFunc);
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(GlobalContext, "afterLoop", curFunc);
    AfterBBStack.push(afterBB);
    ContBBStack.push(condBB);

    // 跳转到 cond 判断是否进入循环
    Builder.CreateBr(condBB);
    Builder.SetInsertPoint(condBB);

    llvm::Value *condValue = expression.genCode(curContext);
    condValue = Builder.CreateICmpNE(condValue, llvm::ConstantInt::get(llvm::Type::getInt1Ty(GlobalContext), 0, true), "whileCond");
    // true - 进入循环体，false - 进入后续操作
    auto branch = Builder.CreateCondBr(condValue, loopBB, afterBB);
    condBB = Builder.GetInsertBlock();

    // 处理循环体
    Builder.SetInsertPoint(loopBB);
    curContext.pushSymbolTable();
    block.genCode(curContext);
    if (curContext.hasReturn) {
        curContext.hasReturn = false;
    } else {
        Builder.CreateBr(condBB);
    }
    curContext.popSymbolTable();

    // 后续操作
    Builder.SetInsertPoint(afterBB);

    // 复位
    ContBBStack.pop();
    AfterBBStack.pop();

    return nullptr;
}

llvm::Value* ForStatementNode::genCode(Context &curContext) {
    // initiation
    llvm::Function *curFunc = curContext.currentFunc;
    llvm::BasicBlock *initBB = llvm::BasicBlock::Create(GlobalContext, "loopinit", curFunc);
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(GlobalContext, "cond", curFunc);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(GlobalContext, "loop", curFunc);
    llvm::BasicBlock *incBB = llvm::BasicBlock::Create(GlobalContext, "loopinc", curFunc);
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(GlobalContext, "afterloop", curFunc);
    AfterBBStack.push(afterBB);
    ContBBStack.push(incBB);

    // 跳到循环初始化操作
    Builder.CreateBr(initBB);
    Builder.SetInsertPoint(initBB);
    this->initiation.genCode(curContext);

    // 跳到循环条件判断
    Builder.CreateBr(condBB);
    Builder.SetInsertPoint(condBB);

    llvm::Value *condValue = this->condition.genCode(curContext);
    condValue = Builder.CreateICmpNE(condValue, llvm::ConstantInt::get(llvm::Type::getInt1Ty(GlobalContext), 0 , true), "forCond");    
    // ture - 跳进循环体
    auto branch = Builder.CreateCondBr(condValue, loopBB, afterBB);
    condBB = Builder.GetInsertBlock();

    // 处理 inc 操作（完成后回到 cond 进行判断）
    Builder.SetInsertPoint(incBB);
    this->increment.genCode(curContext);
    Builder.CreateBr(condBB);

    // 处理 循环体
    Builder.SetInsertPoint(loopBB);
    curContext.pushSymbolTable();
    this->body.genCode(curContext);
    if (curContext.hasReturn) {
        curContext.hasReturn = false;
    } else { // 先做 inc 再做 cond 判断
        Builder.CreateBr(incBB);
    }
    curContext.popSymbolTable();

    // 后续操作
    Builder.SetInsertPoint(afterBB);

    // 复位
    AfterBBStack.pop();
    ContBBStack.pop();
    
    return nullptr;
}

llvm::Value* ReturnStatementNode::genCode(Context &curContext) {
	llvm::Value *returnVal = expression.genCode(curContext);
    if (returnVal->getType() != getVarPtrType(curContext.returnVal)) {
        returnVal = typeCast(returnVal, getVarPtrType(curContext.returnVal));
    }
    
    // 存储，标记返回
    Builder.CreateStore(returnVal, curContext.returnVal);
    curContext.hasReturn = true;

    Builder.CreateBr(curContext.returnBB);
    return nullptr;
}

llvm::Value* ReturnVoidNode::genCode(Context &curContext) {
    curContext.hasReturn = true;
    Builder.CreateBr(curContext.returnBB);
    return nullptr;
}

llvm::Value* BreakStatementNode::genCode(Context &curContext) {
    curContext.hasReturn = true;
    Builder.CreateBr(AfterBBStack.top());
    return nullptr;
}

llvm::Value* ContinueStatementNode::genCode(Context &curContext) {
    curContext.hasReturn = true;
    llvm::BasicBlock* condTarget = getCondBlock();
    if (condTarget) {
        Builder.CreateBr(condTarget);
    } else {
        throw logic_error("Continue statement should only be used in loops.");
    }
    return nullptr;
}

llvm::Value* VariableDeclarationNode::genCode(Context &curContext) {  
    if (size == 0) { //普通变量
        llvm::Type* llvmType = getLLvmType(type.name);
        if (!curContext.currentFunc) { // 全局变量
            // 检测重复定义
            llvm::Value *tmp = curContext.module->getGlobalVariable(identifier.name, true);
            if (tmp) {
                throw logic_error("Redefined Global Variable: " + identifier.name);
            }

            // 全局变量必须被，马上初始化
            llvm::Constant* Initializer = NULL;
            if (assignmentExpression) {
                // 如果跟随赋值语句，则使用 CONST 进行赋值
                llvm::Value* InitialExpr = assignmentExpression->genCode(curContext);
                Initializer = (llvm::Constant*)InitialExpr;
            } else {
                // 没有赋值语句则初始化为 0
                Initializer = llvm::ConstantInt::get(llvmType, 0);
            }
            
            llvm::GlobalVariable* globalVar = new llvm::GlobalVariable(*(curContext.module), llvmType, false, llvm::GlobalValue::PrivateLinkage, Initializer, identifier.name);

            return nullptr;
            
        } else {
            // 检测重复定义
            if (curContext.getTopSymbolTable()->hasVariable(identifier.name)) {
                // 当前域中有该变量, 重复定义
                throw logic_error("Redefined Local Variable: " + identifier.name);
            }

            // 将新定义的变量类型和地址存入符号表中
            auto *block = Builder.GetInsertBlock();
            llvm::AllocaInst *alloc = new llvm::AllocaInst(llvmType,block->getParent()->getParent()->getDataLayout().getAllocaAddrSpace(),(identifier.name.c_str()), block);
            curContext.getTopSymbolTable()->setVarValue(identifier.name, alloc);
            curContext.getTopSymbolTable()->setVarType(identifier.name, llvmType);

            if (assignmentExpression) { // 赋值
                AssignmentNode assn(identifier, *assignmentExpression, lineNo);
                assn.genCode(curContext);
            }

            return nullptr;
        }
    } else { //数组
        llvm::Type* llvmType = getArrayLLvmType(type.name, size); 
        if (!curContext.currentFunc) { // 全局数组
            // 查重
            llvm::Value *tmp = curContext.module->getGlobalVariable(identifier.name, true);
            if (tmp) {
                throw logic_error("Redefined Global Array: " + identifier.name);
            }

            // 创建全局变量
            llvm::GlobalVariable* globalVar = new llvm::GlobalVariable(*(curContext.module), llvmType, false, llvm::GlobalValue::PrivateLinkage, 0, identifier.name);
            
            // 初始化数组
            std::vector<llvm::Constant*> constArrayElem;
            llvm::Constant* constElem = llvm::ConstantInt::get(llvmType->getArrayElementType(), 0);
            for (int i = 0; i < llvmType->getArrayNumElements(); i++) {
                constArrayElem.push_back(constElem);
            }
            llvm::Constant* constArray = llvm::ConstantArray::get(llvm::ArrayType::get(llvmType->getArrayElementType(), llvmType->getArrayNumElements()), constArrayElem);
            globalVar->setInitializer(constArray);

            return nullptr;
        } else {
            // 当前域中有该变量, 重复定义
            if (curContext.getTopSymbolTable()->hasVariable(identifier.name)) {
                throw logic_error("Redefined Local Variable: " + identifier.name);
            }

            if (curContext.isArgs) { // 函数参数（需要指针类型）
                llvmType = getLLvmPtrType(type.name);
            }
        
            // 存储类型与值
            auto *block = Builder.GetInsertBlock();
            llvm::AllocaInst *alloc = new llvm::AllocaInst(llvmType,block->getParent()->getParent()->getDataLayout().getAllocaAddrSpace(),(identifier.name.c_str()), block);
            curContext.getTopSymbolTable()->setVarValue(identifier.name, alloc);
            curContext.getTopSymbolTable()->setVarType(identifier.name, llvmType);
            
            return nullptr;
        }
    }
}

// 函数定义
llvm::Value* FunctionDefinitionNode::genCode(Context &curContext) {
    // 初始化参数类型列表
    vector<llvm::Type*> argTypes;
    for(auto it : args){
        if (it->size == 0) {
            argTypes.push_back(getLLvmType(it->type.name));
        } else {
            argTypes.push_back(getLLvmPtrType(it->type.name));
        }
    }

    // 查看先前是否声明，避免重复声明（不允许同名函数）
    llvm::Function *function = curContext.module->getFunction(identifier.name.c_str());
    llvm::FunctionType *fType;
    if (!function) {
        fType = llvm::FunctionType::get(getLLvmType(type.name), makeArrayRef(argTypes), false);
        function = llvm::Function::Create(fType, llvm::GlobalValue::ExternalLinkage, identifier.name.c_str(), curContext.module);
	} else {
        fType = function->getFunctionType();
        if(fType != llvm::FunctionType::get(getLLvmType(type.name), makeArrayRef(argTypes), false)) {
            throw logic_error("Unmatch return/param type for FUNC[" + identifier.name + "]");
        }
    }
    
    // initiation
    curContext.currentFunc = function;
	llvm::BasicBlock *bBlock = llvm::BasicBlock::Create(GlobalContext, "entry", function, 0);
    curContext.returnBB = llvm::BasicBlock::Create(GlobalContext, "return", function, 0);
    if (type.name.compare("void") != 0) { // 用于处理返回值
        curContext.returnVal = new llvm::AllocaInst(getLLvmType(type.name), bBlock->getParent()->getParent()->getDataLayout().getAllocaAddrSpace(), "", bBlock);
    }

	curContext.pushSymbolTable();
    Builder.SetInsertPoint(bBlock);

    // 处理形参列表
	llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value* argumentValue;
    curContext.isArgs = true;
    for (auto it : args) {
        (*it).genCode(curContext);
        argumentValue = &*argsValues++;
        argumentValue->setName((it)->identifier.name.c_str());
        llvm::StoreInst *inst = new llvm::StoreInst(argumentValue, curContext.getTopSymbolTable()->getVarValue((it)->identifier.name) , false, bBlock);
	}
    curContext.isArgs = false;
	
    // 处理函数体
	block.genCode(curContext);
    curContext.hasReturn = false;

    // 处理预返回阶段
    Builder.SetInsertPoint(curContext.returnBB);
    if (type.name.compare("void") == 0) {
        Builder.CreateRetVoid();
    } else {
        llvm::Value* ret = Builder.CreateLoad(getLLvmType(type.name), curContext.returnVal, "");
        Builder.CreateRet(ret);
    }

    // 复位
	curContext.popSymbolTable();
    curContext.currentFunc = nullptr;

	return nullptr;
}

// 函数声明
llvm::Value* FunctionDeclarationNode::genCode(Context &curContext){
    // 初始化参数类型列表
    vector<llvm::Type*> argTypes;
    for (auto it : args) {
        if (it->size == 0) {
            argTypes.push_back(getLLvmType(it->type.name));
        } else {
            argTypes.push_back(getLLvmPtrType(it->type.name));
        }
    }

    // 创建函数
	llvm::FunctionType *fType = llvm::FunctionType::get(getLLvmType(type.name), makeArrayRef(argTypes), false);
	llvm::Function *function = llvm::Function::Create(fType, llvm::GlobalValue::ExternalLinkage, identifier.name.c_str(), curContext.module);
	
	return nullptr;
}

vector<llvm::Value *> *getPrintfArgs(Context &curContext,vector<ExpressionNode*>args) {

    vector<llvm::Value*> *printf_args = new vector<llvm::Value *>;
    for (auto it: args) {
        llvm::Value* tmp = it->genCode(curContext);
        if (tmp->getType() == llvm::Type::getFloatTy(GlobalContext)) {
            tmp = Builder.CreateFPExt(tmp, llvm::Type::getDoubleTy(GlobalContext), "tmpdouble");
        }
            
        printf_args->push_back(tmp);
    }

    return printf_args;
}

vector<llvm::Value *> *getScanfArgs(Context &curContext,vector<ExpressionNode*>args) {
    vector<llvm::Value *> *scanf_args = new vector<llvm::Value *>;
    for (auto it: args) {
        llvm::Value* tmp = it->genCode(curContext);
        scanf_args->push_back(tmp);
    }

    return scanf_args;
}

vector<llvm::Value *> *getGetsArgs(Context &curContext,vector<ExpressionNode*>args) {
    vector<llvm::Value *> *gets_args = new vector<llvm::Value *>;
    for (auto it: args) {
        llvm::Value* tmp = it->genCode(curContext);
        gets_args->push_back(tmp);
    }

    return gets_args;
}

vector<llvm::Value *> *getScanfArgsAddr(Context &curContext,vector<ExpressionNode*>args) {
    vector<llvm::Value *> *scanf_args = new vector<llvm::Value *>;
    for (auto it: args) {
        llvm::Value* tmp = it->getAddr(curContext);
        scanf_args->push_back(tmp);
    }

    return scanf_args;
}

// 函数调用
llvm::Value* FunctionCallNode::genCode(Context &curContext) {
    // 调用内置函数
    if (identifier.name.compare("printf") == 0) { 
        vector<llvm::Value *> *printf_args = getPrintfArgs(curContext, args);    
        return Builder.CreateCall(curContext.printf, *printf_args, "printf");
    } else if (identifier.name.compare("scanf") == 0) { 
        vector<llvm::Value *> *scanf_args = getScanfArgs(curContext, args);    
        return Builder.CreateCall(curContext.scanf, *scanf_args, "scanf");
    } else if (identifier.name.compare("gets") == 0) {
        vector<llvm::Value *> *gets_args = getGetsArgs(curContext, args);
        return Builder.CreateCall(curContext.gets, *gets_args, "gets");
    }

    //  查找自定义函数
    llvm::Function *func = curContext.module->getFunction(identifier.name.c_str());
    if (!func) {
        throw logic_error("undeclared function [" + identifier.name + "]");
	}

    // 处理实参
    vector<llvm::Value*> argVals;
    vector<ExpressionNode*>::iterator i;
    for (auto i : args) { 
        argVals.push_back((*i).genCode(curContext));
    }

    // 调用（返回结果存储在 %1）
    return llvm::CallInst::Create(func,llvm::makeArrayRef(argVals),"",Builder.GetInsertBlock());
}