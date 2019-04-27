#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "wbuffer.h"
#include "SyntaxModule.h"


void redimBuffer(Wbuffer * wbuffer,WINDOW * mainscreen,int width,int height,int posX,int posY){
  wbuffer->height = height;
  wbuffer->width = width;
  wbuffer->posX = posX;
  wbuffer->posY = posY;
  wbuffer->window = subwin(mainscreen,height,width,posX,posY);
}

void initWbuffer(Wbuffer * wbuffer,WINDOW * mainscreen,int width,int height,int posX,int posY){
  wbuffer->height = height;
  wbuffer->width = width;
  wbuffer->posX = posX;
  wbuffer->posY = posY;
  wbuffer->window = subwin(mainscreen,height,width,posX,posY);

  wbuffer->currentY = 0;  // initialiser les posX & pos Y with (0,0)
  wbuffer->currentX = 0;  // initialiser les posX & pos Y with (0,0)
  wbuffer->rows = (char**)malloc(2 << 16); // allouer 2^16 ligne to the buffer
  wbuffer->rows[0] = (char*)malloc(2 << 16); // allouer la premiere ligne avec 2^1 char !!
  //***********
  /*
  int i;
  for(i=0; i< 1000;i++){
  wbuffer->rows[i] = (char*)malloc(2 << 16);
}
*/
//***********
wbuffer->nbLines = 1;  // initialiser ca avec 1 juste pour commencer
wbuffer->decal = 0;

wbuffer->page_verti = 0;
wbuffer->page_hori  = 0;
wbuffer->MODE = 0;

wbuffer->extention= "c"; // pour tester
}

void initSelectK(KeyBoardSelct *ks){
  ks->startX = 0;
  ks->startY = 0;
  ks->endX = 0;
  ks->endY = 0;
  ks->var = 0; // cette variable sert a dire sire on a clické sur le bouton de selection la premiere ou la deuxieme fois
  // sa modification servira a dire que la selection clavier est annulée
  ks->st = (char *)malloc((2 << 16)*sizeof(char)); // selected texte
  ks->ct = (char *)malloc((2 << 16)*sizeof(char));
  strcpy(ks->st,"");
}

void init_selc(MouseSelct *ms){
  ms->bpX = 0;
  ms->bpY = 0;
  ms->brX = 0;
  ms->brY = 0;
  ms->st = (char *)malloc((2 << 16)*sizeof(char)); // selected texte
  ms->ct = (char *)malloc((2 << 16)*sizeof(char));
  strcpy(ms->st,"");
  //strcpy(ms->ct,"");
}

void setMouseData(MouseSelct *ms){ //
  ms->bpX = 0;
  ms->bpY = 0;
  ms->brX = 0;
  ms->brY = 0;
  strcpy(ms->st,"");
}

void setKeybData(KeyBoardSelct *ks, Wbuffer *bf){ //
  ks->startX = bf->currentX;
  ks->startY = bf->currentY;
  ks->var = 1;
  ks->endX = 0;
  ks->endY = 0;
  strcpy(ks->st,"");
}


void printWbuffer(Wbuffer* wbuffer){
  wborder(wbuffer->window, ' ', ' ', ' ', '=', ' ', ' ', '=', '=');
  int i,j;

  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(8, COLOR_WHITE, COLOR_BLACK);

  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);

  for(i=wbuffer->page_verti ; i< wbuffer->nbLines && i < (wbuffer->height - 1 + wbuffer->page_verti) ; i++){
    if(wbuffer->MODE == 0){
      char * tmp = (char *) malloc(2<<8);
      if(strlen(wbuffer-> rows[i]) < wbuffer->page_hori * COLS){
        tmp ="";
      }else{
        char * tmp2 = wbuffer-> rows[i];
        strncpy(tmp,tmp2+(wbuffer->page_hori * COLS),COLS);
      }
      //printw("%s\n",wbuffer->rows[i]);

      if(strlen(tmp)==COLS)
      printw("%s",tmp,wbuffer);
    //printWithColor(tmp,wbuffer);
      else
      printw("%s\n",tmp,wbuffer);
    //printWithColor(tmp,wbuffer);


    }else{
      printw("%s\n",wbuffer-> rows[i],wbuffer);
      //printWithColor(wbuffer->rows[i],wbuffer);
    }
  }

}

