#!/bin/sh

rm ./dibujar
gcc dibujar.c -o dibujar -lGL -lGLU -lglut -lm
if [[ -f ./dibujar ]];then
	cp -vf ./dibujar /bin
	dibujar
fi
