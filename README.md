# Projekt Symbolische Künstliche Intelligenz

## short info
Build of an small KI for the 2 player game "Guard and Towers".
To optimize the Project we will us: bitboard, ...

## Dependencies
- #### [cmake](https://cmake.org/download/)
- #### any c++ compiler
- ### [AdaptiveCpp](https://github.com/AdaptiveCpp/AdaptiveCpp/tree/develop)
  - #### python 3
  - #### [the Boost C++ libraries](https://www.boost.org/)
  - #### [official LLVM release >= 15](https://releases.llvm.org/download.html)


## getting started
Go to your designated project folder and clone the project.
```
cd existing_repo
```
ssh:
```
git clone git@git.tu-berlin.de:hallo123/projekt-ki.git
```
https:
```
git clone https://git.tu-berlin.de/hallo123/projekt-ki.git
```
Next just download the submodules using:
```
git submodule update --init --recursive --force
```
Next we need to build the compiler using AdaptiveCpp:\
For Linux: \
(maybe set BOOST_ROOT in bash file or follow intsructions for other [backend](https://github.com/AdaptiveCpp/AdaptiveCpp/blob/develop/doc/installing.md))
```
cd ext/AdaptiveCpp
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=../../../compiler/  ..
make install
```
Setup done

---
For Mac:\
First set Flags in Terminal (for zsh in ./zshrc):
```
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/llvm/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm/include"

export LLVM_DIR="/opt/homebrew/Cellar/llvm/20.1.4_1/lib/cmake/llvm"
export BOOST_ROOT="/opt/homebrew/Cellar/boost/1.88.0"
```
After for reload term source ```~/.zshrc```\
Next Compile:
```
cd ext/AdaptiveCpp
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=../../../compiler/ -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm/bin/clang -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ ..
make install
```
Setup done

---

### Build project

If u use CLION u should just richt-click the cmake file select to reload it.\
Afterward u should be able to pick the run config: ``` projekt_ki ``` and the test config: ``` tests ```\
\
If u plan on using the Terminal here are the needed instructions for  Initialization:
First we create an local build folder.\
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

### contribution guidelines
Variable names in snake_case.\
Class names in CamelCase.

###  tools

We use external libraries saved in the Directory /ext .\
For testing and performance analysis we use [catch2](https://github.com/catchorg/Catch2).

### interesting references 

[Bitboards](https://ameye.dev/notes/chess-engine/)
