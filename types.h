//
// Created by Gal Ishai on 7/17/24.
//

#ifndef THEORY_OF_COMPILATION_HW3_TYPES_H
#define THEORY_OF_COMPILATION_HW3_TYPES_H

#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;


class Node
{
public:
    string reg = "";
    string value;
    Node(const string value = "");
    Node(const Node &node);
    virtual ~Node() = default;
};

#define YYSTYPE Node*

class Type: public Node
{
public:
    string type;
    Type(const string type): Node(), type(type){};
    Type(const string type, Node * node);
    virtual ~Type() = default;
};

class Exp: public Node
{
public:
    string type;
    bool isVar;
    string reg = "";
    string true_label = "";
    string false_label = "";
    string next_label = "";
    Exp(Exp *exp): Node(exp->value), type(exp->type){};
    Exp(Node *node, bool isVar);
    Exp(): Node(), type("void"), isVar(false){};
    Exp(const string type, Node *terminal);
    Exp(Node* node1, Node* node2, const string op, const string type1);
    Exp(Type* type1, Exp* exp);
};

class Program: public Node
{
public:
    Program(){};
    virtual ~Program() = default;
};

class Call : public Node {
public:
    string type;
    Call(Node *funcID, Node *node);
    //Call(Node* funcID);
    virtual ~Call() = default;
};

class Statement : public Node
{
public:
    string whileLabel = "";

    Statement(): Node(){}

    Statement(Node *node);

    Statement(Type *type, Node *id);

    Statement(Type *type, Node *id, Exp *exp);

    Statement(Node *id, Exp *exp);

    Statement( const string name, Node *node);

    Statement(Exp *exp, bool isReturn = false);

    Statement(Call *call);

    virtual ~Statement() = default;
};

class Statements : public Node {
public:

    Statements(Statements *statements, Statement *s) : Node() {};

    Statements(Statement *s) : Node() {};

    virtual ~Statements() = default;
};

class Label: public Node{
public:
    string true_label;
    string false_label;
    string next_label;
    Label();
    virtual ~Label() = default;
};

class IF_: public Exp{
public:
    Exp* exp;
    Label* label;
    IF_(Exp* exp, Label* label);
    virtual ~IF_() = default;
};

void is_bool(Node* node);


#endif //THEORY_OF_COMPILATION_HW3_TYPES_H
