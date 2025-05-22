# Projekt Symbolische Künstliche Intelligenz

## short info
Build of a small AI for the 2 player game ["Guard and Towers"](http://www.christoph-endres.de/documents/games/tuw-de.pdf).\
Projekt contains a small Server using python, a client to play ur self, a client for the AI and the AI itself. \
To optimize the AI we will use: bitboards, Alpha-Beta Search and other techniques.

## Dependencies for C++ Client and AI
- #### [cmake](https://cmake.org/download/) as our Build System
- ####  [GNU Compiler Collection](https://gcc.gnu.org/install/index.html) which supports C++ version 20 
- #### for Multithreading, we use [Threading Building Blocks](https://uxlfoundation.github.io/oneTBB/index.html)

## getting started
Go to your designated project folder
```
cd existing_repo
```
and clone the project.\
ssh:
```
git clone git@git.tu-berlin.de:hallo123/projekt-ki.git
```
https:
```
git clone https://git.tu-berlin.de/hallo123/projekt-ki.git
```
Now just download the submodules using:
```
git submodule update --init --recursive --force
```

### Build

If u use CLION u should just right-click the cmake file select to reload it.\
Afterward u should be able to pick a few configs like:

``` projekt_ki ``` or ``` tests ```  or ``` benchmarks ``` or ``` Client ```\
\
If u plan on using the Terminal here are the needed instructions for  Initialization:
First we create a local build folder.\
```
mkdir build && cd build && cmake ..
```
Next u can build the project with from the projekt root directory:\
``` cmake --build . ``` \
or a particular target using:\
``` cmake --build . --target tests ```\
Lastly you are now ready to run the binary from ur build folder:
``` ./projekt_ki ``` \
or other generated binaries like ``` ./tests ```.

## Info for Client
The Client works in combination with the Server in the `server` directory and use our KI as the Player.
The IP and Port are read at runtime from clientInfo/config.txt:
```
127.0.0.1
5555
```
The Client itself is a C++ Version of the [client.py](server/client.py) provided by the `server` and is held extremely basic.
Containing only complicated code to make it work as a Windows Executable.\
TIP: u can just start 2 instances of the Client and let the two Ai's duke it out on their own.


## Info for Server
This Server was provided by the by Sarah Lengfeld an WiMi. \
Alle Infos can be read in the [README](server/README.md).

## Bit Board definition
We us 64 Bit, which get used as followed:\
[x \
x b b b b b b b x \
x b b b b b b b x \
x b b b b b b b x \
x b b b b b b b x \
x b b b b b b b x \
x b b b b b b b x \
x b b b b b b b x] \
63-----------------0
- b are the bits used for the board
- x are the separating bits

## contribution guidelines
Variable names in snake_case.\
Class names in CamelCase.

##  tools

We use external libraries and headers saved in the Directory /ext\
For testing and performance analysis we use [Catch 2](https://github.com/catchorg/Catch2).\
For reading in csv files we use a [csv parser](https://github.com/vincentlaucsb/csv-parser).\
For Json parsing in the client we use the [parser](https://github.com/nlohmann/json) by nlohmann.

## interesting references 

[Bitboards](https://ameye.dev/notes/chess-engine/)
