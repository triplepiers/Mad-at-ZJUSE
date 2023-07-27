#include "context.h"

llvm::LLVMContext GlobalContext;          // 全局 Context
llvm::IRBuilder<> Builder(GlobalContext); // 全局 IRbuilder
stack<llvm::BasicBlock *> AfterBBStack;   // afterloop 标签构成的栈 -> 记录 break 返回位置
stack<llvm::BasicBlock *> ContBBStack;    // cond/loopinc 标签构成的栈 -> 记录 continue 返回位置

// 用于获取 continue 的对象
llvm::BasicBlock* getCondBlock() {
    if (ContBBStack.size() == 0) {
        return nullptr;
    } else {
        return ContBBStack.top();
    }
}

bool symbolTable::hasVariable(string varName) {
    return local_var.count(varName) != 0;
}

void symbolTable::setVarValue(string varName, llvm::Value* varValue) {
    local_var[varName] = varValue;
    return;
}

llvm::Value* symbolTable::getVarValue(string varName) {
    return local_var[varName];
}

void symbolTable::setVarType(string varName, llvm::Type* varType) {
    local_var_type[varName] = varType;
    return;
}

// 获取栈顶符号表
symbolTable* Context::getTopSymbolTable() {  
    return symbolTable_stack.back(); 
}

void Context::pushSymbolTable(){
    symbolTable_stack.push_back(new symbolTable());
}

void Context::popSymbolTable(){
    symbolTable *tmp = symbolTable_stack.back();
    symbolTable_stack.pop_back();
    delete tmp;
}

llvm::Value* Context::getVariable(string variableName) {
    
    // 首先按照作用域由近到远查找局部变量
    vector<symbolTable*>::reverse_iterator riter = symbolTable_stack.rbegin();
    for (; riter != symbolTable_stack.rend(); ++riter)
    {
        symbolTable* curTable = *riter;
        if(curTable->local_var.find(variableName) != curTable->local_var.end())
            return curTable->local_var[variableName];
    }

    // 若没有局部变量, 则查找全局变量
    return this->module->getGlobalVariable(variableName, true);
}

llvm::Function* Context::getPrintf() {
    vector<llvm::Type*> printf_arg_types;               //printf内参数的类型
    printf_arg_types.push_back(Builder.getInt8PtrTy()); //char* -> 第一个参数是字符串

    llvm::FunctionType* printf_type = llvm::FunctionType::get(Builder.getInt32Ty(),printf_arg_types,true);
    llvm::Function* printf_func = llvm::Function::Create(printf_type,llvm::Function::ExternalLinkage,llvm::Twine("printf"),this->module);

    printf_func->setCallingConv(llvm::CallingConv::C);
    return printf_func;
}

llvm::Function* Context::getScanf() {
    llvm::FunctionType* scanf_type = llvm::FunctionType::get(Builder.getInt32Ty(),true);
    llvm::Function* scanf_func = llvm::Function::Create(scanf_type,llvm::Function::ExternalLinkage,llvm::Twine("scanf"),this->module);

    scanf_func->setCallingConv(llvm::CallingConv::C);
    return scanf_func;
}

llvm::Function* Context::getGets() {
    llvm::FunctionType* gets_type = llvm::FunctionType::get(Builder.getInt32Ty(),true);
    llvm::Function* gets_func = llvm::Function::Create(gets_type,llvm::Function::ExternalLinkage,llvm::Twine("gets"),this->module);

    gets_func->setCallingConv(llvm::CallingConv::C);
    return gets_func;
}


void Context::genCode(BlockNode* Root){
    Root->genCode(*this);
    llvm::verifyModule(*this->module, &llvm::outs());
    this->module->print(llvm::outs(), nullptr);
}

Context::Context(){
    this->module = new llvm::Module("main", GlobalContext);
    // 初始化库函数
    this->printf = getPrintf();
    this->scanf = getScanf();
    this->gets = getGets();
    // 其余分量初始化
    this->currentFunc = nullptr;
    this->hasReturn = false;
    this->isArgs = false;
}

