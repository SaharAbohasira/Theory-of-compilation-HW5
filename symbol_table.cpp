//
// Created by Sahar on 12/07/2024.
//

#include "symbol_table.h"
#include "hw3_output.hpp"

extern ScopeSymbolTable scopeSymbolTable;

string to_uppercase(const string &s)
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

bool SymbolTable::is_symbol_exist(const string &name)
{
    for(auto it = this->symbols.rbegin(); it != this->symbols.rend(); ++it)
    {
        if((*it)->name == name)
        {
            return true;
        }
    }
    return false;
}

Symbol *SymbolTable::get_symbol(const string &name)
{
    for(auto it = this->symbols.rbegin(); it != this->symbols.rend(); ++it)
    {
        if((*it)->name == name)
        {
            return *it;
        }
    }
    return nullptr;
}

void SymbolTable::add_symbol(const Symbol &symbol)
{
    Symbol* new_symbol = new Symbol(symbol.name, symbol.type, symbol.offset, symbol.function, symbol.param);
    symbols.push_back(new_symbol);
    if (symbol.offset >= 0)
    {
        this->scope_offset = symbol.offset;
    }
}

/***********************ScopeSymbolTable**********************/

ScopeSymbolTable::ScopeSymbolTable(): stack(), offset_vector()
{
    offset_vector.push_back(0);
    push_scope(false);
    add_symbol("print", "void", true, {"string"});
    add_symbol("printi", "void", true, {"int"});
    add_symbol("readi", "int", true, {"int"});
}

ScopeSymbolTable::~ScopeSymbolTable()
{
    for(auto i = stack.begin(); i != stack.end(); i++){
        SymbolTable* curr = *i;
        delete curr;
    }
}

bool ScopeSymbolTable::is_symbol_exist(const string &name)
{
    for(auto it = this->stack.rbegin(); it != this->stack.rend(); ++it)
    {
        if((*it)->is_symbol_exist(name))
        {
            return true;
        }
    }
    return false;
}

Symbol *ScopeSymbolTable::get_symbol(const string &name)
{
    for(auto it = this->stack.rbegin(); it != this->stack.rend(); ++it)
    {
        Symbol *sym = (*it)->get_symbol(name);
        if (sym)
        {
            return sym;
        }
    }
    return nullptr;
}

void ScopeSymbolTable::push_scope(bool is_loop, string return_type)
{
    SymbolTable* scope = new SymbolTable(offset_vector.back(), is_loop, return_type);
    this->stack.push_back(scope);
    offset_vector.push_back(stack.back()->scope_offset);
    if(stack.size() > 0)
    {
        scope->rbp = stack.back()->rbp;
    }
    else {
        scope->rbp = "";
    }
}

SymbolTable *ScopeSymbolTable::current_scope()
{
    return stack.back();
}

void ScopeSymbolTable::pop_scope()
{
    SymbolTable* scope = stack.back();
    stack.pop_back();
    //output::endScope();
    for(auto it = scope->symbols.begin(); it != scope->symbols.end(); ++it)
    {
        offset_vector.pop_back();
        /*if((*it)->function)
        {
            output::printID((*it)->name, 0, output::makeFunctionType(to_uppercase((*it)->param), to_uppercase((*it)->type)));
        }
        else
        {
            output::printID((*it)->name, (*it)->offset, to_uppercase((*it)->type));
        }*/
    }
    if (!offset_vector.empty())
    {
        offset_vector.pop_back();
    }
    delete scope;
}

void ScopeSymbolTable::add_symbol(const string &name, const string &type, bool function, string param)
{
    int offset;
    if (function)
        offset = 0;
    else
    {
        offset = offset_vector.back();
        offset_vector.push_back(offset + 1);
    }
    Symbol symbol = Symbol(name, type, offset, function, param);
    SymbolTable* scope = stack.back();
    scope->add_symbol(symbol);
}

bool ScopeSymbolTable::is_loop()
{
    for (auto sym = stack.rbegin(); sym != stack.rend(); sym++)
    {
        SymbolTable *table = *sym;
        if (table->is_loop == true)
            return true;
    }
    return false;
}

void ScopeSymbolTable::check_program() {
    while(!scopeSymbolTable.stack.empty())
    {
        scopeSymbolTable.pop_scope();
    }
}







