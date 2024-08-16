//
// Created by Sahar on 12/07/2024.
//

#ifndef תרגיל_בית_3_SYMBOL_TABLE_H
#define תרגיל_בית_3_SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;

class Symbol
{
public:
    string name;
    string type;
    int offset;
    bool function;
    string param;

    Symbol(const string name, const string type, int offset, bool function, string param): name(name), type(type), offset(offset), function(function), param(param) {}

    ~Symbol()=default;
};

class SymbolTable
{
public:
    vector <Symbol*> symbols;
    bool is_loop;
    string* return_type;
    int scope_offset;
    string rbp;

    SymbolTable(int scope_offset, bool is_loop, string return_type = ""): symbols(), scope_offset(scope_offset), is_loop(is_loop)
    {
        this->return_type = new string (return_type);
    }

    bool is_symbol_exist(const string &name);

    Symbol* get_symbol(const string &name);

    void add_symbol(const Symbol &symbol);

};

class ScopeSymbolTable
{
public:
    vector <SymbolTable*> stack;
    vector <int> offset_vector;

    ScopeSymbolTable();

    ~ScopeSymbolTable();

    void push_scope(bool is_loop = false, string return_type = "");

    SymbolTable* current_scope();

    void pop_scope();

    bool is_symbol_exist(const string &name);

    bool is_loop();

    Symbol *get_symbol(const string &name);

    void add_symbol(const string &name, const string &type, bool function, string param = "");
};





#endif //תרגיל_בית_3_SYMBOL_TABLE_H
