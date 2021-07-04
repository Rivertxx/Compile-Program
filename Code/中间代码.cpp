//这各代码存放与中间代码相关的函数，如midcode链表的插入删除等
#define _CRT_SECURE_NO_WARNINGS
#include "midcode.h"
int stringconnum = 0;
extern Factor_ptr exp_root;
extern Factor_ptr factor_ptr;
int reg_count = 0;
int t1_flag = 0;
int t2_flag = 0;
int t3_flag = 0;
int t4_flag = 0;
int t5_flag = 0;
int t6_flag = 0;
int t7_flag = 0;
int t8_flag = 0;
int t9_flag = 0;
char t1[100] = "$t1";
char t2[100] = "$t2";
char t3[100] = "$t3";
char t4[100] = "$t4";
char t5[100] = "$t5";
char t6[100] = "$t6";
char t7[100] = "$t7";
char t8[100] = "$t8";
char t9[100] = "$t9";
char t0[100] = "$t0";
extern char global_char[100];
extern char fun_char[100];
extern char nullchar[100];
extern char intchar[100];
extern char charchar[100];
extern char sp_reg[100];
extern bool paralist_flag;



void insertmidcode(Operation op, char op1[], char op2[], char op3[])
{
	Midcode_ptr q;
	q = (Midcode_ptr)malloc(sizeof(Midcode));
	if (q == NULL)
	{
		cout << "Malloc failed!" << endl;
	}
	else
	{
		q->op = op;
		strcpy(q->operand_1, op1);
		strcpy(q->operand_2, op2);
		strcpy(q->operand_3, op3);
		q->next = NULL;
		if (midcode_root != NULL) {
			midcode_ptr->next = q;
			midcode_ptr = q;
		}
		else {
			midcode_root = q;
			midcode_ptr = q;
		}
	}
}

void insert_Datamidcode(Operation op, char op1[], char op2[], char op3[])
{
	Midcode_ptr q;
	q = (Midcode_ptr)malloc(sizeof(Midcode));
	if (q == NULL)
	{
		cout << "Malloc failed!" << endl;
	}
	else
	{
		q->op = op;
		strcpy(q->operand_1, op1);
		strcpy(q->operand_2, op2);
		strcpy(q->operand_3, op3);
		q->next = NULL;
		if (midcode_data_root != NULL) {
			midcode_data_ptr->next = q;
			midcode_data_ptr = q;
		}
		else {
			midcode_data_root = q;
			midcode_data_ptr = q;
		}
	}

}

void insert_Factor(char name[], FactorType ft, OpType ot, char value[], int offset, bool isglobal, int dim2_num)
{
	Factor_ptr q;
	q = (Factor_ptr)malloc(sizeof(Factor));
	if (q == NULL)
	{
		cout << "malloc fail!" << endl;
	}
	else
	{
		strcpy(q->name, name);
		strcpy(q->value, value);
		q->offset = offset;
		q->dim2_num = dim2_num;
		q->is_global = isglobal;
		q->factorType = ft;
		q->opType = ot;
		q->pre = NULL;
		q->next = NULL;
		if (exp_root == NULL)
		{
			exp_root = q;
			factor_ptr = exp_root;
		}
		else
		{
			factor_ptr->next = q;
			q->pre = factor_ptr;
			factor_ptr = q;
		}
	}
}

void print_factor() {
	Factor_ptr q;
	q = exp_root;
	while (q != NULL)
	{
		switch (q->factorType)
		{
		case OP:
			switch (q->opType)
			{
			case OpAdd:
				cout << "+" << endl;
				break;
			case OpSub:
				cout << "-" << endl;
				break;
			case OpDiv:
				cout << "/" << endl;
				break;
			case OpMult:
				cout << "*" << endl;
				break;
			case DIM1:
				cout << "DIM1" << endl;
				break;
			case DIM2:
				cout << "DIM2" << endl;
				break;
			case CALLFUN:
				cout << "CALLFUN" << endl;
				break;
			default:
				break;
			}
			break;
		case IDENTIFIER:
			cout << q->name << endl;
			break;
		case RETURNFUN:
			cout << q->name << endl;
			break;
		case REGISTER:
			cout << q->name << endl;
			break;
		default:
			cout << q->value << endl;
			break;
		}
		q = q->next;
	}	
}

