#ifndef UNTITLED6_EXPRESSION_H
#define UNTITLED6_EXPRESSION_H
using namespace std;
/**
 * Expression Interface
 */
class Expression {

public:
    virtual double calculate() = 0;
    virtual ~Expression() {}
};
#endif //UNTITLED6_EXPRESSION_H
