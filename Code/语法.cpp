#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include<stdlib.h> 
#include<map>
#include<set>
#include "grammar.h"
#include "word.h"
using namespace std;
int linenum;
FunTab_ptr funtabptr = NULL;
Tab_ptr nowusefun = NULL;
IdenType nowexpressiontype;
IdenType nowfactortype;
IdenType nowitemtype;
bool isreturnfun = false;
IdenType returntype;
IdenType switchtype;
int returnstatement = 0;
char constvalue[100];
char intchar[100] = "INT";
char charchar[100] = "CHAR";
char stringchar[100] = "STRING";
char zeroregister[100] = "$0";
char sp_reg[100] = "$sp";
char ra_reg[100] = "$ra";
char s1_reg[100] = "$s1";
char s2_reg[100] = "$s2";
char temps1_reg[100];
char endline[100] = "endline";
char zerochar[100] = "0";
char nullchar[100] = "";
char global_char[100] = "global";
char fun_char[100] = "fun";
char expressvar[100];
char stringcon[1000];
int stringcount = 0;
int itemflag = 1;//用于标识相邻两个项之间用不同的临时变量名
int expflag = 1;//用于标识相邻两个表达式之间用不同的临时变量名
int intconflag = 1;//用于标识相邻两个<常量>因子之间用不同的临时变量名
int infunc = 0;
int global_offset = 0;
int fun_offset = 0;
//factor栈，以双向链表表示
Factor_ptr exp_root = NULL;
Factor_ptr factor_ptr = NULL;
bool is_new_exp = true;
//跳转相关常量字符串
char MAIN_LABEL[100] = "MAIN";
char END_MAIN[100] = "END_MAIN";
char IF_LABEL[100] = "IF_";
char END_IF_LABEL[100] = "END_IF_";
char IF_ELSE_LABEL[100] = "IF_ELSE_";
char WHILE_LABEL[100] = "WHILE_";
char END_WHILE_LABEL[100] = "END_WHILE_";
char FOR_LABEL[100] = "FOR_";
char FOR_1_LABEL[100] = "FOR_1_";
char FOR_2_LABEL[100] = "FOR_2_";
char END_FOR_LABEL[100] = "END_FOR_";
char END_SWITCH_LABEL[100] = "END_SWITCH_";
//条件跳转相关计数
int If_Count = 1;
int While_Count = 1;
int For_Count = 1;
int Switch_Count = 1;
int if_wh_for = 0;//判断条件是当前哪个语句里的，1代表if，2代表while，3代表for
//全局标记
bool paralist_flag = false;

char* get_label_n(char a[], int n)
{
	char* s;
	s = (char*)malloc(sizeof(char) * 100);
	char num[100];
	int2char(n, num, 10);
	if (s != NULL)
	{
		strcpy(s, a);
		strcat(s, num);
	}
	return s;
}

char* get_switch_case(int a, int b)
{
	char* q;
	q = (char*)malloc(sizeof(char) * 100);
	char s[100] = "SWITCH_0_CASE0";
	if (q != NULL)
	{
		strcpy(q, s);
		q[7] = q[7] + a;
		q[13] = q[13] + b;
	}
	return q;
}

int Unsignednum(char s[])
{
	int i = 0;
	int sum = 0;
	while (s[i] != '\0')
	{
		sum = sum * 10 + (s[i] - '0');
		i++;
	}
	return sum;
}

char* int2char(int n, char* s, int radix)
{
	int count = 0;
	int a;
	int left = 0;
	int right;
	char temp;
	if (n == 0)
	{
		s[0] = '0';
		s[1] = '\0';
		return s;
	}
	while (n > 0)
	{
		a = n % 10;
		s[count] = '0' + a;
		n = n / 10;
		count++;
	}
	s[count] = '\0';
	right = count - 1;
	while (left < right)
	{
		temp = s[right];
		s[right] = s[left];
		s[left] = temp;
		left++;
		right--;
	}
	return s;
}

//程序
void isProcedure()
{
	insertmidcode(TEXT, nullchar, nullchar, nullchar);
	insertmidcode(MOVE,(char*)"$fp" , sp_reg, nullchar);
	bool jmain_flag = false;
	if (p->symbol == CONSTTK)
	{
		isConstantDeclaration();
	}
	if (isTypeIdentifier(p->symbol) && p->next->symbol == IDENFR)
	{
		if (p->next->next->symbol == LPARENT)
		{
			insertmidcode(JLABEL, MAIN_LABEL, nullchar, nullchar);
			jmain_flag = true;
			isHasReturnFun();
			while ((isTypeIdentifier(p->symbol) && p->next->symbol == IDENFR && p->next->next->symbol == LPARENT) || (p->symbol == VOIDTK && p->next->symbol == IDENFR))
			{
				if (p->symbol == VOIDTK)
				{
					isNoReturnFun();
				}
				else
				{
					isHasReturnFun();
				}
			}
		}
		else
		{
			isVariableDeclaration();
		}
	}
	while ((isTypeIdentifier(p->symbol) && p->next->symbol == IDENFR && p->next->next->symbol == LPARENT) || (p->symbol == VOIDTK && p->next->symbol == IDENFR))
	{
		if (p->symbol == VOIDTK)
		{
			if (jmain_flag == false)
			{
				insertmidcode(JLABEL, MAIN_LABEL, nullchar, nullchar);
				jmain_flag = true;
			}
			isNoReturnFun();
		}
		else
		{
			if (jmain_flag == false)
			{
				insertmidcode(JLABEL, MAIN_LABEL, nullchar, nullchar);
				jmain_flag = true;
			}
			isHasReturnFun();
		}
	}
	if (jmain_flag == false)
	{
		insertmidcode(JLABEL, MAIN_LABEL, nullchar, nullchar);
		jmain_flag = true;
	}
	isMain();
	outfile2 << "<程序>" << endl;
}
//是否是类型标识符
bool isTypeIdentifier(Symbol s)
{
	if (s == INTTK || s == CHARTK) return true;
	return false;
}

