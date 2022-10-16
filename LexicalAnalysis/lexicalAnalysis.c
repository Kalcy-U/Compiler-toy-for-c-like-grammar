#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include"head.h"

//global
/**�ʷ�-�﷨��������Ҫ���ϸ��ĵ���**/
int line = 0;			//�м�����
char* cur_ch=NULL;		//��ǰ�ַ�ָ��
char* token_start_pos = NULL;	//��ǰtoken�����ַ�ָ��
int token_type = 0;			//��ǰtoken������
int token_value = 0;		//��ǰtoken��ֵ��Num���ʹ���ֵ

/****�������ı��ȫ�ֱ���*****/
int poolsize= 64 * 1024;   //Ĭ�ϵĻ�������ջ��С
char* srccode;		//Դ�����ַ����׵�ַ
IdInfo* Id_table;	//��ʶ���б���׵�ַ


/****��������****/
int load_file(const char* filename);
int lex_next();
int read_identifier();
int is_letter(char c);
int is_digit(char c);
int read_num();
void usage(const char* programname);
int lexer();

void usage(const char* programname) {
	printf("to compile a c-like program:\n");
	printf("%s program.c\n", programname);
	return;
}
int main(int argc,char** argv) {
	
	if(argc!=2) {
		usage(argv[0]);
		return START_FAIL;
	}
	/****open source code and allocate memory******/
	if (load_file(argv[1]) == START_FAIL)
		return START_FAIL;

	if (!(Id_table = (IdInfo*)malloc(256*sizeof(IdInfo)))) {
		printf("malloc failed.\n");
		return START_FAIL;
	}

	/****lexical analysis****/
	lexer();
	/****free memory****/
	if(srccode)
		free(srccode);
	return 0;
}
/// <summary>
/// ����Դ�����ļ���srccodeָ����ַ���
/// </summary>
/// <param name="filename"></param>
/// <returns></returns>
int load_file(const char*filename) {
	int srccode_size;
	FILE* fp=NULL;
	if ((fp = fopen(filename, "rb")) == NULL) {
		printf("failed to open %s.\n", filename);
		return START_FAIL;
	}
	fseek(fp, 0, SEEK_END);
	srccode_size=ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (!(srccode = (char*)malloc(srccode_size+1))) {
		printf("malloc failed.\n");
		return START_FAIL;
	}
	if ((fread(srccode,1, srccode_size,fp)) !=srccode_size) {
		printf("read failed(%d)\n", srccode_size);
		return START_FAIL;
	}
	srccode[srccode_size] = 0;
	printf("%s\n", srccode);
	cur_ch = srccode;
	fclose(fp);
	return 0;
}


/// <summary>
/// �����ַ�c�Ƿ�����ĸ����Сд���⣩
/// </summary>
/// <param name="c"></param>
/// <returns>����Ϊ����Ϊ��</returns>
int is_letter(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return 1;
	else
		return 0;
}

/// <summary>
/// �����ַ�c�Ƿ�������
/// </summary>
/// <param name="c"></param>
/// <returns>����Ϊ����Ϊ��</returns>
int is_digit(char c)
{
	if (c >= '0' && c <= '9')
		return 1;
	else
		return 0;
}

/// <summary>
/// ��token_start_pos��ʼ����һ��int����
/// ���Զ�ȡ ʮ���ơ�0x��ʮ�����ơ�0�°˽���
/// </summary>
/// <returns>���ض�ȡ��ֵ</returns>
int read_num() {
	int result = *token_start_pos - '0';
	cur_ch =token_start_pos + 1;
	if (result > 0) {
		while (*cur_ch >= '0' && *cur_ch <= '9')
			result = result * 10 + *cur_ch++ - '0';
	}
	else if (*cur_ch == 'x' || *cur_ch == 'X') {
		char c = *++cur_ch;
		while ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
			result = result * 16 + (c & 15) + (c >= 'A' ? 9 : 0);
			c = *++cur_ch;
		}
	}
	else {
		while (*cur_ch >= '0' && *cur_ch <= '7')
			result = result * 8 + *cur_ch++ - '0';
	}
	return result;
}