void printmidcode()
{
	midcodefile << OperationHash[midcode_ptr->op] << " " << midcode_ptr->operand_1 << " " << midcode_ptr->operand_2 << " " << midcode_ptr->operand_3 << endl;
}

void destorymidcode()
{
	midcode_ptr = midcode_root;
	while (midcode_ptr != NULL)
	{
		Midcode_ptr q;
		q = midcode_ptr->next;
		free(midcode_ptr);
		midcode_ptr = q;
	}
}

void create_exp_midcode() {
	factor_ptr = exp_root;
	if (factor_ptr->next == NULL)
	{
		if (factor_ptr->factorType == INTCONST || factor_ptr->factorType == CHARCONST)
		{
			give_const_midcode(factor_ptr, get_reg());
		}
		else if (factor_ptr->factorType == IDENTIFIER)
		{
			give_identifier_midcode(factor_ptr, get_reg());
		}
	}
	else
	{
		while (factor_ptr!=NULL)
		{
			if (factor_ptr->factorType == OP)
			{
				cout << "OPtype:" << factor_ptr->opType << endl;
				Factor_ptr r1 = factor_ptr->pre->pre;
				Factor_ptr r2 = factor_ptr->pre;
				if (factor_ptr->opType == OpAdd || factor_ptr->opType == OpSub || factor_ptr->opType == OpMult || factor_ptr->opType == OpDiv)
				{
					if (r1->factorType == INTCONST || r1->factorType == CHARCONST)
					{
						give_const_midcode(r1, get_reg());
					}
					else if (r1->factorType == IDENTIFIER)
					{
						give_identifier_midcode(r1, get_reg());
					}
					if (r2->factorType == INTCONST || r2->factorType == CHARCONST)
					{
						give_const_midcode(r2, get_reg());
					}
					else if (r2->factorType == IDENTIFIER)
					{
						give_identifier_midcode(r2, get_reg());
					}
					switch (factor_ptr->opType)
					{
						case OpAdd:
							insertmidcode(ADD, r1->name, r1->name, r2->name);
							break;
						case OpSub:
							insertmidcode(SUB, r1->name, r1->name, r2->name);
							break;
						case OpDiv:
							insertmidcode(DIVI, r1->name, r1->name, r2->name);
							break;
						case OpMult:
							insertmidcode(MUL, r1->name, r1->name, r2->name);
							break;
						default:
							break;
					}
					free_reg(r2->name[2]);
					r1->next = factor_ptr->next;
					if (factor_ptr->next != NULL)
					{
						factor_ptr->next->pre = r1;
					}
				}
				else if (factor_ptr->opType == DIM1)
				{
					if (r2->factorType == INTCONST || r2->factorType == CHARCONST)
					{
						give_const_midcode(r2, get_reg());
					}
					else if (r2->factorType == IDENTIFIER)
					{
						give_identifier_midcode(r2, get_reg());
					}
					char para_offset[100];
					char reg[100];
					strcpy(reg, get_reg());
					int2char(r1->offset, para_offset, 10);
					if (r1->is_global)
					{
						insertmidcode(CAOF1, r2->name, nullchar, global_char);
					}
					else
					{
						insertmidcode(CAOF1, r2->name, nullchar, fun_char);
					}
					insertmidcode(LOAD_ARRAY, r1->name, para_offset, reg);
					strcpy(r1->name, reg);
					r1->factorType = REGISTER;
					free_reg(r2->name[2]);
					r1->next = factor_ptr->next;
					if (factor_ptr->next != NULL)
					{
						factor_ptr->next->pre = r1;
					}
				}
				else if (factor_ptr->opType == DIM2)
				{
					Factor_ptr r3;
					char para_offset[100];
					char reg[100];
					char dim_value[100];
					r3 = r1->pre;
					strcpy(reg, get_reg());
					int2char(r3->dim2_num, dim_value, 10);
					int2char(r3->offset, para_offset, 10);
					if (r1->factorType == INTCONST || r1->factorType == CHARCONST)
					{
						give_const_midcode(r1, get_reg());
					}
					else if (r1->factorType == IDENTIFIER)
					{
						give_identifier_midcode(r1, get_reg());
					}
					if (r2->factorType == INTCONST || r2->factorType == CHARCONST)
					{
						give_const_midcode(r2, get_reg());
					}
					else if (r2->factorType == IDENTIFIER)
					{
						give_identifier_midcode(r2, get_reg());
					}
					if (r3->is_global)
					{
						insertmidcode(CAOF2G, r1->name, r2->name, dim_value);
					}
					else
					{
						insertmidcode(CAOF2F, r1->name, r2->name, dim_value);
					}
					insertmidcode(LOAD_ARRAY, r3->name, para_offset, reg);
					strcpy(r3->name, reg);
					r3->factorType = REGISTER;
					free_reg(r2->name[2]);
					free_reg(r1->name[2]);
					r3->next = factor_ptr->next;
					if (factor_ptr->next != NULL)
					{
						factor_ptr->next->pre = r3;
					}
				}
			}
			factor_ptr = factor_ptr->next;
		}
	}
	
}

