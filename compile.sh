#!/bin/bash
g++ -std=c++20 src/*.cpp -I./include -ldl -o bin/main
chmod 777 bin/main
chmod 777 main