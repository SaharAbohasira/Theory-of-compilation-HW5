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

Node::Node(const std::string value): value(value)
{
}

Node::Node(const Node &node): value(node.value)
{
}

Exp::Exp(): Node(), type("void"), isVar(false)
{
    reg = codeGenerator.freshVar();
}

Exp::Exp(Node *node1, Node *node2, const std::string op, const std::string type1): isVar(false)
{
    Exp* exp1 = dynamic_cast<Exp *>(node1);
    Exp* exp2 = dynamic_cast<Exp *>(node2);
    reg = codeGenerator.freshVar();
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
        if(op == "and")
        {
            buffer.emit(reg + " = and i1 " + exp1->reg + ", " + exp2->reg);
        }
        else if(op == "or")
        {
            //buffer.emit(reg + " = or i1 " + exp1->reg + ", " + exp2->reg);

        }
        if(op == "not")
        {
                buffer.emit(reg + " = sub i1 1, " + exp1->reg);
        }
    }
    else if(type1 == "binop")
    {
        if((exp1->type != "int" && exp1->type != "byte")||(exp2->type != "int" && exp2->type != "byte"))
        {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if (exp1->type == "int" || exp2->type == "int")
        {
            this->type = "int";
            string new_reg1 = codeGenerator.freshVar();
            string new_reg2 = codeGenerator.freshVar();
            if(exp1->type == "int")
            {
                buffer.emit(new_reg1 +" = add i32 " + exp1->reg + ", 0");
            }
            else if(exp1->type == "byte")
            {
                buffer.emit(new_reg1 + " = zext i8 " + exp1->reg + " to i32");
            }
            if(exp2->type == "int")
            {
                buffer.emit(new_reg2 +" = add i32 " + exp2->reg + ", 0");
            }
            else if(exp2->type == "byte")
            {
                buffer.emit(new_reg2 + " = zext i8 " + exp2->reg + " to i32");
            }
            if(op == "+")
            {
                buffer.emit(reg + " = add i32 " + new_reg1 + ", " + new_reg2);
            }
            if(op == "-")
            {
                buffer.emit(reg + " = sub i32 " + new_reg1 + ", " + new_reg2);
            }
            if(op == "*")
            {
                buffer.emit(reg + " = mul i32 " + new_reg1 + ", " + new_reg2);
            }
            if(op == "/")
            {
                buffer.emit(reg + " = sdiv i32 " + new_reg1 + ", " + new_reg2);
            }

        } else
        {
            this->type = "byte";
            if(op == "+")
            {
                buffer.emit(reg + " = add i8 " + exp1->reg + ", " + exp2->reg);
            }
            if(op == "-")
            {
                buffer.emit(reg + " = sub i8 " + exp1->reg + ", " + exp2->reg);
            }
            if(op == "*")
            {
                buffer.emit(reg + " = mul i8 " + exp1->reg + ", " + exp2->reg);
            }
            if(op == "/")
            {
                buffer.emit(reg + " = udiv i8 " + exp1->reg + ", " + exp2->reg);
            }
            /*string trunc_reg = codeGenerator.freshVar();
            buffer.emit(trunc_reg + " = trunc i8 " + reg + " to i8");
            string new_reg = codeGenerator.freshVar();
            buffer.emit(new_reg + " = add i8 " + trunc_reg + ", 0");
            reg = new_reg;*/
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
        string new_reg1 = codeGenerator.freshVar();
        string new_reg2 = codeGenerator.freshVar();
        if(exp1->type == "int")
        {
            buffer.emit(new_reg1 +" = add i32 " + exp1->reg + ", 0");
        }
        else if(exp1->type == "byte")
        {
            buffer.emit(new_reg1 + " = zext i8 " + exp1->reg + " to i32");
        }

        if(exp2->type == "int")
        {
            buffer.emit(new_reg2 +" = add i32 " + exp2->reg + ", 0");
        }
        else if(exp2->type == "byte")
        {
            buffer.emit(new_reg2 + " = zext i8 " + exp2->reg + " to i32");
        }
        if(op == "==")
        {
            if(exp1->type == "int" || exp1->type == "byte")
            {
                buffer.emit(reg + "= icmp eq i32 " + new_reg1 + ", " + new_reg2);
            }

            if(exp1->type == "bool")
            {
                buffer.emit(reg + "= icmp eq i1 " + exp1->reg +", " + exp2->reg);
            }
        }
        else if(op == "!=")
        {
            if(exp1->type == "int" || exp1->type == "byte")
            {
                buffer.emit(reg + "= icmp ne i32 " + new_reg1 +", " + new_reg2);
            }
            if(exp1->type == "bool")
            {
                buffer.emit(reg + "= icmp ne i1 " + exp1->reg +", " + exp2->reg);
            }
        }
        else if(op == "<")
        {
            if(exp1->type == "int" || exp1->type == "byte")
            {

                buffer.emit(reg + "= icmp slt i32 " + new_reg1 +", " + new_reg2);
            }
            if(exp1->type == "bool")
            {
                buffer.emit(reg + "= icmp slt i1 " + exp1->reg +", " + exp2->reg);
            }
        }
        else if(op == ">")
        {
            if(exp1->type == "int" || exp1->type == "byte")
            {
                buffer.emit(reg + "= icmp sgt i32 " + new_reg1 +", " + new_reg2);
            }
            if(exp1->type == "bool")
            {
                buffer.emit(reg + "= icmp sgt i1 " + new_reg1 +", " + new_reg2);
            }
        }
        else if(op == "<=")
        {
            if(exp1->type == "int" || exp1->type == "byte")
            {
                buffer.emit(reg + "= icmp sle i32 " + new_reg1 +", " + new_reg2);
            }
            if(exp1->type == "bool")
            {
                buffer.emit(reg + "= icmp sle i1 " + exp1->reg +", " + exp2->reg);
            }
        }
        else if(op == ">=")
        {
            if(exp1->type == "int" || exp1->type == "byte")
            {
                buffer.emit(reg + " = icmp sge i32 " + new_reg1 +", " + new_reg2);
            }
            if(exp1->type == "bool")
            {
                buffer.emit(reg + " = icmp sge i1 " + exp1->reg +", " + exp2->reg);
            }
        }
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
    this->isVar = isVar;
    if(reg == "")
    {
        reg = codeGenerator.freshVar();
    }
    if(!isVar)
    {
        Call* function = dynamic_cast<Call*>(node);
        reg = function->reg;
    }
    else
    {
        /*reg = codeGenerator.freshVar();
        if(type == "int")
        {
            buffer.emit(reg + " = add i32 " + s->reg + ", 0");
        }
        else if(type == "byte")
        {
            buffer.emit(reg + " = add i8 " + s->reg + ", 0");
        }
        else if(type == "bool")
        {
            buffer.emit(reg + " = add i1 " + s->reg + ", 0");
        }
        else if(type == "string")
        {
            buffer.emit(reg + " = add i8* " + s->reg +", 0");
        }*/
        string reg_ptr = codeGenerator.freshVar();
        buffer.emit(reg_ptr + " = getelementptr i32, i32* " + scopeSymbolTable.rbp + ", i32 " + std::to_string(s->offset));
        buffer.emit(reg + " = load i32, i32* " + reg_ptr);
        //buffer.emit("DEBUG" + reg);
        if(s->type == "byte")
        {
            string reg_copy = codeGenerator.freshVar();
            string new_reg = codeGenerator.freshVar();
            //buffer.emit("DEBUG" + reg);
            buffer.emit(reg_copy + " = add i32 " + reg + ", 0");
            buffer.emit(new_reg + " = trunc i32 " + reg_copy + " to i8");
            reg = new_reg;
        }
        else if(s->type == "bool")
        {
            string reg_copy = codeGenerator.freshVar();
            string new_reg = codeGenerator.freshVar();
            //buffer.emit("DEBUG" + reg);
            buffer.emit(reg_copy + " = add i32 " + reg + ", 0");
            buffer.emit(new_reg + " = icmp ne i32 " + reg_copy + ", 0");
            reg = new_reg;
        }
    }
}

Exp::Exp(const string type, Node *terminal): Node(terminal->value), type(type)
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
        string str = terminal->value;
        str.pop_back();
        string temp_reg = codeGenerator.globalFreshVar();
        buffer.emitGlobal(temp_reg + " = constant [" + to_string(str.length()) + " x i8] c" + str + "\\00\"");
        string temp_reg_ptr = temp_reg;
        temp_reg_ptr.replace(0, 1, "%");
        buffer.emit(temp_reg_ptr + ".ptr = getelementptr [" + to_string(str.length()) + " x i8], " + "[" + to_string(str.length()) + " x i8]* " + temp_reg + ", i32 0, i32 0");
        reg = temp_reg_ptr + ".ptr";
    }
    else if(type == "int")
    {
        reg = codeGenerator.freshVar();
        buffer.emit(reg + " = add i32 " + value + ", 0");
    }
    else if(type == "byte")
    {
        reg = codeGenerator.freshVar();
        buffer.emit(reg + " = add i8 " + value + ", 0");
    }
    else if(type == "bool")
    {
        reg = codeGenerator.freshVar();
        if(terminal->value == "true")
        {
            buffer.emit(reg + " = add i1 1, 0");
        }
        else if(terminal->value == "false")
        {
            buffer.emit(reg + " = add i1 0, 0");
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
    reg = codeGenerator.freshVar();
    if(type1->type == "int" && exp->type == "int")
    {
        buffer.emit(reg + " = add i32 " + exp->reg + ", 0");
        type = "int";
    }
    else if(type1->type == "int" && exp->type == "byte")
    {
        buffer.emit(reg + " = zext i8 " + exp->reg + " to i32");
        type = "int";
    }
    else if(type1->type == "byte" && exp->type == "int")
    {
        buffer.emit(reg + " = trunc i32 " + exp->reg + " to i8");
        type = "byte";
    }
    else if(type1->type == "byte" && exp->type == "byte")
    {
        buffer.emit(reg + " = add i8 " + exp->reg + ", 0");
        type = "byte";
    }
    /*if(DEBUG)
    {
        std::cout << "* node value: " << node->value << "\n" << std::endl;
    }*/
}

Exp::Exp(Exp *exp): Node(exp->value), type(exp->type)
{
    reg = codeGenerator.freshVar();
    if(type == "int")
    {
        buffer.emit(reg + " = add i32 " + exp->reg + ", 0");
    }
    else if(type == "byte")
    {
        buffer.emit(reg + " = add i8 " + exp->reg + ", 0");
    }
    else if(type == "bool")
    {
        buffer.emit(reg + " = add i1 " + exp->reg + ", 0");
    }
    else if(type == "string")
    {
        buffer.emit(reg + " = add i8* " + exp->reg +", 0");
    }

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
    reg = codeGenerator.freshVar();
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
    if(type == "void" && (exp->type == "int" || exp->type == "byte") && value == "printi")
    {
        string new_reg = codeGenerator.freshVar();
        if(exp->type == "byte")
        {
            buffer.emit(new_reg + " = zext i8 " + exp->reg + " to i32");
        }
        else
        {
            buffer.emit(new_reg + " = add i32 " + exp->reg + ", 0");
        }
        buffer.emit("call void @" + value + "(i32 " + new_reg + ")");
    }
    else if(type == "void" && exp->type == "string" && value == "print")
    {
        buffer.emit("call void @" + value + "(i8* " + exp->reg + ")");
    }
    else if(type == "int" && exp->type == "int" && value == "readi")
    {
        buffer.emit(reg + " = call i32 @" + value + "(i32 " + exp->reg + ")");
    }
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
    //alue = type->value;
    scopeSymbolTable.add_symbol(id->value, type->type, false,"");
    Symbol *s = scopeSymbolTable.get_symbol(id->value);
    Exp* temp_exp = new Exp();
    temp_exp->reg = codeGenerator.freshVar();
    if(type->type == "bool")
    {
        temp_exp->value = "false";
        string reg_ptr = codeGenerator.freshVar();
        buffer.emit(temp_exp->reg + " = add i32 0, 0");
        buffer.emit(reg_ptr + " = getelementptr i32, i32* " + scopeSymbolTable.rbp + ", i32 " + std::to_string(s->offset));
        //buffer.emit("hi1");
        buffer.emit("store i32 " + temp_exp->reg + ", i32* " + reg_ptr);
    }
    else if(type->type == "int")
    {
        temp_exp->value = "0";
        string reg_ptr = codeGenerator.freshVar();
        buffer.emit(temp_exp->reg + " = add i32 0, 0");
        buffer.emit(reg_ptr + " = getelementptr i32, i32* " + scopeSymbolTable.rbp + ", i32 " + std::to_string(s->offset));
        //buffer.emit("hi2");
        buffer.emit("store i32 " + temp_exp->reg + ", i32* " + reg_ptr);
    }
    else if(type->type == "byte")
    {
        temp_exp->value = "0";
        string reg_ptr = codeGenerator.freshVar();
        buffer.emit(temp_exp->reg + " = add i32 0, 0");
        buffer.emit(reg_ptr + " = getelementptr i32, i32* " + scopeSymbolTable.rbp + ", i32 " + std::to_string(s->offset));
        //buffer.emit("hi3");
        buffer.emit("store i32 " + temp_exp->reg + ", i32* " + reg_ptr);
    }
    delete temp_exp;

    /*if(value == "int")
    {
        buffer.emit(id->reg + " = add i32 0, 0");
    }
    else if(value == "byte")
    {
        buffer.emit(id->reg + " = add i8 0, 0");
    }
    else if(value == "bool")
    {
        buffer.emit(id->reg + " = add i1 0, 0");
    }*/
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
    scopeSymbolTable.add_symbol(id->value, type->type, false, "");
    Symbol *s = scopeSymbolTable.get_symbol(id->value);
    if(type->type == "bool")
    {
        string reg_ptr = codeGenerator.freshVar();
        string new_reg = codeGenerator.freshVar();
        buffer.emit(new_reg + " = zext i1 " + exp->reg + " to i32");
        buffer.emit(reg_ptr + " = getelementptr i32, i32* " + scopeSymbolTable.rbp + ", i32 " + std::to_string(s->offset));
        //buffer.emit("hi4");
        buffer.emit("store i32 " + new_reg + ", i32* " + reg_ptr);
    }
    else if(type->type == "int")
    {
        string new_reg = codeGenerator.freshVar();
        if(exp->type == "byte")
        {
            buffer.emit(new_reg + " = zext i8 " + exp->reg + " to i32");
        }
        else
        {
            new_reg = exp->reg;
        }
        string reg_ptr = codeGenerator.freshVar();
        buffer.emit(reg_ptr + " = getelementptr i32, i32* " + scopeSymbolTable.rbp + ", i32 " + std::to_string(s->offset));
        //buffer.emit("hi5");
        buffer.emit("store i32 " + new_reg + ", i32* " + reg_ptr);
    }
    else if(type->type == "byte")
    {
        string reg_ptr = codeGenerator.freshVar();
        string new_reg = codeGenerator.freshVar();
        buffer.emit(new_reg + " = zext i8 " + exp->reg + " to i32");
        buffer.emit(reg_ptr + " = getelementptr i32, i32* " + scopeSymbolTable.rbp + ", i32 " + std::to_string(s->offset));
        //buffer.emit("hi6");
        buffer.emit("store i32 " + new_reg + ", i32* " + reg_ptr);
    }

    //Exp *id_exp = new Exp();
    //id_exp->type = type->value;
    //id_exp->reg = codeGenerator.freshVar();
    //id_exp->value = id->value;
    /*
    if(value == "int")
    {
        buffer.emit(id->reg + " = add i32 " + exp->reg + ", 0");
    }
    else if(value == "byte")
    {
        buffer.emit(id->reg + " = add i8 " + exp->reg + ", 0");
    }
    else if(value == "bool")
    {
        buffer.emit(id->reg + " = add i1 " + exp->reg + ", 0");
    }
    else if(value == "string")
    {
        buffer.emit(reg + " = add i8* " + exp->reg +", 0");
    }
    //buffer.emit("DEBUG " + exp->value + " " + type->value + "isEmpty");
    /*if(type->value == "bool")
    {
        if(exp->value == "true")
        {
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
    }*/
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
    if(symbol->type == "bool")
    {
        string reg_ptr = codeGenerator.freshVar();
        string new_reg = codeGenerator.freshVar();
        buffer.emit(new_reg + " = zext i1 " + exp->reg + " to i32");
        buffer.emit(reg_ptr + " = getelementptr i32, i32* " + scopeSymbolTable.rbp + ", i32 " + std::to_string(symbol->offset));
        //buffer.emit("hi7");
        buffer.emit("store i32 " + new_reg + ", i32* " + reg_ptr);
    }
    else if(symbol->type == "int")
    {
        string reg_ptr = codeGenerator.freshVar();
        buffer.emit(reg_ptr + " = getelementptr i32, i32* " + scopeSymbolTable.rbp + ", i32 " + std::to_string(symbol->offset));
        //buffer.emit("hi8");
        buffer.emit("store i32 " + exp->reg + ", i32* " + reg_ptr);
    }
    else if(symbol->type == "byte")
    {
        string reg_ptr = codeGenerator.freshVar();
        string new_reg = codeGenerator.freshVar();
        buffer.emit(new_reg + " = zext i8 " + exp->reg + " to i32");
        buffer.emit(reg_ptr + " = getelementptr i32, i32* " + scopeSymbolTable.rbp + ", i32 " + std::to_string(symbol->offset));
        //buffer.emit("hi9");
        buffer.emit("store i32 " + new_reg + ", i32* " + reg_ptr);
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
    reg = codeGenerator.freshVar();

    //buffer.emit("br i8 " + exp->reg + ", label " + exp->true_label + ", label "+ exp->false_label);
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

Label::Label() : Node("")
{
    true_label = buffer.freshLabel(); // + "_true";
    false_label = buffer.freshLabel(); // + "_false";
    next_label = buffer.freshLabel(); // + "_next";
    false_label2 = buffer.freshLabel();
}

IF_::IF_(Exp* exp, Label* label)
{
    this->exp = exp;
    this->label = label;
}