/*char * getsbstr(char * str,int decal_page_hori){
char * buf = (char*)malloc(2<<8);
int i,j;
//if(strcmp(str,"") == 0 ) return "";
for(i=0+decal_page_hori,j=0; i < COLS +decal_page_hori -1 && i <strlen(str); buf[j++]=str[i++]);

buf  = strcat(buf,"\0");
return buf;
}*/

void insertChar(Wbuffer * bf,int caractere, MouseSelct *ms, KeyBoardSelct *ks){
  // on teste ici si le curseur est entre 0 et la longueur de la chaine
  if(caractere == '\t'){
    // 4 spaces == \t
    insertChar(bf,' ',ms,ks);
    insertChar(bf,' ',ms,ks);
    insertChar(bf,' ',ms,ks);
    insertChar(bf,' ',ms,ks);
    return;
  }

  if(caractere == KEY_BACKSPACE){
    // ici on tester is le char est un DEL est aussi plus grand que 0
    if (bf->currentX == 0 && bf->currentY > 0){ // le cas ou il faut supprier une ligne // on veut que le curseur monte si on est a la premiere ligne
      // on supprime la ligne et on garde ce qui reste derriere on l ajoute ala fin de la nouvelle ligne

      removeLine(bf,bf->currentY);
    } else if(!((bf->currentX == 0) && (bf->currentY == 0))){
      bf->currentX--;
      int i;
      for(i= bf->currentX ; i < strlen(bf->rows[bf->currentY]) ; i++){
        // c juste pour ecraser le char i par char i+1
        bf->rows[bf->currentY][i] = bf->rows[bf->currentY][i+1];
      }
    }
    crush_st(bf,caractere,ms,ks);
  }
  else if(isChar(caractere)){ // pour remplacer une zone selectionner

    replace_st(bf,caractere,ms,ks);
    int i;
    for( i= strlen(bf->rows[bf->currentY])  ; i> bf->currentX  ; i--){
      bf->rows[bf->currentY][i] = bf->rows[bf->currentY][i-1];
    }
    bf->rows[bf->currentY][bf->currentX] = caractere;
    moveCursurTo(bf,'R');
  }

  else if(caractere == '\n'){


    bf->rows[ bf->nbLines] = (char*)realloc(bf->rows[ bf->nbLines], 2 << 16);



    replace_st(bf,caractere,ms,ks);
    bf->rows[ bf->nbLines] = (char*)realloc(bf->rows[ bf->nbLines], 2 << 16);
    int i;
    for(i = bf->nbLines  ; i > bf->currentY  ; i-- ){
      free(bf->rows[i]);
      bf->rows[i] = (char*)calloc(2 << 16,1);
      strcpy(bf->rows[i],bf->rows[i-1]);
    }
    char * tmp = (char*)calloc(strlen(bf->rows[(bf->currentY)]) - bf->currentX + 1,1);
    int j=0;
    for(i=bf->currentX; i<strlen(bf->rows[(bf->currentY)]);i++){
      tmp[j] = bf->rows[(bf->currentY)][i];
      j++;
    }
    free(bf->rows[(bf->currentY) +1]);
    bf->rows[(bf->currentY) +1] = (char*)calloc(2 << 16,1);
    strcpy(bf->rows[(bf->currentY) +1],&tmp[0]);
    free(tmp);
    tmp = (char*)calloc(2<<16,1);
    for(i=0;i<bf->currentX;i++){
      tmp[i] = bf->rows[bf->currentY][i];
    }
    free(bf->rows[bf->currentY]);
    bf->rows[bf->currentY] = (char*)calloc(2 << 16,1);
    strcpy(bf->rows[bf->currentY],&tmp[0]);
    bf->nbLines++;
    bf->currentX = 0; // mettre la position a 0

    moveCursurTo(bf,'N'); // faire changer la position to "Down"

  } else if(isCommand(caractere)){
    execute_command(caractere,bf,ms,ks);
  }/*else if (selectKeys(caractere)){

  }*/
}