char* get_reg()
{
	if (t1_flag == 0)
	{
		t1_flag = 1;
		//cout << "get reg_1" << endl;
		return t1;
	}
	else if (t2_flag == 0)
	{
		t2_flag = 1;
		//cout << "get reg_2" << endl;
		return t2;
	}
	else if (t3_flag == 0)
	{
		t3_flag = 1;
		//cout << "get reg_3" << endl;
		return t3;
	}
	else if (t4_flag == 0)
	{
		t4_flag = 1;
		//cout << "get reg_4" << endl;
		return t4;
	}
	else if (t5_flag == 0)
	{
		t5_flag = 1;
		//cout << "get reg_5" << endl;
		return t5;
	}
	else if (t6_flag == 0)
	{
		t6_flag = 1;
		//cout << "get reg_6" << endl;
		return t6;
	}
	else if (t7_flag == 0)
	{
		t7_flag = 1;
		//cout << "get reg_7" << endl;
		return t7;
	}
	else if (t8_flag == 0)
	{
		t8_flag = 1;
		//cout << "get reg_8" << endl;
		return t8;
	}
	else if (t9_flag == 0)
	{
		t9_flag = 1;
		//cout << "get reg_9" << endl;
		return t9;
	}
	else
	{
		//cout << "No register!" << endl;
		return t0;
	}
}

void free_reg(char a)
{
	int n = a - '0';
	//cout << "free reg_" << n << endl;
	switch (n)
	{
	case 1:
		t1_flag = 0;
		break;
	case 2:
		t2_flag = 0;
		break;
	case 3:
		t3_flag = 0;
		break;
	case 4:
		t4_flag = 0;
		break;
	case 5:
		t5_flag = 0;
		break;
	case 6:
		t6_flag = 0;
		break;
	case 7:
		t7_flag = 0;
		break;
	case 8:
		t8_flag = 0;
		break;
	case 9:
		t9_flag = 0;
		break;
	default:
		break;
	}
}

//传入一个标识符的因子，制造中间代码
void give_identifier_midcode(Factor_ptr q, char reg[])
{
	char para_offset[100];
	int2char(q->offset, para_offset, 10);
	if (q->is_global)
	{
		insertmidcode(GET_GPARA, q->name, para_offset, reg);
	}
	else
	{
		if (paralist_flag)
		{
			insertmidcode(GET_FPARA2, q->name, para_offset, reg);
		}
		else
		{
			insertmidcode(GET_FPARA, q->name, para_offset, reg);
		}
	}
	strcpy(q->name, reg);
	q->factorType = REGISTER;
}

void give_const_midcode(Factor_ptr q, char reg[])
{
	if (q->factorType == INTCONST)
	{
		insertmidcode(SET, intchar, reg, q->value);
	}
	else if (q->factorType == CHARCONST)
	{
		insertmidcode(SET, charchar, reg, q->value);
	}
	strcpy(q->name, reg);
	q->factorType = REGISTER;
}

