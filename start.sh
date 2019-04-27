#!/bin/bash   
clear

echo "####### starting compiling Editor v0.3 #########"
echo "|-------------------------------------------|"

rm History/*
rm tmp
if [ -f "Run" ]; then
	make clean
	make
	./Run
else
	make
	./Run
fi
