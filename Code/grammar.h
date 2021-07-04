#pragma once
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include<stdlib.h> 
#include<cstdlib>
#include<map>
#include<set>
#include "word.h"
#include "error.h"
#include "tab.h"
#include "enum.h"
#include "midcode.h"
using namespace std;

struct FunTab {
	bool hasReturn;
	int para_num;
	IdenType para_type[20];
};
struct ArraytTab {
	int dim;
	int dim_one_num;
	int dim_two_num;
};
struct Tab {
	char name[100];
	IdenType type;
	bool is_const;
	bool is_array;
	bool is_global;
	int lev;
	int offset;
	int dim2_num;
	struct FunTab* funtab;
	struct ArrayTab* arraytab;
	struct Tab* pre;
	struct Tab* next;
	struct Tab* link;
};
struct Midcode {
	Operation op;
	char operand_1[100];
	char operand_2[100];
	char operand_3[100];
	struct Midcode* next;
};
struct Factor {
	char name[100];
	FactorType factorType;
	OpType opType;
	char value[100];
	int offset;
	int dim2_num;
	bool is_global;
	struct Factor* pre;
	struct Factor* next;
};
extern ifstream infile;
extern ofstream outfile;
extern ofstream outfile2;
extern ofstream midcodefile;
extern ofstream mipsfile;
typedef struct Word Words;
typedef struct Word* Nodeptr;
typedef struct FunTab FunTab;
typedef struct Tab Tab;
typedef struct FunTab* FunTab_ptr;
typedef struct ArrayTab ArrayTab;
typedef struct ArrayTab* ArrayTab_ptr;
typedef struct Tab* Tab_ptr;
typedef struct Midcode Midcode;
typedef struct Midcode* Midcode_ptr;
typedef struct Factor Factor;
typedef struct Factor* Factor_ptr;
extern Nodeptr root;
extern Nodeptr p;
extern int line;
extern bool isconst;
extern Tab_ptr tabroot;
extern Tab_ptr tab_ptr;
extern IdenType ident_type;
extern char ident_name[100];
extern int level;
extern int dim;
extern map<string, Tab_ptr> TabHash;
extern map<Operation, string> OperationHash;
extern Midcode_ptr midcode_root;
extern Midcode_ptr midcode_ptr;
extern Midcode_ptr midcode_data_root;
extern Midcode_ptr midcode_data_ptr;


char* get_label_n(char a[], int n);
char* get_switch_case(int a, int b);
char* int2char(int n, char* s, int radix);
int Unsignednum(char s[]);
void isProcedure();//程序
void isConstantDeclaration();
void isVariableDeclaration();
bool isTypeIdentifier(Symbol s);
void isMain();
void isHasReturnFun();
void isNoReturnFun();
void isConstDefi();
void isInteger();
void isUnsigned(bool from_integer);
void isVariableDefi();
void isVarDefInit();
void isVarDefNoInit();
void isConst();
void isParaList();
void isCompStatement();
void isHeadStatement();
void isStatementColumn();
void isStatement();
void isLoopStatement();
void isIfStatement();
void isAssignStatement();
void isReadStatement();
void isWriteStatement();
void isSwitchStatement();
void isReturnStatement();
bool isStatementCondition();
void isCondition();
void isExpression();
void isStepSize();
bool isRelationOperator(Symbol s);
void isString();
void isCaseList(int switch_num);
void isDefault();
void isCaseStatement(int switch_num, int case_num, char exp_reg[]);
void isItem();
void isFactor();
void isCallReturnFun();
void isCallNoReturnFun();
void isValueParaList();