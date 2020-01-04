//
// Created by yaellax on 17/12/2019.
//
#include "command.h"



void command::setLexer(vector<string> lexer1) {
    this->lexerVector = lexer1;
}

void command::updateSymbolTable(map<string, Var> *symbolTable1) {
    this->symbolTable = symbolTable1;
}

void command::updateInterepter(Interpreter *interpreter1) {
    this->interpreter = interpreter1;
}

void command::setIndex(int i1) {
    i = i1;
}

void OpenServerCommand::initMapOfXml() {
    mapOfXml.insert({"/instrumentation/airspeed-indicator/indicated-speed-kt",0});
    mapOfXml.insert({"/sim/time/warp",1});
    mapOfXml.insert({"/controls/switches/magnetos",2});
    mapOfXml.insert({"/instrumentation/heading-indicator/offset-deg",3});
    mapOfXml.insert({"/instrumentation/altimeter/indicated-altitude-ft",4});
    mapOfXml.insert({"/instrumentation/altimeter/pressure-alt-ft",5});
    mapOfXml.insert({"/instrumentation/attitude-indicator/indicated-pitch-deg",6});
    mapOfXml.insert({"/instrumentation/attitude-indicator/indicated-roll-deg",7});
    mapOfXml.insert({"/instrumentation/attitude-indicator/internal-pitch-deg",8});
    mapOfXml.insert({"/instrumentation/attitude-indicator/internal-roll-deg",9});
    mapOfXml.insert({"/instrumentation/encoder/indicated-altitude-ft",10});
    mapOfXml.insert({"/instrumentation/encoder/pressure-alt-ft",11});
    mapOfXml.insert({"/instrumentation/gps/indicated-altitude-ft",12});
    mapOfXml.insert({"/instrumentation/gps/indicated-ground-speed-kt",13});
    mapOfXml.insert({"/instrumentation/gps/indicated-vertical-speed",14});
    mapOfXml.insert({"/instrumentation/heading-indicator/indicated-heading-deg",15});
    mapOfXml.insert({"/instrumentation/magnetic-compass/indicated-heading-deg",16});
    mapOfXml.insert({"/instrumentation/slip-skid-ball/indicated-slip-skid",17});
    mapOfXml.insert({"/instrumentation/turn-indicator/indicated-turn-rate",18});
    mapOfXml.insert({"/instrumentation/vertical-speed-indicator/indicated-speed-fpm",19});
    mapOfXml.insert({"/controls/flight/aileron",20});
    mapOfXml.insert({"/controls/flight/elevator",21});
    mapOfXml.insert({"/controls/flight/rudder",22});
    mapOfXml.insert({"/controls/flight/flaps",23});
    mapOfXml.insert({"/controls/engines/engine/throttle",24});
    mapOfXml.insert({"/controls/engines/current-engine/throttle",25});
    mapOfXml.insert({"/controls/switches/master-avionics",26});
    mapOfXml.insert({"/controls/switches/starter",27});
    mapOfXml.insert({"/engines/active-engine/auto-start",28});
    mapOfXml.insert({"/controls/flight/speedbrake",29});
    mapOfXml.insert({"/sim/model/c172p/brake-parking",30});
    mapOfXml.insert({"/controls/engines/engine/primer",31});
    mapOfXml.insert({"/controls/engines/current-engine/mixture",32});
    mapOfXml.insert({"/controls/switches/master-bat",33});
    mapOfXml.insert({"/controls/switches/master-alt",34});
    mapOfXml.insert({"/engines/engine/rpm",35});
}

void OpenServerCommand::readFromClient(int client_socket, int socketfd) {
    initMapOfXml();
    string preBuffer;
    bool first = true;
    while (programRun) {
        //reading from client
        char buffer[1024] = {0};
        if(programRun == false){
            break;
        }
        int valread = read(client_socket, buffer, 1024);
        string s(buffer);
        if(s.compare(preBuffer) == 0){
            preBuffer = s;
            continue;
        }if(first){
            first = false;
        }
        //double splitBuffer[1000];
        vector<double> splitBuffer = split(s);

        mtx.lock();
        //for (auto s : *symbolTable) {
        map<string, Var>::iterator it;
        for (it = symbolTable->begin(); it != symbolTable->end(); it++) {
            if (it != symbolTable->end()) {
                if (it->second.updateFromSimulator) {
                    it->second.value = splitBuffer[mapOfXml[it->second.sim]];
                    interpreter->setVariables(it->first + "=" + to_string(it->second.value));
                }
            }
        }
        mtx.unlock();
    }
    this_thread::sleep_for(chrono::seconds(5));
    close(socketfd);
}