void save_temp_reg(int* save_reg, int* reg_num)
{
	int save_reg_count = 0;
	char save_offset[100];
	char const_reg[100];
	int save_reg_num = 0;
	if (t1_flag == 1)
	{
		save_reg[save_reg_num] = 1;
		t1_flag = 0;
		int2char(save_reg_count, save_offset, 10);
		save_reg_count += 4;
		save_reg_num++;
		insertmidcode(FUN_ASS, nullchar, t1, save_offset);
	}
	if (t2_flag == 1)
	{
		save_reg[save_reg_num] = 2;
		t2_flag = 0;
		int2char(save_reg_count, save_offset, 10);
		save_reg_count += 4;
		save_reg_num++;
		insertmidcode(FUN_ASS, nullchar, t2, save_offset);
	}
	if (t3_flag == 1)
	{
		save_reg[save_reg_num] = 3;
		t3_flag = 0;
		int2char(save_reg_count, save_offset, 10);
		save_reg_count += 4;
		save_reg_num++;
		insertmidcode(FUN_ASS, nullchar, t3, save_offset);
	}
	if (t4_flag == 1)
	{
		save_reg[save_reg_num] = 4;
		t4_flag = 0;
		int2char(save_reg_count, save_offset, 10);
		save_reg_count += 4;
		save_reg_num++;
		insertmidcode(FUN_ASS, nullchar, t4, save_offset);
	}
	if (t5_flag == 1)
	{
		save_reg[save_reg_num] = 5;
		t5_flag = 0;
		int2char(save_reg_count, save_offset, 10);
		save_reg_count += 4;
		save_reg_num++;
		insertmidcode(FUN_ASS, nullchar, t5, save_offset);
	}
	if (t6_flag == 1)
	{
		save_reg[save_reg_num] = 6;
		t6_flag = 0;
		int2char(save_reg_count, save_offset, 10);
		save_reg_count += 4;
		save_reg_num++;
		insertmidcode(FUN_ASS, nullchar, t6, save_offset);
	}
	if (t7_flag == 1)
	{
		save_reg[save_reg_num] = 7;
		t7_flag = 0;
		int2char(save_reg_count, save_offset, 10);
		save_reg_count += 4;
		save_reg_num++;
		insertmidcode(FUN_ASS, nullchar, t7, save_offset);
	}
	if (t8_flag == 1)
	{
		save_reg[save_reg_num] = 8;
		t8_flag = 0;
		int2char(save_reg_count, save_offset, 10);
		save_reg_count += 4;
		save_reg_num++;
		insertmidcode(FUN_ASS, nullchar, t8, save_offset);
	}
	if (t9_flag == 1)
	{
		save_reg[save_reg_num] = 9;
		t9_flag = 0;
		int2char(save_reg_count, save_offset, 10);
		save_reg_count += 4;
		save_reg_num++;
		insertmidcode(FUN_ASS, nullchar, t9, save_offset);
	}
	*reg_num = save_reg_num;
	int2char(save_reg_count, save_offset, 10);
	strcpy(const_reg, get_reg());
	insertmidcode(SET, intchar, const_reg, save_offset);
	insertmidcode(SUB, sp_reg, sp_reg, const_reg);
	free_reg(const_reg[2]);
}

void recover_temp_reg(int* save_reg, int save_reg_num)
{
	int offset = 4;
	char regn[100] = "$t0";
	char recover_offset[100];
	char const_reg[100];
	int i;
	int n;
	for (i = save_reg_num-1; i >= 0; i--)
	{
		n = save_reg[i];
		int2char(offset, recover_offset, 10);
		regn[2] = '0' + n;
		insertmidcode(RECOVERREG, nullchar, regn, recover_offset);
		offset += 4;
		switch (n)
		{
		case 1:
			t1_flag = 1;
			break;
		case 2:
			t2_flag = 1;
			break;
		case 3:
			t3_flag = 1;
			break;
		case 4:
			t4_flag = 1;
			break;
		case 5:
			t5_flag = 1;
			break;
		case 6:
			t6_flag = 1;
			break;
		case 7:
			t7_flag = 1;
			break;
		case 8:
			t8_flag = 1;
			break;
		case 9:
			t9_flag = 1;
			break;
		default:
			break;
		}
	}
	int2char(offset-4, recover_offset, 10);
	strcpy(const_reg, get_reg());
	insertmidcode(SET, intchar, const_reg, recover_offset);
	insertmidcode(ADD, sp_reg, sp_reg, const_reg);
	free_reg(const_reg[2]);
	save_reg_num = 0;
}


