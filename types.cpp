//
// Created by Gal Ishai on 7/17/24.
//

#include "hw3_output.hpp"
#include "types.h"
#include "symbol_table.h"
#include "general.h"
#include "cg.hpp"

#define MAX_BYTE 255

extern ScopeSymbolTable scopeSymbolTable;
extern int yylineno;
extern CodeBuffer buffer;
extern general codeGenerator;

using namespace std;

Exp::Exp(Node *node1, Node *node2, const std::string op, const std::string type1): isVar(false)
{
    Exp* exp1 = dynamic_cast<Exp *>(node1);
    Exp* exp2 = dynamic_cast<Exp *>(node2);
    if(type1 == "bool")
    {
        if(exp1->type != "bool" || exp2->type != "bool")
        {
            output::errorMismatch(yylineno);
            exit(0);
        }
        this->type = "bool";
        true_label = buffer.freshLabel();
        false_label = buffer.freshLabel();
        next_label = buffer.freshLabel();
    }
    else if(type1 == "binop")
    {
        if((exp1->type != "int" && exp1->type != "byte")||(exp2->type != "int" && exp2->type != "byte"))
        {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if (exp1->type == "int" || exp2->type == "int") {
            this->type = "int";
        } else {
            this->type = "byte";
        }
    }
    else if(type1 == "relop")
    {
        if((exp1->type != "int" && exp1->type != "byte")||(exp2->type != "int" && exp2->type != "byte"))
        {
            output::errorMismatch(yylineno);
            exit(0);
        }
        this->type = "bool";
    }
}

Exp::Exp(Node *node, bool isVar): Node(), isVar(isVar)
{
    if(!scopeSymbolTable.is_symbol_exist(node->value) && isVar)
    {
        output::errorUndef(yylineno, node->value);
        exit(0);
    }
    Symbol* s = scopeSymbolTable.get_symbol(node->value);
    value = node->value;
    type = s->type;
}

Exp::Exp(const string type, Node *terminal): Node(terminal->value), type(type), isVar(false)
{
    if(type == "byte")
    {
        if(stoi(terminal->value) > MAX_BYTE)
        {
            output::errorByteTooLarge(yylineno, terminal->value);
            exit(0);
        }
    }
    if(type == "string")
    {
        string str = exp->value;
        str.pop_back();
        string temp_reg = codeGenerator.globalFreshVar();
        buffer.emit(temp_reg + " = constant [" + to_string(str.length())) + " x i8] c" + str + "\\00\"");
        reg.replace(0, 1, "%");
        buffer.emit(temp_reg + ".ptr = getelementptr [" + to_string(str.length())) + " x i8]");
        reg = temp_reg + ".ptr";
    }
    else if(type == "byte" || type == "int")
    {
        reg = codeGenerator.freshVar();
        buffer.emit(reg + " = add i32 " + value + ", 0");
    }
    else if(type == "bool")
    {
        reg = codeGenerator.freshVar();
        if(terminal->value == "true")
        {
            buffer.emit(reg + " = add i32 1, 0");
        }
        else if(terminal->value == "false")
        {
            buffer.emit(reg + " = add i32 0, 0");
        }

    }
}

/*Exp::Exp(Exp *exp): Node(exp->value)
{

    /*if(!scopeSymbolTable.is_symbol_exist(node->value))
    {
        if(DEBUG)
        {
            std::cout << "1" << std::endl;
        }
        output::errorUndef(yylineno, node->value);
        exit(0);
    }
    Symbol* s = scopeSymbolTable.get_symbol(node->value);
    type = exp->type;
}*/

Exp::Exp(Type* type1, Exp* exp): Node(exp->value), type(type1->type)
{
    /*if (!scopeSymbolTable.is_symbol_exist(exp->value))
    {
        if (DEBUG)
        {
            std::cout << "2" << std::endl;
        }
        output::errorUndef(yylineno, exp->value);
        exit(0);
    }
    Symbol *s = scopeSymbolTable.get_symbol(exp->value);
     */
    if(type1->type == "bool")
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (!((type1->type == exp->type) || (type1->type == "int" && exp->type == "byte") ||
          (type1->type == "byte" && exp->type == "int")))
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    /*if(DEBUG)
    {
        std::cout << "* node value: " << node->value << "\n" << std::endl;
    }*/
}


string stringToUpper(const string &s)
{
    if (s == "void")
        return "VOID";
    else if (s == "int")
        return "INT";
    else if (s == "byte")
        return "BYTE";
    else if (s == "bool")
        return "BOOL";
    else
        return "STRING";
}


Call::Call(Node *funcID, Node *node)
{
    if(!scopeSymbolTable.is_symbol_exist(funcID->value))
    {
        output::errorUndefFunc(yylineno, funcID->value);
        exit(0);
    }
    Symbol* symbol = scopeSymbolTable.get_symbol(funcID->value);
    if(!symbol->function)
    {
        output::errorUndefFunc(yylineno, funcID->value);
        exit(0);
    }
    Exp *exp = dynamic_cast<Exp *>(node);
    if ( exp->type != symbol->param)
    {
        if (symbol->param != "int" || exp->type != "byte")
        {
            output::errorPrototypeMismatch(yylineno, funcID->value, stringToUpper(symbol->param));
            exit(0);
        }
    }
    this->value = symbol->name;
    this->type = symbol->type;
}

