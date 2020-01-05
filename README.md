Interpreter flight simulator, CPP language - HW3

Description

the purpose of this project is to run the flight simulator by a code lines.
our project read a txt file, that we get from the command line arguments,  analyze the file to  code lines.  the code run the flight 
simulator by get data and sending command to the simulator follow the txt file.

Program structure:

the program have two parts.
*  the parts that read the txt file and making command from it
* the parts that execute the commands.

reading the txt file
we build a lexer that read the text from the file and put it into a vector in a way that will be comfortable to read from it. we also build a parser that take the vector analyze every command the write in the vector and send it to the part that execute the command.

execute the commands
every command have a  special function that handle this specific command. there is 2 command that responsible to the connection with the flight simulator.
“OpenDataServer” - command that open a server that the simulator connected to it as client.  the simulator send data to the program and update value in the symbole table.
“symbolTable” - MAP that holding all the data of the vars that the simulator need, we have to update the map after every connect.
“connectCommand” -  command that connect to the simulator as client. every time that our code update one of our vars in the symbolTable 
we send a message to the simulator to update the value of this in the simulator.

Installation

To use our program you need to download “FLIGHT SIMULATOR” 
you can do this on this link - ‫‪http://home.flightgear.org‬‬.
after the installation, open it and add to the setting->Additinal setting this line:

“--generic=socket,out,10,127.0.0.1,5400,tcp,generic_small
--telnet=socket,in,10,127.0.0.1,5402,tcp”

this line used the connections.
now you need to compile our programs in your computer’ use this line:

“g++ -std=c++14 */*.cpp  *.cpp -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -o a.out -pthread”

now, put the “FLY.TXT” in the same folder and write

“./a.out fly.txt”

and then the program will start.

Support

for any questions you can talk with us in this Email
yaellax@gmail.com
hayaler88@gmail.com
Authors and acknowledgment
Haya Rizel and Yael Lax-Thee,  CS students from Bar-Ilan university, Israel.

