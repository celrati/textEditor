#ifndef S_MD
#define D_MD

typedef struct Triplet{
	// posI posF Color
	int posI,posF,color;
}Triplet;

typedef struct ListTriplet{
	Triplet * list;
	int size;
}ListTriplet;

void initTriplet(Triplet*,int,int,int);
void initListTriplet(ListTriplet*);
void addTripletToListTriplet(ListTriplet*,Triplet*);
int isTripletExistInListTriplet(ListTriplet*,int ); // -1 = non  , others posF
int getColorFromTriplet(ListTriplet*,int);
/////////////////////////////////////////////
int getPosSubstringOfString(char*,char*);
ListTriplet getListOfTripletOfString(char*,char*);

#endif