void tomips() {
	Midcode_ptr q = midcode_ptr;
	switch (q->op)
	{
		case ADD:
			Add_mips(q);
			break;
		case SUB:
			Sub_mips(q);
			break;
		case MUL:
			Mul_mips(q);
			break;
		case DIVI:
			Divi_mips(q);
			break;
		case GLOBAL_PARA:
			GLOBAL_PARA_mips(q);
			break;
		case FUN_PARA:
			FUN_PARA_mips(q);
			break;
		case PRINT:
			Print_mips(q);
			break;
		case STRINGDATA:
			Stringdata_mips(q);
			break;
		case GLOBAL_SCANF:
			Global_Scanf_mips(q);
			break;
		case FUN_SCANF:
			Fun_Scanf_mips(q);
			break;
		case GLOBAL_ASS:
			Global_Ass_mips(q);
			break;
		case FUN_ASS:
			Fun_Ass_mips(q);
			break;
		case STORE_ARRAY:
			Store_Array_mips(q);
			break;
		case LABEL:
			Label_mips(q);
			break;
		case DATA:
			Data_mips(q);
			break;
		case TEXT:
			Text_mips(q);
			break;
		case SET:
			Set_mips(q);
			break;
		case GET_GPARA:
			Get_Gpara_mips(q);
			break;
		case GET_FPARA:
			Get_Fpara_mips(q);
			break;
		case GET_FPARA2:
			Get_Fpara2_mips(q);
			break;
		case MOVE:
			Move_mips(q);
			break;
		case JLABEL:
			Jlabel_mips(q);
			break;
		case CAOF1:
			Caof1_mips(q);
			break;
		case CAOF2F:
			Caof2F_mips(q);
			break;
		case CAOF2G:
			Caof2G_mips(q);
			break;
		case LOAD_ARRAY:
			load_array_mips(q);
			break;
		case BRANCH:
			Branch_mips(q);
			break;
		case RECOVERREG:
			Recoverreg_mips(q);
			break;
		case JAL:
			Jal_mips(q);
			break;
		case JR:
			Jr_mips(q);
			break;
		default:
		break;
	}
}

void Add_mips(Midcode_ptr q)
{
	mipsfile << "addu " << q->operand_1 << "," << q->operand_2 << "," << q->operand_3 << endl;
}

void Sub_mips(Midcode_ptr q)
{
	mipsfile << "subu " << q->operand_1 << "," << q->operand_2 << "," << q->operand_3 << endl;
}

void Mul_mips(Midcode_ptr q)
{
	mipsfile << "mult " << q->operand_2 << "," << q->operand_3 << endl;
	mipsfile << "mflo " << q->operand_1 << endl;
}

void Divi_mips(Midcode_ptr q)
{
	mipsfile << "div " << q->operand_2 << "," << q->operand_3 << endl;
	mipsfile << "mflo " << q->operand_1 << endl;
}

void GLOBAL_PARA_mips(Midcode_ptr q)
{
	int offset;
	offset = Unsignednum(q->operand_2);
	if (strcmp(q->operand_1, "INT") == 0)
	{
		mipsfile << "li $t0," << q->operand_3 << endl;
		mipsfile << "sw $t0,-" << offset << "($fp)" << endl;
	}
	else
	{
		mipsfile << "li $t0,'" << q->operand_3 << "'" << endl;
		mipsfile << "sw $t0,-" << offset << "($fp)" << endl;
	}
}

void FUN_PARA_mips(Midcode_ptr q)
{
	int offset;
	offset = Unsignednum(q->operand_2);
	if (strcmp(q->operand_1, "INT") == 0)
	{
		mipsfile << "li $t0," << q->operand_3 << endl;
		mipsfile << "sw $t0,-" << offset << "($sp)" << endl;
	}
	else
	{
		mipsfile << "li $t0,'" << q->operand_3 << "'" << endl;
		mipsfile << "sw $t0,-" << offset << "($sp)" << endl;
	}
}

