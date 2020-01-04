//
// Created by yaellax on 27/12/2019.
//

#ifndef UNTITLED6_COMMAND_H
#define UNTITLED6_COMMAND_H
using namespace std;

#include <sys/socket.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <map>
#include <algorithm>
#include <vector>
#include <iterator>
#include <pthread.h>
#include <thread>
#include <chrono>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <mutex>
#include "ex1.h"
static bool programRun = true;
static mutex mtx;
static bool sendToSimulator = false;

class Var{
public:
    string sim;
    double value;
    bool updateFromSimulator;
    bool needToUpdateTheSimulator;
};

class command {
protected:
    vector<string> lexerVector;
    int i;
    map<string,Var> *symbolTable;
    Interpreter* interpreter;
public:
    virtual ~command() {};
    virtual int execute() = 0;
    void setLexer(vector<string> lexer1);
    void setIndex(int i1);
    void updateSymbolTable(map<string,Var> *symbolTable1);
    void updateInterepter(Interpreter *interpreter1);
};

class OpenServerCommand : public command {
    map<string, int> mapOfXml;
public:
    void initMapOfXml();
    void readFromClient(int client_socket, int socketfd);
    static vector<double> split(string line);
    int execute();

};
class ConnectCommand : public command {
public:
    void sendToServer(int client_socket);
    int execute();
};


class DefineVarCommand : public command {
public:
    int execute();
};
class UpdateVarCommand : public command {
    int execute();
};

class PrintCommand : public command {
public:
    int execute();
};
class SleepCommand : public command {
public:
    int execute();
};

class ConditionParser : public command {
protected:
    int returnValue = 1;
    map<string, command *> commandMap;
public:
    virtual int execute() = 0;
    map<string,command*> initMap(string);
    void deleteMap();
    void funcInsideLoop();
    double var1;
    double var2;
    void updateVars();

};
class loopCommand : public ConditionParser {
public:
    int execute();
};

class ifCommand : public ConditionParser {
public:
    int execute();
};

struct func{
    int startIndex;
    int endIndex;
    string varValue;
    string varName;
};

class funcCommand : public ConditionParser {
    map<string,func> funcList;
public:
    int execute();
    void InsideLoop();
    int findCloseSugriim(int);
};

#endif //UNTITLED6_COMMAND_H
