%{
#include <stdio.h>
#include <iostream>
#include <string>

#include "node.h"
#include "parse.hpp"

#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
%}

%option yylineno

%%
    /* comment */
"//"[^\n]*                              { }

    /* keywords */
"while"                                 { return TOKEN(WHILE);    }
"break"                                 { return TOKEN(BREAK);    }
"continue"                              { return TOKEN(CONTINUE); }
"if"                                    { return TOKEN(IF);       }
"else"                                  { return TOKEN(ELSE);     }
"for"                                   { return TOKEN(FOR);      }
"return"                                { return TOKEN(RETURN);   }


    /* support types */
"int"|"float"|"char"|"void"             { SAVE_TOKEN; return TYPE; }

    /* identifier */
[a-zA-Z_]([a-zA-Z_]|[0-9])*             { SAVE_TOKEN; return IDENTIFIER; }

    /* constant */
[-+]?0[xX][a-fA-F0-9]+                  { SAVE_TOKEN; return CONST_HEX; }
[-+]?0[0-7]*                            { SAVE_TOKEN; return CONST_OCT; }
[-+]?[1-9][0-9]*                        { SAVE_TOKEN; return CONST_DEC; }
[0-9]+([Ee][+-]?[0-9]+)                 { SAVE_TOKEN; return CONST_FLOAT; }
[0-9]*"."[0-9]+([Ee][+-]?[0-9]+)?       { SAVE_TOKEN; return CONST_FLOAT; }
[0-9]+"."[0-9]*([Ee][+-]?[0-9]+)?       { SAVE_TOKEN; return CONST_FLOAT; }
\'.\'|\'\\.\'                           { SAVE_TOKEN; return CONST_CHAR; }
\"(\\.|[^"\\])*\"                       { SAVE_TOKEN; return CONST_STRING; }

    /* operators */
"&&"                                    { return TOKEN(AND); }
"||"                                    { return TOKEN(OR); }

"<="                                    { return TOKEN(LEQ); }
">="                                    { return TOKEN(GEQ); }
"=="                                    { return TOKEN(EQU); }
"!="                                    { return TOKEN(NEQ); }
"<"                                     { return TOKEN(LESST); }
">"                                     { return TOKEN(GREATERT); }

"++"                                    { return TOKEN(INC); }
"+="                                    { return TOKEN(PLUS_EQ); }
"-="                                    { return TOKEN(MINUS_EQ); }
"*="                                    { return TOKEN(MUL_EQ); }
"/="                                    { return TOKEN(DIV_EQ); }
"-"                                     { return TOKEN(MINUS); }
"+"                                     { return TOKEN(PLUS); }
"*"                                     { return TOKEN(MUL); }
"/"                                     { return TOKEN(DIV); }

","                                     { return TOKEN(COMMA); }
";"                                     { return TOKEN(SEMI); }
"="                                     { return TOKEN(ASSIGN); }
"{"                                     { return TOKEN(L_BRACE); }
"}"                                     { return TOKEN(R_BRACE); }
"("                                     { return TOKEN(L_ROUND_BRAC); }
")"                                     { return TOKEN(R_ROUND_BRAC); }
"["                                     { return TOKEN(L_SQUARE_BRAC); }
"]"                                     { return TOKEN(R_SQUARE_BRAC); }
"&"                                     { return TOKEN(GAD); }

[ \t\v\n\f]                             { }
.                                       { printf("unknown token : %s in line: %d\n", yytext, yylineno); }

%%

int yywrap(void) {
    return 1;
}