void execute_command(int car, Wbuffer *bf, MouseSelct *ms, KeyBoardSelct *ks){

  if(car == KEY_F(2)){ // copier

    //if( strlen(ms->st) > 0 ){
    strcpy(ms->ct,""); //mouse
    strcpy(ms->ct,ms->st);
    strcpy(ms->st,"");
    //}

    //if( strlen(ks->st) > 0 ){
    strcpy(ks->ct,""); // keyb
    strcpy(ks->ct,ks->st);
    strcpy(ks->st,"");
    //}

  } else if(car == KEY_F(3)){ // couper

    strcpy(ms->ct,""); //mouse
    strcpy(ms->ct,ms->st);
    strcpy(ms->st,"");

    strcpy(ks->ct,""); //keyb
    strcpy(ks->ct,ks->st);
    strcpy(ks->st,"");

    execute_cut(bf,ms,ks);


  } else if(car == KEY_F(4)){ // coller

    int i;
    for(i = 0; i<strlen(ms->ct);i++){
      move(bf->currentY,bf->currentX);
      clear();
      insertChar(bf,ms->ct[i],ms,ks);
    }
    strcpy(ms->st,"");


    int j;
    for(j = 0; j<strlen(ks->ct);j++){
      move(bf->currentY,bf->currentX);
      clear();
      insertChar(bf,ks->ct[j],ms,ks);
    }
    strcpy(ks->st,"");

  }else if(car == KEY_F(5)){ // LINE WRAPPING
    if (bf->MODE == 0) {
      bf->MODE = 1;
    }else{
      bf->MODE = 0;
    }
  }
}

// supprimer une zone selection
void crush_st(Wbuffer *bf, int c , MouseSelct *ms ,KeyBoardSelct *ks){
  if(strlen(ms->st)>0){
    while(strlen(ms->st) > 1){ // pour supprimer une zone selectionner
      ms->st[strlen(ms->st)-1] = '\0';
      insertChar(bf,c,ms,ks);
    }
    strcpy(ms->st,"");
  }else

  if(strlen(ks->st) > 0 ){
    while(strlen(ks->st) > 1){ // pour supprimer une zone selectionner
      ks->st[strlen(ks->st)-1] = '\0';
      insertChar(bf,c,ms,ks);
    }
    strcpy(ks->st,"");
  }
}

// remplacer une zone selection par un texte a coller
void replace_st(Wbuffer *bf, int c , MouseSelct *ms, KeyBoardSelct *ks){
  if(strlen(ms->st)>0){
    while(strlen(ms->st) > 0){
      insertChar(bf,KEY_BACKSPACE,ms,ks);
    }
    strcpy(ms->st,"");
  }else
  if(strlen(ks->st)>0){
    while(strlen(ks->st) > 0){
      insertChar(bf,KEY_BACKSPACE,ms,ks);
    }
    strcpy(ks->st,"");
  }
}

int isCommand(int caractere){
  if(caractere == KEY_F(2) || // copier
  caractere == KEY_F(3) || // couper
  caractere == KEY_F(4) || // coller
  caractere == KEY_F(5)) // LW
  {
    return 1;
  }
  return 0;
}

void move_to_last(Wbuffer *bf, MouseSelct *ms, KeyBoardSelct *ks){

  if (strlen(ms->st)>0  ){
    if(ms->brY < ms->bpY ){
      bf->currentX = ms->bpX;
      bf->currentY = ms->bpY ;
    } else if(ms->brY == ms->bpY && ms->brX < ms->bpX){
      bf->currentX = ms->bpX;
    }
  }
  if (strlen(ks->st)>0  ){
    if(ks->endY < ks->startY ){
      bf->currentX = ks->startX;
      bf->currentY = ks->startY ;
    } else if(ks->endY == ks->startY && ks->endX < ks->startX){
      bf->currentX = ks->startX;
    }
  }

}

