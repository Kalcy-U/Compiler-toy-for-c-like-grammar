#define None 0
#define isVt 1
#define isVn 2

#define Max_num 100
#define Max_prodlength 20

#define Max_Vsign 200
#define Max_Vn 52

#define Vt_Start 's'
#define Vt_end 'z'

#include<iostream>
#include<fstream>
#include<stack>
using namespace std;

/*
用于存放符号表，项目表等必要数据
*/
struct params
{
	/*
	标识符表：
		用于指明ascii值对应的符号 是终结符、非终结符亦或什么也不是。
		若为0，表示这不是一个符号；
		  为1(isVt)，表明是终结符；
		  为2(isVn)，表明是非终结符。
		//暂定A-z预留给非终结符使用。

	e.g.:	isV['A'] = 2，表明A是一个非终结符。
	*/
	int isV[Max_Vsign];
	int Vn_sum;
	int Vt_sum;

	/*
	文法产生式表：
		用于存放文法的产生式。

		e.g : G[5] = "A(B);"表示第六个产生式是"A->(B);"

	*/
	char G[Max_num][Max_prodlength];
	int prodsum;

	/*
	First表：
		用于存放First表。
		First[i]中值为True的符号在First('A'+i)中。

		注：终结符的First就是其本身，所以这里只构造非终结符的First表。
			其中0-25行存非终结符A-Z，
			26行之后存a以后的非终结符。
			所以进行查找或者输出时需要对这个分界线进行相应的处理。
	*/
	bool First[Max_Vn][Max_Vsign];


	/*
	项目：
		prod说明这个项目是基于第prod个产生式；
		dot说明下一个字符是第几个；
		next表明展望串(符)是哪一个。

		e.g. : prod = 0,dot = 0,next = '#'指的是项目"S'->.S , '#'"。
	*/
	struct project
	{
		int prod;
		int dot;

		char next;
	};

	/*
	项目集

	//想的还不是很清楚
	每一行存一个项目集？然后再开一个数组存每一个项目集的长度；
	再想想

	*/
	project closure[Max_num][Max_num];
	int closure_sum = 0;
	int closure_length[Max_num];

	/*
	Action/Goto表每一格的动作。

	kind指明移进、规约、acc还是错误；
	next以下标指明项目集族。

	*/
	struct elem
	{
		char kind;
		int next;
	};



	/*
	Action表
	*/
	elem Action[Max_num][Max_Vsign];

	/*
	Goto表
	*/
	elem Goto[Max_num][Max_Vsign];


	/*
	初始化：
	*/
	params();

	/*
	读取文法；
	完成标识符表IsV和产生式表G。
	*已完成
	*/
	void getGrammar(const char* grammarfilepath);

	/*
	打印文法。
	有空加个输出到文档
	*/
	void showGrammar();

	/*
	计算First表。
	*已完成
	*/
	void set_First();
	struct pair
	{
		char Vn;
		char Vt;
	};
	void insert(stack<pair>* mystack,pair target);


	/*
	打印First表。
	*/
	void showFirst();




};





/*
计算项目集族Closure。
*/
//还没想好

void get_closure(params* param);


/*
计算Action表和Goto表。
*/
void get_Action();


/* 其实不知道要不要把这些全部写成结构体里面的函数*/


/*
判断句子是否是该文法的语言
*/
void judge(params* param, const char* sentensepath);




params::params()
{
	for (int i = 0; i < Max_Vsign; i++)
		isV[i] = None;

	for (int i = 0; i < Max_num; i++)
		for (int j = 0; j < Max_Vsign; j++)
			G[i][j] = 0;

	return;
}


