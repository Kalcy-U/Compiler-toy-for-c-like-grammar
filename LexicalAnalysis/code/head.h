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
	//�������ţ���{}(),;��)Ϊ��һ���ţ��������ASCֵ��ʾ
};

/// <summary>
/// Identifier Info
/// ��ʶ����Ϣ�ṹ�壬��ɱ�ʶ���б�
/// </summary>
typedef struct  {
	int token;	//enum�е�ֵ�������Ͼ�ΪIdentifier
	int type;//��ʶ�����ͣ�Int�������ߺ�����
	int hash_id;//��ʶ����Ψһhashֵ
	char* name;//��ʶ�����ƣ�ָ��Դ�����ַ����б���������ʼλ��
	int namelen;//��ʶ��������
	int value; //����ֵ������ַ
}IdInfo;
