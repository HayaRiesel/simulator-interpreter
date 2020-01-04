//
// Created by yaellax on 30/12/2019.
//
#ifndef UNTITLED6_EX1_H
#define UNTITLED6_EX1_H

#include "Expression.h"
#include <stack>
#include <map>
#include <queue>
#include <list>
#include <regex>

class Variable : public Expression
{
private:
    string name;
    double value;
public:
    Variable(string name1, double value1) : name(name1), value(value1) {};
    Variable& operator++();
    Variable& operator--();
    Variable& operator+=(double num);
    Variable& operator-=(double num);
    Variable& operator++(int);
    Variable& operator--(int);
    double calculate();
    virtual ~Variable() {};
};


class Value : public Expression
{
private:
    const double num;
public:

    Value(double number) : num(number) {};
    double calculate();
    virtual ~Value() {};
};

class UnaryOperator : public Expression
{
protected:
    Expression* expUnary;
public:
    UnaryOperator(Expression *exp1) : expUnary(exp1) {};
};

class UPlus : public UnaryOperator
{
public:
    UPlus(Expression *exp) : UnaryOperator(exp) {};
    double calculate();
    virtual ~UPlus();
};

class UMinus : public UnaryOperator
{
public:
    UMinus(Expression *exp) : UnaryOperator(exp) {};
    double calculate();
    virtual ~UMinus();
};

class BinaryOperator : public Expression
{
protected:
    Expression *left;
    Expression *right;

public:
    BinaryOperator(Expression *expLeft, Expression *expRight) : left(expLeft), right(expRight) {};
};

class Plus : public BinaryOperator
{
public:
    Plus(Expression *ex1, Expression *ex2) : BinaryOperator(ex1, ex2) {};
    double calculate();
    virtual ~Plus();
};

class Minus : public BinaryOperator
{
public:
    Minus(Expression *ex1, Expression *ex2) : BinaryOperator(ex1, ex2) {};
    double calculate();
    virtual ~Minus();
};

class Mul : public BinaryOperator
{
public:
    Mul(Expression *ex1, Expression *ex2) : BinaryOperator(ex1, ex2) {};
    double calculate();
    virtual ~Mul();
};

class Div : public BinaryOperator
{
public:
    Div(Expression *ex1, Expression *ex2) : BinaryOperator(ex1, ex2) {};
    double calculate();
    virtual ~Div();
};


class Interpreter
{
private:
    map<string, double> Variables;

public:
    Interpreter() {};
    Expression* interpret(string stringInfix);
    queue<string> alg(string stringInfix);
    Expression* doExp(queue<string> stringPostfix);
    void setVariables(string);
    vector<string> splitEx1(string str, char splitBy);
    bool isOperator(string str);
    bool isOperator(char c);
    string getString(char x);
};

#endif //UNTITLED6_EX1_H
