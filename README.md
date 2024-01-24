git clone https://github.com/derejar/GraphicalEditor --recursive
cmake -S . -B build -G "MinGW Makefiles"
mingw32-make --directory=build
GraphilcalEditor.exe будет в build
