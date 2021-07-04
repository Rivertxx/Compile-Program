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
#include "enum.h"
using namespace std;

ifstream infile;
ofstream outfile;
ofstream outfile2;
ofstream errorfile;
ofstream midcodefile;
ofstream mipsfile;
Nodeptr root;
Nodeptr p;
int line;
map<Symbol, string> Hash;
map<Operation, string> OperationHash;
map<string, Tab_ptr> TabHash;
map<string, string> RegisterHash;
set<string> HasReturnFun;
bool isconst = false;
Tab_ptr tabroot = NULL;
Tab_ptr tab_ptr = NULL;
IdenType ident_type;
Midcode_ptr midcode_root = NULL;
Midcode_ptr midcode_ptr = NULL;
Midcode_ptr midcode_data_root = NULL;
Midcode_ptr midcode_data_ptr = NULL;
char ident_name[100];
int level = 0;
int dim = 0;

void CreatHash();
void CreatOperationHash();
void writemidcodefile();
void wirtetomipsfile();
void CreatRegisterHash();

int main()
{
	infile.open("testfile.txt");
	outfile.open("output2.txt");
	outfile2.open("output.txt");
	errorfile.open("error.txt");
	midcodefile.open("中间代码.txt");
	mipsfile.open("mips.txt");
	string linedata;
	int i, j, k;
	char word[100];
	CreatHash();
	CreatOperationHash();
	CreatRegisterHash();
	while (getline(infile, linedata))
	{
		line++;
		for (i = 0; i < linedata.length(); i++)
		{
			if (linedata[i] == ' ' || linedata[i] == '\t')
			{
				continue;
			}
			else if (linedata[i] == 34) //34 "
			{
				i++;
				j = 0;
				while (linedata[i] != 34)
				{
					word[j] = linedata[i];
					i++;
					j++;
				}
				word[j] = '\0';
				outfile << "STRCON " << word << endl;
				Insert(word, STRCON);
			}
			else if (linedata[i] == 39) //39 '
			{
				i++;
				outfile << "CHARCON " << linedata[i] << endl;
				i++;
				char charcon[10];
				int tempi = i - 1;
				charcon[0] = linedata[tempi];
				charcon[1] = '\0';
				Insert(charcon, CHARCON);
			}
			else if (linedata[i] == '+')
			{
				outfile << "PLUS " << linedata[i] << endl;
				char plus[1000] = "+";
				Insert(plus, PLUS);
			}
			else if (linedata[i] == '-')
			{
				outfile << "MINU " << linedata[i] << endl;
				char minu[1000] = "-";
				Insert(minu, MINU);
			}
			else if (linedata[i] == '*')
			{
				outfile << "MULT " << linedata[i] << endl;
				char mult[1000] = "*";
				Insert(mult, MULT);
			}
			else if (linedata[i] == '/')
			{
				outfile << "DIV " << linedata[i] << endl;
				char div[1000] = "/";
				Insert(div, DIV);
			}
			else if (linedata[i] == '<')
			{
				i++;
				if (linedata[i] == '=')
				{
					outfile << "LEQ <=" << endl;
					char leq[1000] = "<=";
					Insert(leq, LEQ);
				}
				else
				{
					i--;
					outfile << "LSS " << linedata[i] << endl;
					char lss[1000] = "<";
					Insert(lss, LSS);
				}
			}
			else if (linedata[i] == '>')
			{
				i++;
				if (linedata[i] == '=')
				{
					outfile << "GEQ >=" << endl;
					char geq[1000] = ">=";
					Insert(geq, GEQ);
				}
				else
				{
					i--;
					outfile << "GRE " << linedata[i] << endl;
					char gre[1000] = ">";
					Insert(gre, GRE);
				}
			}
			else if (linedata[i] == '=')
			{
				i++;
				if (linedata[i] == '=')
				{
					outfile << "EQL ==" << endl;
					char bql[1000] = "==";
					Insert(bql, EQL);
				}
				else
				{
					i--;
					outfile << "ASSIGN " << linedata[i] << endl;
					char assign[1000] = "=";
					Insert(assign, ASSIGN);
				}
			}
			else if (linedata[i] == '!')
			{
				i++;
				outfile << "NEQ !=" << endl;
				char neq[1000] = "!=";
				Insert(neq, NEQ);
			}
			else if (linedata[i] == ':')
			{
				outfile << "COLON " << linedata[i] << endl;
				char colon[1000] = ":";
				Insert(colon, COLON);
			}
			else if (linedata[i] == ';')
			{
				outfile << "SEMICN " << linedata[i] << endl;
				char semicn[1000] = ";";
				Insert(semicn, SEMICN);
			}
			else if (linedata[i] == ',')
			{
				outfile << "COMMA " << linedata[i] << endl;
				char comma[1000] = ",";
				Insert(comma, COMMA);
			}
			else if (linedata[i] == '(')
			{
				outfile << "LPARENT " << linedata[i] << endl;
				char lparent[1000] = "(";
				Insert(lparent, LPARENT);
			}
			else if (linedata[i] == ')')
			{
				outfile << "RPARENT " << linedata[i] << endl;
				char rparent[1000] = ")";
				Insert(rparent, RPARENT);
			}
			else if (linedata[i] == '[')
			{
				outfile << "LBRACK " << linedata[i] << endl;
				char lbrack[1000] = "[";
				Insert(lbrack, LBRACK);
			}
			else if (linedata[i] == ']')
			{
				outfile << "RBRACK " << linedata[i] << endl;
				char rbrack[1000] = "]";
				Insert(rbrack, RBRACK);
			}
			else if (linedata[i] == '{')
			{
				outfile << "LBRACE " << linedata[i] << endl;
				char lbrace[1000] = "{";
				Insert(lbrace, LBRACE);
			}
			else if (linedata[i] == '}')
			{
				outfile << "RBRACE " << linedata[i] << endl;
				char rbrace[1000] = "}";
				Insert(rbrace, RBRACE);
			}
			else if (isDigit(linedata[i]))
			{
				k = 0;
				while (isDigit(linedata[i]))
				{
					word[k] = linedata[i];
					i++;
					k++;
				}
				word[k] = '\0';
				i--;
				outfile << "INTCON " << word << endl;
				Insert(word, INTCON);
			}
			else if (isLetter(linedata[i]))
			{
				k = 0;
				word[k] = linedata[i];
				k++;
				i++;
				while (isLetter(linedata[i]) || isDigit(linedata[i]))
				{
					word[k] = linedata[i];
					i++;
					k++;
				}
				word[k] = '\0';
				if (!reserver(word))
				{
					outfile << "IDENFR " << word << endl;
					Insert(word, IDENFR);
				}
				i--;
			}
		}
	}
	infile.close();
	outfile.close();
	p = root;
	tab_ptr = tabroot;
	isProcedure();
	Tab_ptr tempptr = tabroot;
	while (tempptr != NULL)
	{
		cout << tempptr->name << " " << tempptr->type << " " << tempptr->lev << " " << tempptr->offset <<endl;
		tempptr = tempptr->next;
	}
	Destory(root);
	writemidcodefile();
	wirtetomipsfile();
	destorymidcode();
	errorfile.close();
	midcodefile.close();
	outfile2.close();
	mipsfile.close();
}

