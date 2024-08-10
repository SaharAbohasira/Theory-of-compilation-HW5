//
// Created by Sahar on 10/08/2024.
//

#include "general.h"

extern CodeBuffer buffer;
extern ScopeSymbolTable scope_symbol_table;

string general::freshVar()
{
    string reg = "%var_" + std::to_string(current_register_num);   //TODO: check if need to be % / &
    current_register_num++;
    return reg;
}

string general::globalFreshVar()
{
    string reg = "&var_" + std::to_string(current_register_num);
    current_register_num ++;
    return reg;
}

string general::freshLabel(string name)
{
    string new_label = name + std::to_string(current_label_num);
    current_label_num ++;
    return new_label;
}

void general::binopCode(Exp* exp,const Exp &opr1, const Exp &opr2, const string &op)
{
    string op_to_string;
    exp->reg = freshVar();

    if(op == "+")
    {
        op_to_string = "add";
    }
    if(op == "-")
    {
        op_to_string = "sub";
    }
    if(op == "*")
    {
        op_to_string = "mul";
    }
    if(op == "/" && exp->type == "int")
    {
        op_to_string = "sdiv";
    }
    if(op == "/" && exp->type != "int")
    {
        op_to_string = "udiv";
    }
    if(op_to_string == "udiv" || op_to_string == "sdiv")
    {
        buffer.emit("call void @check_division(i32 " + opr2.reg ")");
    }
    buffer.emit(exp->reg + " = " + op_to_string + " i32 " + opr1.reg + ", " + opr2.reg);
    else
    {
        if (exp->type == "byte")
        {
            string current_reg = exp->reg;
            exp->reg = freshVar();
            buffer.emit(exp->reg + " = and i32 255, " + current_reg);
        }
    }
}

void general::relopCode(Exp* exp, const Exp &opr1, const Exp &opr2, const string &op)
{
    exp->reg = freshVar();
    string op_to_string;
    if(op == "==")
    {
        op_to_string = "eq";
    }
    if(op == "!=")
    {
        op_to_string = "ne";
    }
    if(op == ">")
    {
        op_to_string = "sgt";
    }
    if(op == ">=")
    {
        op_to_string = "sge";
    }
    if(op == "<")
    {
        op_to_string = "slt";
    }
    if(op == "<=")
    {
        op_to_string = "sle";
    }
    buffer.emit(exp->reg + " = icmp " + op_to_string + " i32 " + opr1.reg + ", " + opr2.reg);
    int jump_address = buffer.emit("br i1 " + exp->reg + ", label @, label @");
    exp->true_list = buffer.makelist(pair<int, BranchLabelIndex>(address, FIRST));
    exp->false_list = buffer.makelist(pair<int, BranchLabelIndex>(address, SECOND));
}