/*
Call::Call(Node* funcID)
{
    if(!scopeSymbolTable.is_symbol_exist(funcID->value))
    {
        output::errorUndefFunc(yylineno, funcID->value);
        exit(0);
    }
    Symbol* symbol = scopeSymbolTable.get_symbol(funcID->value);
    if(!symbol->function)
    {
        output::errorUndefFunc(yylineno, funcID->value);
        exit(0);
    }
    if(symbol->param.size() != 0)
    {
        output::errorPrototypeMismatch(yylineno, funcID->value, symbol->param);
        exit(0);
    }
    this->value = symbol->name;
    this->type = symbol->type;
}*/

Statement::Statement(Type *type, Node *id) : Node()
{
    if(scopeSymbolTable.is_symbol_exist(id->value))
    {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    value = type->value;
    scopeSymbolTable.add_symbol(id->value, type->type, false);
}

Statement::Statement(Type *type, Node *id, Exp *exp)
{
    if(scopeSymbolTable.is_symbol_exist(id->value))
    {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    if((!scopeSymbolTable.is_symbol_exist(exp->value) && exp->isVar))
    {
        output::errorUndef(yylineno, exp->value);
        exit(0);
    }
    else if(scopeSymbolTable.is_symbol_exist(exp->value) && exp->isVar)
    {
        Symbol *symbol = scopeSymbolTable.get_symbol(exp->value);
        if(symbol->function == exp->isVar)
        {
            output::errorUndef(yylineno, exp->value);
            exit(0);
        }
    }
    if (!((type->type == exp->type) || (type->type == "int" && exp->type == "byte")))
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    scopeSymbolTable.add_symbol(id->value, type->type, false);
    Exp *id_exp = new Exp();
    id_exp->type = type->value;
    id_exp->reg = codeGenerator.freshVar();
    id_exp->value = id->value;
    buffer.emit("DEBUG " + exp->value + " " + type->value + "isEmpty");
    if(type->value == "bool")
    {
        buffer.emit("DEBUG1* ");
        if(exp->value == "true")
        {
            buffer.emit("DEBUG2 ");
            buffer.emit(id_exp->reg + "= add i1 1, 0");
        }
        else if(exp->value == "false")
        {
            buffer.emit(id_exp->reg + "= add i1 0, 0");
        }
    }
    else if(type->value == "byte")
    {
        buffer.emit(id_exp->reg + "= add i8 0, " + exp->value);
    }
    else if(type->value == "int")
    {
        buffer.emit(id_exp->reg + "= add i32 0, " + exp->value);
    }
}

Statement::Statement(Node *id, Exp *exp)
{
    if(!scopeSymbolTable.is_symbol_exist(id->value))
    {
        output::errorUndef(yylineno, id->value);
        exit(0);
    }
    if((!scopeSymbolTable.is_symbol_exist(exp->value) && exp->isVar))
    {
        output::errorUndef(yylineno, exp->value);
        exit(0);
    }
    else if(scopeSymbolTable.is_symbol_exist(exp->value) && exp->isVar)
    {
        Symbol *symbol = scopeSymbolTable.get_symbol(exp->value);
        if(symbol->function == exp->isVar)
        {
            output::errorUndef(yylineno, exp->value);
            exit(0);
        }
    }
    Symbol *symbol = scopeSymbolTable.get_symbol(id->value);
    if(symbol->function)
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if(!((symbol->type == exp->type) || (symbol->type == "int" && exp->type == "byte")))
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    //std::cout << "symbol: "<< symbol->type << "exp: " << exp->type << std::endl;
}

Statement::Statement(Call *call)
{
    if(!scopeSymbolTable.is_symbol_exist(call->value))
    {
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
    Symbol *symbol = scopeSymbolTable.get_symbol(call->value);
    if(!symbol->function)
    {
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
}

Statement::Statement(Exp *exp, bool isReturn)
{
    SymbolTable *currentScope = scopeSymbolTable.current_scope();
    string *returnType = currentScope->return_type;

    if ((*returnType != "" && *returnType != exp->type) && (*returnType != "int" || exp->type != "byte"))
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (exp->isVar)
    {
        if(!scopeSymbolTable.is_symbol_exist(exp->value))
        {
            output::errorDef(yylineno, exp->value);
            exit(0);
        }
        /*Symbol *symbol = scopeSymbolTable.get_symbol(exp->value);
        if (symbol->is_function)
        {
            output::errorUndef(yylineno, symbol->name);
            exit(0);
        }*/
    }
    //TODO: is return func() allowed?
}

void is_bool(Node *node) {
    Exp *exp = dynamic_cast<Exp *>(node);
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

Statement::Statement(const string name, Node *node)
{
    Exp* exp = dynamic_cast<Exp*>(node);
    if (exp->type != "bool")
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

Statement::Statement(Node *node)
{
    if(node->value == "continue" || node->value == "break")
    {
        if (!scopeSymbolTable.is_loop())
        {
            if(node->value == "break")
            {
                output::errorUnexpectedBreak(yylineno);
            }
            else if(node->value == "continue")
            {
                output::errorUnexpectedContinue(yylineno);
            }
            exit(0);
        }
    }
}