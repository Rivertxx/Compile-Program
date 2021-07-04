#include "error.h"

void error(char type, int linenum)
{
	errorfile << linenum << " " << type << endl;
}

void error1()
{

}