vector<double> OpenServerCommand::split(string line) {
    //we need to choose between the \n
    vector<double> array;
    int startOfLine = 0;
    int endOfLine = line.find('\n');
    int endOfLine2 = line.find('\n', endOfLine + 1);
    if (endOfLine2 < line.length()) {
        startOfLine = endOfLine + 1;
        endOfLine = endOfLine2;
    }
    string newLine = line.substr(startOfLine,endOfLine - startOfLine + 1);
    string tempWord = "";
    for (int i = 0; i < newLine.size(); i++) {
        if(line[i] == '\n'){
            array.push_back(stod(tempWord));
            break;
        }
        else if(line[i] == ',') {
            array.push_back(stod(tempWord));
            tempWord = "";
        } else {
            tempWord += line[i];
        }

    }
    return array;
}

int OpenServerCommand::execute()  {
    Expression* e = nullptr;
    e = interpreter->interpret(lexerVector[i+1]);
    int port = (int)e->calculate();
    delete(e);
    //create socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        //error
        std::cerr << "Could not create a socket" << std::endl;
        return -1;
    }

    //bind socket to IP address
    // we first need to create the sockaddr obj.
    sockaddr_in address; //in means IP4
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //give me any IP allocated for my machine
    address.sin_port = htons(port);
    //we need to convert our number
    // to a number that the network understands.

    //the actual bind command
    if (bind(socketfd, (struct sockaddr *) &address, sizeof(address)) == -1) {
        std::cerr << "Could not bind the socket to an IP" << std::endl;
        return -2;
    }

    //making socket listen to the port
    if (listen(socketfd, 5) == -1) { //can also set to SOMAXCON (max connections)
        std::cerr << "Error during listening command" << std::endl;
        return -3;
    } else {
        std::cout << "Server is now listening ..." << std::endl;
    }

    // accepting a client
    int client_socket = accept(socketfd, (struct sockaddr *) &address,
                               (socklen_t *) &address);

    if (client_socket == -1) {
        std::cerr << "Error accepting client" << std::endl;
        return -4;
    }

    thread(&OpenServerCommand::readFromClient,this ,client_socket ,socketfd).detach();

    //for the index
    return 2;

}


void ConnectCommand::sendToServer(int client_socket) {
    while(programRun) {
        while(sendToSimulator == false){}
        mtx.lock();
        map<string, Var>::iterator it;
        for (it = (*symbolTable).begin(); it != (*symbolTable).end(); it++){
            //for (auto const& s : *symbolTable) {

            if(it->second.needToUpdateTheSimulator) {
                string simWithoutSlash = it->second.sim;
                simWithoutSlash.substr(1);
                simWithoutSlash.replace(0,1,"");
                string mes = "set " + simWithoutSlash + " " + to_string(it->second.value) +"\r\n";
                const char* message = mes.c_str();
                int is_sent = send(client_socket, message, strlen(message), 0);
                if (is_sent == -1) {
                    std::cout << "Error sending message" << std::endl;
                } else {
                    std::cout << "message sent to server" << std::endl;
                }
                it->second.needToUpdateTheSimulator = false;
            }
        }
        sendToSimulator = false;
        mtx.unlock();
    }
    close(client_socket);
}

int ConnectCommand::execute() {
    string temp = lexerVector[i+1];
    temp.pop_back();
    //string tempIp = lexerVector[i+1].substr(1, lexerVector[i+1].size()-1);
    char const* ip = &temp[1];
    //int port = stoi(lexerVector[i+2]);
    Expression* e = nullptr;
    e = interpreter->interpret(lexerVector[i+2]);
    int port = (int)e->calculate();
    delete(e);

    //create socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        //error
        std::cerr << "Could not create a socket"<<std::endl;
        return -1;
    }

    //We need to create a sockaddr obj to hold address of server
    sockaddr_in address; //in means IP4
    address.sin_family = AF_INET;//IP4
    address.sin_addr.s_addr = inet_addr(ip);  //the localhost address
    address.sin_port = htons(port);
    //we need to convert our number (both port & localhost)
    // to a number that the network understands.

    // Requesting a connection with the server on local host with port 8081
    int is_connect = connect(client_socket, (struct sockaddr *)&address, sizeof(address));
    if (is_connect == -1) {
        std::cerr << "Could not connect to host server"<<std::endl;
        return -2;
    } else {
        std::cout<<"Client is now connected to server" <<std::endl;
    }
    thread(&ConnectCommand::sendToServer,this,client_socket).detach();
    return 3;

}

