#!/bin/bash

make clean
make

mkdir outputs/bin
cp ./out/iot-server outputs/bin
cp ./out/dh2048.pem outputs/bin
cp ./out/server.pem outputs/bin
cp ./out/settings.ini outputs/bin
rm -rf ./out/*
cp -R IOTPanel/output/ ./outputs/www
cp -R ./doc ./outputs/