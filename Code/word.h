#pragma once
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include<stdlib.h> 
#include<map>
#include<set>
#include "enum.h"
using namespace std;

struct Word {
	char origin[100];
	Symbol symbol;
	int linenum;
	struct Word* next;
};
typedef struct Word Words;
typedef struct Word* Nodeptr;
extern Nodeptr root;
extern Nodeptr p;
extern map<Symbol, string> Hash;
extern set<string> HasReturnFun;


bool isDigit(char s);
bool isLetter(char s);
void Insert(char* name, Symbol symbol);
void Destory(Nodeptr r);
Symbol printnum();
bool reserver(char s[]);
void printwords();
bool isCharCon(char a);
bool isStrCon(char a[]);