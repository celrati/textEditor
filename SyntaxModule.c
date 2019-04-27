#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SyntaxModule.h"

void initTriplet(Triplet* t,int a,int b,int c){
	t->posI = a;
	t->posF = b;
	t->color = c;
}
void initListTriplet(ListTriplet* l){
	l->size = 0;
	l->list = (Triplet*)malloc((2 << 6) *sizeof(Triplet));
}
void addTripletToListTriplet(ListTriplet* l,Triplet* t){
	l->list[l->size] = *t;
	(l->size)++;
}
int isTripletExistInListTriplet(ListTriplet* l,int pos){
	int i;
	for(i=0; i<l->size ; i++){
		if(l->list[i].posI == pos) return l->list[i].posF;
	}
	return -1;
}
int getPosSubstringOfString(char* str,char* substr){
	char * r = strstr(str,substr);
	if(!r) return -1;
	return (r - str);
}
ListTriplet getListOfTripletOfString(char* line,char* extention){
	ListTriplet list ;
	initListTriplet(&list);
	FILE * file;
	if(strcmp(extention,"c") == 0){
		file = fopen("syntax/c","r");
	}
	if(file != NULL){
		char key_color[32];
		while(fgets(key_color,32,file) != NULL){
			char key[20];
			int color;
			sscanf(key_color,"%d-%s",&color,key);
			int i=0;
			while(i < strlen(line)){
				Triplet t;
				int pos = getPosSubstringOfString(line+i,key);
				if(pos != -1 ){
					initTriplet(&t,pos+i,pos+ strlen(key) -1 +i ,color);
					i += strlen(key);
					addTripletToListTriplet(&list,&t);
					
				}else{
					i++;
				}
				
			}
		}
	}
	return list;
}

int getColorFromTriplet(ListTriplet* l,int pos){
	int i;
	for(i=0; i<l->size ; i++){
		if(l->list[i].posI == pos) return l->list[i].color;
	}
}