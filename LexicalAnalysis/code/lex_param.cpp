#include<iostream>
#include<fstream>
#include<stack>
#include<queue>
#include<sstream>
#include"head.h"
using namespace std;

int get_lex(const char* filename);
int get_param(const char* fgramm, const char* fsentence);

void usage(const char* programname) {
	printf("to compile a c-like program:\n");
	printf("%s program.c\n", programname);
	return;
}
int main(int argc, char** argv) {

	if (argc != 2) {
		usage(argv[0]);
		return START_FAIL;
	}
	if(get_lex(argv[1]) == LOG_SUCCESS)
		get_param(".\\testgrammarC.txt", "syntaxin.txt");

	return 0;
}