//常量说明
void isConstantDeclaration()
{
	isconst = true;
	if (p->symbol == CONSTTK)
	{
		printwords();
		p = p->next;
		isConstDefi();
		if (p->symbol != SEMICN)
		{
			error('k', p->linenum);
		}
		else
		{
			printwords();
			p = p->next;
		}
		while (p->symbol == CONSTTK)
		{
			printwords();
			p = p->next;
			isConstDefi();
			if (p->symbol == SEMICN)
			{
				printwords();
				p = p->next;
			}
			else error('k', p->linenum);
		}
		outfile2 << "<常量说明>" << endl;
	}
	else
	{
		error1();
	}
	isconst = false;
}
//常量定义
void isConstDefi()
{
	char op3[100];
	if (p->symbol == INTTK)
	{
		ident_type = INT;
		printwords();
		p = p->next;
		if (p->symbol == IDENFR)
		{
			strcpy(ident_name, p->origin);
			linenum = p->linenum;
			to_tolower(ident_name);
			//根据是否在函数中，决定offset
			if (infunc == 0)
			{
				AddConst(ident_name, ident_type, isconst, level, false, linenum, global_offset, true);
				global_offset += 4;
			}
			else
			{
				AddConst(ident_name, ident_type, isconst, level, false, linenum, fun_offset, false);
				fun_offset += 4;
			}
			printwords();
			p = p->next;
			if (p->symbol == ASSIGN)
			{
				printwords();
				p = p->next;
				isInteger();
				//添加常量定义的中间代码
				if (infunc == 0)
				{
					int2char(global_offset - 4, op3, 10);
					insertmidcode(GLOBAL_PARA, intchar, op3, constvalue);
				}
				else {
					int2char(fun_offset - 4, op3, 10);
					insertmidcode(FUN_PARA, intchar, op3, constvalue);
				}
			}
			while (p->symbol == COMMA)
			{
				printwords();
				p = p->next;
				if (p->symbol == IDENFR)
				{
					strcpy(ident_name, p->origin);
					linenum = p->linenum;
					to_tolower(ident_name);
					if (infunc == 0)
					{
						AddConst(ident_name, ident_type, isconst, level, false, linenum, global_offset, true);
						global_offset += 4;
					}
					else
					{
						AddConst(ident_name, ident_type, isconst, level, false, linenum, fun_offset, false);
						fun_offset += 4;
					}
					printwords();
					p = p->next;
					if (p->symbol == ASSIGN)
					{
						printwords();
						p = p->next;
						isInteger();
						//添加常量定义的中间代码
						if (infunc == 0)
						{
							int2char(global_offset-4, op3, 10);
							insertmidcode(GLOBAL_PARA, intchar, op3, constvalue);
						}
						else {
							int2char(fun_offset-4, op3, 10);
							insertmidcode(FUN_PARA, intchar, op3, constvalue);
						}
					}
				}
			}
		}
		outfile2 << "<常量定义>" << endl;
	}
	else if (p->symbol == CHARTK)
	{
		ident_type = CHAR;
		printwords();
		p = p->next;
		if (p->symbol == IDENFR)
		{
			strcpy(ident_name, p->origin);
			linenum = p->linenum;
			to_tolower(ident_name);
			if (infunc == 0)
			{
				AddConst(ident_name, ident_type, isconst, level, false, linenum, global_offset, true);
				global_offset += 4;
			}
			else
			{

				AddConst(ident_name, ident_type, isconst, level, false, linenum, fun_offset, false);
				fun_offset += 4;
			}
			printwords();
			p = p->next;
			if (p->symbol == ASSIGN)
			{
				printwords();
				p = p->next;
				if (p->symbol == CHARCON)
				{
					if (!isCharCon(p->origin[0]))
					{
						error('a', p->linenum);
					}
					printwords();
					//添加常量定义的中间代码
					if (infunc == 0)
					{
						int2char(global_offset - 4, op3, 10);
						insertmidcode(GLOBAL_PARA, charchar, op3, p->origin);
					}
					else {
						int2char(fun_offset - 4, op3, 10);
						insertmidcode(FUN_PARA, charchar, op3, p->origin);
					}
					p = p->next;
				}
			}
			while (p->symbol == COMMA)
			{
				printwords();
				p = p->next;
				if (p->symbol == IDENFR)
				{
					strcpy(ident_name, p->origin);
					linenum = p->linenum;
					to_tolower(ident_name);
					if (infunc == 0)
					{
						AddConst(ident_name, ident_type, isconst, level, false, linenum, global_offset, true);
						global_offset += 4;
					}
					else
					{
						AddConst(ident_name, ident_type, isconst, level, false, linenum, fun_offset, false);
						fun_offset += 4;
					}
					printwords();
					p = p->next;
					if (p->symbol == ASSIGN)
					{
						printwords();
						p = p->next;
						if (p->symbol == CHARCON)
						{
							if (!isCharCon(p->origin[0]))
							{
								error('a', p->linenum);
							}
							printwords();
							//添加常量定义的中间代码
							if (infunc == 0)
							{
								int2char(global_offset - 4, op3, 10);
								insertmidcode(GLOBAL_PARA, charchar, op3, p->origin);
							}
							else {
								int2char(fun_offset - 4, op3, 10);
								insertmidcode(FUN_PARA, charchar, op3, p->origin);
							}
							p = p->next;
						}
					}
				}
			}
		}
		outfile2 << "<常量定义>" << endl;
	}
	else error1();
}
//整数
void isInteger()
{
	if (p->symbol == PLUS || p->symbol == MINU)
	{
		if (p->symbol == PLUS) {
			constvalue[0] = '+';
		}
		else {
			constvalue[0] = '-';
		}
		constvalue[1] = '\0';
		printwords();
		p = p->next;
		isUnsigned(true);
		outfile2 << "<整数>" << endl;
	}
	else if (p->symbol == INTCON)
	{
		constvalue[0] = '\0';
		isUnsigned(true);
		outfile2 << "<整数>" << endl;
	}
	else error1();
}
//无符号整数
void isUnsigned(bool from_integer)
{
	if (p->symbol == INTCON)
	{
		printwords();
		if (from_integer)
		{
			strcat(constvalue, p->origin);
		}
		else
		{
			strcpy(constvalue, p->origin);
		}
		p = p->next;
		outfile2 << "<无符号整数>" << endl;
	}
	else error1();
}

