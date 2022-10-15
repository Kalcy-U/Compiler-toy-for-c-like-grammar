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

/****不经常改变的全局变量*****/
int poolsize= 64 * 1024;   //默认的缓冲区和栈大小
char* srccode;		//源代码字符串首地址
IdInfo* Id_table;	//标识符列表的首地址


/****函数声明****/
int load_file(const char* filename);
int lex_next();
int read_identifier();
int is_letter(char c);
int is_digit(char c);
int read_num(char* p);
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
/// 加载源程序文件到srccode指向的字符串
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
/// 从p开始读出一个int数字
/// </summary>
/// <returns>返回读出的值</returns>
int read_num(char* p) {

	return 0;
}


/// <summary>
/// 从token_start_pos开始读一个标识符或关键字。计算哈希值。
/// 如果是关键字，修改相应的token_type；
/// 如果不是关键字，则认为是标识符。搜索标识符表比对哈希值，查询标识是否已经出现过，如果没有出现过则加入标识符表。
/// 需要确保cur_ch最终指向语素尾部的下一个字符，保证下一个语素分析的起始位置正确
/// </summary>
/// <returns>(</returns>
int read_identifier() {
	return 0;
}


/// <summary>
/// 词法分析器 用于输出整个程序词法分析的结果->lexer.txt
/// </summary>
/// <returns></returns>
int lexer() {
	char buffer[20];
	int token_len;
	printf("token key				token value\n");
	while (lex_next() == LEXER_SUCCESS) {
		token_len = cur_ch - token_start_pos;
		memcpy(buffer, token_start_pos, cur_ch - token_start_pos);
		buffer[token_len] = 0;
		printf("%-40s%d\n", buffer, token_type);
	}
	return 0;
}

/// <summary>
/// 词法分析 函数调用一次分析一个词
/// </summary>
/// <returns>出错返回LEXER_ERROR，成功读取一条语素返回LEXER_SUCCESS，遇到文件尾返回LEXER_END
/// </returns>
int lex_next() {
	char ch = 0;					//当前字符
	int ret = 0;

	while (1) {
		ch = *cur_ch;
		token_start_pos = cur_ch;
		++cur_ch;//按理说应该放在循环末尾，但为了半成品能有输出姑且放这里
		if (ch == 0) {//EOF
			return LEXER_END;
		}
		else if (ch == '\n')
			++line;
		else if (ch == ' ' || ch == '\t' || ch == '\r')
			continue;
		else if (is_letter(ch)) { //以字母开头，可能是标识符或关键字
			//识别一个标识符或关键字,标识符需要维护标识符表
			read_identifier();
			//TODO:可能要处理返回值
			return LEXER_SUCCESS;
		}
		else if (is_digit(ch)) {
			//识别一个正整数
			token_value = read_num(token_start_pos);
			token_type = Num;
			return LEXER_SUCCESS;
		}
		//TODO:算符 + | - | * | / | = | == | > | >= | < | <= | != 

		else if (ch == ';' || ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == ',') {
			token_type = ch;//用字符的ASC本身标识token类型
			return LEXER_SUCCESS;
		}
		else
		{
			//return LEXER_ERROR;//遇到未知符号报错
		}

	}
	return 0;
}