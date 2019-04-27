#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"

// configuration
#define WIDTH_WBUFFER COLS
#define HEIGHT_WBUFFER LINES - 8
#define POSX_WBUFFER 0
#define POSY_WBUFFER 0

#define WIDTH_CMD COLS
#define HEIGHT_CMD 3
#define POSX_CMD LINES - 6 - 2
#define POSY_CMD 0

#define WIDTH_INFO COLS
#define HEIGHT_INFO 5
#define POSX_INFO	LINES - 3 - 2
#define POSY_INFO 0


void initEditor(Editor * editor, WINDOW * mainScreen){
	editor->currentHand = 1;
	initWbuffer(&(editor->wbuffer),mainScreen,WIDTH_WBUFFER,HEIGHT_WBUFFER,POSX_WBUFFER,POSY_WBUFFER);
	initCmd(&(editor->cmd),mainScreen,WIDTH_CMD,HEIGHT_CMD,POSX_CMD,POSY_CMD);
	initInfo(&(editor->info),mainScreen,WIDTH_INFO,HEIGHT_INFO,POSX_INFO,POSY_INFO);
	//printEditor(editor);
	editor->currentX = 0;
	editor->currentY = 0;
    ///////////////////////////// for the list
    editor->listWbuffer = (Wbuffer*)malloc(256 *(sizeof(Wbuffer))); // we allocate here 256 buffer in the list
    editor->currentWbufferNumber = 0; // the index of the current buffer
    editor->howManyBufferRightNow = 1;

    ////////////////
    editor->listWbuffer[0] = editor->wbuffer;
        editor->listBufferHistoryIndex = (int*)calloc(2<<8,sizeof(int));
    editor->listBufferHistoryIndexRightNow = (int*)calloc(2<<8,sizeof(int));
    ////////////////////////////////////////////////////////

}
void printEditor(Editor * editor){
	printWbuffer(&(editor->wbuffer));
	printCmd(&(editor->cmd));
	printInfo(&(editor->info));
}
int isItTurnOfBuffer(Editor * editor){
	if(editor->currentHand == 1) return 1;
	return 0;
}
void updateDimEditor(Editor* editor,WINDOW * mainScreen){
	redimBuffer(&(editor->wbuffer),mainScreen,WIDTH_WBUFFER,HEIGHT_WBUFFER,POSX_WBUFFER,POSY_WBUFFER);
	redimCmd(&(editor->cmd),mainScreen,WIDTH_CMD,HEIGHT_CMD,POSX_CMD,POSY_CMD);
	initInfo(&(editor->info),mainScreen,WIDTH_INFO,HEIGHT_INFO,POSX_INFO,POSY_INFO);
}
void giveTheHandToTheBuffer(Editor* editor){
	editor->currentHand = 0;
	editor->currentX = 0;
	editor->currentHand = 1;

}
void giveTheHandToTheCmd(Editor* editor){
	editor->currentHand = 0;
	editor->currentX = 19;
	editor->currentY = editor->wbuffer.height + 1;
	editor->cmd.currentX = 19;

	//free(editor->cmd.commandToExecute);
	//editor->cmd.commandToExecute = (char*)calloc(2 << 8,1);
	strcpy(editor->cmd.commandToExecute,"");
}

void makeKeyboardListenerCommand(Editor * editor, Cmd * cmd,int key){
	switch(key){
		case KEY_LEFT:{
			if(cmd->currentX >19){
				cmd->currentX--;
			}
			break;
		}

		case KEY_RIGHT:{
			if(cmd->currentX  < strlen(cmd->commandToExecute) +19){
				cmd->currentX++;
			}
			break;
		}
		case '\n':{
            // execute Command
			executeCmd(editor,cmd);
            //giveTheHandToTheBuffer(editor);
			//break;
		}

		case KEY_BACKSPACE:{
			if(((cmd->currentX > 19))){

				int i;
				cmd->currentX--;
				for(i= cmd->currentX -19; i <= strlen(cmd->commandToExecute)  ; i++){
                     // c juste pour ecraser le char i par char i+1
					cmd->commandToExecute[i] = cmd->commandToExecute[i+1];
				}

			}
			break;
		}
	}
}
void updateCmd(Cmd * cmd, char key){
        if(cmd->currentX < strlen(cmd->commandToExecute) + 19){
            //aaa|ab   4
            //mmpok
            unsigned int i;
            for(i=strlen(cmd->commandToExecute) ; i > cmd->currentX - 19 ; i--){
                cmd->commandToExecute[i] = cmd->commandToExecute[i-1];
            }
            cmd->commandToExecute[i] = key;
            cmd->currentX++;

        }else{
            cmd->commandToExecute[strlen(cmd->commandToExecute)] = key;
            cmd->currentX++;
        }
}

