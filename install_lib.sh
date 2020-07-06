#!/bin/bash

cd Lib_TCP_UDP_Server
./compile.sh ../build
cd ..

cd Lib_TCP_UDP_Client
./compile.sh ../build
