#pragma once
#include "grammar.h"

typedef struct Midcode* Midcode_ptr;
typedef struct Factor Factor;
typedef struct Factor* Factor_ptr;
extern map<string, string> RegisterHash;

void insertmidcode(Operation op, char op1[], char op2[], char op3[]);

void insert_Datamidcode(Operation op, char op1[], char op2[], char op3[]);

void print_factor();

void insert_Factor(char name[], FactorType ft, OpType ot, char value[], int offset, bool isglobal, int dim2_num);

void give_identifier_midcode(Factor_ptr q, char reg[]);

void give_const_midcode(Factor_ptr q, char reg[]);

char* get_reg();

void free_reg(char a);

void printmidcode();

void destorymidcode();

void create_exp_midcode();

void save_temp_reg(int* save_reg, int* reg_num);

void recover_temp_reg(int* save_reg, int save_reg_num);

void tomips();

void Add_mips(Midcode_ptr q);

void Sub_mips(Midcode_ptr q);

void Mul_mips(Midcode_ptr q);

void Divi_mips(Midcode_ptr q);

void GLOBAL_PARA_mips(Midcode_ptr q);

void FUN_PARA_mips(Midcode_ptr q);

void Print_mips(Midcode_ptr q);

void Stringdata_mips(Midcode_ptr q);

void Global_Scanf_mips(Midcode_ptr q);

void Fun_Scanf_mips(Midcode_ptr q);

void Global_Ass_mips(Midcode_ptr q);

void Recoverreg_mips(Midcode_ptr q);

void Fun_Ass_mips(Midcode_ptr q);

void Store_Array_mips(Midcode_ptr q);

void Label_mips(Midcode_ptr q);

void Data_mips(Midcode_ptr q);

void Text_mips(Midcode_ptr q);

void Set_mips(Midcode_ptr q);

void Get_Gpara_mips(Midcode_ptr q);

void Get_Fpara_mips(Midcode_ptr q);

void Get_Fpara2_mips(Midcode_ptr q);

void Move_mips(Midcode_ptr q);

void Jlabel_mips(Midcode_ptr q);

void Caof1_mips(Midcode_ptr q);

void Caof2G_mips(Midcode_ptr q);

void Caof2F_mips(Midcode_ptr q);

void load_array_mips(Midcode_ptr q);

void Branch_mips(Midcode_ptr q);

void Jal_mips(Midcode_ptr q);

void Jr_mips(Midcode_ptr q);
