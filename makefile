CC=gcc
CFLAGS=-W -Wall -lncurses

all: Run

Run:  SyntaxModule.o mainprogram.o editor.o wbuffer.o cmd.o info.o 
	gcc -o Run  SyntaxModule.o mainprogram.o editor.o wbuffer.o cmd.o info.o  -lncurses

SyntaxModule.o: SyntaxModule.c SyntaxModule.h
	gcc -c SyntaxModule.c

cmd.o: cmd.c cmd.h
	gcc -c cmd.c

info.o: info.c info.h
	gcc -c info.c

wbuffer.o:  wbuffer.c wbuffer.h SyntaxModule.h
	gcc -c wbuffer.c

editor.o: editor.c wbuffer.h info.h cmd.h
	gcc -c editor.c

mainprogram: mainprogram.c editor.h
	gcc -c mainprogram.c

clean: 
	rm mainprogram.o editor.o wbuffer.o cmd.o info.o SyntaxModule.o Run


 