void executeCmd(Editor * editor, Cmd * cmd){
    char * nomCommand = (char*)malloc(256);
    char * nomFile    = (char*)malloc(256);
    int i,j,k;
    // to split the String
    for(i=0 ;i<strlen(cmd->commandToExecute); i++){
        if( cmd->commandToExecute[i] == ' ') break;
        nomCommand[i] = cmd->commandToExecute[i];
    }
    nomCommand[i++] = '\0';
    for(j=i,k=0 ;j<strlen(cmd->commandToExecute); j++,k++){
        if(cmd->commandToExecute[j] == '\0') break;
        nomFile[k] = cmd->commandToExecute[j];
    }

    // add more command here..
    //strcpy(editor->commandSide.cmd,nomFile);
    if(strcmp(nomCommand,"clear") == 0){
        // clear the buffer
         free(cmd->commandToExecute);
            cmd->commandToExecute = (char*)calloc(256,1);
            strcpy(cmd->commandToExecute,"\0");


            free(nomCommand);
            free(nomFile);

            //initEditor(editor,stdscr);
            //free(&(editor->listWbuffer[editor->currentWbufferNumber]));

            //initWbuffer(&(editor->wbuffer),stdscr,WIDTH_WBUFFER,HEIGHT_WBUFFER,POSX_WBUFFER,POSY_WBUFFER);;
            initWbuffer(&(editor->listWbuffer[editor->currentWbufferNumber]),stdscr,WIDTH_WBUFFER,HEIGHT_WBUFFER,POSX_WBUFFER,POSY_WBUFFER);
            editor->wbuffer = editor->listWbuffer[editor->currentWbufferNumber];

            editor->currentX = 0;
            editor->currentY = 0;

            free(editor->info.notification);
            editor->info.notification = (char*)malloc(2<< 8);
            strcpy(editor->info.notification,"cleaning the buffeer...");
            giveTheHandToTheBuffer(editor);
            clear();
            printEditor(editor);
    }

    if(strcmp(nomCommand,"open") == 0){
        /*editor->windowBuffer.buffer1->currentY = 0;  // initialiser les posX & pos Y with (0,0)
        editor->windowBuffer.buffer1->currentX = 0;
        editor->windowBuffer.buffer1->rows = (char**)malloc(2 << 16);
        editor->windowBuffer.buffer1->nbLines = 0;*/
        //initEditor(editor,stdscr);
        editor->currentX = 0;
        editor->currentY = 0;




        FILE* src;
        src =fopen(nomFile,"r");
        char ch;
        if(src != NULL){
             initWbuffer(&(editor->listWbuffer[editor->currentWbufferNumber]),stdscr,WIDTH_WBUFFER,HEIGHT_WBUFFER,POSX_WBUFFER,POSY_WBUFFER);
             editor->wbuffer = editor->listWbuffer[editor->currentWbufferNumber];
             MouseSelct ms;
             init_selc(&ms);
						 KeyBoardSelct  ks;
						 initSelectK(&ks);

            do {
              ch = fgetc(src);
              if( feof(src)) break;
              insertChar(&(editor->wbuffer),ch,&ms,&ks);
            }while(1);

        free(cmd->commandToExecute);
        cmd->commandToExecute = (char*)calloc(256,1);
        strcpy(cmd->commandToExecute,"");
        //strcpy(editor->buttomSide.message,"file loaded ");


        //editor->currentX = 15;
        clear();

        free(editor->info.notification);
        editor->info.notification = (char*)malloc(2<< 8);
        strcpy(editor->info.notification,"opening the file...");

        editor->wbuffer.page_verti = 0;
        editor->wbuffer.page_hori = 0;

        clear();

        printEditor(editor);
        giveTheHandToTheBuffer(editor);
        editor->wbuffer.currentX=0;
        editor->wbuffer.currentY=0;

        //move(0,0);

        free(nomCommand);
        free(nomFile);
        }else{
            //exit(0); //change it later
            //editor->currentX = 15;
             free(cmd->commandToExecute);
            cmd->commandToExecute = (char*)calloc(256,1);
            strcpy(cmd->commandToExecute,"\0");
            //strcpy(editor->buttomSide.message,"error file not found");
            free(nomCommand);
            free(nomFile);
            giveTheHandToTheBuffer(editor);
            //initBuffer(editor->windowBuffer.buffer1);
             free(editor->info.notification);
        editor->info.notification = (char*)malloc(2<< 8);
        strcpy(editor->info.notification,"ERROR: file not found...");
          //updateCursurEditorFromBuffer(editor);
          printEditor(editor);

        }
    }else if(strcmp(nomCommand,"save") == 0){
        //save file
        FILE* src;
        src =fopen(nomFile,"w+");
        if(src != NULL){
            int i;
            for(i=0; i< editor->wbuffer.nbLines ; i++){

                fprintf(src,"%s\n",editor->wbuffer.rows[i]);
            }
        }
        //strcpy(editor->buttomSide.message,"file saved");

        free(editor->info.notification);
        editor->info.notification = (char*)malloc(2<< 8);
        strcpy(editor->info.notification,"saving the file...");
                    free(cmd->commandToExecute);
            cmd->commandToExecute = (char*)calloc(256,1);
            strcpy(cmd->commandToExecute,"\0");
        

        giveTheHandToTheBuffer(editor);
        fclose(src);
        clear();
         printEditor(editor);

    }else if(strcmp(nomCommand,"new") == 0){
        // creating a new Buffer
        ////    sauvegarder le current buffer dans le tableau
        editor->listWbuffer[editor->currentWbufferNumber] = editor->wbuffer;
        ////
        initWbuffer(&(editor->listWbuffer[editor->howManyBufferRightNow]),stdscr,WIDTH_WBUFFER,HEIGHT_WBUFFER,POSX_WBUFFER,POSY_WBUFFER);
        editor->currentX = 0;
        editor->currentY = 0;
        editor->currentWbufferNumber = editor->howManyBufferRightNow;
        editor->howManyBufferRightNow++;
        strcpy(editor->info.notification,"preparing a new buffer ...");

        editor->wbuffer = editor->listWbuffer[editor->currentWbufferNumber];

        setNewCurrentBuffer(&(editor->info),  editor->currentWbufferNumber);
        free(cmd->commandToExecute);
        cmd->commandToExecute = (char*)calloc(256,1);
        strcpy(cmd->commandToExecute,"\0");
        clear();
        //printEditor(editor);
        giveTheHandToTheBuffer(editor);

    }else if(strcmp(nomCommand,"buffer-") == 0){
        // go the previous buffer..
        if(editor->currentWbufferNumber >= 1){
             ////    sauvegarder le current buffer dans le tableau
            editor->listWbuffer[editor->currentWbufferNumber] = editor->wbuffer;
            ////
            editor->currentX = 0;
            editor->currentY = 0;
            editor->currentWbufferNumber--;
            strcpy(editor->info.notification," <--- buffer  ");
            editor->wbuffer = editor->listWbuffer[editor->currentWbufferNumber];
            giveTheHandToTheBuffer(editor);
            setNewCurrentBuffer(&(editor->info),  editor->currentWbufferNumber);
            free(cmd->commandToExecute);
            cmd->commandToExecute = (char*)calloc(256,1);
            strcpy(cmd->commandToExecute,"\0");
            clear();
            printEditor(editor);
        }else{
            strcpy(editor->info.notification," there is no buffer -1 !!!!! ");
            free(cmd->commandToExecute);
            cmd->commandToExecute = (char*)calloc(256,1);
            strcpy(cmd->commandToExecute,"\0");
            clear();
            giveTheHandToTheBuffer(editor);
            printEditor(editor);
        }
    }else if(strcmp(nomCommand,"buffer+") == 0){
        // go the next buffer..
        if(editor->currentWbufferNumber < editor->howManyBufferRightNow -1){
             ////    sauvegarder le current buffer dans le tableau
            editor->listWbuffer[editor->currentWbufferNumber] = editor->wbuffer;
            ////
            editor->currentX = 0;
            editor->currentY = 0;
            editor->currentWbufferNumber++;
            strcpy(editor->info.notification," buffer ---->  ");
            editor->wbuffer = editor->listWbuffer[editor->currentWbufferNumber];
            giveTheHandToTheBuffer(editor);
            setNewCurrentBuffer(&(editor->info),  editor->currentWbufferNumber);
            free(cmd->commandToExecute);
            cmd->commandToExecute = (char*)calloc(256,1);
            strcpy(cmd->commandToExecute,"\0");
            clear();
            printEditor(editor);
        }else{
            strcpy(editor->info.notification," this is the limit !!! ");
            free(cmd->commandToExecute);
            cmd->commandToExecute = (char*)calloc(256,1);
            strcpy(cmd->commandToExecute,"\0");
            clear();
            giveTheHandToTheBuffer(editor);
            printEditor(editor);
        }
    }


}