void params::getGrammar(const char* grammarfilepath)
{
	char line[20];
	
	Vn_sum = 0;
	Vt_sum = 0;

	ifstream grammar;
	grammar.open(grammarfilepath, ios::in);
	int prod_sum = 0;
	grammar >> prod_sum;
	grammar.get();
	for (int i = 0; i < prod_sum; i++)
	{
		grammar.getline(line, 100);

		if (isV[line[0]] == None)
		{
			isV[line[0]] = isVn;
			Vn_sum++;
		}
		G[i][0] = line[0];//产生式的左部

		int j = 3;
		while (line[j] != 0)//产生式的右部
		{
			char cur = line[j];

			if (isV[cur] == None)
			{
				if (cur >= 'A' && cur <= 'Z' || cur >= 'a' && cur < Vt_Start)
				{
					isV[cur] = isVn;
					Vn_sum++;
				}
				else
				{
					isV[cur] = isVt;
					Vt_sum++;
				}
			}

			G[i][j - 2] = cur;
			
			j++;
		}
		G[i][j - 2] = 0;

		//prodsum++;
	}

	prodsum = prod_sum;
	grammar.close();
	return;
};

void params::showGrammar()
{
	for (int i = 0; i < prodsum; i++)
	{
		int prodlen = strlen(G[i]);
		char* curprod = G[i];

		cout << curprod[0] << "->";
		for (int j = 1; j < prodlen; j++)
			cout << curprod[j];
		cout << endl;
	}

	return;
};

/*
* 我感觉参照的那个方法太麻烦了 四重循环了都 按照书上的写一写试一试
void params::set_First()
{
	bool done = false;

	while (!done)//第一层循环
	{
		int t;
		bool isepsilon;
		for (int i = 0; i < prodsum; i++)
		{
			char* cur = G[i];

			t = 1;
			isepsilon = true;

			while (t < strlen(G[i]) && isepsilon)
			{
				if (cur[t] >= 'A' && cur[t] < Vt_Start)
				{
					for (int k = 0; k < '@'; k++)
					{
						if (First[cur[t]][k] == true)
							First[cur[0]][k] = true;//右部符号是

					}


				}

			}


		}



	}


}
*/


void params::insert(stack<pair>* mystack, pair cur)
{
	int i;
	if ((cur.Vn - 'A') < 26)
		i = cur.Vn - 'A';
	else
		i = cur.Vn - 'a' + 26;
	if (!First[i][cur.Vt])
	{
		First[i][cur.Vt] = true;
		mystack->push(cur);
	}

	return;
}


void params::set_First()
{
	stack<pair> mystack;
	pair cur;

	for (int i = 0; i < Vn_sum; i++)
	{
		for (int j = 0; j < Max_Vsign; j++)
		{
			First[i][j] = false;
		}
	}

	for (int i = 0; i < prodsum; i++)
	{
		if (isV[G[i][1]] == isVt)
		{
			cur.Vn = G[i][0];
			cur.Vt = G[i][1];
			insert(&mystack, cur);
		}
		else if (isV[G[i][1]] == isVn && isV[G[i][2]] == isVt)
		{
			cur.Vn = G[i][0];
			cur.Vt = G[i][2];
			insert(&mystack, cur);
		}
	}

	while (!mystack.empty())
	{
		cur = mystack.top();
		mystack.pop();

		for (int i = 0; i < prodsum; i++)
		{
			if (G[i][1] == cur.Vn)
			{
				pair temp;
				temp.Vn = G[i][0];
				temp.Vt = cur.Vt;
				insert(&mystack, temp);
			}
		}
	}

	return;
}

void params::showFirst()
{
	cout << "    ";
	for (int j = 0; j < Max_Vsign; j++)
	{
		if (isV[j] == isVt)
			cout << char(j) << " ";
	}
	cout << endl;

	for (int i = 0; i < Vn_sum; i++)
	{
		if (i < 26)
			cout << char('A' + i) << " : ";
		else
			cout << char('a' + i - 26) << " : ";
		for (int j = 0; j < Max_Vsign; j++)
		{
			if (isV[j] == isVt)
			{
				cout << (int)First[i][j] << " ";
			}
		}

		cout << endl;
	}

	return;
}


int main()
{
	params param;
	param.getGrammar("H:\\编译原理\\大作业1\\Compiler-toy-for-c-like-grammar\\LexicalAnalysis\\testgrammar2.txt");
	param.showGrammar();

	cout << param.Vn_sum << "   " << param.Vt_sum << endl;

	param.set_First();
	param.showFirst();
	return 0;
}
