#ifndef EDITOR
#define EDITOR

#include "wbuffer.h"
#include "cmd.h"
#include "info.h"

typedef struct Editor{
	Info info;   // pour l'information 
	Cmd cmd;	 // pour la CMD

	// wbuffer lists..
	/////////////////////////////////////////////////
	int currentWbufferNumber;
	Wbuffer  * listWbuffer;
	int howManyBufferRightNow;
	//------------------------------------
	Wbuffer wbuffer;  // pour la liste des buffers
	/////////////////////////////////////////////////

	int currentHand;  // 1 for the wbuffer  &  0 for the cmd
	int currentX;
	int currentY;
		int * listBufferHistoryIndex;
	int * listBufferHistoryIndexRightNow;
}Editor;
	
// les prototypes...
void initEditor(Editor * editor,WINDOW * mainscreen);
void printEditor(Editor *);
int isItTurnOfBuffer(Editor *); // return 1 if for the buffer 0 for the cmd
void giveTheHandToTheBuffer(Editor*);
void giveTheHandToTheCmd(Editor*);
void updateDimEditor(Editor*,WINDOW *);


void makeKeyboardListenerCommand(Editor * editor,Cmd * cmd,int key);
void updateCmd(Cmd * cmd, char key);
void executeCmd(Editor * editor,Cmd * cmd);

#endif