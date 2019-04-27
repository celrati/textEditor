#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"

void redimCmd(Cmd * cmd,WINDOW * mainscreen,int width,int height,int posX,int posY){
	cmd->height = height;
	cmd->width = width;
	cmd->posX = posX;
	cmd->posY = posY;
	cmd->window = subwin(mainscreen,height,width,posX,posY);	
}
void initCmd(Cmd * cmd,WINDOW * mainscreen,int width,int height,int posX,int posY){

	cmd->height = height;
	cmd->width = width;
	cmd->posX = posX;
	cmd->posY = posY;
	cmd->window = subwin(mainscreen,height,width,posX,posY);
	cmd->commandToExecute = (char*)calloc(2 << 8,1);
	strcpy(cmd->commandToExecute, "");
	cmd->currentX = 19;
}
void printCmd(Cmd * cmd){
	wborder(cmd->window, '|', '|', '-', '_', '*', '*', '*', '*');
	mvwprintw(cmd->window,0,3,"[Command]");
	mvwprintw(cmd->window,1,3,"Editor3/~/cmd# :%s",cmd->commandToExecute);
}