//变量说明
void isVariableDeclaration()
{
	isVariableDefi();
	if (p->symbol == SEMICN)
	{
		printwords();
		p = p->next;
	}
	else error('k', p->linenum);
	while (isTypeIdentifier(p->symbol) && p->next->symbol == IDENFR && p->next->next->symbol != LPARENT)
	{
		isVariableDefi();
		if (p->symbol == SEMICN)
		{
			printwords();
			p = p->next;
		}
		else error('k', p->linenum);
	}
	outfile2 << "<变量说明>" << endl;
}
//变量定义
void isVariableDefi()
{
	Nodeptr q = p->next->next;
	if (q->symbol == ASSIGN)//简单变量初始化
	{
		isVarDefInit();
	}
	else if (q->symbol == COMMA || q->symbol == SEMICN || q->symbol != LBRACK)//简单变量无初始化
	{
		isVarDefNoInit();
	}
	else
	{
		if (q->symbol == LBRACK)
		{
			Nodeptr r = q->next->next;
			if (r->symbol == RBRACK)
				r = r->next;
			if (r->symbol == ASSIGN)//一维变量初始化
			{
				isVarDefInit();
			}
			else if (r->symbol == COMMA || r->symbol == SEMICN || r->symbol != LBRACK)//一维变量无初始化
			{
				isVarDefNoInit();
			}
			else
			{
				if (r->symbol == LBRACK)
				{
					Nodeptr z = r->next->next;
					if (z->symbol == RBRACK)
						z = z->next;
					if (z->symbol == ASSIGN)//二维变量初始化
					{
						isVarDefInit();
					}
					else if (z->symbol == COMMA || z->symbol == SEMICN || z->symbol != ASSIGN)//二维变量无初始化
					{
						isVarDefNoInit();
					}
				}
			}
		}
	}
	outfile2 << "<变量定义>" << endl;
}
//变量定义及初始化
void isVarDefInit()
{
	int dim_num1;
	int dim_num2;
	int dim_count1 = 0;
	int dim_count2 = 0;
	int start_offset = 0;
	char op3[100];
	IdenType valuetype;
	if (isTypeIdentifier(p->symbol))
	{
		if (p->symbol == INTTK)//确定符号表符号类型
		{
			ident_type = INT;
		}
		else ident_type = CHAR;
		printwords();
		p = p->next;
		if (p->symbol == IDENFR)
		{
			linenum = p->linenum;
			strcpy(ident_name, p->origin);
			printwords();
			p = p->next;
			if (p->symbol == LBRACK)//一维左[
			{
				printwords();
				p = p->next;
				dim_num1 = Unsignednum(p->origin);
				isUnsigned(false);
				if (p->symbol == RBRACK)
				{
					printwords();
					p = p->next;
				}
				else error('m', p->linenum);
				if (p->symbol == LBRACK)//二维[
				{
					to_tolower(ident_name);
					//原来写在这
					//AddConst(ident_name, ident_type, isconst, level, true,linenum);
					printwords();
					p = p->next;
					dim_num2 = Unsignednum(p->origin);
					if (infunc == 0)
					{
						start_offset = global_offset;
						AddConst(ident_name, ident_type, isconst, level, true, linenum, global_offset, true);
						tab_ptr->dim2_num = dim_num2;
						global_offset += dim_num1 * dim_num2 * 4;
					}
					else
					{
						start_offset = fun_offset;
						AddConst(ident_name, ident_type, isconst, level, true, linenum, fun_offset, false);
						tab_ptr->dim2_num = dim_num2;
						fun_offset += dim_num1 * dim_num2 * 4;
					}
					isUnsigned(false);
					if (p->symbol == RBRACK)
					{
						printwords();
						p = p->next;
					}
					else error('m', p->linenum);
					if (p->symbol == ASSIGN)
					{
						printwords();
						p = p->next;
						if (p->symbol == LBRACE)
						{
							printwords();
							p = p->next;
							if (p->symbol == LBRACE)
							{
								dim_count1++;
								printwords();
								p = p->next;
								if (p->symbol == CHARCON) valuetype = CHAR;
								else valuetype = INT;
								if (ident_type != valuetype) error('o', p->linenum);
								isConst();
								int2char(start_offset, op3, 10);
								if (ident_type == INT)
								{
									if (infunc == 0)
										insertmidcode(GLOBAL_PARA, intchar, op3, constvalue);
									else
										insertmidcode(FUN_PARA, intchar, op3, constvalue);
								}
								else
								{
									if (infunc == 0)
										insertmidcode(GLOBAL_PARA, charchar, op3, constvalue);
									else
										insertmidcode(FUN_PARA, charchar, op3, constvalue);
								}
								start_offset += 4;
								dim_count2++;
								while (p->symbol == COMMA)
								{
									printwords();
									p = p->next;
									if (p->symbol == CHARCON) valuetype = CHAR;
									else valuetype = INT;
									if (ident_type != valuetype) error('o', p->linenum);
									isConst();
									int2char(start_offset, op3, 10);
									if (ident_type == INT)
									{
										if (infunc == 0)
											insertmidcode(GLOBAL_PARA, intchar, op3, constvalue);
										else
											insertmidcode(FUN_PARA, intchar, op3, constvalue);
									}
									else
									{
										if (infunc == 0)
											insertmidcode(GLOBAL_PARA, charchar, op3, constvalue);
										else
											insertmidcode(FUN_PARA, charchar, op3, constvalue);
									}
									start_offset += 4;
									dim_count2++;
								}
								if (dim_count2 != dim_num2)
								{
									error('n', p->linenum);
								}
								dim_count2 = 0;
								if (p->symbol == RBRACE)
								{
									printwords();
									p = p->next;
								}
							}
							while (p->symbol == COMMA)
							{
								printwords();
								p = p->next;
								if (p->symbol == LBRACE)
								{
									dim_count1++;
									printwords();
									p = p->next;
									if (p->symbol == CHARCON) valuetype = CHAR;
									else valuetype = INT;
									if (ident_type != valuetype) error('o', p->linenum);
									isConst();
									int2char(start_offset, op3, 10);
									if (ident_type == INT)
									{
										if (infunc == 0)
											insertmidcode(GLOBAL_PARA, intchar, op3, constvalue);
										else
											insertmidcode(FUN_PARA, intchar, op3, constvalue);
									}
									else
									{
										if (infunc == 0)
											insertmidcode(GLOBAL_PARA, charchar, op3, constvalue);
										else
											insertmidcode(FUN_PARA, charchar, op3, constvalue);
									}
									start_offset += 4;
									dim_count2++;
									while (p->symbol == COMMA)
									{
										printwords();
										p = p->next;
										if (p->symbol == CHARCON) valuetype = CHAR;
										else valuetype = INT;
										if (ident_type != valuetype) error('o', p->linenum);
										isConst();
										int2char(start_offset, op3, 10);
										if (ident_type == INT)
										{
											if (infunc == 0)
												insertmidcode(GLOBAL_PARA, intchar, op3, constvalue);
											else
												insertmidcode(FUN_PARA, intchar, op3, constvalue);
										}
										else
										{
											if (infunc == 0)
												insertmidcode(GLOBAL_PARA, charchar, op3, constvalue);
											else
												insertmidcode(FUN_PARA, charchar, op3, constvalue);
										}
										start_offset += 4;
										dim_count2++;
									}
									if (dim_count2 != dim_num2)
									{
										error('n', p->linenum);
									}
									dim_count2 = 0;
									if (p->symbol == RBRACE)
									{
										printwords();
										p = p->next;
									}
								}
							}
							if (dim_count1 != dim_num1)
							{
								error('m', p->linenum);
							}
							if (p->symbol == RBRACE)
							{
								printwords();
								p = p->next;
							}
						}
					}
				}
				else
				{
					to_tolower(ident_name);
					if (infunc == 0)
					{
						start_offset = global_offset;
						AddConst(ident_name, ident_type, isconst, level, true,linenum, global_offset, true);
						global_offset += dim_num1 * 4;
					}
					else
					{
						start_offset = fun_offset;
						AddConst(ident_name, ident_type, isconst, level, true, linenum, fun_offset, false);
						fun_offset += dim_num1 * 4;
					}
					if (p->symbol == ASSIGN)
					{
						printwords();
						p = p->next;
						if (p->symbol == LBRACE)
						{
							printwords();
							p = p->next;
							if (p->symbol == CHARCON) valuetype = CHAR;
							else valuetype = INT;
							if (ident_type != valuetype) error('o', p->linenum);
							isConst();
							int2char(start_offset, op3, 10);
							if (ident_type == INT)
							{
								if (infunc == 0)
									insertmidcode(GLOBAL_PARA, intchar, op3, constvalue);
								else
									insertmidcode(FUN_PARA, intchar, op3, constvalue);
							}
							else
							{
								if (infunc == 0)
									insertmidcode(GLOBAL_PARA, charchar, op3, constvalue);
								else
									insertmidcode(FUN_PARA, charchar, op3, constvalue);
							}
							dim_count1++;
							start_offset += 4;
							while (p->symbol == COMMA)
							{
								printwords();
								p = p->next;
								if (p->symbol == CHARCON) valuetype = CHAR;
								else valuetype = INT;
								if (ident_type != valuetype) error('o', p->linenum);
								isConst();
								int2char(start_offset, op3, 10);
								if (ident_type == INT)
								{
									if (infunc == 0)
										insertmidcode(GLOBAL_PARA, intchar, op3, constvalue);
									else 
										insertmidcode(FUN_PARA, intchar, op3, constvalue);
								}
								else
								{
									if (infunc == 0)
										insertmidcode(GLOBAL_PARA, charchar, op3, constvalue);
									else 
										insertmidcode(FUN_PARA, charchar, op3, constvalue);
								}
								dim_count1++;
								start_offset += 4;
							}
							if (dim_count1 != dim_num1)
							{
								error('n', p->linenum);
							}
							if (p->symbol == RBRACE)
							{
								printwords();
								p = p->next;
							}
						}
					}
				}
			}
			else
			{
				if (p->symbol == ASSIGN)
				{
					to_tolower(ident_name);
					if (infunc == 0)
					{
						AddConst(ident_name, ident_type, isconst, level, false, linenum, global_offset, true);
						global_offset += 4;
					}
					else {
						AddConst(ident_name, ident_type, isconst, level, false, linenum, fun_offset, false);
						fun_offset += 4;
					}
					printwords();
					p = p->next;
					if (p->symbol == CHARCON) valuetype = CHAR;
					else valuetype = INT;
					if (ident_type != valuetype) error('o', p->linenum);
					isConst();
					if (ident_type == INT)
					{
						//添加变量定义的中间代码【初始化】
						if (infunc == 0)
						{
							int2char(global_offset - 4, op3, 10);
							insertmidcode(GLOBAL_PARA, intchar, op3, constvalue);
						}
						else {
							int2char(fun_offset - 4, op3, 10);
							insertmidcode(FUN_PARA, intchar, op3, constvalue);
						}
						//insertmidcode(VAR, intchar, ident_name, constvalue);
					}
					else
					{
						//添加变量定义的中间代码【初始化】
						if (infunc == 0)
						{
							int2char(global_offset - 4, op3, 10);
							insertmidcode(GLOBAL_PARA, charchar, op3, constvalue);
						}
						else {
							int2char(fun_offset - 4, op3, 10);
							insertmidcode(FUN_PARA, charchar, op3, constvalue);
						}
						//insertmidcode(VAR, charchar, ident_name, constvalue);
					}
				}
			}
		}
		outfile2 << "<变量定义及初始化>" << endl;
	}
	else error1();
}
//变量定义无初始化
void isVarDefNoInit()
{
	int dim_num1 = 0;
	int dim_num2 = 0;
	if (isTypeIdentifier(p->symbol))
	{
		if (p->symbol == INTTK)//确定符号表符号类型
		{
			ident_type = INT;
		}
		else ident_type = CHAR;
		printwords();
		p = p->next;
		if (p->symbol == IDENFR)
		{
			strcpy(ident_name, p->origin);
			linenum = p->linenum;
			printwords();
			p = p->next;
			if (p->symbol == LBRACK)//一维左[
			{
				printwords();
				p = p->next;
				dim_num1 = Unsignednum(p->origin);
				isUnsigned(false);
				if (p->symbol == RBRACK)
				{
					printwords();
					p = p->next;
				}
				else error('m', p->linenum);
				if (p->symbol == LBRACK)//二维[
				{
					printwords();
					p = p->next;
					dim_num2 = Unsignednum(p->origin);
					isUnsigned(false);
					if (p->symbol == RBRACK)
					{
						printwords();
						p = p->next;
					}
					else error('m', p->linenum);
					to_tolower(ident_name);
					if (infunc == 0)
					{
						AddConst(ident_name, ident_type, isconst, level, true, linenum, global_offset, true);
						global_offset += dim_num1 * dim_num2 * 4;
					}
					else
					{
						AddConst(ident_name, ident_type, isconst, level, true, linenum, fun_offset, false);
						fun_offset += dim_num1 * dim_num2 * 4;
					}
					tab_ptr->dim2_num = dim_num2;
				}
				else
				{
					to_tolower(ident_name);
					if (infunc == 0)
					{
						AddConst(ident_name, ident_type, isconst, level, true, linenum, global_offset, true);
						global_offset += dim_num1 * 4;
					}
					else
					{
						AddConst(ident_name, ident_type, isconst, level, true, linenum, fun_offset, false);
						fun_offset += dim_num1 * 4;
					}
				}
			}
			else
			{
				to_tolower(ident_name);
				if (infunc == 0)
				{
					AddConst(ident_name, ident_type, isconst, level, false, linenum, global_offset, true);
					global_offset += 4;
				}
				else {
					AddConst(ident_name, ident_type, isconst, level, false, linenum, fun_offset, false);
					fun_offset += 4;
				}
				/*if (ident_type == INT){
					insertmidcode(VAR, intchar, ident_name, nullchar);
				}
				else {
					insertmidcode(VAR, charchar, ident_name, nullchar);
				}*/
			}
			while (p->symbol == COMMA)
			{
				printwords();
				p = p->next;
				if (p->symbol == IDENFR)
				{
					strcpy(ident_name, p->origin);
					linenum = p->linenum;
					printwords();
					p = p->next;
					if (p->symbol == LBRACK)//一维左[
					{
						printwords();
						p = p->next;
						dim_num1 = Unsignednum(p->origin);
						isUnsigned(false);
						if (p->symbol == RBRACK)
						{
							printwords();
							p = p->next;
						}
						else error('m', p->linenum);
						if (p->symbol == LBRACK)//二维[
						{
							printwords();
							p = p->next;
							dim_num2 = Unsignednum(p->origin);
							isUnsigned(false);
							if (p->symbol == RBRACK)
							{
								printwords();
								p = p->next;
							}
							else error('m', p->linenum);
							to_tolower(ident_name);
							if (infunc == 0)
							{
								AddConst(ident_name, ident_type, isconst, level, true, linenum, global_offset, true);
								global_offset += dim_num1 * dim_num2 * 4;
							}
							else
							{
								AddConst(ident_name, ident_type, isconst, level, true, linenum, fun_offset, false);
								fun_offset += dim_num1 * dim_num2 * 4;
							}
							tab_ptr->dim2_num = dim_num2;
						}
						else
						{
							to_tolower(ident_name);
							if (infunc == 0)
							{
								AddConst(ident_name, ident_type, isconst, level, true, linenum, global_offset, true);
								global_offset += dim_num1 * 4;
							}
							else
							{
								AddConst(ident_name, ident_type, isconst, level, true, linenum, fun_offset, false);
								fun_offset += dim_num1 * 4;
							}
						}
					}
					else
					{
						to_tolower(ident_name);
						if (infunc == 0)
						{
							AddConst(ident_name, ident_type, isconst, level, false, linenum, global_offset, true);
							global_offset += 4;
						}
						else {
							AddConst(ident_name, ident_type, isconst, level, false, linenum, fun_offset, false);
							fun_offset += 4;
						}
						/*if (ident_type == INT) {
							insertmidcode(VAR, intchar, ident_name, nullchar);
						}
						else {
							insertmidcode(VAR, charchar, ident_name, nullchar);
						}*/
					}
				}
			}
		}
		outfile2 << "<变量定义无初始化>" << endl;
	}
	else error1();
}
//常量
void isConst()
{
	if (p->symbol == CHARCON)
	{
		if (!isCharCon(p->origin[0]))
		{
			error('a', p->linenum);
		}
		printwords();
		strcpy(constvalue, p->origin);
		p = p->next;
		outfile2 << "<常量>" << endl;
	}
	else
	{
		isInteger();
		outfile2 << "<常量>" << endl;
	}
}


