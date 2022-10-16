#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include"head.h"

//global
/**词法-语法分析中需要不断更改的量**/
int line = 0;			//行计数器
char* cur_ch=NULL;		//当前字符指针
char* token_start_pos = NULL;	//当前token的首字符指针
int token_type = 0;			//当前token的类型
int token_value = 0;		//当前token的值，Num类型存数值
int Id_count=0;			//标识符列表Id_table中ID的个数


/****不经常改变的全局变量*****/
size_t poolsize= 64 * 1024;   //默认的缓冲区和栈大小
char* srccode=NULL;		//源代码字符串首地址
IdInfo* Id_table=NULL;	//标识符列表的首地址
int Id_table_len=128;	//当前标识符列表长度

/****函数声明****/
int load_file(const char* filename);
int lex_next();
int read_identifier();
int is_letter(char c);
int is_digit(char c);
int read_num();
void usage(const char* programname);
int lexer(const char* filename);
int log_lexinfo(FILE* fp,const int lexstatus,const char* filename);
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

	if (!(Id_table = (IdInfo*)malloc(Id_table_len*sizeof(IdInfo)))) {
		printf("malloc failed.\n");
		return START_FAIL;
	}

	/****lexical analysis****/
	lexer(argv[1]);
	/****free memory****/
	
	if(srccode)
		free(srccode);
	if (Id_table)
		free(Id_table);
	return 0;
}
/// <summary>
/// 加载源程序文件到srccode指向的字符串
/// </summary>
/// <param name="filename"></param>
/// <returns></returns>
int load_file(const char*filename) {
	size_t srccode_size;
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
	if ((fread(srccode,sizeof(char), srccode_size, fp)) != srccode_size) {
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
/// 返回字符c是否是字母（大小写任意）
/// </summary>
/// <param name="c"></param>
/// <returns>非零为真零为假</returns>
int is_letter(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return 1;
	else
		return 0;
}

/// <summary>
/// 返回字符c是否是数字
/// </summary>
/// <param name="c"></param>
/// <returns>非零为真零为假</returns>
int is_digit(char c)
{
	if (c >= '0' && c <= '9')
		return 1;
	else
		return 0;
}

/// <summary>
/// 从token_start_pos开始读出一个int数字
/// 可以读取 十进制、0x下十六进制、0下八进制
/// </summary>
/// <returns>返回读取的值</returns>
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
/// 从token_start_pos开始读一个标识符或关键字。计算哈希值。
/// 如果是关键字，修改相应的token_type；
/// 如果不是关键字，则认为是标识符。搜索标识符表比对哈希值，查询标识是否已经出现过，如果没有出现过则加入标识符表。
/// 需要确保cur_ch最终指向语素尾部的下一个字符，保证下一个语素分析的起始位置正确
/// </summary>
/// <returns>目前关键字返回标记值，标识符返回索引值的相反数</returns>
int read_identifier() {
	cur_ch = token_start_pos+1;
	while ((*cur_ch >= 'a' && *cur_ch <= 'z') || (*cur_ch >= 'A' && *cur_ch <= 'Z') || (*cur_ch >= '0' && *cur_ch <= '9'))
		cur_ch++;
	int len = (int)(cur_ch - token_start_pos);
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
	for (; index < Id_count; index++) {
		if (Id_table[index].hash_id == hash)
			return -index;
	}
	if (Id_count >= Id_table_len) {
		//扩容Id_table
		IdInfo* tempp=NULL;
		tempp=realloc(Id_table, sizeof(IdInfo) * 2 * Id_table_len);
		if (tempp == NULL) {
			printf("malloc failed.\n");
			return LEXER_ERROR;
		}
		else{
			Id_table_len = Id_table_len *2;
			Id_table = tempp;
		}
	}
	Id_table[Id_count].hash_id = hash;
	Id_table[Id_count].name = token_start_pos;
	Id_table[Id_count].namelen = len;
	Id_table[Id_count].token = Identifier;
	++Id_count;
	return Identifier;
}


/// <summary>
/// 词法分析器 用于输出整个程序词法分析的结果->lexer.txt
/// </summary>
/// <returns></returns>
int lexer(const char* filename) {
	int lexer_ret=0;
	FILE* fp;
	fp = fopen("lexicalAnalysis.log", "wb");
	if (fp == NULL) {
		printf("Log File Create Failed");
		return START_FAIL;
	}
	log_lexinfo(fp, LEXER_START, filename);
	while ((lexer_ret=lex_next()) == LEXER_SUCCESS)
		log_lexinfo(fp, LEXER_SUCCESS, NULL);
	if (lexer_ret == LEXER_ERROR)
		log_lexinfo(fp, LEXER_ERROR, NULL);
	log_lexinfo(fp, LEXER_END, NULL);
	fclose(fp);
	printf("词法分析已结束，结果请查看日志文件lexicalAnalysis.log");
	return LEXER_END;
}

/// <summary>
/// 词法分析 函数调用一次分析一个词
/// </summary>
/// <returns>出错返回LEXER_ERROR，成功读取一条语素返回LEXER_SUCCESS，遇到文件尾返回LEXER_END
/// </returns>
int lex_next() {
	char ch = 0;					//当前字符
	int temp = 0;
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
		else if (is_letter(ch)) { //以字母开头，可能是标识符或关键字
			//识别一个标识符或关键字,标识符需要维护标识符表
			read_identifier();
			//TODO:可能要处理返回值
			return LEXER_SUCCESS;
		}
		else if (is_digit(ch)) {
			//识别一个正整数
			token_value = read_num();
			token_type = Num;
			return LEXER_SUCCESS;
		}
		//TODO:注释
		else if (ch == '/') {
			++cur_ch;
			if (*cur_ch == '/') {
				while (*(cur_ch) != '\n') {
					if (*cur_ch == 0)
						return LEXER_END;
					++cur_ch;
				}
				continue;
			}
			else if (*cur_ch == '*') {
				cur_ch++;
				while (1) {
					if (*cur_ch == 0)
						return LEXER_END;
					if (*cur_ch == '*' && *(cur_ch+1) == '/') {
						cur_ch += 2;
						break;
					}
					++cur_ch;
				}
				continue;
			}
			else
			{
				token_type = Div;
				cur_ch++;
				return LEXER_SUCCESS;
			}
		}
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
			token_type = ch;//用字符的ASC本身标识token类型
			cur_ch++;
			return LEXER_SUCCESS;
		}
		else
		{
			return LEXER_ERROR;//遇到未知符号报错
		}
		cur_ch++;
	}
	return 0;
}
/// <summary>
/// 输出单步词法分析结果到fp指向文件中
/// </summary>
/// <returns>
/// </returns>
int log_lexinfo(FILE* fp,const int lexstatus,const char* filename) {
	int token_len;
	if (lexstatus == LEXER_START) {
		fprintf(fp, "###LEXER_STARTINFO###\n");
		fprintf(fp, "Compiled File: %s\n",filename);
		//可以再加入一些日志信息
		fprintf(fp, "###LEXER_RUNINFO###\n");
		fprintf(fp, "Token_Name				Token_Type      Type_Code\n");
	}
	else if (lexstatus == LEXER_SUCCESS) {
		token_len = (int)(cur_ch - token_start_pos);
		char* Token_Name = (char*)malloc(token_len + 1);
		if (Token_Name == NULL)
			return LOG_ERROR;
		Token_Name[token_len] = 0;
		memcpy(Token_Name, token_start_pos, token_len);
		char* Token_Type = (char*)malloc(14);
		if (Token_Type == NULL)
			return LOG_ERROR;
		Token_Type[13] = 0;
		if (token_type == Int || Token_Type == Void || Token_Type == If || Token_Type == Else || Token_Type == While || Token_Type == Return)
			memcpy(Token_Type, "KEYWORD", 8);
		else if(token_type==Identifier)
			memcpy(Token_Type, "IDENTIFIER", 11);
		else if(token_type==Num)
			memcpy(Token_Type, "NUM", 4);
		else if(token_type==Assign)
			memcpy(Token_Type, "ASSIGN", 7);
		else if(token_type>=Add&&Token_Type<=NEq)
			memcpy(Token_Type, "OPERATOR", 9);
		else if(token_type==';')
			memcpy(Token_Type, "DELIMITER", 10);
		else if (token_type == ',')
			memcpy(Token_Type, "SEPARATOR", 10);
		else if (token_type == '(')
			memcpy(Token_Type, "LEFT_BRACKET", 13);
		else if (token_type == ')')
			memcpy(Token_Type, "RIGHT_BRACKET", 14);
		else if (token_type == '{')
			memcpy(Token_Type, "LEFT_BRACE", 11);
		else if (token_type == '}')
			memcpy(Token_Type, "RIGHT_BRACE", 12);
		fprintf("%-40s%s%d\n", Token_Name, Token_Type, token_type);
		free(Token_Name);
		free(Token_Type);
	}
	else if (lexstatus == LEXER_ERROR){
		fprintf("ERROR: Unexpected sign %c in line %d.\n",*cur_ch,line);
	}
	else if (lexstatus == LEXER_END) {
		if(*cur_ch==0)
			fprintf(fp,"###Lexical Analysis Succeeded.###");
		else
			fprintf(fp, "###Lexical Analysis End with Error.###");
	}
	return LOG_SUCCESS;
}