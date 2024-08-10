//
// Created by Sahar on 10/08/2024.
//

#ifndef THEORY_OF_COMPILATION_HW5_GENERAL_H
#define THEORY_OF_COMPILATION_HW5_GENERAL_H

#include <string.h>
#include "types.h"
#include "cg.hpp"
#include "symbol_table.h"

using std::string;

class general
{
    int current_label_num;
    int current_register_num;

public:
    general(): current_label_num(0), current_register_num(0){}
    string freshVar();
    string globalFreshVar();
    string freshLabel(string name);
    void binopCode(Exp* exp,const Exp &opr1, const Exp &opr2, const string &op);
    void relopCode(Exp* exp, const Exp &opr1, const Exp &opr2, const string &op);

};


#endif //THEORY_OF_COMPILATION_HW5_GENERAL_H