/// <summary>
/// ��token_start_pos��ʼ��һ����ʶ����ؼ��֡������ϣֵ��
/// ����ǹؼ��֣��޸���Ӧ��token_type��
/// ������ǹؼ��֣�����Ϊ�Ǳ�ʶ����������ʶ����ȶԹ�ϣֵ����ѯ��ʶ�Ƿ��Ѿ����ֹ������û�г��ֹ�������ʶ����
/// ��Ҫȷ��cur_ch����ָ������β������һ���ַ�����֤��һ�����ط�������ʼλ����ȷ
/// </summary>
/// <returns>Ŀǰ�ؼ��ַ��ر��ֵ����ʶ����������ֵ���෴��</returns>
int read_identifier() {
	cur_ch = token_start_pos+1;
	while ((*cur_ch >= 'a' && *cur_ch <= 'z') || (*cur_ch >= 'A' && *cur_ch <= 'Z') || (*cur_ch >= '0' && *cur_ch <= '9'))
		cur_ch++;
	int len = cur_ch - token_start_pos;
	if (len == 3 && strncmp(token_start_pos, "int", 3) == 0) {
		token_type = Int;
		return Int;
	}
	else if (len == 4 && strncmp(token_start_pos, "void", 4) == 0) {
		token_type = Void;
		return Void;
	}
	else if (len == 2 && strncmp(token_start_pos, "if", 2) == 0) {
		token_type = If;
		return If;
	}
	else if (len == 4 && strncmp(token_start_pos, "else", 4) == 0) {
		token_type = Else;
		return Else;
	}
	else if (len == 5 && strncmp(token_start_pos, "While", 5) == 0) {
		token_type = While;
		return While;
	}
	else if (len == 6 && strncmp(token_start_pos, "return", 6) == 0) {
		token_type = Return;
		return Return;
	}
	token_type = Identifier;
	char* p = token_start_pos;
	int hash = 0;
	while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9'))
		hash = hash * 147 + *p++;
	int index = 0;
	for (; index < 256; index++) {
		if (Id_table[index].hash_id == hash)
			return -index;
	}
	//��δ���ǳ���������
	Id_table[index].hash_id = hash;
	Id_table[index].name = token_start_pos;
	Id_table[index].namelen = len;
	Id_table[index].token = Identifier;
	return -index;
}


/// <summary>
/// �ʷ������� ���������������ʷ������Ľ��->lexer.txt
/// </summary>
/// <returns></returns>
int lexer() {
	char buffer[20];
	int token_len;
	printf("token key				token type\n");
	while (lex_next() == LEXER_SUCCESS) {
		token_len = cur_ch - token_start_pos;
		memcpy(buffer, token_start_pos, cur_ch - token_start_pos);
		buffer[token_len] = 0;
		printf("%-40s%d\n", buffer, token_type);
	}
	return 0;
}

/// <summary>
/// �ʷ����� ��������һ�η���һ����
/// </summary>
/// <returns>������LEXER_ERROR���ɹ���ȡһ�����ط���LEXER_SUCCESS�������ļ�β����LEXER_END
/// </returns>
int lex_next() {
	char ch = 0;					//��ǰ�ַ�
	//int ret = 0;
	while (1) {
		ch = *cur_ch;
		token_start_pos = cur_ch;
		if (ch == 0) {//EOF
			return LEXER_END;
		}
		else if (ch == '\n')
			++line;
		else if (ch == ' ' || ch == '\t' || ch == '\r')
			;
		else if (is_letter(ch)) { //����ĸ��ͷ�������Ǳ�ʶ����ؼ���
			//ʶ��һ����ʶ����ؼ���,��ʶ����Ҫά����ʶ����
			read_identifier();
			//TODO:����Ҫ������ֵ
			return LEXER_SUCCESS;
		}
		else if (is_digit(ch)) {
			//ʶ��һ��������
			token_value = read_num();
			token_type = Num;
			return LEXER_SUCCESS;
		}
		//TODO:��� + | - | * | / | = | == | > | >= | < | <= | != 
		else if (*(cur_ch + 1) == '=' && ch == '=') {
			token_type = Equal;
			cur_ch+=2;
			return LEXER_SUCCESS;
		}
		else if (*(cur_ch + 1) == '!' && ch == '=') {
			token_type = NEq;
			cur_ch += 2;
			return LEXER_SUCCESS;
		}
		else if (*(cur_ch + 1) == '>' && ch == '=') {
			token_type = GEq;
			cur_ch += 2;
			return LEXER_SUCCESS;
		}
		else if (*(cur_ch + 1) == '<' && ch == '=') {
			token_type = LEq;
			cur_ch += 2;
			return LEXER_SUCCESS;
		}
		else if (ch == '+') {
			token_type = Add;
			cur_ch++;
			return LEXER_SUCCESS;
		}
		else if (ch == '-') {
			token_type = Sub;
			cur_ch++;
			return LEXER_SUCCESS;
		}
		else if (ch == '*') {
			token_type = Mul;
			cur_ch++;
			return LEXER_SUCCESS;
		}
		else if (ch == '/') {
			token_type = Div;
			cur_ch++;
			return LEXER_SUCCESS;
		}
		else if (ch == '=') {
			token_type = Assign;
			cur_ch++;
			return LEXER_SUCCESS;
		}
		else if (ch == '>') {
			token_type = Greater;
			cur_ch++;
			return LEXER_SUCCESS;
		}
		else if (ch == '<') {
			token_type = Less;
			cur_ch++;
			return LEXER_SUCCESS;
		}
		else if (ch == ';' || ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == ',') {
			token_type = ch;//���ַ���ASC�����ʶtoken����
			cur_ch++;
			return LEXER_SUCCESS;
		}
		else
		{
			return LEXER_ERROR;//����δ֪���ű���
		}
		cur_ch++;
	}
	return 0;
}