void execute_cut(Wbuffer *bf, MouseSelct *ms, KeyBoardSelct *ks){
  int i, k;

  for(k = 0; k<strlen(ks->ct); k++){
    move(bf->currentY,bf->currentX);
    clear();
    insertChar(bf,KEY_BACKSPACE,ms,ks);
  }
  for(i = 0; i<strlen(ms->ct); i++){
    move(bf->currentY,bf->currentX);
    clear();
    insertChar(bf,KEY_BACKSPACE,ms,ks);
  }
}

int is_f_key(int caractere){
  int i;
  for(i=1; i<63;i++){
    if (caractere == KEY_F(i)) return 1;
  }
  return 0;
}

int isChar(int caractere){
  if(caractere != '\n' && caractere != KEY_UP && caractere != KEY_SRIGHT && caractere != KEY_MOUSE &&
  caractere != KEY_DOWN && caractere != KEY_LEFT && caractere != KEY_SLEFT && caractere != 2 &&
  caractere != KEY_RIGHT && caractere != KEY_RESIZE && !is_f_key(caractere)){
    return 1;
  }
  return 0;
}



int selectKeys(int c){
  if(c == KEY_SLEFT || c == KEY_SRIGHT) return 1;
  return 0;
}

void makeKeyboardArrowsListener(Wbuffer *bf, int a, MouseSelct *ms){ // ca comme une fonction listner de java
  switch(a){
    case KEY_LEFT:{
      if(bf->currentX > 0){
        setMouseData(ms);
        moveCursurTo(bf,'L');
      } else if ( bf->currentX == 0  && bf->currentY > 0 ) {
        setMouseData(ms);
        moveCursurTo(bf,'U');
        int i=0;
        for(i; i< strlen(bf->rows[bf->currentY]);i++){
          moveCursurTo(bf,'R');
        }
      }
      break;
    }
    case KEY_UP:{
      if(bf->currentY > 0 || bf->currentX > COLS){
        setMouseData(ms);
        moveCursurTo(bf,'U');
      }
      break;
    }
    case KEY_RIGHT:{
      if(bf->currentX < strlen(bf->rows[bf->currentY])){
        setMouseData(ms);
        moveCursurTo(bf,'R');
      } else if ( bf->currentX == strlen(bf->rows[bf->currentY]) && bf->currentY < bf->nbLines -1 ){
        setMouseData(ms);
        moveCursurTo(bf,'D');
        int i=bf->currentX;
        for(i;i>0;i--){
          moveCursurTo(bf,'L');
        }

      }
      break;
    }
    case KEY_DOWN:{
      setMouseData(ms);
      /*if(bf->currentY + bf-> page_verti == bf->nbLines-1) return;
      if(bf->currentY  >= (bf->height -1))  bf->page_verti++;
      else*/
      if(bf->MODE == 0){
        if(bf->currentY < bf->nbLines - 1 )
          moveCursurTo(bf,'D');
        else if(bf->currentY == bf->nbLines -1){
            bf->currentX = bf->currentX = strlen(bf->rows[bf->currentY]);
          }
      }else if((bf->currentY < bf->nbLines - 1 ) || (bf->currentX-(bf->currentX % COLS)+ COLS) < strlen(bf->rows[bf->currentY]) ){
        moveCursurTo(bf,'D');
      }
      break;
    }
  }
}

