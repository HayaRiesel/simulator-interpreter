//
// Created by yaellax on 27/12/2019.
//
#include "ex1.h"

Variable& Variable:: operator++() {
    this->value = this->value + 1;
    return *this;
}
Variable& Variable::operator--() {
    this->value--;
    return *this;

}
Variable& Variable::operator+= (double num) {
    this->value = this->value + num;
    return *this;

}
Variable& Variable :: operator-= (double num) {
    this->value = this->value - num;
    return *this;

}
Variable& Variable::operator++(int)
{
    this->value = this->value + 1;
    return *this;
}
Variable& Variable::operator--(int)
{
    this->value = this->value - 1;
    return *this;
}
double Variable :: calculate() {
    return this->value;
}




double Value::calculate() {
    return this->num;
}

double Plus::calculate()
{
    return this->left->calculate()+ this->right->calculate();
}

double Minus::calculate()
{
    return this->left->calculate() - this->right->calculate();
}

double Mul::calculate()
{
    return this->left->calculate() * this->right->calculate();
}

double Div::calculate()
{
    if (this->right->calculate() == 0) {
        throw "division by zero";
    }
    return this->left->calculate() / this->right->calculate();
}

double UPlus::calculate()
{
    return this->expUnary->calculate();
}

double UMinus::calculate()
{
    return (-1 * (this->expUnary->calculate()));
}

Mul::~Mul() {
    if (this->right != NULL) {
        delete right;
    }
    if (this->left != NULL) {
        delete left;
    }
}

Minus::~Minus() {
    if (this->right != NULL) {
        delete this->right;
    }
    if (this->left != NULL) {
        delete this->left;
    }
}

Plus::~Plus() {
    if (this->right != NULL) {
        delete this->right;
    }
    if (this->left != NULL) {
        delete this->left;
    }
}

Div::~Div() {
    if (this->right != NULL) {
        delete this->right;
    }
    if (this->left != NULL) {
        delete this->left;
    }
}
UPlus::~UPlus() {
    if (this->expUnary != NULL) {
        delete this->expUnary;
    }
}
UMinus::~UMinus() {
    if (this->expUnary != NULL) {
        delete this->expUnary;
    }
}


Expression* Interpreter::interpret(string stringInfix)
{
    queue<string> postfix = this->alg(stringInfix);
    return doExp(postfix);
}



queue<string> Interpreter::alg(string stringInfix)
{
    //infix to profix
    stack <char> stack;
    //string outputQueue;
    //stack.push_front(' ');
    queue<string> quene;
    string numOrVar = "";

    try {
        
        for (unsigned int i = 0; i < stringInfix.length(); i++) {
            //if the '-' is first
            if (i == 0 && stringInfix[0] == '-') {
                quene.push("0");
                stack.push('-');
                continue;
            }
            //if the '+' is first
            if (i == 0 && stringInfix[0] == '+') {
                continue;
            }
            //can't be with ' '
            if (stringInfix[i] == ' ') {
                throw "bad input";
            }
            if (isOperator(stringInfix[i]) || stringInfix[i] == '(' || stringInfix[i] == ')') {
                if (!numOrVar.empty()) {
                    quene.push(numOrVar);
                    numOrVar.clear();
                }
            }
            if (isOperator(stringInfix[i])) {
                if (!stack.empty()) {
                    //by the alguritem (the Priorities of the operator)
                    while (((stringInfix[i] == '+' || stringInfix[i] == '-') && (stack.top() == '*' || stack.top() == '/'))
                           || (stringInfix[i] == '-' && stack.top() == '+')
                           || (stringInfix[i] == '+' && stack.top() == '-')) {
                        quene.push(getString(stack.top()));
                        stack.pop();
                        if (stack.empty()) {
                            break;
                        }
                    }

                }
                stack.push(stringInfix[i]);
            }
            else if (stringInfix[i] == '(') {
                if (stringInfix[i + 1] == '+') {
                    stack.push(stringInfix[i]);
                    i++;
                    continue;
                }
                if (stringInfix[i + 1] == '-') {
                    if (stringInfix[i + 2] == '(') {
                        stack.push(stringInfix[i]);
                        quene.push("0");
                        continue;
                    }
                    else {
                        stack.push(stringInfix[i]);
                        numOrVar = numOrVar + "@";
                        i++;
                        continue;
                    }
                }

                stack.push(stringInfix[i]);

            }
            //close the phare
            else if (stringInfix[i] == ')') {
                while (stack.size() != 0 && stack.top() != '(') {
                    quene.push(getString(stack.top()));
                    stack.pop();
                }
                if (stack.size() == 0) {
                    //to much of ')'
                    throw "bad input";
                }
                else {
                    stack.pop();
                }
            }
            //this is the continue of the name
            else {
                numOrVar = numOrVar + stringInfix[i];
                //the number stay in the end
                if (stringInfix.length() == (unsigned int)(i + 1)) {
                    quene.push(numOrVar);
                    numOrVar.clear();
                }

            }
        }
        //put what left in the quene
        while (stack.size() != 0) {
            quene.push(getString(stack.top()));
            stack.pop();
        }
        return quene;
    }
    catch (exception e) {
        throw "bad input";
    }

}