int DefineVarCommand::execute() {
    //if : var breaks -> sim("/controls/flight/speedbreak")
    if (lexerVector[i+3] == "sim") {
        Var *var = new Var();
        if (lexerVector[i+2] == "->") {
            var->updateFromSimulator = false;
        }
        if (lexerVector[i+2] == "<-") {
            var->updateFromSimulator = true;
        }
        string tempSim = lexerVector[i+4];
        tempSim.pop_back();
        tempSim.replace(0,1,"");
        var->sim = tempSim;
        var->value = 0;
        //nio realy need this
        mtx.lock();
        interpreter->setVariables(lexerVector[i+1]+"="+to_string(var->value));

        (*symbolTable).insert({lexerVector[i+1], *var});
        mtx.unlock();
        delete(var);
        return 5; //if sim
    }
        //if : var h0 = heading
    else {
        Expression* e = interpreter->interpret(lexerVector[i+3]);
        string var2 = to_string(e->calculate());
        delete e;
        string temp = (lexerVector[i+1]+"="+var2);
        mtx.lock();
        interpreter->setVariables(temp);
        mtx.unlock();
        return 4;
    }
}

int UpdateVarCommand::execute() {
    mtx.lock();
    Expression* e = interpreter->interpret(lexerVector[i+2]);
    double newVal = e->calculate();
    delete e;


    if(symbolTable->find(lexerVector[i])!= symbolTable->end()){
        (*symbolTable)[lexerVector[i]].value = newVal;
        (*symbolTable)[lexerVector[i]].needToUpdateTheSimulator = true;
        sendToSimulator = true;
    }
    interpreter->setVariables(lexerVector[i]+lexerVector[i+1]+to_string(newVal));
    mtx.unlock();
    return 3;
}


int PrintCommand::execute() {
    if(lexerVector[i+1][0]=='"'){
        string  withoutParenthesis = lexerVector[i+1];
        withoutParenthesis.replace(0,1,"");
        withoutParenthesis.pop_back();
        cout << withoutParenthesis << std::endl;
    } else{
        mtx.lock();
        Expression* e = nullptr;
        e = interpreter->interpret(lexerVector[i+1]);
        cout << e->calculate() << std::endl;
        delete e;
        mtx.unlock();
    }
    return 2;
}

int SleepCommand::execute() {
    std::this_thread::sleep_for(std::chrono::milliseconds(stoi(lexerVector[i+1])));
    return 2;
}

void ConditionParser::deleteMap() {
    map<string,command*>::iterator it;
    for (it = commandMap.begin(); it != commandMap.end(); it++) {
        delete(it->second);
    }
}

void ConditionParser::funcInsideLoop() {
    int index = returnValue;
    while (lexerVector[i + index] != "}") {
        command *c;
        if (lexerVector[i + index + 1] == "=") {
            c = commandMap[lexerVector[i + index + 1]];
        } else {
            c = commandMap[lexerVector[i + index]];
        }
        c->setLexer(lexerVector);
        c->setIndex(i + index);
        c->updateSymbolTable(symbolTable);
        c->updateInterepter(interpreter);
        index += c->execute();
    }
}

void ConditionParser::updateVars() {
    mtx.lock();
    map<string,Var>::iterator it = symbolTable->find(lexerVector[i + 1]);
    if(it != symbolTable->end()){
        var1 = (*symbolTable)[lexerVector[i + 1]].value;
    }else{
        Expression* e = nullptr;
        e = interpreter->interpret(lexerVector[i+1]);
        var1 = e->calculate();
        delete e;
    }
    it = symbolTable->find(lexerVector[i + 3]);
    if(it != symbolTable->end()){
        var2 =(*this->symbolTable)[this->lexerVector[this->i+3]].value;
    } else{
        var2 = interpreter->interpret(lexerVector[i+3])->calculate();
        //var2 = stod(lexerVector[i+3]);
    }
    mtx.unlock();

}

map<string, command *> ConditionParser::initMap(string commandName) {
    map<string, command*> map;
    map.insert({"var",new DefineVarCommand()});
    map.insert({"Print",new PrintCommand()});
    map.insert({"Sleep",new SleepCommand()});
    map.insert({"=",new UpdateVarCommand()});
    if(commandName.compare("while") != 0){
        map.insert({"while",new loopCommand()});
    }
    if(commandName.compare("if") != 0){
        map.insert({"if",new ifCommand()});
    }
    return map;
}