void Print_mips(Midcode_ptr q)
{
	if (strcmp(q->operand_1, "INT") == 0)
	{
		mipsfile << "move $a0," << q->operand_2 << endl;
		mipsfile << "li $v0,1" << endl;
		mipsfile << "syscall" << endl;
	}
	else if (strcmp(q->operand_1, "CHAR") == 0)
	{
		mipsfile << "move $a0," << q->operand_2 << endl;
		mipsfile << "li $v0,11" << endl;
		mipsfile << "syscall" << endl;
	}
	else if (strcmp(q->operand_1, "endline") == 0)
	{
		mipsfile << "li $a0,10" << endl;
		mipsfile << "li $v0,11" << endl;
		mipsfile << "syscall" << endl;
	}
	else if (strcmp(q->operand_1, "STRING") == 0)
	{
		mipsfile << "la $a0,MyStrCon_M" << q->operand_2 <<  endl;
		mipsfile << "li $v0,4" << endl;
		mipsfile << "syscall" << endl;
	}
}

void Stringdata_mips(Midcode_ptr q) {
	char s[200];
	int i = 0;
	int j = 0;
	while (q->operand_2[i] != '\0')
	{
		if (q->operand_2[i] == '\\')
		{
			s[j] = '\\';
			j++;
			s[j] = '\\';
			j++;
		}
		else
		{
			s[j] = q->operand_2[i];
			j++;
		}
		i++;
	}
	s[j] = '\0';
	mipsfile << "MyStrCon_M" << q->operand_1 << ": .asciiz \"" << s << "\"" << endl;
}

void Global_Scanf_mips(Midcode_ptr q)
{
	if (strcmp(q->operand_1, "INT") == 0)
	{
		mipsfile << "li $v0,5" << endl;
		mipsfile << "syscall" << endl;
		mipsfile << "sw $v0,-" << q->operand_3 << "($fp)"<< endl;
	}
	else if (strcmp(q->operand_1, "CHAR") == 0)
	{
		mipsfile << "li $v0,12" << endl;
		mipsfile << "syscall" << endl;
		mipsfile << "sw $v0,-" << q->operand_3 << "($fp)" << endl;
	}
}

void Fun_Scanf_mips(Midcode_ptr q)
{
	if (strcmp(q->operand_1, "INT") == 0)
	{
		mipsfile << "li $v0,5" << endl;
		mipsfile << "syscall" << endl;
		mipsfile << "sw $v0,-" << q->operand_3 << "($sp)" << endl;
	}
	else if (strcmp(q->operand_1, "CHAR") == 0)
	{
		mipsfile << "li $v0,12" << endl;
		mipsfile << "syscall" << endl;
		mipsfile << "sw $v0,-" << q->operand_3 << "($sp)" << endl;
	}
}

void Global_Ass_mips(Midcode_ptr q)
{
	mipsfile << "sw " << q->operand_2 << ",-" << q->operand_3 << "($fp)" << endl;
}

void Fun_Ass_mips(Midcode_ptr q)
{
	if (q->operand_1[0] == '$')
	{
		mipsfile << "sw " << q->operand_2 << ",-" << q->operand_3 << "(" <<  q->operand_1 << ")" << endl;
	}
	else
	{
		mipsfile << "sw " << q->operand_2 << ",-" << q->operand_3 << "($sp)" << endl;
	}
}

void Recoverreg_mips(Midcode_ptr q)
{
	mipsfile << "lw " << q->operand_2 << "," << q->operand_3 << "($sp)" << endl;
}

void Store_Array_mips(Midcode_ptr q)
{
	mipsfile << "sw " << q->operand_2 << ",-" << q->operand_3 << "($s0)" << endl;
}

void Set_mips(Midcode_ptr q)
{
	if (strcmp(q->operand_1, "INT") == 0)
	{
		mipsfile << "li " << q->operand_2 << "," << q->operand_3 << endl;
	}
	else if (strcmp(q->operand_1, "CHAR") == 0)
	{
		//int asscivalue = q->operand_3[0];
		mipsfile << "li " << q->operand_2 << ",'" << q->operand_3 << "'" << endl;
	}
}

void Get_Gpara_mips(Midcode_ptr q)
{
	mipsfile << "lw " << q->operand_3 << ",-" << q->operand_2 << "($fp)" << endl;
}

void Get_Fpara_mips(Midcode_ptr q)
{
	mipsfile << "lw " << q->operand_3 << ",-" << q->operand_2 << "($sp)" << endl;
}

