#!/bin/bash

# Exxecutre fromthe root of the ecits project

echo "Recompiling Ecits"
g++ aux/Publisher.cpp ecits.cpp Consensus.cpp -l c -std=c++0x -lzmq -lprotobuf -lpthread -std=c++11 -ljsoncpp -o ecits
