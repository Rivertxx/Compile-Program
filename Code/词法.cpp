#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include<stdlib.h> 
#include<map>
#include<set>
#include "word.h"
#include "grammar.h"
using namespace std;


bool isDigit(char s)
{
	if (s > 47 && s < 58)
	{
		return true;
	}
	return false;
}

bool isLetter(char s)
{
	if (s > 64 && s < 91)
	{
		return true;
	}
	if (s > 96 && s < 123)
	{
		return true;
	}
	if (s == '_')
	{
		return true;
	}
	return false;
}

void Insert(char* name, Symbol symbol)
{
	Nodeptr q;
	q = (Nodeptr)malloc(sizeof(Words));
	if (q != NULL)
	{
		strcpy(q->origin, name);
		q->symbol = symbol;
		q->linenum = line;
		q->next = NULL;
	}
	if (root == NULL)
	{
		root = q;
		p = q;
	}
	else
	{
		p->next = q;
		p = q;
	}
}

void Destory(Nodeptr r)
{
	if (r != NULL)
	{
		Destory(r->next);
		free(r);
		r = NULL;
	}
}

Symbol printnum()
{
	Symbol a = p->symbol;
	p = p->next;
	return a;
}

bool reserver(char s[])
{
	int len = (int)strlen(s);
	char news[100];
	int i;
	for (i = 0; i < len; i++) {
		if (s[i] >= 'A' && s[i] <= 'Z') {
			news[i] = tolower(s[i]);
		}
		else
		{
			news[i] = s[i];
		}
	}
	news[i] = '\0';
	//cout << "news = " << news << endl;
	if (strcmp(news, "const") == 0)
	{
		outfile << "CONSTTK " << s << endl;
		Insert(s, CONSTTK);
		return true;
	}
	if (strcmp(news, "int") == 0)
	{
		outfile << "INTTK " << s << endl;
		Insert(s, INTTK);
		return true;
	}
	if (strcmp(news, "char") == 0)
	{
		outfile << "CHARTK " << s << endl;
		Insert(s, CHARTK);
		return true;
	}
	if (strcmp(news, "void") == 0)
	{
		outfile << "VOIDTK " << s << endl;
		Insert(s, VOIDTK);
		return true;
	}
	if (strcmp(news, "main") == 0)
	{
		outfile << "MAINTK " << s << endl;
		Insert(s, MAINTK);
		return true;
	}
	if (strcmp(news, "if") == 0)
	{
		outfile << "IFTK " << s << endl;
		Insert(s, IFTK);
		return true;
	}
	if (strcmp(news, "else") == 0)
	{
		outfile << "ELSETK " << s << endl;
		Insert(s, ELSETK);
		return true;
	}
	if (strcmp(news, "switch") == 0)
	{
		outfile << "SWITCHTK " << s << endl;
		Insert(s, SWITCHTK);
		return true;
	}
	if (strcmp(news, "case") == 0)
	{
		outfile << "CASETK " << s << endl;
		Insert(s, CASETK);
		return true;
	}
	if (strcmp(news, "default") == 0)
	{
		outfile << "DEFAULTTK " << s << endl;
		Insert(s, DEFAULTTK);
		return true;
	}
	if (strcmp(news, "while") == 0)
	{
		outfile << "WHILETK " << s << endl;
		Insert(s, WHILETK);
		return true;
	}
	if (strcmp(news, "for") == 0)
	{
		outfile << "FORTK " << s << endl;
		Insert(s, FORTK);
		return true;
	}
	if (strcmp(news, "scanf") == 0)
	{
		outfile << "SCANFTK " << s << endl;
		Insert(s, SCANFTK);
		return true;
	}
	if (strcmp(news, "printf") == 0)
	{
		outfile << "PRINTFTK " << s << endl;
		Insert(s, PRINTFTK);
		return true;
	}
	if (strcmp(news, "return") == 0)
	{
		outfile << "RETURNTK " << s << endl;
		Insert(s, RETURNTK);
		return true;
	}
	return false;
}

void printwords()
{
	outfile2 << Hash[p->symbol] << " " << p->origin << endl;
}

bool isCharCon(char a)
{
	if (a == '+' || a == '-' || a == '*' || a == '/' || a == '_')
	{
		return true;
	}
	else if (a >= '0' && a <= '9')
	{
		return true;
	}
	else if (a >= 'a' && a <= 'z')
	{
		return true;
	}
	else if (a >= 'A' && a <= 'Z')
	{
		return true;
	}
	return false;
}

bool isStrCon(char a[])
{
	int i = 0;
	if (a[i] == '\0')
	{
		return false;
	}
	while (a[i] != '\0')
	{
		if (a[i] == 32 || a[i] == 33)
		{
			i++;
			continue;
		}
		else if (a[i] >= 35 && a[i] <= 126)
		{
			i++;
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}