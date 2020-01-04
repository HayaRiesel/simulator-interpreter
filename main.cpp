#include <iostream>
#include "command.h"
using namespace std;

map<string ,command*> initMap();
vector<string> lexer(string* filename);
void parser(vector<string> lexer);
void insideParser(vector<string> lexer , int i, int endIndex,map<string,Var> *symbolTable);
void endProgram();

int main(int argc, char *argv[]) {
    string name = argv[1];
    //char fileName[] = name[0];
    vector<string> array = lexer(&name);
    parser(array);
    //std::this_thread::sleep_for(std::chrono::milliseconds(10000));

}
void endProgram() {


}
vector<string> lexer(string* filename) {
    fstream data;
    data.open(filename[0], ios::in);
    if (!data) {
        std::cout << "file doesn't exsist" << std::endl;
    }
    vector<string> lexerArray;
    string tempLine, tempWord;
    while (getline(data, tempLine)) {
        for (int i = 0; i < tempLine.length(); i++) {
            // put together word after "<="
            if (tempLine[i+1] == '=' && (tempLine[i] == '!' || tempLine[i] == '>' || tempLine[i] == '='|| tempLine[i] == '<')) {
                if (!tempWord.empty()) {
                    lexerArray.push_back(tempWord);
                    tempWord = "";
                }
                lexerArray.push_back(string() + tempLine[i] + tempLine[i+1]);
                i+=2;
                while (tempLine[i] != '\0' && (tempLine[i] != '{') ) {
                    if (tempLine[i] != ' ') {
                        tempWord += tempLine[i];
                    }
                    i++;
                }
                if (tempLine[i] != '{') {
                    lexerArray.push_back(string() + '{');
                }
                lexerArray.push_back(tempWord);
                tempWord = "";
            }
                // all the word after =
            else if (tempLine[i] == '=') {
                if (!tempWord.empty()) {
                    lexerArray.push_back(tempWord);
                    tempWord = "";
                }
                if (tempLine[i+1] == '=') {
                    lexerArray.push_back(string() + tempLine[i] + tempLine[i+1]);
                    i++;
                } else {
                    lexerArray.push_back(string() + tempLine[i]);
                    i++;
                }
                while (tempLine[i] != '\0') {
                    if (tempLine[i] != ' ') {
                        tempWord += tempLine[i];
                    }
                    i++;
                }
                lexerArray.push_back(tempWord);
                tempWord = "";
            }
            if (tempLine[i] == '}') {
                lexerArray.push_back(string() + '}');
                continue;
            }
            if ((tempLine[i] == ' ') || (tempLine[i] == '\t') || (tempLine[i] == '\0') || (tempLine[i] == '(') ||
                (tempLine[i] == ',') || (tempLine[i] == ')')) {
                if (!tempWord.empty()) {
                    lexerArray.push_back(tempWord);
                    tempWord = "";
                }
            } else if (tempLine[i] == '"') {
                tempWord += '"';
                if (i < tempLine.length()) {
                    while (tempLine[i + 1] != '"') {
                        tempWord += tempLine[i + 1];
                        i++;
                    }
                    tempWord += '"';
                    i++;
                    lexerArray.push_back(tempWord);
                    tempWord = "";
                }
            } else {
                tempWord += tempLine[i];
            }
        }
    }
    return lexerArray;
}
void parser(vector<string> lexer) {
    map<string,Var> symbolTable;
    insideParser(lexer,0,lexer.size(),&symbolTable);


}
void insideParser(vector<string> lexer , int i, int endIndex,map<string,Var> *symbolTable){
    map<string,command*> commandMap = initMap();
    Interpreter* interpreter = new Interpreter();
    while (i < endIndex) {
        command *c;
        if(lexer[i+1] == "="){
            c = commandMap[lexer[i+1]];
        }
        else {
            if(commandMap.find(lexer[i]) != commandMap.end()){
                c = commandMap[lexer[i]];
            }
            else{
                c = commandMap["func"];
            }
        }
        c->setLexer(lexer);
        c->setIndex(i);
        c->updateSymbolTable(symbolTable);
        c->updateInterepter(interpreter);
        i += c->execute();
    }
    mtx.lock();
    programRun = false;
    mtx.unlock();
    //wait to the end of the tread
    this_thread::sleep_for(std::chrono::seconds(10));

    delete(interpreter);
    //delete the map
    map<string,command*>::iterator it;
    for (it = commandMap.begin(); it != commandMap.end(); it++) {
        delete(it->second);
    }

}
map<string,command*> initMap() {
    map<string, command *> map;
    map.insert({"openDataServer", new OpenServerCommand()});
    map.insert({"connectControlClient", new ConnectCommand()});
    map.insert({"var",new DefineVarCommand()});
    map.insert({"Print",new PrintCommand()});
    map.insert({"Sleep",new SleepCommand()});
    map.insert({"=",new UpdateVarCommand()});
    map.insert({"while",new loopCommand()});
    map.insert({"if",new ifCommand()});
    map.insert({"func",new funcCommand()});
    return map;
}
