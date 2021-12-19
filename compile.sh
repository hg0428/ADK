#!/bin/bash
g++ -std=c++20 src/*.cpp -I./include -ldl -o bin/main
chmod 777 main
#C++20 (experimental)