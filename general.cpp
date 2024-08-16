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

/*string general::freshLabel(string name)                             //TODO: check if really needed
{
    string new_label = name + std::to_string(current_label_num);
    current_label_num ++;
    return new_label;
}*/

void general::globalCode()
{
    buffer.emit("@.DIV_BY_ZERO_ERROR = internal constant [23 x i8] c\"Error division by zero\\00\"");
    buffer.emit("define void @check_division(i32) {");
    buffer.emit("%valid = icmp eq i32 %0, 0");
    buffer.emit("br i1 %valid, label %ILLEGAL, label %LEGAL");
    buffer.emit("ILLEGAL:");
    buffer.emit("call void @print(i8* getelementptr([23 x i8], [23 x i8]* @.DIV_BY_ZERO_ERROR, i32 0, i32 0))");
    buffer.emit("call void @exit(i32 0)");
    buffer.emit("ret void");
    buffer.emit("LEGAL:");
    buffer.emit("ret void");
    buffer.emit("}");
}

void general::binopCommand(Exp* exp,const Exp &opr1, const Exp &opr2, const string &op)
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
        buffer.emit("call void @check_division(i32 " + opr2.reg + ")");
    }
    buffer.emit(exp->reg + " = " + op_to_string + " i32 " + opr1.reg + ", " + opr2.reg);
    if(!(op_to_string == "udiv" || op_to_string == "sdiv"))
    {
        if (exp->type == "byte")
        {
            string current_reg = exp->reg;
            exp->reg = freshVar();
            buffer.emit(exp->reg + " = and i32 255, " + current_reg);
        }
    }
}

void general::relopCommand(Exp* exp, const Exp &opr1, const Exp &opr2, const string &op)
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
    //string true_label = buffer.freshLabel();
    //string false_label = buffer.freshLabel();
    int jump_address = buffer.emit("br i1 " + exp->reg + ", label %" + exp->true_label + ", label %" + exp->false_label);      //TODO: handle short circuit
    buffer.emit(exp->true_label + ":");
    //conditionBranch(true_label, false_label);
    //buffer.emit(exp->reg + " = phi i32 [ 1, %" + true_label +"], [0, %" + false_label + "]");
}

/*void general::conditionBranch(string &true_label, string &false_label)
{
    buffer.emit(true_label + ":");
    string print_result = buffer.freshLabel();
    buffer.emit("br label %" + print_result)
    buffer.emit(false_label + ":");
    buffer.emit("br label %" + print_result)
    buffer.emit(print_result + ":");
}*/

void general::returnCommand(string &type, string &reg)          //TODO: check if really needed
{
    /*string ret_type;
    if(type == "int")
    {
        ret_type = "i32";
    }
    else if(type == "string")
    {
        ret_type = "i8*";
    }
    else if(type.empty())
    {
        buffer.emit("ret void");
    }
    buffer.emit("ret " + ret_type + reg);*/
    buffer.emit("ret i32 0");
}

void general::if_else_code(Exp* exp, Label* label)
{
    if (exp->type != "bool")
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    buffer.emit("br i8 " + exp->reg + ", label " + label->true_label + ", label "+ label->false_label);
}