void CreatHash()
{
	Hash[IDENFR] = "IDENFR";
	Hash[INTCON] = "INTCON";
	Hash[CHARCON] = "CHARCON";
	Hash[STRCON] = "STRCON";
	Hash[CONSTTK] = "CONSTTK";
	Hash[INTTK] = "INTTK";
	Hash[CHARTK] = "CHARTK";
	Hash[VOIDTK] = "VOIDTK";
	Hash[MAINTK] = "MAINTK";
	Hash[IFTK] = "IFTK";
	Hash[ELSETK] = "ELSETK";
	Hash[SWITCHTK] = "SWITCHTK";
	Hash[CASETK] = "CASETK";
	Hash[DEFAULTTK] = "DEFAULTTK";
	Hash[WHILETK] = "WHILETK";
	Hash[FORTK] = "FORTK";
	Hash[SCANFTK] = "SCANFTK";
	Hash[PRINTFTK] = "PRINTFTK";
	Hash[RETURNTK] = "RETURNTK";
	Hash[PLUS] = "PLUS";
	Hash[MINU] = "MINU";
	Hash[MULT] = "MULT";
	Hash[DIV] = "DIV";
	Hash[LSS] = "LSS";
	Hash[LEQ] = "LEQ";
	Hash[GRE] = "GRE";
	Hash[GEQ] = "GEQ";
	Hash[EQL] = "EQL";
	Hash[NEQ] = "NEQ";
	Hash[COLON] = "COLON";
	Hash[ASSIGN] = "ASSIGN";
	Hash[SEMICN] = "SEMICN";
	Hash[COMMA] = "COMMA";
	Hash[LPARENT] = "LPARENT";
	Hash[RPARENT] = "RPARENT";
	Hash[LBRACK] = "LBRACK";
	Hash[RBRACK] = "RBRACK";
	Hash[LBRACE] = "LBRACE";
	Hash[RBRACE] = "RBRACE";
}

