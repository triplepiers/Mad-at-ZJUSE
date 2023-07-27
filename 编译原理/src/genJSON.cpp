#include <string>

#include "node.h"
#include "parse.hpp"

using namespace std;

void IntNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"IntValue\",\n");
    s.append("\"children\" : [{ \"name\" : \"" + to_string(this->value) + "\" }]\n");
    s.append("}");
}

void FloatNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"FloatValue\",\n");
    s.append("\"children\" : [{ \"name\" : \"" + to_string(this->value) + "\" }]\n");
    s.append("}");
}

void CharNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"CharValue\",\n");
    s.append("\"children\" : [{ \"name\" : \"" + this->value.substr(1, this->value.length() - 2) + "\" }]\n");
    s.append("}");
}

void StringNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"StringValue\",\n");
    s.append("\"children\" : [{ \"name\" : \"" + this->value.substr(1, this->value.length() - 2) + "\" }]\n");
    s.append("}");
}

void IdentifierNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"Identifier\",\n");
    s.append("\"children\" : [{ \"name\" : \"" + this->name + "\" }]\n");
    s.append("}");
}

void TypeNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"Type\",\n");
    s.append("\"children\" : [{ \"name\" : \"" + this->name + "\" }]\n");
    s.append("}");
}

void ArrayElementNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"ArrayElement\",\n");
    s.append("\"children\" : [");
    
    this->identifier.genJSON(s);
    s.append(",");
    this->index.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void ArrayElementAssignNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"ArrayElementAssign\",\n");
    s.append("\"children\" : [");
    
    this->identifier.genJSON(s);
    s.append(",");
    this->index.genJSON(s);
    s.append(",");
    this->value.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void FunctionCallNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"FunctionCall\",\n");
    s.append("\"children\" : [");
    
    // 函数名
    s.append("\n{\n");
    s.append("\"name\" : \"FunctionName\",\n");
    s.append("\"children\" : [{ \"name\" : \"" + this->identifier.name + "\" }]\n");
    s.append("}");
    
    // 参数
    if (!this->args.empty()) {
        s.append(",\n{");
        s.append("\"name\" : \"Arguments\",\n");
        s.append("\"children\" : [\n");
        for (TreeNode* node : this->args) {
            node->genJSON(s);
            if (node != this->args.back()) {
                s.append(",");
            }
        }
        s.append("]\n}");
    }
    

    s.append("\n]\n");
    s.append("}");
}

void BinaryOpNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"BinaryOperation\",\n");
    s.append("\"children\" : [");
    
    // left oprand
    this->lhs.genJSON(s);
    s.append(",");

    // operator
    s.append("\n{\n");
    s.append("\"name\" : \"Operator\",\n");
    s.append("\"children\" : [{ \"name\" : \"");
    switch (this->op) {
        case PLUS :     { s.append("+");    break; }
        case MINUS :    { s.append("-");    break; }
        case MUL :      { s.append("*");    break; }
        case DIV :      { s.append("/");    break; }
        case OR :       { s.append("||");   break; }
        case AND :      { s.append("&&");   break; }
        case EQU :      { s.append("==");   break; }
        case NEQ :      { s.append("!=");   break; }
        case LESST :    { s.append("<");    break; }
        case GREATERT : { s.append(">");    break; }
        case LEQ :      { s.append("<=");   break; }
        case GEQ :      { s.append(">=");   break; }
        default:        { s.append(" ");    break; } //"ERROR"
    }
    s.append("\" }]\n");
    s.append("},");

    // right oprand
    this->rhs.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void BinaryOpEqualNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"BinaryOperation\",\n");
    s.append("\"children\" : [");
    
    // left oprand
    this->identifier.genJSON(s);
    s.append(",");

    // operator
    s.append("\n{\n");
    s.append("\"name\" : \"Operator\",\n");
    s.append("\"children\" : [{ \"name\" : \"");
    switch (this->op) {
        case PLUS_EQ :     { s.append("+=");    break; }
        case MINUS_EQ :    { s.append("-=");    break; }
        case MUL_EQ :      { s.append("*=");    break; }
        case DIV_EQ :      { s.append("/=");    break; }
        default:        { s.append(" ");    break; } //"ERROR"
    }
    s.append("\" }]\n");
    s.append("},");

    // right oprand
    this->expression.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void PostIncreaseNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"" + this->identifier.name +"++\"");
    s.append("}");
}

void PreIncreaseNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"++" + this->identifier.name +"\"");
    s.append("}");
}

void getAddrNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"getAddr\",\n");
    s.append("\"children\" : [");
    
    this->identifier.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void getArrayAddrNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"getArrayAddr\",\n");
    s.append("\"children\" : [");
    
    this->identifier.genJSON(s);

    s.append(",");

    this->index.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void AssignmentNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"Assignment\",\n");
    s.append("\"children\" : [");
    
    this->identifier.genJSON(s);
    s.append(",");
    this->value.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void BlockNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"Block\",\n");
    s.append("\"children\" : [");
    for (TreeNode* node : this->statementList) {
        node->genJSON(s);
        if (node != this->statementList.back()) {
            s.append(",");
        }
    }
    s.append("\n]\n");
    s.append("}");
}

void ExpressionStatementNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"ExpressionStatement\",\n");
    s.append("\"children\" : [");
    this->expression.genJSON(s);
    s.append("\n]\n");
    s.append("}");
}

void BreakStatementNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"break\", \n");
    s.append("}");
}

void ContinueStatementNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"continue\", \n");
    s.append("}");
}

void IfStatementNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"IfStatement\",\n");
    s.append("\"children\" : [");
    
    this->expression.genJSON(s);
    s.append(",");
    this->ifBlock.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void IfElseStatementNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"IfElseStatement\",\n");
    s.append("\"children\" : [");
    
    this->expression.genJSON(s);
    s.append(",");
    this->ifBlock.genJSON(s);
    s.append(",");
    this->elseBlock.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void WhileStatementNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"WhileStatement\",\n");
    s.append("\"children\" : [");
    
    this->expression.genJSON(s);
    s.append(",");
    this->block.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void ReturnStatementNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"ReturnStatement\",\n");
    s.append("\"children\" : [");
    this->expression.genJSON(s);
    s.append("\n]\n");
    s.append("}");
}

void ReturnVoidNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"ReturnVoid\",\n");
    s.append("\"children\" : [");
    s.append("\n]\n");
    s.append("}");
}


void VariableDeclarationNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"VariableDeclaration\",\n");
    s.append("\"children\" : [");
    
    this->type.genJSON(s);
    s.append(",");
    this->identifier.genJSON(s);

    if (this->assignmentExpression != nullptr) {
        s.append(",");
        this->assignmentExpression->genJSON(s);
    }

    s.append("\n]\n");
    s.append("}");
}

void FunctionDefinitionNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"FunctionDefinitionNode\",\n");
    s.append("\"children\" : [");
    
    // function type
    this->type.genJSON(s);
    s.append(",");

    // functino name
    this->identifier.genJSON(s);
    s.append(",");

    // params
    if (!this->args.empty()) {
        s.append("{\n");
        s.append("\"name\" : \"Params\",\n");
        s.append("\"children\" : [");
        for (TreeNode* node : this->args) {
            node->genJSON(s);
            s.append(",");
        }
        s.append("]},");
    }
    
    // function body
    this->block.genJSON(s);

    s.append("\n]\n");
    s.append("}");
}

void FunctionDeclarationNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"FunctionDeclarationNode\",\n");
    s.append("\"children\" : [");
    
    // function type
    this->type.genJSON(s);
    s.append(",");

    // functino name
    this->identifier.genJSON(s);
    s.append(",");

    // params
    if (!this->args.empty()) {
        s.append("{\n");
        s.append("\"name\" : \"Params\",\n");
        s.append("\"children\" : [");
        for (TreeNode* node : this->args) {
            node->genJSON(s);
            s.append(",");
        }
        s.append("]},");
    }
    
    s.append("\n]\n");
    s.append("}");
}

void EmptyStatementNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"EmptyStatementNode\",\n");
    s.append("}");
}

void ForStatementNode::genJSON(string &s) {
    s.append("\n{\n");
    s.append("\"name\" : \"ForStatementNode\",\n");
    s.append("\"children\" : [");
    
    this->initiation.genJSON(s);
    s.append(",");

    this->condition.genJSON(s);
    s.append(",");

    this->increment.genJSON(s);
    s.append(",");

    this->body.genJSON(s);

    s.append("]");
    s.append("}");
}