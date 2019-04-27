#ifndef CMD
#define CMD



typedef struct Cmd{
	WINDOW * window;

	int height;
	int width;

	int posX;
	int posY;

	char * commandToExecute;

	int currentX;


}Cmd;

void initCmd(Cmd * cmd,WINDOW * mainScreen,int width,int height,int posX,int posY);
void printCmd(Cmd * cmd);

void redimCmd(Cmd * cmd,WINDOW * mainScreen,int width,int height,int posX,int posY);

#endif