void moveCursurTo(Wbuffer * bf, char dir){ // fonctions pour changer la positin du cursur
  int col = COLS;
  switch(dir){
    case 'U':{
      if(bf->MODE == 0){
        if (bf->currentY > 0 ){
          bf->currentY--;
          if (bf->currentX >= strlen(bf->rows[bf->currentY])){
            bf->currentX = strlen(bf->rows[bf->currentY]);
          }
          if(bf->currentY == bf->page_verti-1  ){
            bf->page_verti--;
          }
        }
      }else{
        if((bf->currentX/col== 0)){
          if (bf->currentY > 0 ){
            bf->currentY--;
            if (bf->currentX >= strlen(bf->rows[bf->currentY])%col){
              bf->currentX = strlen(bf->rows[bf->currentY]);
            } else {
              bf->currentX = ((strlen(bf->rows[bf->currentY])/col)*col)+bf->currentX;
            }
          }
        }else{
          bf->currentX = bf->currentX - col ;
        }
      }
      break;

    }
    case 'D':{
      if(bf->MODE == 0){
          bf->currentY++;
          if (bf->currentX >= strlen(bf->rows[bf->currentY])){
            bf->currentX = strlen(bf->rows[bf->currentY]);
          }
          if(bf->currentY - bf->page_verti == bf->height -1 ){
            bf->page_verti++;
          }

      }else{
        if((bf->currentX-(bf->currentX % COLS)+ COLS) < strlen(bf->rows[bf->currentY])){

          if(bf->currentX + COLS >= strlen(bf->rows[bf->currentY])){
            bf->currentX = strlen(bf->rows[bf->currentY]);
          }else{
            bf->currentX = bf->currentX + col ;
          }
        }else{
          bf->currentY++;
          if(bf->currentX % COLS >= strlen(bf->rows[bf->currentY])){
            bf->currentX = strlen(bf->rows[bf->currentY]);
          }else{
            bf->currentX = bf->currentX % COLS;
          }
        }
      }
      break;
    }
    case 'N':{
      if(bf->MODE == 0){
        bf->currentY++;

        if(bf->currentY - bf->page_verti == bf->height - 1 ){
          bf->page_verti++;
        }
      }else{
        bf->currentY++;
        if(bf->currentX % COLS >= strlen(bf->rows[bf->currentY])){
          bf->currentX = strlen(bf->rows[bf->currentY]);
        }else{
          bf->currentX = bf->currentX % COLS;
        }
      }
      break;
    }
    case 'R':{
      if ( strlen(bf->rows[bf->currentY]) > bf->currentX ){
        bf->currentX++;
      } /*else {
        bf->curre
      }*/

      break;
    }
    case 'L':{
      if(bf->currentX > 0){
        bf->currentX--;
      } /*else if(bf->currentX == 0){
        bf->currentY++;
        //moveCursurTo(bf,'U');
      }*/
      break;
    }

  }
}

void insertLine(Wbuffer * bf, char * line){  // cette fonction pour ajouter un ligne dans le buffer
  bf->rows[bf->nbLines] = (char*)malloc(2 << 16);
  strcpy(bf->rows[bf->nbLines],line);
  bf->nbLines++;
  //moveCursurTo(bf,'D');
  //bf->currentY = strlen(bf->rows[bf->currentY]);
}
//////////////////////////////////////////////////
void removeLine(Wbuffer *bf, int ligne){
  int i;
  char *tmp = malloc(sizeof(char)*strlen(bf->rows[bf->currentY ]));
  tmp = bf->rows[bf->currentY ];
  for(i = bf->currentY ; i < bf->nbLines; i++){
    bf->rows[i] = bf->rows[i+1];
  }
  bf->currentY--;
  if(bf->currentY == bf->page_verti-1  ){
    bf->page_verti--;
  }
  bf->currentX = strlen(bf->rows[bf->currentY ]);
  strcat(bf->rows[bf->currentY ],tmp);
  bf->nbLines--;
  free(tmp);

}

void ncurses_mouse() {
  mousemask(ALL_MOUSE_EVENTS, NULL);
}



int souris_getpos(int *x, int *y, int *bouton) {
  MEVENT event;
  int resultat = getmouse(&event);
  if(resultat == OK) {
    *x = event.x;
    *y = event.y ;
    *bouton = event.bstate;
  }
  return resultat;
}

