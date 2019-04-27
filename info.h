#ifndef INFO
#define INFO


typedef struct Info{
	WINDOW * window;

	unsigned int height;
	unsigned int width;

	unsigned int posX;
	unsigned int posY;
	unsigned int page_verti;

	int posxCursur;
	int posyCursur;
	char * notification;

	int currentBufferInfo;

}Info;

void initInfo(Info * info,WINDOW * mainscreen,int width,int height,int posX,int posY);
void printInfo(Info * info);
void setPositionInfo(Info *info, int x,int y,int z);
void setNewCurrentBuffer(Info * info, int currentBuffer);

#endif
