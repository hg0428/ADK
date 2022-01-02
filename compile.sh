#!/bin/bash
GREEN="\e[32m"
ENDCOLOR="\e[0m"

echo -e "${GREEN}Building...${ENDCOLOR}"
g++ -std=c++20 src/*.cpp -I./include -ldl -o bin/main
echo -e "${GREEN}Done!${ENDCOLOR}"
chmod 777 bin/main
chmod 777 main