void mouse_ev(Wbuffer *bf, int sx, int sy){ // sourisx, sourisy
  //decal_counter(bf);
  //sy += bf->page_verti;
  if(bf->MODE == 0){
    if(sy + bf->page_verti > bf->nbLines-1){
      bf->currentX = strlen(bf->rows[bf->nbLines-1]);
      bf->currentY = bf->nbLines-1;
    }else{
    bf->currentY = sy + bf->page_verti;
    if(sx+ (COLS * bf->page_hori) > strlen(bf->rows[bf->currentY])){
      bf->currentX = strlen(bf->rows[bf->currentY]);
    }else{
      bf->currentX = sx+ (COLS * bf->page_hori);
    }
  }
  }else{

  int dec = 0,Li = bf->nbLines-1, affect = 0 , dec_li=0,dec_av = 0 ,dec_act = 0,li_ex = 0;
  for (int i = 0 ; i < bf->nbLines ; i++) {
    dec_av = dec;
    dec =dec+ (strlen(bf->rows[i]) /COLS);
    if(dec+i >= sy && affect == 0) {
      Li = i ;
      dec_act = dec_av ;
      affect = 1;
    }
  }
  if(Li < bf->nbLines ){
    bf->currentY = Li;
    dec_li = strlen(bf->rows[Li]) / COLS ;
    for(int i = 0 ; i<= dec_li ; i++){
      if(dec_act+i+Li == sy)
      li_ex = i;
    }
    //mvprintw(20,0,"Li_ex %d , dec_act %d , sy %d ,sx %d",li_ex,dec_act,sy,sx);
    if(sx + (COLS * li_ex) >= strlen(bf->rows[bf->currentY ]) ){
      bf->currentX = strlen(bf->rows[bf->currentY ]);
    } else {
      bf->currentX = sx + (COLS * li_ex);
    }
  } else {
    bf->currentX = strlen(bf->rows[bf->nbLines-1]);
    bf->currentY = bf->nbLines-1;
  }
  //mvprintw(20,0,"x %d , y %d , dec %d",sx,sy,bf->decal);
}
}

void make_select(Wbuffer *bf, MouseSelct *ms){

  char *tmp_1 = malloc(sizeof(char)*(2<<16)); // la variable qui contient le champs selectionner
  int i,j;
  int cpt = 0;
  if(ms->brY < ms->bpY){ // si la selection est du bas en haut
    for(i = ms->brY; i<=ms->bpY; i++){
      if(i == ms->brY){ // si on est a la prem ligne on enregistre du ms->brX jusqu a la fin de la ligne
        for(j = ms->brX; j<strlen(bf->rows[ms->brY]); j++){
          tmp_1[cpt] = bf->rows[ms->brY][j];
          cpt++;
        }
        tmp_1[cpt] = '\0';
        strcat(tmp_1,"\n");
        cpt++;
      } else if (i == ms->bpY){ // si on est a la derniere ligne on enregistre du debut jusqu au ms->bpX
        //cpt = 0;
        for(j = 0 ; j<ms->bpX; j++){
          tmp_1[cpt] = bf->rows[ms->bpY][j];
          cpt++;
        }
        tmp_1[cpt] = '\0';
      } else { //si on est au milieu on enregistre toute la ligne
        cpt += strlen(bf->rows[i]) + 1; // +1 pour le \n
        strcat(tmp_1,bf->rows[i]);
        strcat(tmp_1,"\n");
      }
    }
  } else if (ms->brY > ms->bpY){ // si la selection est du haut en bas
    for(i = ms->bpY; i<=ms->brY; i++){
      if(i == ms->bpY){ // si on est a la prem ligne on enregistre du ms->brX jusqu a la fin de la ligne
        for(j = ms->bpX; j<strlen(bf->rows[ms->bpY]); j++){
          tmp_1[cpt] = bf->rows[ms->bpY][j];
          cpt++;
        }
        tmp_1[cpt] = '\0';
        strcat(tmp_1,"\n");
        cpt++;
      } else if (i == ms->brY){ // si on est a la derniere ligne on enregistre du debut jusqu au ms->bpX
        for(j = 0 ; j<ms->brX; j++){
          tmp_1[cpt] = bf->rows[ms->brY][j];
          cpt++;
        }
        tmp_1[cpt] = '\0';
      } else { //si on est au milieu on enregistre toute la ligne
        cpt += strlen(bf->rows[i])+1; // +1 pour le \n
        strcat(tmp_1,bf->rows[i]);
        strcat(tmp_1,"\n");
      }
    }
  } else if(ms->bpY == ms->brY){ // si la selection est linéaire

    if(ms->brX < ms->bpX){ // on fait une recuperation normal
      for(i = ms->brX; i<ms->bpX; i++){
        tmp_1[cpt] = bf->rows[bf->currentY][i];
        cpt++;
      }
    } else { // on recupere le champ selectionner a l envers
      for(i = ms->bpX; i<ms->brX; i++){
        tmp_1[cpt] = bf->rows[bf->currentY][i];
        cpt++;
      }
    }
    tmp_1[cpt] = '\0';
  }
  FILE *log2 = fopen("selected_zone.txt","w");
  strcpy(ms->st,tmp_1);
  fputs(ms->st,log2);
  fclose(log2);
  free(tmp_1);
}

