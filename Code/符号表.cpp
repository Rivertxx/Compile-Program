#define _CRT_SECURE_NO_WARNINGS
#include "tab.h"
#include "grammar.h"

typedef struct ArrayTab ArrayTab;
typedef struct ArrayTab* ArrayTab_ptr;

char* to_tolower(char *s)
{
	int len = (int)strlen(s);
	int i;
	for (i = 0; i < len; i++) {
		if (s[i] >= 'A' && s[i] <= 'Z') {
			s[i] = tolower(s[i]);
		}
		else
		{
			s[i] = s[i];
		}
	}
	s[i] = '\0';
	return s;
}

void AddConst(char s[], IdenType t, bool con, int level, bool isarray, int linenum, int offset ,bool is_global) {
	if (TabHash.find(s) == TabHash.end())
	{
		Tab_ptr q;
		q = (Tab_ptr)malloc(sizeof(Tab));
		if (q != NULL)
		{
			strcpy(q->name, s);
			q->type = t;
			q->is_const = con;
			q->is_array = isarray;
			q->lev = level;
			q->offset = offset;
			q->is_global = is_global;
			q->next = NULL;
			q->pre = NULL;
			q->link = NULL;
			TabHash[s] = q;
			if (tabroot == NULL)
			{
				tabroot = q;
				tab_ptr = tabroot;
			}
			else
			{
				q->pre = tab_ptr;
				tab_ptr->next = q;
				tab_ptr = q;
			}
		}
	}
	else if (level != TabHash[s]->lev)
	{
		Tab_ptr q;
		q = (Tab_ptr)malloc(sizeof(Tab));
		if (q != NULL)
		{
			strcpy(q->name, s);
			q->type = t;
			q->is_const = con;
			q->is_array = isarray;
			q->lev = level;
			q->offset = offset;
			q->is_global = is_global;
			q->next = NULL;
			q->pre = NULL;
			//符号表的已经有该名的全局变量，把这个表放在前面，然后把全局的放在后面
			q->link = TabHash[s];
			TabHash[s] = q;
			if (tabroot == NULL)
			{
				tabroot = q;
				tab_ptr = tabroot;
			}
			else
			{
				q->pre = tab_ptr;
				tab_ptr->next = q;
				tab_ptr = q;
			}
		}
	}
	else error('b', linenum);
}

void AddFun(char s[], IdenType t, int level, int linenum)
{
	Tab_ptr q;
	q = (Tab_ptr)malloc(sizeof(Tab));
	if (q != NULL)
	{
		strcpy(q->name, s);
		q->type = t;
		q->lev = level;
		q->next = NULL;
		q->pre = NULL;
		q->funtab = (FunTab_ptr)malloc(sizeof(FunTab));
		if (TabHash.find(s) == TabHash.end())
		{
			q->link = NULL;
			TabHash[s] = q;
		}
		else
		{
			error('b', linenum);
			q->link = TabHash[s];
			TabHash[s] = q;
		}
		if (tabroot == NULL)
		{
			tabroot = q;
			tab_ptr = tabroot;
		}
		else
		{
			q->pre = tab_ptr;
			tab_ptr->next = q;
			tab_ptr = q;
		}
	}
	
}