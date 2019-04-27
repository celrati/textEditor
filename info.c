#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"

void initInfo(Info * info,WINDOW * mainscreen,int width,int height,int posX,int posY){
	info->height = height;
	info->width = width;
	info->posX = posX;
	info->posY = posY;
	info->window = subwin(mainscreen,height,width,posX,posY);

	info->page_verti = 0;
	info->posxCursur = 0;
	info->posyCursur = 0;
	info->notification = (char*)malloc(2 << 8);
	strcpy(info->notification, "~~");

	info->currentBufferInfo = 0;
}
void printInfo(Info * info){
	wborder(info->window, '|', '|', '-', '_', '*', '*', '*', '*');
	mvwprintw(info->window,0,3,"[Informations] | currentBuffer : %d]",info->currentBufferInfo);
	mvwprintw(info->window,1,3,"-->(ESC:exit)(ctrl+a:switch)(F2:copier)(ctrl+z:redo)(ctrl+y:undo)");
	mvwprintw(info->window,2,3,"-->(F3:couper)(F4:coller)(F5: ON/OFF Linewrapping)");
	mvwprintw(info->window,3,3,"--> [%d,%d,%d] notification : %s  ",info->posxCursur,info->posyCursur,info->page_verti,info->notification);

}
void setPositionInfo(Info *info, int x,int y,int z){
	info->posxCursur = x;
	info->posyCursur = y;
	info->page_verti = z;
}
void setNewCurrentBuffer(Info * info, int currentBuffer){
	info->currentBufferInfo = currentBuffer;
}