//int bp; // first time u press de button
void mouse_move(Wbuffer *bf,int car, MouseSelct *ms){
  int sourisX, sourisY, bouton;
  switch(car){
    case KEY_MOUSE:
    if(souris_getpos(&sourisX, &sourisY, &bouton) == OK) {
      if(bouton & BUTTON1_CLICKED){
        strcpy(ms->st,""); // on vide la chaine ou on enregistre la selection
        mouse_ev(bf,sourisX,sourisY);
        //attroff(COLOR_PAIR(1));
      } else if (bouton & BUTTON1_PRESSED){
        //attron(COLOR_PAIR(1));
        strcpy(ms->st,"");  // on vide la chaine ou on enregistre la selection
        mouse_ev(bf,sourisX,sourisY);
        ms->bpX = bf->currentX;
        ms->bpY = bf->currentY;

      } else if (bouton & BUTTON1_RELEASED){
        mouse_ev(bf,sourisX,sourisY);
        ms->brX = bf->currentX;
        ms->brY = bf->currentY;
        make_select(bf,ms);
        //color_st(ms);
        //attron(COLOR_PAIR(1));
        // on colorie la zone selectionner en ce moment !

      }
    }
    break;
  }
}

void makeSelect2(Wbuffer *bf, KeyBoardSelct *ks){
  char *tmp_1 = malloc(sizeof(char)*(2<<16)); // la variable qui contient le champs selectionner
  int i,j;
  int cpt = 0;
  if(ks->endY < ks->startY){ // si la selection est du bas en haut
    for(i = ks->endY; i<=ks->startY; i++){
      if(i == ks->endY){ // si on est a la prem ligne on enregistre du ms->brX jusqu a la fin de la ligne
        for(j = ks->endX; j<strlen(bf->rows[ks->endY]); j++){
          tmp_1[cpt] = bf->rows[ks->endY][j];
          cpt++;
        }
        tmp_1[cpt] = '\0';
        strcat(tmp_1,"\n");
        cpt++;
      } else if (i == ks->startY){ // si on est a la derniere ligne on enregistre du debut jusqu au ms->bpX
        //cpt = 0;
        for(j = 0 ; j<ks->startX; j++){
          tmp_1[cpt] = bf->rows[ks->startY][j];
          cpt++;
        }
        tmp_1[cpt] = '\0';
      } else { //si on est au milieu on enregistre toute la ligne
        cpt += strlen(bf->rows[i]) + 1; // +1 pour le \n
        strcat(tmp_1,bf->rows[i]);
        strcat(tmp_1,"\n");
      }
    }
  } else if (ks->endY > ks->startY){ // si la selection est du haut en bas
    for(i = ks->startY; i<=ks->endY; i++){
      if(i == ks->startY){ // si on est a la prem ligne on enregistre du ms->brX jusqu a la fin de la ligne
        for(j = ks->startX; j<strlen(bf->rows[ks->startY]); j++){
          tmp_1[cpt] = bf->rows[ks->startY][j];
          cpt++;
        }
        tmp_1[cpt] = '\0';
        strcat(tmp_1,"\n");
        cpt++;
      } else if (i == ks->endY){ // si on est a la derniere ligne on enregistre du debut jusqu au ms->bpX
        for(j = 0 ; j<ks->endX; j++){
          tmp_1[cpt] = bf->rows[ks->endY][j];
          cpt++;
        }
        tmp_1[cpt] = '\0';
      } else { //si on est au milieu on enregistre toute la ligne
        cpt += strlen(bf->rows[i])+1; // +1 pour le \n
        strcat(tmp_1,bf->rows[i]);
        strcat(tmp_1,"\n");
      }
    }
  } else if(ks->startY == ks->endY){ // si la selection est linéaire

    if(ks->endX < ks->startX){ // on fait une recuperation normal
      for(i = ks->endX; i<ks->startX; i++){
        tmp_1[cpt] = bf->rows[bf->currentY][i];
        cpt++;
      }
    } else { // on recupere le champ selectionner a l envers
      for(i = ks->startX; i<ks->endX; i++){
        tmp_1[cpt] = bf->rows[bf->currentY][i];
        cpt++;
      }
    }
    tmp_1[cpt] = '\0';
  }
  FILE *log2 = fopen("selected_zone2.txt","w");
  strcpy(ks->st,tmp_1);
  fputs(ks->st,log2);
  fclose(log2);
  free(tmp_1);
}

