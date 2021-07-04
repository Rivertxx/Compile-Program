#pragma once
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include<stdlib.h> 
#include<map>
#include<set>
using namespace std;
extern ofstream errorfile;

void error1();
void error(char type,int linenum);
