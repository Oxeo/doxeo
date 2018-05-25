#!/bin/bash

git pull
if [ $? -ne 0 ]; then
    printf  "\n=> ERROR: Git pull failed!\n"
    exit
fi

make release
if [ $? -ne 0 ]; then
    printf  "\n=> ERROR: Compilation failed!\n"
    exit
fi

printf  "\nStopping Doxeo-Monitor...\n"
./doxeo-monitor --quit
if [ $? = 0 ]; then
    printf  "=> Doxeo-Monitor stopped!\n"
fi

printf  "\nStarting Doxeo-Monitor...\n"
sleep 10
./doxeo-monitor &

printf  "=> Success!\n"