Expression* Interpreter::doExp(queue<string> stringPostfix) {
    list <Expression*> stack2;
    Expression *right;
    Expression *left;
    string numOrVar;
    while (stringPostfix.size() != 0) {
        try {
            if (isOperator(stringPostfix.front())) {
                if (stack2.size() < 2) {
                    throw "bad input";
                }
                left = stack2.front();
                stack2.pop_front();
                right = stack2.front();
                stack2.pop_front();

                if (stringPostfix.front().compare("+") == 0) {
                    stack2.push_front(new Plus(right, left));
                }
                if (stringPostfix.front().compare("-") == 0) {
                    stack2.push_front(new Minus(right, left));

                }
                if (stringPostfix.front().compare("*") == 0) {
                    stack2.push_front(new Mul(right, left));
                }
                if (stringPostfix.front().compare("/") == 0) {
                    try {
                        stack2.push_front(new Div(right, left));
                    }
                    catch (exception e) {
                        throw "bad input";
                    }
                }
            }
            else {
                try {
                    if (stringPostfix.front()[0] == '@') {
                        stringPostfix.front()[0] = '-';
                    }
                    string strr = stringPostfix.front();
                    double dbl;
                    dbl = stod(stringPostfix.front());
                    stack2.push_front(new Value(dbl));
                }
                catch (exception e) {
                    if (stringPostfix.front().compare("(") == 0 || stringPostfix.front().compare(")") == 0) {
                        throw "bad input";
                    }
                    std::map<std::string, double>::iterator it;
                    it = this->Variables.find(stringPostfix.front());
                    // Check if element exists in map or not
                    if (it != Variables.end()) {
                        stack2.push_front(new Variable(stringPostfix.front(), this->Variables[stringPostfix.front()]));
                    }
                    else {
                        //variable no found
                        throw "bad input";
                    }

                }

            }
            stringPostfix.pop();
        }
        catch (exception e) {
            throw "bad input";
        }
    }
    if (stack2.size() <= 1) {
        Expression *exp = stack2.front();
        return exp;
    }
    else {
        throw "bad input";
    }


}

void Interpreter::setVariables(string variables) {

    vector<string> splitVariables = this->splitEx1(variables, ';');



    for (unsigned int i = 0; i < splitVariables.size(); i++) {
        vector<string> varApartNum = this->splitEx1(splitVariables[i], '=');

        if (varApartNum.size() != 2) {
            throw "bad input";
        }

        //how the var need to be (legal)
        regex b("^([a-z]|[A-Z]|[_])+([a-z]|[A-Z]|[_]|[0-9])*$");
        if (!regex_match(varApartNum[1], b)) {
            throw "bad input";
        }

        //check if the var allredy excist
        if (!this->Variables.empty()) {
            try {
                this->Variables[varApartNum[1]];
                this->Variables.erase(varApartNum[1]);
            }
            catch (exception e) {

            }
        }

        //make string to int
        try {
            double dbl;
            dbl = stod(varApartNum[0]);

            this->Variables.insert({ varApartNum[1], dbl });
        }
        catch (exception e) {
            throw "bad input";
        }
    }
}

vector<string> Interpreter::splitEx1(string str, char splitBy) {
    string strToInsert;
    vector<string> splitStr;
    auto it = splitStr.begin();
    for (unsigned int i = 0; i < str.length(); i++) {
        if (splitBy == str[i]) {
            it = splitStr.insert(it, strToInsert);
            strToInsert = "";
        }
        else {
            strToInsert += str[i];
        }
    }
    if (strToInsert.compare("") != 0) {
        splitStr.insert(it, strToInsert);
    }
    return splitStr;

}


string Interpreter::getString(char x)
{
    string s(1, x);
    return s;
}

bool Interpreter::isOperator(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/') {
        return true;
    }
    return false;
}

bool Interpreter::isOperator(string str) {
    if (str.compare("+") == 0 || str.compare("-") == 0 || str.compare("*") == 0 || str.compare("/") == 0) {
        return true;
    }
    return false;
}

