%{
    #include <cstdio>
    #include <cstdlib>
    #include <sstream>
    
    #include "node.h"
    
    #define YYERROR_VERBOSE 1

	BlockNode *prog;

	extern int yylex();
    extern int yylineno;

    void yyerror(const char *errMsg) { std::printf("Error: %s @ line %d\n", errMsg, yylineno); }

    // 进制转换
    int HtoD(string *s) {
        int d_int;
        stringstream ss;
        ss << hex << *s;
        ss >> d_int;
        return d_int;
    }

    int OtoD(string *s) {
        int d_int;
        stringstream ss;
        ss << oct << *s;
        ss >> d_int;
        return d_int;
    }
%}

%define parse.error verbose

%union {
    int token;
    int constInt;
    std::string *string;
	
    TypeNode *type;
	IdentifierNode *identifier;

    TreeNode *node;
	BlockNode *block;
    StatementNode *statement;
    ExpressionNode *expression;
    
	std::vector<ExpressionNode*> *expList;
    VariableDeclarationNode *varDeclaration;
	std::vector<VariableDeclarationNode*> *varDeclarationList; // int i, int j, float a
}
	

%token <string> IDENTIFIER TYPE
%token <string> CONST_HEX CONST_OCT CONST_DEC CONST_FLOAT CONST_CHAR CONST_STRING
%token <token> PLUS MINUS MUL DIV INC
%token <token> PLUS_EQ MINUS_EQ MUL_EQ DIV_EQ
%token <token> AND OR                           // &&  ||
%token <token> GAD                              // &
%token <token> EQU NEQ LESST GREATERT LEQ GEQ   // == !=  <  >  <=  >=
%token <token> RETURN
%token <token> SEMI COMMA ASSIGN                // ; , =
%token <token> L_BRACE R_BRACE                  // {}
%token <token> L_ROUND_BRAC R_ROUND_BRAC        // ()
%token <token>  L_SQUARE_BRAC R_SQUARE_BRAC     // []
%token <token> IF ELSE WHILE BREAK CONTINUE FOR

%left AND OR
%left EQU NEQ LESST GREATERT LEQ GEQ
%left PLUS MINUS MUL DIV
%left L_ROUND_BRAC R_ROUND_BRAC L_SQUARE_BRAC R_SQUARE_BRAC

%type <identifier> identifier
%type <type> type
%type <expression> const expression
%type <constInt> const_int
%type <statement> statement return_statement while_statement if_statement for_statement empty_statement
%type <statement> var_decl func_decl func_def 
%type <varDeclarationList> params
%type <expList> args
%type <block> program statements block

%start program

%%

program:        
    statements                                                                  {   prog = $1; }
    ;                                                   
                    
statements:                                                     
    statement                                                                   {
                                                                                    $$ = new BlockNode(yylineno); 
                                                                                    $$->statementList.push_back($<statement>1);
                                                                                }
    | statements statement                                                      {   $1->statementList.push_back($<statement>2); }
    ;                                       

statement: 
    var_decl SEMI                                                               {   $$ = $1; }
    | func_decl SEMI                                                            {   $$ = $1; }
    | func_def                                                                  {   $$ = $1; }
    | while_statement                                                           {   $$ = $1; }   
    | if_statement                                                              {   $$ = $1; }
    | for_statement                                                             {   $$ = $1; }
    | return_statement                                                          {   $$ = $1; }   
    | empty_statement                                                           {   $$ = $1; }
    | expression SEMI                                                           {   $$ = new ExpressionStatementNode(*$1, yylineno); }
    ;

return_statement:
    RETURN SEMI                                                                 {   $$ = new ReturnVoidNode(yylineno); }
    | RETURN expression SEMI                                                    {   $$ = new ReturnStatementNode(*$2, yylineno); }
    ;

while_statement:
    WHILE L_ROUND_BRAC expression R_ROUND_BRAC block                            {   $$ = new WhileStatementNode(*$3, *$5, yylineno); }
    | CONTINUE SEMI                                                             {   $$ = new ContinueStatementNode(yylineno); }
    | BREAK SEMI                                                                {   $$ = new BreakStatementNode(yylineno); }
    ;

if_statement:
    IF L_ROUND_BRAC expression R_ROUND_BRAC block                               {   $$ = new IfStatementNode(*$3, *$5, yylineno); }
    | IF L_ROUND_BRAC expression R_ROUND_BRAC block ELSE block                  {   $$ = new IfElseStatementNode(*$3, *$5, *$7, yylineno); }
    ;

for_statement:
    FOR L_ROUND_BRAC expression SEMI expression SEMI expression R_ROUND_BRAC block     {   $$ = new ForStatementNode(*$3, *$5, *$7, *$9, yylineno);}
    ;

empty_statement:
    SEMI                                                                        {   $$ = new EmptyStatementNode(yylineno); }
    ;

