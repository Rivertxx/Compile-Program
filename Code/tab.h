#pragma once
#include "grammar.h"
#include "enum.h"

typedef struct ArrayTab ArrayTab;
typedef struct ArrayTab* ArrayTab_ptr;

char* to_tolower(char* s);

void AddConst(char s[], IdenType t, bool con, int level, bool isarray, int linenum, int offset, bool is_global);

void AddFun(char s[], IdenType t, int level, int linenum);