//参数表
void isParaList()
{
	int paracount = 0;
	funtabptr->para_num = 0;
	if (isTypeIdentifier(p->symbol))
	{
		if (p->symbol == INTTK)//确定符号表符号类型
		{
			ident_type = INT;
		}
		else ident_type = CHAR;
		printwords();
		p = p->next;
		if (p->symbol == IDENFR)
		{
			paracount++;
			funtabptr->para_num = paracount;
			funtabptr->para_type[paracount - 1] = ident_type;
			strcpy(ident_name, p->origin);
			to_tolower(ident_name);
			if (infunc == 0)
			{
				AddConst(ident_name, ident_type, isconst, level, false, linenum, global_offset, true);
				global_offset += 4;
			}
			else
			{
				AddConst(ident_name, ident_type, isconst, level, false, linenum, fun_offset, false);
				fun_offset += 4;
			}
			printwords();
			p = p->next;
			while (p->symbol == COMMA)
			{
				printwords();
				p = p->next;
				if (isTypeIdentifier(p->symbol))
				{
					if (p->symbol == INTTK)//确定符号表符号类型
					{
						ident_type = INT;
					}
					else ident_type = CHAR;
					printwords();
					p = p->next;
					if (p->symbol == IDENFR)
					{
						paracount++;
						funtabptr->para_num = paracount;
						funtabptr->para_type[paracount - 1] = ident_type;
						strcpy(ident_name, p->origin);
						to_tolower(ident_name);
						if (infunc == 0)
						{
							AddConst(ident_name, ident_type, isconst, level, false, linenum, global_offset, true);
							global_offset += 4;
						}
						else
						{
							AddConst(ident_name, ident_type, isconst, level, false, linenum, fun_offset, false);
							fun_offset += 4;
						}
						printwords();
						p = p->next;
					}
				}
			}
		}
	}
	outfile2 << "<参数表>" << endl;
}
//无返回值函数定义
void isNoReturnFun()
{
	int templevel = level + 1;
	fun_offset = 0;
	isreturnfun = false;
	infunc = 1;
	if (p->symbol == VOIDTK)
	{
		printwords();
		p = p->next;
		if (p->symbol == IDENFR)
		{
			strcpy(ident_name, p->origin);
			linenum = p->linenum;
			to_tolower(ident_name);
			insertmidcode(LABEL, ident_name, nullchar, nullchar);
			AddFun(ident_name, VOID, level, linenum);
			strcpy(temps1_reg, get_reg());
			insertmidcode(MOVE, temps1_reg, s1_reg,nullchar);
			funtabptr = tab_ptr->funtab;
			printwords();
			p = p->next;
			level = level + 1;
			if (p->symbol == LPARENT)
			{
				printwords();
				p = p->next;
				//参数表
				isParaList();
				if (p->symbol == RPARENT)
				{
					printwords();
					p = p->next;
				}
				else error('l', p->linenum);
				if (p->symbol == LBRACE)
				{
					printwords();
					p = p->next;
					isCompStatement();
					returnstatement = 0;
					if (p->symbol == RBRACE)
					{
						printwords();
						p = p->next;
					}
				}
			}
			insertmidcode(MOVE, s1_reg, temps1_reg, nullchar);
			free_reg(temps1_reg[2]);
			insertmidcode(JR, nullchar, nullchar, nullchar);
		}
		outfile2 << "<无返回值函数定义>" << endl;
	}
	else error1();
	//符号表退表
	while (tab_ptr != NULL && tab_ptr->lev == templevel)
	{
		Tab_ptr z = tab_ptr;
		tab_ptr = tab_ptr->pre;
		tab_ptr->next = NULL;
		if (TabHash[z->name]->link != NULL)
		{
			TabHash[z->name] = TabHash[z->name]->link;
		}
		else
		{
			TabHash.erase(z->name);
		}
		free(z);
	}
	level = level - 1;
	infunc = 0;
}
//有返回值函数定义
void isHasReturnFun()
{
	fun_offset = 0;
	isreturnfun = true;
	infunc = 1;
	int templevel = level + 1;
	if (isTypeIdentifier(p->symbol))
	{
		isHeadStatement();
		level = level + 1;
		if (p->symbol == LPARENT)
		{
			printwords();
			p = p->next;
			isParaList();
			if (p->symbol == RPARENT)
			{
				printwords();
				p = p->next;
			}
			else error('l', p->linenum);
			if (p->symbol == LBRACE)
			{
				printwords();
				p = p->next;
				isCompStatement();
				if (returnstatement == 0)
					error('h', p->linenum);
				returnstatement = 0;
				if (p->symbol == RBRACE)
				{
					printwords();
					p = p->next;
				}
			}
		}
		outfile2 << "<有返回值函数定义>" << endl;
	}
	while (tab_ptr != NULL && tab_ptr->lev == templevel)
	{
		Tab_ptr z = tab_ptr;
		tab_ptr = tab_ptr->pre;
		tab_ptr->next = NULL;
		if (TabHash[z->name]->link != NULL)
		{
			TabHash[z->name] = TabHash[z->name]->link;
		}
		else
		{
			TabHash.erase(z->name);
		}
		free(z);
	}
	level = level - 1;
	infunc = 0;
}
//声明头部
void isHeadStatement()
{
	if (isTypeIdentifier(p->symbol))
	{
		if (p->symbol == INTTK)//确定符号表符号类型
		{
			ident_type = INT;
			returntype = INT;
		}
		else
		{
			ident_type = CHAR;
			returntype = CHAR;
		}
		printwords();
		p = p->next;
		if (p->symbol == IDENFR)
		{
			strcpy(ident_name, p->origin);
			linenum = p->linenum;
			to_tolower(ident_name);
			insertmidcode(LABEL, ident_name, nullchar, nullchar);
			AddFun(ident_name, ident_type, level, linenum);
			strcpy(temps1_reg, get_reg());
			insertmidcode(MOVE, temps1_reg, s1_reg, nullchar);
			funtabptr = tab_ptr->funtab;
			HasReturnFun.insert(p->origin);
			printwords();
			p = p->next;
		}
		outfile2 << "<声明头部>" << endl;
	}
	else error1();
}

//主函数
void isMain()
{
	insertmidcode(LABEL, MAIN_LABEL, nullchar, nullchar);
	isreturnfun = false;
	level = level + 1;
	if (p->symbol == VOIDTK)
	{
		printwords();
		p = p->next;
		if (p->symbol == MAINTK)
		{
			printwords();
			p = p->next;
			if (p->symbol == LPARENT)
			{
				printwords();
				p = p->next;
				if (p->symbol == RPARENT)
				{
					printwords();
					p = p->next;
				}
				else error('l', p->linenum);
				if (p->symbol == LBRACE)
				{
					printwords();
					p = p->next;
					isCompStatement();
					if (p->symbol == RBRACE)
					{
						printwords();
						p = p->next;
					}
					insertmidcode(LABEL, END_MAIN, nullchar, nullchar);
				}
			}
		}
	}
	else error1();
	outfile2 << "<主函数>" << endl;
}