void Get_Fpara2_mips(Midcode_ptr q)
{
	mipsfile << "lw " << q->operand_3 << ",-" << q->operand_2 << "($s1)" << endl;
}

void Label_mips(Midcode_ptr q)
{
	mipsfile << endl;
	mipsfile << q->operand_1 << ":" << endl;
}

void Data_mips(Midcode_ptr q)
{	
	mipsfile << ".data" << endl;
}

void Text_mips(Midcode_ptr q)
{
	mipsfile << endl;
	mipsfile << ".text" << endl;
}

void Move_mips(Midcode_ptr q) {
	mipsfile << "move " << q->operand_1 << "," << q->operand_2 << endl;
}

void Jlabel_mips(Midcode_ptr q)
{
	mipsfile << "j " << q->operand_1 << endl;
}

void Caof1_mips(Midcode_ptr q) {
	if (strcmp(q->operand_3,"global")==0)
	{
		mipsfile << "li $s0,4" << endl;
		mipsfile << "mult $s0," << q->operand_1 << endl;
		mipsfile << "mflo $s0" << endl;
		mipsfile << "subu $s0,$fp,$s0" << endl;
	}
	else
	{
		mipsfile << "li $s0,4" << endl;
		mipsfile << "mult $s0," << q->operand_1 << endl;
		mipsfile << "mflo $s0" << endl;
		mipsfile << "subu $s0,$sp,$s0" << endl;
	}
}

void Caof2G_mips(Midcode_ptr q) {
	mipsfile << "li $s0," << q->operand_3 << endl;
	mipsfile << "mult $s0,"<< q->operand_1 << endl;
	mipsfile << "mflo $s1" << endl;
	mipsfile << "addu $s1,$s1," << q->operand_2 << endl;
	mipsfile << "li $s0,4" << endl;
	mipsfile << "mult $s0,$s1" << endl;
	mipsfile << "mflo $s0" << endl;
	mipsfile << "subu $s0,$fp,$s0" << endl;
}

void Caof2F_mips(Midcode_ptr q) {
	mipsfile << "li $s0," << q->operand_3 << endl;
	mipsfile << "mult $s0," << q->operand_1 << endl;
	mipsfile << "mflo $s1" << endl;
	mipsfile << "addu $s1,$s1," << q->operand_2 << endl;
	mipsfile << "li $s0,4" << endl;
	mipsfile << "mult $s0,$s1" << endl;
	mipsfile << "mflo $s0" << endl;
	mipsfile << "subu $s0,$sp,$s0" << endl;
}

void load_array_mips(Midcode_ptr q) {
	mipsfile << "lw " << q->operand_3 << ",-" << q->operand_2 << "($s0)" << endl; 
}

void Branch_mips(Midcode_ptr q)
{
	if (strcmp(q->operand_1, (char*)"LSS") == 0) // <
	{
		mipsfile << "bgez " << q->operand_3 << "," << q->operand_2 << endl;
	}
	if (strcmp(q->operand_1, (char*)"LEQ") == 0) // <=
	{
		mipsfile << "bgtz " << q->operand_3 << "," << q->operand_2 << endl;
	}
	if (strcmp(q->operand_1, (char*)"GRE") == 0) // >
	{
		mipsfile << "blez " << q->operand_3 << "," << q->operand_2 << endl;
	}
	if (strcmp(q->operand_1, (char*)"GEQ") == 0) // >=
	{
		mipsfile << "bltz " << q->operand_3 << "," << q->operand_2 << endl;
	}
	if (strcmp(q->operand_1, (char*)"EQL") == 0) // ==
	{
		mipsfile << "bne " << q->operand_3 << ",$0," << q->operand_2 << endl;
		cout << q->operand_2 << endl;
	}
	if (strcmp(q->operand_1, (char*)"NEQ") == 0) // !=
	{
		mipsfile << "beq " << q->operand_3 << ",$0," << q->operand_2 << endl;
	}
}

void Jal_mips(Midcode_ptr q) {
	mipsfile << "jal " << q->operand_1 << endl;
}

void Jr_mips(Midcode_ptr q) {
	mipsfile << "jr $ra" << endl;
}