block:                          
    L_BRACE statements R_BRACE                                                  {   $$ = $2; }
    | L_BRACE R_BRACE                                                           {   $$ = new BlockNode(yylineno); }
    ;

var_decl:
    type identifier                                                             {   $$ = new VariableDeclarationNode(*$1, *$2, yylineno); }
    | type identifier ASSIGN expression                                         {   $$ = new VariableDeclarationNode(*$1, *$2, $4, yylineno); }
    | type identifier L_SQUARE_BRAC const_int R_SQUARE_BRAC                     {   $$ = new VariableDeclarationNode(*$1, *$2, $4, yylineno); }
    ;

func_def: 
    type identifier L_ROUND_BRAC params R_ROUND_BRAC block                      {   $$ = new FunctionDefinitionNode(*$1, *$2, *$4, *$6, yylineno); }
    ;

func_decl: 
    type identifier L_ROUND_BRAC params R_ROUND_BRAC                            {   $$ = new FunctionDeclarationNode(*$1, *$2, *$4, yylineno); }
    ;
    
params:
    {   $$ = new std::vector<VariableDeclarationNode*>(); }
    | var_decl                                                                  {
                                                                                    $$ = new std::vector<VariableDeclarationNode*>();
                                                                                    $$->push_back($<varDeclaration>1);
                                                                                }
    | params COMMA var_decl                                                     {   $1->push_back($<varDeclaration>3);}
    ;

identifier:
    IDENTIFIER                                                                  {   $$ = new IdentifierNode(*$1, yylineno); }
    ;

type:
    TYPE                                                                        {   $$ = new TypeNode(*$1, yylineno); }
    ;

const: 
    const_int                                                                   {   $$ = new IntNode($1, yylineno); }
    | CONST_FLOAT                                                               {   $$ = new FloatNode(atof($1->c_str()), yylineno); }
    | CONST_CHAR                                                                {   $$ = new CharNode(*$1, yylineno); }
    | CONST_STRING                                                              {   $$ = new StringNode(*$1, yylineno); }
    ;

const_int:
    CONST_DEC                                                                   {   $$ = atoi($1->c_str()); }
    | CONST_HEX                                                                 {   $$ = HtoD($1); }
    | CONST_OCT                                                                 {   $$ = OtoD($1); }
    ;

args:
    {   $$ = new std::vector<ExpressionNode*>(); }
    | expression                                                                {
                                                                                    $$ = new std::vector<ExpressionNode*>();
                                                                                    $$->push_back($1);
                                                                                }
    | args COMMA expression                                                     {   $1->push_back($3); }
    ;

expression:
    identifier ASSIGN expression                                                {   $$ = new AssignmentNode(*$1, *$3, yylineno); }
    | identifier L_ROUND_BRAC args R_ROUND_BRAC                                 {   $$ = new FunctionCallNode(*$1, *$3, yylineno); }
    | identifier                                                                {   $$ = $1; }
    | GAD identifier                                                            {   $$ = new getAddrNode(*$2, yylineno); }
    | GAD identifier L_SQUARE_BRAC expression R_SQUARE_BRAC                     {   $$ = new getArrayAddrNode(*$2, *$4, yylineno); }
    | identifier INC                                                            {   $$ = new PostIncreaseNode(*$1, yylineno);      }
    | INC identifier                                                            {   $$ = new PreIncreaseNode(*$2, yylineno);       }
    | identifier PLUS_EQ expression                                             {   $$ = new BinaryOpEqualNode($2, *$1, *$3, yylineno);}
    | identifier MINUS_EQ expression                                            {   $$ = new BinaryOpEqualNode($2, *$1, *$3, yylineno);}
    | identifier MUL_EQ expression                                              {   $$ = new BinaryOpEqualNode($2, *$1, *$3, yylineno);}    
    | identifier DIV_EQ expression                                              {   $$ = new BinaryOpEqualNode($2, *$1, *$3, yylineno);}
    | expression MUL expression                                                 {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression DIV expression                                                 {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression PLUS expression                                                {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression MINUS expression                                               {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression AND expression                                                 {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression OR expression                                                  {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression LESST expression                                               {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression GREATERT expression                                            {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression EQU expression                                                 {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression NEQ expression                                                 {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression LEQ expression                                                 {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | expression GEQ expression                                                 {   $$ = new BinaryOpNode($2, *$1, *$3, yylineno); }
    | L_ROUND_BRAC expression R_ROUND_BRAC                                      {   $$ = $2; }
    | identifier L_SQUARE_BRAC expression R_SQUARE_BRAC                         {   $$ = new ArrayElementNode(*$1, *$3, yylineno); }
    | identifier L_SQUARE_BRAC expression R_SQUARE_BRAC ASSIGN expression       {   $$ = new ArrayElementAssignNode(*$1, *$3, *$6, yylineno); }
    | const                                                                     {   $$ = $1; }
    ;       
%%