//复合语句
void isCompStatement()
{
	if (p->symbol == CONSTTK)
	{
		isConstantDeclaration();
	}
	if (isTypeIdentifier(p->symbol))
	{
		isVariableDeclaration();
	}
	isStatementColumn();
	outfile2 << "<复合语句>" << endl;
}

//语句列
void isStatementColumn() {
	while (isStatementCondition())
	{
		isStatement();
	}
	outfile2 << "<语句列>" << endl;
}
//语句条件
bool isStatementCondition()
{
	if (p->symbol == WHILETK || p->symbol == FORTK)//循环语句
	{
		return true;
	}
	else if (p->symbol == IFTK)//条件语句
	{
		return true;
	}
	else if (p->symbol == IDENFR && p->next->symbol == LPARENT)//函数调用语句 to do!
	{
		return true;
	}
	else if (p->symbol == IDENFR && (p->next->symbol == ASSIGN || p->next->symbol == LBRACK))//赋值语句
	{
		return true;
	}
	else if (p->symbol == SCANFTK)//读语句
	{
		return true;
	}
	else if (p->symbol == PRINTFTK)//写语句
	{
		return true;
	}
	else if (p->symbol == SWITCHTK)//情况语句
	{
		return true;
	}
	else if (p->symbol == SEMICN)//空语句
	{
		return true;
	}
	else if (p->symbol == RETURNTK)//返回语句
	{
		return true;
	}
	else if (p->symbol == LBRACE)//语句列
	{
		return true;
	}
	return false;
}
//语句
void isStatement()
{
	if (p->symbol == WHILETK || p->symbol == FORTK)//循环语句
	{
		isLoopStatement();
	}
	else if (p->symbol == IFTK)//条件语句
	{
		isIfStatement();
	}
	else if (p->symbol == IDENFR && p->next->symbol == LPARENT)//函数调用语句 to do!
	{
		if (HasReturnFun.count(p->origin) == 1)
		{
			isCallReturnFun();
		}
		else
		{
			isCallNoReturnFun();
		}
		if (p->symbol == SEMICN)
		{
			printwords();
			p = p->next;
		}
		else error('k', p->linenum);
	}
	else if (p->symbol == IDENFR && (p->next->symbol == ASSIGN || p->next->symbol == LBRACK))//赋值语句
	{
		isAssignStatement();
		if (p->symbol == SEMICN)
		{
			printwords();
			p = p->next;
		}
		else {
			error('k', p->linenum);
		}
			
	}
	else if (p->symbol == SCANFTK)//读语句
	{
		isReadStatement();
		if (p->symbol == SEMICN)
		{
			printwords();
			p = p->next;
		}
		else error('k', p->linenum);
	}
	else if (p->symbol == PRINTFTK)//写语句
	{
		isWriteStatement();
		if (p->symbol == SEMICN)
		{
			printwords();
			p = p->next;
		}
	}
	else if (p->symbol == SWITCHTK)//情况语句
	{
		isSwitchStatement();
	}
	else if (p->symbol == SEMICN)//空语句
	{
		printwords();
		p = p->next;
	}
	else if (p->symbol == RETURNTK)//返回语句
	{
		isReturnStatement();
		if (p->symbol == SEMICN)
		{
			printwords();
			p = p->next;
		}
		else error('k', p->linenum);
	}
	else if (p->symbol == LBRACE)//语句列
	{
		printwords();
		p = p->next;
		isStatementColumn();
		if (p->symbol == RBRACE)
		{
			printwords();
			p = p->next;
		}
	}
	outfile2 << "<语句>" << endl;
}
//循环语句
void isLoopStatement()
{
	if (p->symbol == WHILETK)
	{
		int while_num = While_Count;
		insertmidcode(LABEL, get_label_n(WHILE_LABEL, while_num), nullchar, nullchar);
		While_Count++;
		if_wh_for = 2;
		printwords();
		p = p->next;
		if (p->symbol == LPARENT)
		{
			printwords();
			p = p->next;
			isCondition();
			if (p->symbol == RPARENT)
			{
				printwords();
				p = p->next;
			}
			else error('l', p->linenum);
			isStatement();
			insertmidcode(JLABEL, get_label_n(WHILE_LABEL, while_num), nullchar, nullchar);
			insertmidcode(LABEL, get_label_n(END_WHILE_LABEL, while_num), nullchar, nullchar);
		}
	}
	else if (p->symbol == FORTK)
	{
		int for_num = For_Count;
		For_Count++;
		if_wh_for = 3;
		printwords();
		p = p->next;
		if (p->symbol == LPARENT)
		{
			printwords();
			p = p->next;
			if (p->symbol == IDENFR)
			{
				//条件判断前部分
				char tempname[100];
				bool is_gloabl;
				char para_offset[100];
				int offset;
				strcpy(tempname, p->origin);
				to_tolower(tempname);
				if (TabHash.find(tempname) == TabHash.end())
				{
					error('c', p->linenum);
				}
				else
				{
					offset = TabHash[tempname]->offset;
					int2char(offset, para_offset, 10);
					is_gloabl = TabHash[tempname]->is_global;
				}
				printwords();
				p = p->next;
				if (p->symbol == ASSIGN)
				{
					printwords();
					p = p->next;
					isExpression();
					if (is_gloabl)
					{
						insertmidcode(GLOBAL_ASS, tempname, expressvar, para_offset);
					}
					else
					{
						insertmidcode(FUN_ASS, tempname, expressvar, para_offset);
					}
					free_reg(expressvar[2]);
					if (p->symbol == SEMICN)
					{
						printwords();
						p = p->next;
					}
					else error('k', p->linenum);

					//条件判断
					insertmidcode(LABEL, get_label_n(FOR_LABEL, for_num), nullchar, nullchar);
					
					isCondition();
					if (p->symbol == SEMICN)
					{
						printwords();
						p = p->next;
					}
					else error('k', p->linenum);
					//加减步长
					insertmidcode(LABEL, get_label_n(FOR_1_LABEL, for_num), nullchar, nullchar);
					if (p->symbol == IDENFR)
					{
						char tempname[100];
						bool is_global;
						char name_pre[100];
						char offset_pre[100];
						strcpy(tempname, p->origin);
						to_tolower(tempname);
						strcpy(name_pre, tempname);
						if (TabHash.find(tempname) == TabHash.end())
						{
							error('c', p->linenum);
						}
						else
						{
							offset = TabHash[tempname]->offset;
							int2char(offset,offset_pre, 10);
							is_global = TabHash[tempname]->is_global;
						}
						printwords();
						p = p->next;
						if (p->symbol == ASSIGN)
						{
							printwords();
							p = p->next;
							if (p->symbol == IDENFR)
							{
								char tempname[100];
								char reg[100];
								strcpy(tempname, p->origin);
								strcpy(reg, get_reg());
								to_tolower(tempname);
								if (TabHash.find(tempname) == TabHash.end())
								{
									error('c', p->linenum);
								}
								else
								{
									offset = TabHash[tempname]->offset;
									int2char(offset, para_offset, 10);
									is_gloabl = TabHash[tempname]->is_global;
								}

								if (is_gloabl)
								{
									insertmidcode(GET_GPARA, tempname, para_offset, reg);
								}
								else
								{
									insertmidcode(GET_FPARA, tempname, para_offset, reg);
								}
								printwords();
								p = p->next;
								if (p->symbol == PLUS || p->symbol == MINU)
								{
									Symbol for_symbol = p->symbol;
									char const_reg[100];
									strcpy(const_reg, get_reg());
									printwords();
									p = p->next;
									isStepSize();
									insertmidcode(SET, intchar, const_reg, constvalue);
									if (for_symbol == PLUS)
									{
										insertmidcode(ADD, reg, reg, const_reg);
									}
									if (for_symbol == MINU)
									{
										insertmidcode(SUB, reg, reg, const_reg);
									}
									free_reg(const_reg[2]);
									//赋值＜标识符＞＝＜标识符＞(+|-)＜步长＞
									if (is_global)
									{
										insertmidcode(GLOBAL_ASS, name_pre, reg, offset_pre);
									}
									else
									{
										insertmidcode(FUN_ASS, name_pre, reg, offset_pre);
									}
									free_reg(reg[2]);

									if (p->symbol == RPARENT)
									{
										printwords();
										p = p->next;
									}
									else error('l', p->linenum);
									//条件成立执行语句
									insertmidcode(JLABEL, get_label_n(FOR_LABEL, for_num), nullchar, nullchar);
									insertmidcode(LABEL, get_label_n(FOR_2_LABEL, for_num), nullchar, nullchar);
									isStatement();
									insertmidcode(JLABEL, get_label_n(FOR_1_LABEL, for_num), nullchar, nullchar);
									insertmidcode(LABEL, get_label_n(END_FOR_LABEL, for_num), nullchar, nullchar);
								}
							}
						}
					}
				}
			}
		}
	}
	else error1();
	outfile2 << "<循环语句>" << endl;
}
//步长
void isStepSize()
{
	isUnsigned(false);
	outfile2 << "<步长>" << endl;
}
//条件
void isCondition()
{
	char exp_pre[100];
	Symbol com_symbol;
	isExpression();
	strcpy(exp_pre, expressvar);
	if (nowexpressiontype != INT)
	{
		error('f', p->linenum);
	}
	if (isRelationOperator(p->symbol))
	{
		com_symbol = p->symbol;
		printwords();
		p = p->next;
		isExpression();
		if (nowexpressiontype != INT)
		{
			error('f', p->linenum);
		}
	}
	insertmidcode(SUB, exp_pre, exp_pre, expressvar);
	free_reg(expressvar[2]);
	if (if_wh_for == 1)
	{
		insertmidcode(BRANCH, (char*)Hash[com_symbol].data(), get_label_n(IF_ELSE_LABEL, If_Count - 1), exp_pre);
	}
	else if (if_wh_for == 2)
	{
		insertmidcode(BRANCH, (char*)Hash[com_symbol].data(), get_label_n(END_WHILE_LABEL, While_Count - 1), exp_pre);
	}
	else if (if_wh_for == 3)
	{
		insertmidcode(BRANCH, (char*)Hash[com_symbol].data(), get_label_n(END_FOR_LABEL, For_Count- 1), exp_pre);
		insertmidcode(JLABEL, get_label_n(FOR_2_LABEL, For_Count - 1), nullchar, nullchar);
	}
	free_reg(exp_pre[2]);
	outfile2 << "<条件>" << endl;
}
//关系运算符
bool isRelationOperator(Symbol s)
{
	if (s == LSS || s == LEQ || s == GRE || s == GEQ || s == EQL || s == NEQ)
	{
		return true;
	}
	return false;
}
//条件语句
void isIfStatement()
{
	int if_num = If_Count;
	If_Count++;
	if (p->symbol == IFTK)
	{
		printwords();
		p = p->next;
		if (p->symbol == LPARENT)
		{
			printwords();
			p = p->next;
			if_wh_for = 1;
			isCondition();
			if (p->symbol == RPARENT)
			{
				printwords();
				p = p->next;
			}
			else error('l', p->linenum);
			isStatement();
			insertmidcode(JLABEL, get_label_n(END_IF_LABEL, if_num), nullchar, nullchar);
			insertmidcode(LABEL, get_label_n(IF_ELSE_LABEL, if_num), nullchar, nullchar);
			if (p->symbol == ELSETK)
			{
				printwords();
				p = p->next;
				isStatement();
			}
			insertmidcode(LABEL, get_label_n(END_IF_LABEL, if_num), nullchar, nullchar);
		}
	}  
	else error1();
	outfile2 << "<条件语句>" << endl;
}
//赋值语句
void isAssignStatement()
{
	if (p->symbol == IDENFR)
	{
		char tempname[100];
		char para_offset[100];
		char dim1_reg[100];
		char dim2_reg[100];
		char dim2_value[100];
		int offset;
		bool is_gloabl;
		strcpy(tempname, p->origin);
		to_tolower(tempname);
		if (TabHash.find(tempname) == TabHash.end())
		{
			error('c', p->linenum);
		}
		else
		{
			if (TabHash[tempname]->is_const)
			{
				error('j', p->linenum);
			}
			offset = TabHash[tempname]->offset;
			int2char(offset, para_offset, 10);
			is_gloabl = TabHash[tempname]->is_global;
		}
		IdenType assigntype = TabHash[tempname]->type;
		int2char(TabHash[tempname]->dim2_num, dim2_value, 10);
		printwords();
		p = p->next;
		if (p->symbol == ASSIGN)
		{
			printwords();
			p = p->next;
			isExpression();
			if (is_gloabl)
			{
				insertmidcode(GLOBAL_ASS, tempname, expressvar, para_offset);
			}
			else
			{
				insertmidcode(FUN_ASS, tempname, expressvar, para_offset);
			}
			free_reg(expressvar[2]);
		}
		else if (p->symbol == LBRACK)//一维
		{
			printwords();
			p = p->next;
			isExpression();
			strcpy(dim1_reg, expressvar);
			if (nowexpressiontype != INT)
			{
				error('i', p->linenum);
			}
			if (p->symbol == RBRACK)
			{
				printwords();
				p = p->next;
			}
			else error('m', p->linenum);
			if (p->symbol == ASSIGN)
			{
				printwords();
				p = p->next;
				isExpression();
				if (is_gloabl)
					insertmidcode(CAOF1, dim1_reg, nullchar, global_char);
				else
					insertmidcode(CAOF1, dim1_reg, nullchar, fun_char);
				insertmidcode(STORE_ARRAY, tempname, expressvar, para_offset);
				free_reg(dim1_reg[2]);
				free_reg(expressvar[2]);
			}
			else if (p->symbol == LBRACK)//二维
			{
				printwords();
				p = p->next;
				isExpression();
				strcpy(dim2_reg, expressvar);
				if (nowexpressiontype != INT)
				{
					error('i', p->linenum);
				}
				if (p->symbol == RBRACK)
				{
					printwords();
					p = p->next;
				}
				else error('m', p->linenum);
				if (p->symbol == ASSIGN)
				{
					printwords();
					p = p->next;
					isExpression();
					if (is_gloabl)
						insertmidcode(CAOF2G, dim1_reg, dim2_reg, dim2_value);
					else
						insertmidcode(CAOF2F, dim1_reg, dim2_reg, dim2_value);
					insertmidcode(STORE_ARRAY, tempname, expressvar, para_offset);
					free_reg(dim1_reg[2]);
					free_reg(dim2_reg[2]);
					free_reg(expressvar[2]);
				}
			}
		}
	}
	else error1();
	outfile2 << "<赋值语句>" << endl;
}
//读语句
void isReadStatement()
{
	if (p->symbol == SCANFTK)
	{
		printwords();
		p = p->next;
		if (p->symbol == LPARENT)
		{
			printwords();
			p = p->next;
			if (p->symbol == IDENFR)
			{
				char tempname[100];
				char para_offset[100];
				int offset;
				bool is_global;
				strcpy(tempname, p->origin);
				to_tolower(tempname);
				if (TabHash.find(tempname) == TabHash.end())
				{
					error('c', p->linenum);
				}
				else
				{
					if (TabHash[tempname]->is_const)
					{
						error('j', p->linenum);
					}
					offset = TabHash[tempname]->offset;
					int2char(offset, para_offset, 10);
					is_global = TabHash[tempname]->is_global;
				}
				//输入添加中间代码
				IdenType scanftype = TabHash[tempname]->type;
				if (scanftype == INT)
				{
					if (is_global)
					{
						insertmidcode(GLOBAL_SCANF, intchar, tempname, para_offset);
					}
					else
					{
						insertmidcode(FUN_SCANF, intchar, tempname, para_offset);
					}
				}
				else {
					if (is_global)
					{
						insertmidcode(GLOBAL_SCANF, charchar, tempname, para_offset);
					}
					else
					{
						insertmidcode(FUN_SCANF, charchar, tempname, para_offset);
					}
				}
				printwords();
				p = p->next;
				if (p->symbol == RPARENT)
				{
					printwords();
					p = p->next;
				}
				else error('l', p->linenum);
			}
		}
	}
	else error1();
	outfile2 << "<读语句>" << endl;
}
//写语句
void isWriteStatement()
{
	if (p->symbol == PRINTFTK)
	{
		printwords();
		p = p->next;
		if (p->symbol == LPARENT)
		{
			printwords();
			p = p->next;
			if (p->symbol == STRCON)
			{
				isString();
				if (stringcount == 0)
				{
					insert_Datamidcode(DATA, nullchar, nullchar, nullchar);
				}
				stringcount++;
				char stringcountchar[100];
				int2char(stringcount, stringcountchar, 10);
				insert_Datamidcode(STRINGDATA, stringcountchar, stringcon, nullchar);
				insertmidcode(PRINT,stringchar, stringcountchar, nullchar);
				if (p->symbol == COMMA)
				{
					printwords();
					p = p->next;
					isExpression();
					//输出表达式
					if (nowexpressiontype == INT)
						insertmidcode(PRINT, intchar, expressvar, nullchar);
					else
						insertmidcode(PRINT, charchar, expressvar, nullchar);
					free_reg(expressvar[2]);
					if (p->symbol == RPARENT)
					{
						printwords();
						p = p->next;
					}
					else error('l', p->linenum);
				}
				else if (p->symbol == RPARENT)
				{
					printwords();
					p = p->next;
				}
				else error('l', p->linenum);
			}
			else
			{
				isExpression();
				
				//输出仅为表达式时
				if (nowexpressiontype == INT)
					insertmidcode(PRINT, intchar, expressvar, nullchar);
				else
					insertmidcode(PRINT, charchar, expressvar, nullchar);
				free_reg(expressvar[2]);
				if (p->symbol == RPARENT)
				{
					printwords();
					if (p->next->symbol != SEMICN)
					{
						error('k', p->linenum);
					}
					p = p->next;
				}
				else error('l', p->linenum);
			}
		}
	}
	else error1();
	insertmidcode(PRINT, endline, nullchar, nullchar);
	outfile2 << "<写语句>" << endl;
}
//字符串
void isString()
{
	if (p->symbol == STRCON)
	{
		if (!isStrCon(p->origin))
		{
			error('a', p->linenum);
		}
		printwords();
		strcpy(stringcon, p->origin);
		p = p->next;
	}
	outfile2 << "<字符串>" << endl;
}
//情况语句
void isSwitchStatement()
{
	if (p->symbol == SWITCHTK)
	{
		int switch_num = Switch_Count;
		Switch_Count++;
		printwords();
		p = p->next;
		if (p->symbol == LPARENT)
		{
			printwords();
			p = p->next;
			isExpression();
			switchtype = nowexpressiontype;
			if (p->symbol == RPARENT)
			{
				printwords();
				p = p->next;
				
			}
			else error('l', p->linenum);
			if (p->symbol == LBRACE)
			{
				printwords();
				p = p->next;
				isCaseList(switch_num);
				isDefault();
				if (p->symbol == RBRACE)
				{
					printwords();
					p = p->next;
				}
				insertmidcode(LABEL, get_label_n(END_SWITCH_LABEL,switch_num), nullchar, nullchar);
			}
		}
	}
	else error1();
	outfile2 << "<情况语句>" << endl;
}
//情况表
void isCaseList(int switch_num)
{
	if (p->symbol == CASETK)
	{
		char exp_reg[100];
		int case_count = 1;
		strcpy(exp_reg, expressvar);
		insertmidcode(LABEL, get_switch_case(switch_num, case_count), nullchar, nullchar);
		isCaseStatement(switch_num,case_count,exp_reg);
		insertmidcode(JLABEL, get_label_n(END_SWITCH_LABEL, switch_num), nullchar, nullchar);
		case_count++;
		while (p->symbol == CASETK)
		{
			insertmidcode(LABEL, get_switch_case(switch_num, case_count), nullchar, nullchar);
			isCaseStatement(switch_num, case_count, exp_reg);
			insertmidcode(JLABEL, get_label_n(END_SWITCH_LABEL, switch_num), nullchar, nullchar);
			case_count++;
		}
		free_reg(exp_reg[2]);
		insertmidcode(LABEL, get_switch_case(switch_num, case_count), nullchar, nullchar);
	}
	else error1();
	outfile2 << "<情况表>" << endl;
}
//情况子语句
void isCaseStatement(int switch_num, int case_num, char exp_reg[])
{
	IdenType casetype;
	char const_reg[100];
	strcpy(const_reg, get_reg());
	if (p->symbol == CASETK)
	{
		printwords();
		p = p->next;
		if (p->symbol == CHARCON)
		{
			casetype = CHAR;
		}
		else
		{
			casetype = INT;
		}
		if (casetype != switchtype)
		{
			error('o', p->linenum);
		}
		isConst();

		if (casetype == INT)
		{
			insertmidcode(SET, intchar, const_reg, constvalue);
		}
		else if (casetype == CHAR)
		{
			insertmidcode(SET, charchar, const_reg, constvalue);
		}
		insertmidcode(SUB, const_reg, exp_reg, const_reg);
		insertmidcode(BRANCH, (char*)Hash[EQL].data(), get_switch_case(switch_num,case_num+1), const_reg);
		free_reg(const_reg[2]);
		if (p->symbol == COLON)
		{
			printwords();
			p = p->next;
			isStatement();
		}
	}
	else error1();
	outfile2 << "<情况子语句>" << endl;
}
//缺省
void isDefault()