void keyBoardMove(Wbuffer *bf, int key, KeyBoardSelct *ks){
  if(key == 2){
    ks->var++;
    if(ks->var == 2){ // on enregistre endX, endY ,puis la zone selectionner

      ks->endX = bf->currentX;
      ks->endY = bf->currentY;
      makeSelect2(bf,ks);

    } else if (ks->var == 1){ // on enregistre startX et startY
      ks->startX = bf->currentX;
      ks->startY = bf->currentY;

    } else {

      setKeybData(ks,bf);
    }
  }

}

void decal_counter(Wbuffer *bf){
  int dec = 0;
  int x = bf->currentX/COLS;

  for (int i = 0 ; i < bf->currentY  ; i++) {
    dec =dec+ (strlen(bf->rows[i]) /COLS);
  }
  move(bf->currentY+x+dec,bf->currentX%COLS);
  bf->decal = x+dec;

  //move(bf->currentY+decal_line(bf,getcury(stdscr)),bf->currentX%col);
  /*for (int i = 0 ; i < bf->nbLines ; i++) {
  bf->decal =bf->decal+ (strlen(bf->rows[i]) /COLS);
}*/
//insertChar(bf,decal_line(bf,getcury(stdscr))+'0');
//printw("(%d %d %d )",bf->currentY,bf->currentX,bf->decal);
}

void scrol_hori(Wbuffer *bf){

  bf->page_hori = (bf->currentX)/COLS;
  move(bf->currentY-bf->page_verti,bf->currentX% COLS);
  //move(bf->currentY+decal_line(bf,getcury(stdscr)),bf->currentX%col);
  /*for (int i = 0 ; i < bf->nbLines ; i++) {
  bf->decal =bf->decal+ (strlen(bf->rows[i]) /COLS);
}*/
//insertChar(bf,decal_line(bf,getcury(stdscr))+'0');
//printw("(%d %d %d )",bf->currentY,bf->currentX,bf->decal);
}

void printWithColor(char * line, Wbuffer * wbuffer){ // to get the syntaxe



  attron(COLOR_PAIR(8));
  //if(strcmp(wbuffer->extention,"c") == 0){

  ListTriplet list = getListOfTripletOfString(line,"c");
  int i = 0;
  while(i<strlen(line)){
    int posF = isTripletExistInListTriplet(&list,i);
    if(posF != -1){
      int color = getColorFromTriplet(&list,i);
      start_color();
      attroff(COLOR_PAIR(8));
      attron(COLOR_PAIR(color));
      int j;
      for(j=i ; j <= posF; j++){
        printw("%c",line[j]);
      }
      i = posF +1;
      attroff(COLOR_PAIR(color));
      attroff(COLOR_PAIR(8));


    }else if(posF == -1){
      start_color();
      attron(COLOR_PAIR(8));
      printw("%c",line[i]);
      i++;
      attroff(COLOR_PAIR(8));

    }
  }
  // }
  start_color();
  attron(COLOR_PAIR(8));
  printw("\n");
  attroff(COLOR_PAIR(8));
}
