#ifndef WBUFFER
#define WBUFFER


typedef struct Wbuffer{
	WINDOW * window;

	unsigned int height;
	unsigned int width;

	unsigned int posX;
	unsigned int posY;

    char ** rows;
    int currentX;
    int currentY;

 	int nbLines;
 	int decal;
 	int line_decal;
  //////////////////// page verti hori
 	int page_verti;
  int page_hori;
  //////////////////////////
  int MODE;  // 1 for line wraping  0 for scrolling hori

  ////////////::
  char * extention;

}Wbuffer;

typedef struct MouseSelct MouseSelct;
struct MouseSelct{
  int bpX; // X de boutton pressed
  int bpY; // Y de boutton pressed
  int brX; // X de boutton released
  int brY; // Y de boutton released
  char *st; // selected text
  char *ct; // copied text
};

typedef struct KeyBoardSelct KeyBoardSelct;
struct KeyBoardSelct{
  int startX; // X de boutton pressed
  int startY; // Y de boutton pressed
  int endX; // X de boutton released
  int endY; // Y de boutton released
	int var; // voir la fonctionnalité dans la fonction initSelectK
  char *st; // selected text
  char *ct; // copied text
};


void initWbuffer(Wbuffer * wbuffer,WINDOW * mainscreen,int width,int height,int posX,int posY);
void printWbuffer(Wbuffer * wbuffer);

void ncurses_mouse();
void mouse_move(Wbuffer *bf,int car, MouseSelct *ms);
int souris_getpos(int *x, int *y, int *bouton);
void init_selc(MouseSelct *ms);
void initSelectK(KeyBoardSelct *ks);
void mouse_ev(Wbuffer *bf, int sx, int sy);

void setMouseData(MouseSelct *ms);
int selectKeys(int c);
void setKeybData(KeyBoardSelct  *ks, Wbuffer *bf);
void keyBoardMove(Wbuffer *bf,int key, KeyBoardSelct *ks);

int is_f_key(int caractere);
void move_to_last(Wbuffer *bf,MouseSelct *ms, KeyBoardSelct *ks);
void replace_st(Wbuffer *bf, int c , MouseSelct *ms,KeyBoardSelct *ks);
void crush_st(Wbuffer *bf, int c , MouseSelct *ms, KeyBoardSelct *ks);
void execute_command(int car, Wbuffer *bf, MouseSelct *ms, KeyBoardSelct *ks);
void execute_cut(Wbuffer *bf, MouseSelct *ms, KeyBoardSelct *ks);
int isChar(int caractere);

void scrol_hori(Wbuffer *bf);
void decal_counter(Wbuffer *bf);
void insertChar(Wbuffer * bf,int caractere, MouseSelct *ms, KeyBoardSelct *ks);
void moveCursurTo(Wbuffer * bf, char dir); // U D R L
void makeKeyboardArrowsListener(Wbuffer *bf,int a, MouseSelct *ms);

void insertLine(Wbuffer * bf, char * line);
void removeLine(Wbuffer * bf, int ligne);

int isCommand(int caractere);
void redimBuffer(Wbuffer * wbuffer,WINDOW * mainscreen,int width,int height,int posX,int posY);
char * getsbstr(char * str,int decal_page_hori);

void printWithColor(char * line, Wbuffer * wbuffer);
#endif