{
	if (p->symbol == DEFAULTTK)
	{
		printwords();
		p = p->next;
		if (p->symbol == COLON)
		{
			printwords();
			p = p->next;
			isStatement();
		}
	}
	else
	{
		error('p',p->linenum);
	}
	outfile2 << "<缺省>" << endl;
}
//返回语句
void isReturnStatement()
{
	returnstatement = 1;
	if (p->symbol == RETURNTK)
	{

		printwords();
		p = p->next;
		if (p->symbol == LPARENT)//(
		{
			printwords();
			p = p->next;
			if (p->symbol == RPARENT)
			{
				if (isreturnfun == true)
				{
					error('h', p->linenum);
				}
				else error('g', p->linenum);
				printwords();
				p = p->next;
			}
			else 
			{
				isExpression();
				insertmidcode(MOVE, (char*)"$s0", expressvar, nullchar);
				free_reg(expressvar[2]);
				if (infunc == 0)
				{
					insertmidcode(JLABEL, END_MAIN, nullchar, nullchar);
				}
				else
				{
					insertmidcode(MOVE, s1_reg, temps1_reg, nullchar);
					free_reg(temps1_reg[2]);
					insertmidcode(JR, nullchar, nullchar, nullchar);
				}
				if (isreturnfun == true && returntype != nowexpressiontype)
				{
					error('h', p->linenum);
				}
				else if(isreturnfun == false)
				{
					error('g', p->linenum);
				}
				if (p->symbol == RPARENT)
				{
					printwords();
					p = p->next;
				}
				else error('l', p->linenum);
			}
		}
		else
		{
			if (isreturnfun == true)
			{
				error('h', p->linenum);
			}
			if (infunc == 0)
			{
				insertmidcode(JLABEL, END_MAIN, nullchar, nullchar);
			}
			else
			{
				insertmidcode(MOVE, s1_reg, temps1_reg, nullchar);
				free_reg(temps1_reg[2]);
				insertmidcode(JR, nullchar, nullchar, nullchar);
			}
		}
	}
	else
	{
		if (isreturnfun == true)
		{
			error('h', p->linenum);
		}
	}
	outfile2 << "<返回语句>" << endl;
}
//有返回值函数调用语句
void isCallReturnFun()
{
	bool new_exp_flag = true;
	int save_reg[10];
	int reg_num = 0;
	if (p->symbol == IDENFR)
	{
		char tempname[100];
		strcpy(tempname, p->origin);
		to_tolower(tempname);
		if (TabHash.find(tempname) == TabHash.end())
		{
			error('c', p->linenum);
		}
		else
		{
			nowusefun = TabHash[tempname];
		}
		printwords();
		p = p->next;
		if (p->symbol == LPARENT)
		{
			printwords();
			p = p->next;
			char offset_value[100];
			char const_reg[100];
			strcpy(const_reg, get_reg());
			if (paralist_flag == false)
			{
				insertmidcode(MOVE, s1_reg, sp_reg, nullchar);
			}
			if (infunc == 0)//主函数调用函数
			{
				int2char(global_offset, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(SUB, sp_reg, sp_reg, const_reg);
				free_reg(const_reg[2]);
			}
			else//函数递归调用函数
			{
				int2char(fun_offset, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(SUB, sp_reg, sp_reg, const_reg);
				int2char(0, offset_value, 10);
				insertmidcode(FUN_ASS, nullchar, ra_reg, offset_value);
				int2char(4, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(SUB, sp_reg, sp_reg, const_reg);
				free_reg(const_reg[2]);
			}
			save_temp_reg(save_reg, &reg_num);
			if (is_new_exp == false)
			{
				is_new_exp = true;
				new_exp_flag = false;
			}
			isValueParaList();

			//调用函数
			insertmidcode(JAL, tempname, nullchar, nullchar);
			//调用结束后返回，恢复现场
			recover_temp_reg(save_reg, reg_num);
			strcpy(const_reg, get_reg());
			if (infunc == 0)//主函数调用函数
			{
				int2char(global_offset, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(ADD, sp_reg, sp_reg, const_reg);
				free_reg(const_reg[2]);
			}
			else//函数递归调用函数
			{
				int2char(4, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(ADD, sp_reg, sp_reg, const_reg);
				int2char(0, offset_value, 10);
				insertmidcode(GET_FPARA, nullchar, offset_value, ra_reg);
				int2char(fun_offset, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(ADD, sp_reg, sp_reg, const_reg);
				free_reg(const_reg[2]);
			}

			if (new_exp_flag == false)
			{
				is_new_exp = false;
			}
			if (p->symbol == RPARENT)
			{
				printwords();
				p = p->next;
			}
			else error('l', p->linenum);
		}
	}
	outfile2 << "<有返回值函数调用语句>" << endl;
}
//无返回值函数调用语句
void isCallNoReturnFun()
{
	bool new_exp_flag = true;
	int save_reg[10];
	int reg_num = 0;
	if (p->symbol == IDENFR)
	{
		char tempname[100];
		strcpy(tempname, p->origin);
		to_tolower(tempname);
		if (TabHash.find(tempname) == TabHash.end())
		{
			error('c', p->linenum);
		}
		else
		{
			nowusefun = TabHash[tempname];
		}
		printwords();
		p = p->next;
		if (p->symbol == LPARENT)
		{
			printwords();
			p = p->next;

			char offset_value[100];
			char const_reg[100];
			strcpy(const_reg, get_reg());
			if (paralist_flag == false)
			{
				insertmidcode(MOVE, s1_reg, sp_reg, nullchar);
			}
			if (infunc == 0)//主函数调用函数
			{
				int2char(global_offset, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(SUB, sp_reg, sp_reg, const_reg);
				free_reg(const_reg[2]);
			}
			else//函数递归调用函数
			{
				int2char(fun_offset, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(SUB, sp_reg, sp_reg, const_reg);
				int2char(0, offset_value, 10);
				insertmidcode(FUN_ASS, nullchar, ra_reg, offset_value);
				int2char(4, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(SUB, sp_reg, sp_reg, const_reg);
				free_reg(const_reg[2]);
			}
			save_temp_reg(save_reg, &reg_num);

			if (is_new_exp == false)
			{
				is_new_exp = true;
				new_exp_flag = false;
			}
			isValueParaList();

			//调用函数
			insertmidcode(JAL, tempname, nullchar, nullchar);
			//调用结束后返回，恢复现场
			recover_temp_reg(save_reg, reg_num);
			strcpy(const_reg, get_reg());
			if (infunc == 0)//主函数调用函数
			{
				int2char(global_offset, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(ADD, sp_reg, sp_reg, const_reg);
				free_reg(const_reg[2]);
			}
			else//函数递归调用函数
			{
				int2char(4, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(ADD, sp_reg, sp_reg, const_reg);
				int2char(0, offset_value, 10);
				insertmidcode(GET_FPARA, nullchar, offset_value, ra_reg);
				int2char(fun_offset, offset_value, 10);
				insertmidcode(SET, intchar, const_reg, offset_value);
				insertmidcode(ADD, sp_reg, sp_reg, const_reg);
				free_reg(const_reg[2]);
			}

			if (new_exp_flag == false)
			{
				is_new_exp = false;
			}

			if (p->symbol == RPARENT)
			{
				printwords();
				p = p->next;
			}
			else error('l', p->linenum);
		}
	}
	outfile2 << "<无返回值函数调用语句>" << endl;
}  
//值参数表
void isValueParaList()
{
	int paracount = 0;
	int offset_count = 0;
	char offset_char[100];
	bool first_flag = false;
	if (paralist_flag == false)
	{
		first_flag = true;
		paralist_flag = true;
	}
	if (p->symbol != RPARENT && p->symbol != SEMICN)//每行最多一个错误，不会出现同时缺少）；
	{
		isExpression();
		int2char(offset_count, offset_char, 10);
		insertmidcode(FUN_ASS, nullchar, expressvar, offset_char);
		free_reg(expressvar[2]);
		offset_count += 4;
		if (nowusefun->funtab->para_type[paracount] != nowexpressiontype && nowusefun->funtab->para_num > paracount)
		{
			error('e', p->linenum);
		}
		paracount++;
		while (p->symbol == COMMA)
		{
			printwords();
			p = p->next;
			isExpression();
			int2char(offset_count, offset_char, 10);
			insertmidcode(FUN_ASS, nullchar, expressvar, offset_char);
			free_reg(expressvar[2]);
			offset_count += 4;
			if (nowusefun->funtab->para_type[paracount] != nowexpressiontype && nowusefun->funtab->para_num > paracount)
			{
				error('e', p->linenum);
			}
			paracount++;
		}
	} 
	if (paracount != nowusefun->funtab->para_num)
	{
		error('d', p->linenum);
	}
	if (first_flag)
	{
		paralist_flag = false;
	}
	outfile2 << "<值参数表>" << endl;
}


//表达式
void isExpression()
{
	bool hasminu = false;
	bool new_exp_flag;//判断当前表达式是新的表达式，还是表达式中的子表达式
	if (is_new_exp) {
		new_exp_flag = true;
		is_new_exp = false;
	}
	else
		new_exp_flag = false;
	if (p->symbol == PLUS || p->symbol == MINU)
	{
		printwords();
		if (p->symbol == MINU)
		{
			hasminu = true;
		}
		p = p->next;
	}
	if (hasminu)//添加一个0因子
	{
		insert_Factor(nullchar, INTCONST, NOTOP, zerochar, 0, false, 0);
	}
	isItem();
	if (hasminu)
	{
		nowexpressiontype = INT;
		insert_Factor(nullchar, OP, OpSub, nullchar, 0, false, 0);//添加-操作符
	}
	else
		nowexpressiontype = nowitemtype;
	while (p->symbol == PLUS || p->symbol == MINU)
	{
		Symbol temp_exp_type = p->symbol;
		printwords();
		p = p->next;
		isItem();
		if (temp_exp_type == PLUS) insert_Factor(nullchar, OP, OpAdd, nullchar, 0, false, 0);
		else insert_Factor(nullchar, OP, OpSub, nullchar, 0, false, 0);
		nowexpressiontype = INT;
	}
	if (new_exp_flag)
	{
		is_new_exp = true;
		print_factor();
		create_exp_midcode();
		strcpy(expressvar, exp_root->name);
		free(exp_root);
		exp_root = NULL;
	}
	outfile2 << "<表达式>" << endl;
}
//项
void isItem()
{
	isFactor();
	nowitemtype = nowfactortype;
	while (p->symbol == MULT || p->symbol == DIV)
	{
		Symbol temp_item_type = p->symbol;
		printwords();
		p = p->next;
		isFactor();
		nowitemtype = INT;
		if (temp_item_type == MULT) insert_Factor(nullchar, OP, OpMult, nullchar, 0, false, 0);
		else insert_Factor(nullchar, OP, OpDiv, nullchar, 0, false, 0);
	}
	outfile2 << "<项>" << endl;
}
//因子
void isFactor()
{
	if (p->symbol == IDENFR && p->next->symbol == LPARENT)//函数（）
	{
		char tempname[100];
		char fun_reg[100];
		Factor_ptr temp_factor_ptr = factor_ptr;
		Factor_ptr temp_root_ptr = exp_root;
		factor_ptr = NULL;
		exp_root = NULL;
		strcpy(tempname, p->origin);
		to_tolower(tempname);
		nowfactortype = TabHash[tempname]->type;
		isCallReturnFun();
		factor_ptr = temp_factor_ptr;
		exp_root = temp_root_ptr;
		strcpy(fun_reg, get_reg());
		insertmidcode(MOVE, fun_reg, (char*)"$s0", nullchar);
		insert_Factor(fun_reg, REGISTER, NOTOP, nullchar, 0, false, 0);
		nowfactortype = TabHash[tempname]->type;
		cout << nowexpressiontype << endl;
	}
	else if (p->symbol == IDENFR)//变量
	{
		char tempname[100];
		int offset;
		bool is_global;
		int dim2_num;
		strcpy(tempname, p->origin);
		to_tolower(tempname);
		if (TabHash.find(tempname) == TabHash.end())
		{
			error('c', p->linenum);
		}
		else
		{
			nowfactortype = TabHash[tempname]->type;
		}
		offset = TabHash[tempname]->offset;
		is_global = TabHash[tempname]->is_global;
		dim2_num = TabHash[tempname]->dim2_num;
		insert_Factor(tempname, IDENTIFIER, NOTOP, nullchar, offset, is_global, dim2_num);
		printwords();
		p = p->next;
		if (p->symbol == LBRACK)//一维数组左括号
		{
			printwords();
			p = p->next;
			isExpression();
			if (nowexpressiontype != INT)
			{
				error('i',p->linenum);
			}
			if (p->symbol == RBRACK)
			{
				printwords();
				p = p->next;
			}
			else error('m', p->linenum);
			if (p->symbol == LBRACK)//二维数组左括号
			{
				printwords();
				p = p->next;
				isExpression();
				insert_Factor(nullchar, OP, DIM2, nullchar, 0, false, 0);
				if (nowexpressiontype != INT)
				{
					error('i', p->linenum);
				}
				if (p->symbol == RBRACK)
				{
					printwords();
					p = p->next;
				}
				else error('m', p->linenum);
			}
			else
			{
				insert_Factor(nullchar, OP, DIM1, nullchar, 0, false, 0);
			}
		}
		if (TabHash.find(tempname) != TabHash.end())
		{
			nowfactortype = TabHash[tempname]->type;
		}
	}
	else if (p->symbol == LPARENT)//（表达式）
	{
		printwords();
		p = p->next;
		isExpression();
		nowfactortype = INT;
		if (p->symbol == RPARENT)
		{
			printwords();
			p = p->next;
		}
		else error('l', p->linenum);
	}
	else if (p->symbol == CHARCON)
	{
		if (!isCharCon(p->origin[0]))
		{
			error('a', p->linenum);
		}
		nowfactortype = CHAR;
		printwords();
		//添加因子
		insert_Factor(nullchar, CHARCONST, NOTOP, p->origin, 0, false, 0);
		p = p->next;
	}
	else if (p->symbol == PLUS || p->symbol == MINU || p->symbol == INTCON)//整数
	{
		nowfactortype = INT;
		isInteger();
		insert_Factor(nullchar, INTCONST, NOTOP, constvalue, 0, false, 0);
	}
	outfile2 << "<因子>" << endl;
}



