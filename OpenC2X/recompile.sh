#!/bin/bash

# Exxecutre fromthe root of the OpenC2X project

echo "Recompiling OpenC2X"
cd common/asn1/
./generate.sh
cd ../../
mkdir build
cd build
cmake ..
make all

cd ../scripts
sudo ./runOpenC2X.sh 