void CreatOperationHash()
{
	OperationHash[ADD] = "ADD";
	OperationHash[SUB] = "SUB";
	OperationHash[MUL] = "MUL";
	OperationHash[DIVI] = "DIVI";
	OperationHash[GLOBAL_PARA] = "GLOBAL_PARA";
	OperationHash[FUN_PARA] = "FUN_PARA";
	OperationHash[PRINT] = "PRINT";
	OperationHash[STRINGDATA] = "STRINGDATA";
	OperationHash[GLOBAL_SCANF] = "GLOBAL_SCANF";
	OperationHash[FUN_SCANF] = "FUN_SCANF";
	OperationHash[GLOBAL_ASS] = "GLOBAL_ASS";
	OperationHash[FUN_ASS] = "FUN_ASS";
	OperationHash[STORE_ARRAY] = "STORE_ARRAY";
	OperationHash[LABEL] = "LABEL";
	OperationHash[DATA] = "DATA";
	OperationHash[TEXT] = "TEXT";
	OperationHash[SET] = "SET";
	OperationHash[GET_GPARA] = "GET_GPARA";
	OperationHash[GET_FPARA] = "GET_FPARA";
	OperationHash[GET_FPARA2] = "GET_FPARA2";
	OperationHash[MOVE] = "MOVE";
	OperationHash[JLABEL] = "JLABEL";
	OperationHash[CAOF1] = "CAOF1";
	OperationHash[CAOF2G] = "CAOF2G";
	OperationHash[CAOF2F] = "CAOF2F";
	OperationHash[LOAD_ARRAY] = "LOAD_ARRAY";
	OperationHash[BRANCH] = "BRANCH";
	OperationHash[RECOVERREG] = "RECOVERREG";
	OperationHash[JAL] = "JAL";
	OperationHash[JR] = "JR";
}

void CreatRegisterHash()
{
	RegisterHash["$temp_var1"] = "$t1";
	RegisterHash["$temp_var2"] = "$t2";
	RegisterHash["$temp_var3"] = "$t3";
	RegisterHash["$temp_var4"] = "$t4";
	RegisterHash["$charcon_var"] = "$t0";
	RegisterHash["$intcon_var1"] = "$t5";
	RegisterHash["$intcon_var2"] = "$t6";
}

void writemidcodefile()
{
	midcode_ptr = midcode_data_root;
	while (midcode_ptr != NULL)
	{
		printmidcode();
		midcode_ptr = midcode_ptr->next;
	}
	midcode_ptr = midcode_root;
	while (midcode_ptr != NULL)
	{
		printmidcode();
		midcode_ptr = midcode_ptr->next;
	}
}

void wirtetomipsfile()
{
	midcode_ptr = midcode_data_root;
	while (midcode_ptr != NULL)
	{
		tomips();
		midcode_ptr = midcode_ptr->next;
	}
	midcode_ptr = midcode_root;
	while (midcode_ptr != NULL)
	{
		tomips();
		midcode_ptr = midcode_ptr->next;
	}
}