int loopCommand::execute() {
    //count the condition + {
    this->returnValue = +5;
    updateVars();
    this->commandMap = initMap("while");

    if (this->lexerVector[this->i + 2] == "<") {
        while (var1 < var2) {
            this->funcInsideLoop();
            updateVars();
        }
    }
    if (this->lexerVector[this->i + 2] == "<=") {
        while (var1 <= var2) {
            funcInsideLoop();
            updateVars();
        }
    }
    if (this->lexerVector[this->i + 2] == ">") {
        while (var1 > var2) {
            this->funcInsideLoop();
            updateVars();
        }
    }
    if (this->lexerVector[this->i + 2] == ">=") {
        while (var1 >= var2) {
            this->funcInsideLoop();
            updateVars();
        }
    }
    if (this->lexerVector[this->i + 2] == "==") {
        while (var1 == var2) {
            this->funcInsideLoop();
            updateVars();
        }
    }
    if (this->lexerVector[this->i + 2] == "!=") {
        while (var1 != var2) {
            this->funcInsideLoop();
            updateVars();
        }
    }
    //free memory
    deleteMap();
    vector<string>::iterator start = lexerVector.begin() + i + returnValue;
    std::vector<string>::iterator it2;
    it2 = find(start, lexerVector.end(), "}");
    int itIndex = distance(start, it2);
    returnValue += itIndex + 1;
    return this->returnValue;
}

int ifCommand::execute() {
    //count the condition + {
    this->returnValue = +5;
    updateVars();
    this->commandMap = initMap("if");

    if (this->lexerVector[this->i + 2] == "<") {
        if (var1 < var2) {
            this->funcInsideLoop();
        }
    }
    if (this->lexerVector[this->i + 2] == "<=") {
        if (var1 <= var2) {
            funcInsideLoop();
        }
    }
    if (this->lexerVector[this->i + 2] == ">") {
        if (var1 > var2) {
            this->funcInsideLoop();
        }
    }
    if (this->lexerVector[this->i + 2] == ">=") {
        if (var1 >= var2) {
            this->funcInsideLoop();
        }
    }
    if (this->lexerVector[this->i + 2] == "==") {
        if (var1 == var2) {
            this->funcInsideLoop();
        }
    }

    if (this->lexerVector[this->i + 2] == "!=") {
        if (var1 != var2) {
            this->funcInsideLoop();
        }
    }
    //free memory
    deleteMap();
    vector<string>::iterator start = lexerVector.begin() + i + returnValue;
    std::vector<string>::iterator it2;
    it2 = find(start, lexerVector.end(), "}");
    int itIndex = distance(start, it2);
    returnValue += itIndex + 1;
    return this->returnValue;
}

int funcCommand::findCloseSugriim(int index){
    int countSugraim = 1;
    for(; countSugraim != 0 ; index++){
        if(lexerVector[index].compare("{") == 0){
            countSugraim++;
        }
        if(lexerVector[index].compare("}") == 0){
            countSugraim--;
        }
    }
    //the index right now on the "}"
    return index-1;
}
int funcCommand::execute() {
    //define func
    if(funcList.find(lexerVector[i]) == funcList.end()){
        func* f = new func;
        f->startIndex = i+4;
        f->varName = lexerVector[i+2];
        f->endIndex = findCloseSugriim(i+4);
        funcList.insert({lexerVector[i],*f});
        delete(f);
        return funcList[lexerVector[i]].endIndex - i +1;
    }
    //the func define allready
    else {
        commandMap = initMap("func");
        funcList[lexerVector[i]].varValue = lexerVector[i + 1];
        InsideLoop();
        deleteMap();
        return 2;
    }
}


void funcCommand::InsideLoop() {
    func func = funcList[lexerVector[i]];
    vector<string> newLexer;
    for(int j=func.startIndex;j<=func.endIndex;j++){
        if(lexerVector[j].compare(func.varName) == 0){
            newLexer.push_back(func.varValue);
        }
        else {
            newLexer.push_back(lexerVector[j]);
        }
    }
    int index = 0;
    while (index < newLexer.size()-1) {
        command *c;
        if (newLexer[index + 1] == "=") {
            c = commandMap[newLexer[index + 1]];
        } else {
            c = commandMap[newLexer[index]];
        }
        c->setLexer(newLexer);
        c->setIndex(index);
        c->updateSymbolTable(symbolTable);
        c->updateInterepter(interpreter);
        index += c->execute();
    }
}
