#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"

void startScreen();
void getUndoFromFile(Editor * editor, FILE * file);
int main(){

	startScreen();
	ncurses_mouse();
	Editor editor;
	MouseSelct ms;
	KeyBoardSelct ks;
	initEditor(&editor,stdscr);
	init_selc(&ms);
	initSelectK(&ks);
	int key = ':';
	//int select_kb = 0;
	printEditor(&editor);

	while(key !=  27){



		if(isItTurnOfBuffer(&editor)){
			//move(editor.wbuffer.currentY,editor.wbuffer.currentX);

			if(editor.wbuffer.MODE == 0)
						scrol_hori(&(editor.wbuffer));
					else{
						editor.wbuffer.page_hori = 0;
   		    	decal_counter(&(editor.wbuffer)); ///
					}

			key = getch();
			//printf("%d",editor.wbuffer.height);
			//clear();
			refresh();
			printEditor(&editor);
			if(key == 26){
   		    		// vide le buffer
   		    		// affecter le history to buffer
   		    		char * name_hist_fic = malloc(20*sizeof(char));
    				sprintf(name_hist_fic,"History/hist%d",editor.currentWbufferNumber);
    				if(editor.listBufferHistoryIndexRightNow[editor.currentWbufferNumber]>=0)
    				editor.listBufferHistoryIndexRightNow[editor.currentWbufferNumber]--;
    				FILE * fileHistory = fopen(name_hist_fic,"r");
    				getUndoFromFile(&editor,fileHistory);
    				// open tmp
    				
    				Cmd c;	
    				c.commandToExecute = (char*)calloc(2 << 8,1);
    				//
    				int kk1 = editor.wbuffer.page_verti;
    				int kk2 = editor.wbuffer.page_hori;
    				int kk3 = editor.wbuffer.MODE;
    				int kk4 = editor.wbuffer.currentX;
    				int kk5 = editor.wbuffer.currentY;

    				strcpy(c.commandToExecute, "clear");
    				executeCmd(&editor,&c);
					strcpy(c.commandToExecute, "open tmp");
    				executeCmd(&editor,&c);

    				editor.wbuffer.page_verti = kk1;
    				editor.wbuffer.page_hori = kk2;
    				editor.wbuffer.MODE = kk3;
    				editor.wbuffer.currentX = kk4;
    				editor.wbuffer.currentY = kk5;




    				continue;



   		    	}if(key== 25){
   		    		  		    		char * name_hist_fic = malloc(20*sizeof(char));
    				sprintf(name_hist_fic,"History/hist%d",editor.currentWbufferNumber);
    				if(editor.listBufferHistoryIndexRightNow[editor.currentWbufferNumber]< 
    					editor.listBufferHistoryIndex[editor.currentWbufferNumber] -1)
    				editor.listBufferHistoryIndexRightNow[editor.currentWbufferNumber]++;
    				FILE * fileHistory = fopen(name_hist_fic,"r");
    				getUndoFromFile(&editor,fileHistory);
    				// open tmp
    				
    				Cmd c;	
    				c.commandToExecute = (char*)calloc(2 << 8,1);
    				int kk1 = editor.wbuffer.page_verti;
    				int kk2 = editor.wbuffer.page_hori;
    				int kk3 = editor.wbuffer.MODE;
    				int kk4 = editor.wbuffer.currentX;
    				int kk5 = editor.wbuffer.currentY;
    				strcpy(c.commandToExecute, "clear");
    				executeCmd(&editor,&c);
					strcpy(c.commandToExecute, "open tmp");
    				executeCmd(&editor,&c);
    				editor.wbuffer.page_verti = kk1;
    				editor.wbuffer.page_hori = kk2;
    				editor.wbuffer.MODE = kk3;
    				editor.wbuffer.currentX = kk4;
    				editor.wbuffer.currentY = kk5;
    				continue;
   		    	}
			if(key == 1){
				giveTheHandToTheCmd(&editor);
				continue;
			}
			clear();
			makeKeyboardArrowsListener(&(editor.wbuffer),key,&ms);

			//if(key == KEY_SRIGHT) printw("TOTO");
			//if(key == 2 ){ //}&& select_kb < 3){
			//}
			keyBoardMove(&(editor.wbuffer),key,&ks); // pour selectionner avec le clavier
			mouse_move(&(editor.wbuffer),key,&ms);
			move_to_last(&(editor.wbuffer),&ms,&ks);
			insertChar(&(editor.wbuffer),key,&ms,&ks);
			/***/		
					//we add here the history
    			char * name_hist_fic = malloc(20*sizeof(char));
    			sprintf(name_hist_fic,"History/hist%d",editor.currentWbufferNumber);

    			FILE * fileHistory = fopen(name_hist_fic,"a+");

    			fprintf(fileHistory,"#%d\n",editor.listBufferHistoryIndex[editor.currentWbufferNumber]);
    			editor.listBufferHistoryIndex[editor.currentWbufferNumber]++;
    			editor.listBufferHistoryIndexRightNow[editor.currentWbufferNumber] =
    			editor.listBufferHistoryIndex[editor.currentWbufferNumber];

    			int i;
    			for(i=0;i<editor.wbuffer.nbLines;i++){
    				fprintf(fileHistory,"%s\n",editor.wbuffer.rows[i]);
    			}


    			fprintf(fileHistory,"#endHistory\n");

    			fclose(fileHistory);
				

    			/***/

			setPositionInfo(&(editor.info),editor.wbuffer.currentX,editor.wbuffer.currentY,editor.wbuffer.nbLines);
			if(editor.wbuffer.MODE == 0)
						editor.wbuffer.page_hori = (editor.wbuffer.currentX)/COLS;
			printEditor(&editor);

			updateDimEditor(&editor,stdscr);
			setNewCurrentBuffer(&(editor.info),  editor.currentWbufferNumber);
			//refresh();

		}else{// turn of cmd

			clear();
			printEditor(&editor);
			move(editor.currentY,editor.cmd.currentX);


			key = getch();
			//printw("");
			makeKeyboardListenerCommand(&editor,&(editor.cmd),key);
			if(key == 1){
				giveTheHandToTheBuffer(&editor);
				continue;
			}
			if( key != KEY_RIGHT && key != KEY_LEFT && key != KEY_UP && key != KEY_DOWN && key != KEY_BACKSPACE && key != '\n'){
				updateCmd(&(editor.cmd),(char)key);
			}
			printCmd(&(editor.cmd));
			printInfo(&(editor.info));

			//updateDimEditor(&editor,stdscr);
		}

	}

	endwin();
	exit(0); // end
	return 0;
}




void startScreen(){
	initscr();
	noecho();
	keypad(stdscr,TRUE);
	//refresh();
	raw();
	scrollok(stdscr, TRUE);
}
void getUndoFromFile(Editor * editor, FILE * file){
	FILE * tmp = fopen("tmp","w+");
	int c = 0;
	if(editor->listBufferHistoryIndexRightNow[editor->currentWbufferNumber] >= 0){
		char * etat = (char*)malloc(2<<4);
		sprintf(etat,"#%d\n",editor->listBufferHistoryIndexRightNow[editor->currentWbufferNumber]);
		//printf("%s\n",etat);
		if( tmp != NULL){
			char buf[2<<8];
			while(fgets(buf,2<<8,file) != NULL){
		//fprintf(tmp,"%s\n",etat);

				if(strcmp(buf,etat) == 0){
					c = 1;
					continue;
				}
				if(c == 1){
					if(strcmp(buf,"#endHistory\n") == 0){
						c = 0;
						fclose(tmp);
						return;			
					}
					fprintf(tmp,"%s",buf);
				}
			}
			fclose(tmp);
		}
	} 


}