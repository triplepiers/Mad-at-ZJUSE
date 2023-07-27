#include <fstream>

#include <llvm/Support/TargetSelect.h>

#include "context.h"
#include "node.h"

extern int yyparse(void);
extern BlockNode* prog;   // root node of program

int main() {

    // parse input
    yyparse();
    
    // gen AST(data.js)
    string jsonStr = "";
    prog->genJSON(jsonStr);
    ofstream fout;
    fout.open("../visual/data.js");
    fout << ("const data =");
    fout << jsonStr;
    fout << (";\nexport default { data }");

    // gen IR(.ll)
    Context* generator = new Context();

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    generator->genCode(prog);

    return 0;

}