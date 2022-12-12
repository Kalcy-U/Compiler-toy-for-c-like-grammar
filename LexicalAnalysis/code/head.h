#pragma once

//define
#define START_FAIL -1
#define LEXER_ERROR -2
#define PARZER_ERROR -3

#define LEXER_SUCCESS 0
#define LEXER_END 1
#define LEXER_START 2

#define LOG_ERROR -1
#define LOG_SUCCESS 0
enum TType{
	Num = 128, Identifier,
	Int, Void, If, Else, While, Return,
	Assign, Add, Sub, Mul, Div, Equal, Greater, GEq, Less, LEq, NEq 
	//=| +| -| *| /  | == | > | >= | < | <= | !=
	//其他符号（如{}(),;等)为单一符号，用自身的ASC值表示
};

/// <summary>
/// Identifier Info
/// 标识符信息结构体，组成标识符列表
/// </summary>
typedef struct  {
	int token;	//enum中的值，理论上均为Identifier
	int type;//标识符类型，Int变量或者函数名
	int hash_id;//标识符的唯一hash值
	char* name;//标识符名称，指向源程序字符串中变量名的起始位置
	int namelen;//标识符名长度
	int value; //变量值或函数地址
}IdInfo;
