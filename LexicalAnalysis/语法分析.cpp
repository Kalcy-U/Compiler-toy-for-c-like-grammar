#define None 0
#define isVt 1
#define isVn 2

#define Max_num 300
#define Max_prodlength 20

#define Max_Vsign 128
#define Max_Vn 40

#define Vt_Start 's'
#define Vt_end 'z'

#define myEpsilon '@'
#define myFinal '#'

#define Noduplicate -1

#include<iostream>
#include<fstream>
#include<stack>
#include<queue>
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

	bool First_trans(char Vn, char Vt);


	/*
	项目：
		prod说明这个项目是基于第prod个产生式；
		dot说明下一个字符是第几个；
		next表明展望串(符)是哪一个/几个。

		e.g. : prod = 0,dot = 0,next = '#'指的是项目"S'->.S , '#'"。
			G[P.prod][P.dot+1]指向当前.的前面那一个。
	*/
	struct project
	{
		int prod = 0;
		int dot = 0;

		string next = "";
	};

	/*
	项目集
	*/
	project* closure[Max_num];
	int closure_sum = 0;
	int closure_eachlength[Max_num];



	/*
	Action/Goto表每一格的动作。

	kind指明移进、规约、acc还是错误；
	next以下标指明项目集族。

	*/
	struct elem
	{
		char kind;
		int num;
	};

	/*
	Action表
		在构建closure的时候初始化，申请空间，并一并构建。
	*/
	elem* ActionGoto[Max_num];



	/*
	Goto表
	*/
	//elem Goto[Max_num][Max_Vsign];



	friend bool operator==(const project& proj1, const project& proj2)
	{
		if (proj1.prod != proj2.prod)
			return false;
		if (proj1.dot != proj2.dot)
			return false;
		if (proj1.next != proj2.next)
			return false;
		
		return true;
	}

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



	/*
	计算项目集和Action表
	*/
	void set_Closure_and_Action();


	/*
	打印项目集族
	*/
	void showClosures();



	/*
	已变更：
	根据传入的i=curclosure，用closure[i]和closure_eachlength[i]来构造项目集；
	结果放入当前的closure[i]中，并刷新closure_eachlength，(可选)重新申请适合长度的空间。
	*/
	void setClosure(int curclosure_num);



	/*
		根据buffer和输入的buf_projcnt来构造一个项目，结果存回buffer中，返回新的buf_projcnt。
	*/
	project buffer[Max_num];
	int getclosure(int buf_projcnt);


	/*
	查重1：
	将项目集src和当前项目集族内所有项目集相比较；
		如果没有重复，返回-1；
		如果重复，返回重复项目集的下标。
	*/
	int dupcheck(int src);


	/*
	打印ActionGoto表。	
	*/
	void showActionGoto();

	/*
	获得向前搜索符。
	*/
	string get_firstBa(project src);


};




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

	for (int i = 0; i < Max_num; i++)
	{
		closure[i] = new project[Max_num];
		closure_eachlength[i] = 0;
	}

	return;
}

bool params::First_trans(char Vn, char Vt)
{
	if (Vn >= 'A' && Vn <= 'Z')
		return First[(int)Vn - 'A'][Vt];

	else
		return First[(int)Vn - 'a' + 26][Vt];

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

	for (int i = 0; i < Max_Vn; i++)
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
	
	for (int i = 0; i < Max_Vn; i++)
	{
		int k;
		if (i < 26)
			k = i + 'A';
		else
			k = i + 'a' - 26;

		if (isV[k] != isVn)
			continue;
		else
			cout << char(k) << " : ";



		/*
		if (i < 26)
			cout << char('A' + i) << " : ";
		else
			cout << char('a' + i - 26) << " : ";
		*/


		for (int j = 0; j < Max_Vsign; j++)
		{
			if (isV[j] == isVt)
			{
				cout << (int)First[i][j] << " ";
			}
		}

		cout << endl;
	}
	/*
	for (int i = 0; i < Max_Vn; i++)
	{
		cout << char('A' + i) << " : ";

		for (int j = 0; j < Max_Vsign; j++)
		{
			if (isV[j] == isVt)
			{
				cout << (int)First[i][j] << " ";
			}
		}
		cout << endl;
	}
	*/

	return;
}



string params::get_firstBa(project src)
{
	if (G[src.prod][src.dot + 2] == 0)//.后面的第二个字符是空
		return src.next;
	else
	{
		string rst;
		for (int j = 0; j < Max_Vsign; j++)
		{

			if (First_trans(G[src.prod][src.dot + 2], char(j)) == true)
				rst += char(j);
		}

		return rst;
	}
}


int params::dupcheck(int src)
{
	bool isdup = false;

	project* srcclosure = closure[src];

	for (int i = 0; i < closure_sum; i++)
	{
		if (i == src)
			continue;

		project* curclosure = closure[i];

		bool cur_isdup = true;
		for (int k = 0; k < closure_eachlength[src]; k++)
		{
			if (!(curclosure[k] == srcclosure[k]))
			{
				cur_isdup = false;
				break;
			}
		}

		if (cur_isdup == true)
			return i;
	}
	
	return Noduplicate;
}





void params::setClosure(int curclosure_num)
{
	project curproj;
	project tempproj;
	
	project* curclosure = closure[curclosure_num];
	int projcnt = closure_eachlength[curclosure_num];

	for (int i = 0; i < projcnt; i++)
	{
		curproj = curclosure[i];

		if (isV[G[curproj.prod][curproj.dot + 1]] == isVn)//如果当前项目.之后的那个符号是非终结符
		{
			for (int k = 0; k < prodsum; k++)//则加入所有以它为左部的项目
			{
				if (G[k][0] == G[curproj.prod][curproj.dot + 1])
				{
					tempproj.prod = k;
					tempproj.dot = 0;
					tempproj.next = get_firstBa(curproj);

					curclosure[projcnt] = tempproj;
					projcnt++;

				}

			}
		}
	}

	closure_eachlength[curclosure_num] = projcnt;

	return;
}


int params::getclosure(int buf_projcnt)
{
	project curproj;
	project tempproj;

	//project* curclosure = closure[curclosure_num];
	int projcnt = buf_projcnt;

	for (int i = 0; i < projcnt; i++)
	{
		curproj = buffer[i];

		if (isV[G[curproj.prod][curproj.dot + 1]] == isVn)//如果当前项目.之后的那个符号是非终结符
		{
			for (int k = 0; k < prodsum; k++)//则加入所有以它为左部的项目
			{
				if (G[k][0] == G[curproj.prod][curproj.dot + 1])
				{
					tempproj.prod = k;
					tempproj.dot = 0;
					tempproj.next = get_firstBa(curproj);

					buffer[projcnt] = tempproj;
					projcnt++;

				}

			}
		}
	}

	return projcnt;
}


/*
void params::set_Closure_and_Action()
{

	project* quebuf[Max_num];
	bool quebuf_used[Max_num];
	for (int i = 0; i < Max_num; i++)
		quebuf_used[i] = false;

	queue<project*> buf_que;
	queue<int> bufcnt_que;

	//首先要构造项目集0:
	buffer[0].prod = 0;
	buffer[0].dot = 0;
	buffer[0].next = myFinal;
	
	quebuf[0] = new project[1];
	quebuf[0][0] = buffer[0];
	buf_projcnt = 1;
	quebuf_used[0] = true;

	closure_sum = 0;
	/*

	int buf_projcnt = getClosure(buffer, 1);

	closure[0] = new project[buf_projcnt]();
	for (int j = 0; j < buf_projcnt; j++)
		closure[0][j] = buffer[j];

	closure_length[0] = buf_projcnt;
	closure_sum++;
	delete[] buffer;
	*/
/*
	buf_que.push(quebuf[0]);
	bufcnt_que.push(1);

	while (!buf_que.empty())
	{
		project* temp = buf_que.front();
		buf_projcnt = bufcnt_que.front();
		buf_que.pop();
		bufcnt_que.pop();

		for (int i = 0; i < buf_projcnt; i++)
			buffer[i] = temp[i];

		buf_projcnt = getClosure();
		closure[closure_sum] = new project[buf_projcnt]();
		for (int j = 0; j < buf_projcnt; j++)
			closure[closure_sum][j] = buffer[j];
		closure_length[closure_sum] = buf_projcnt;
		closure_sum++;

		if (G[buffer[0].prod][buffer[0].dot + 1] == 0)//如果这个项目集已经是终结状态
		{
			delete[] temp;
			quebuf_used[];;////' gfvvvvvvvvvvvvvvvvvvvgbgvvvg

			continue;
		}


		int next_projcnt = 0;
		bool* isused = new bool[buf_projcnt]();
		for (int i = 0; i < buf_projcnt; i++)
			isused[i] = false;
		int temp_sel[Max_num];

		for (int j = 0; j < buf_projcnt; j++)
		{
			if (isused[j] == true)
				continue;

			char X = G[buffer[j].prod][buffer[j].dot + 1];
			next_projcnt = 1;
			temp_sel[0] = j;
			isused[j] = true;

			for (int k = 0; k < buf_projcnt && isused[k] == false; k++)//使用某个字符后，如果有其他的项目能够使用该字符而让.往前读一个，那么它也要用来产生新闭包。
			{
				char tmp = G[buffer[k].prod][buffer[k].dot + 1];
				if (tmp == X)
				{
					temp_sel[next_projcnt] = k;
					next_projcnt++;
					isused[k] = true;
				}
			}

			project* next = new project[next_projcnt];

			for (int i = 0; i < next_projcnt; i++)
			{
				project temp;
				temp.prod = buffer[temp_sel[i]].prod;
				temp.dot = buffer[temp_sel[i]].dot + 1;
				temp.next = buffer[temp_sel[i]].next;

				next[i] = temp;
			}
			
			buf_que.push(next);
			bufcnt_que.push(next_projcnt);

		}
	
		break;
	}





	return;
}
*/



void params::set_Closure_and_Action()
{
	//为ActionGoto表申请空间并初始化
	for (int i = 0; i < Max_num; i++)
	{
		ActionGoto[i] = new elem[Max_Vsign];
		for (int j = 0; j < Max_Vsign; j++)
		{
			ActionGoto[i][j].kind = ' ';
			ActionGoto[i][j].num = 0;
		}
	}
	ActionGoto[1][myFinal].kind = 'a';

	closure[0]->prod = 0;
	closure[0]->dot = 0;
	closure[0]->next = myFinal;

	closure_eachlength[0] = 1;



	/*
	一个队列；队首数字代表当前正在处理的closure下标。
	*/
	queue<int> que;
	que.push(0);
	closure_sum++;

	while (!que.empty())
	{
		int curclosure_num = que.front();
		que.pop();

		setClosure(curclosure_num);

		/*
		以下为根据当前closure构建新的closure基底，并同时填ActionGoto表。
		*/
		project* curclosure = closure[curclosure_num];
		for (int j = 0; j < closure_eachlength[curclosure_num]; j++)
		{
			char X = G[curclosure[j].prod][curclosure[j].dot + 1];
			if (X != 0)//如果.后面非空
			{
				int nextclosure = closure_sum;
				closure[nextclosure][0] = curclosure[j];
				closure[nextclosure][0].dot++;
				closure_eachlength[nextclosure]++;


				for (int k = 0; k != j && k < closure_eachlength[curclosure_num]; k++)//如果还有.后面非空且和X相同的项目，也要加入新项目集中
				{
					char X2 = G[curclosure[k].prod][curclosure[k].dot + 1];
					if (X2 == X)
					{
						closure[nextclosure][closure_eachlength[nextclosure]] = curclosure[k];
						closure[nextclosure][closure_eachlength[nextclosure]].dot++;
						closure_eachlength[nextclosure]++;
					}
				}


				int dupnum = dupcheck(nextclosure);
				if (dupnum == Noduplicate)
				{
					que.push(nextclosure);
					//此处构造ActionGoto表
					if (isV[X] == isVt)
					{
						ActionGoto[curclosure_num][X].kind = 's';
						ActionGoto[curclosure_num][X].num = nextclosure;
					}
					else if (isV[X] == isVn)
					{
						//这里是构建goto
						ActionGoto[curclosure_num][X].kind = 'G';
						ActionGoto[curclosure_num][X].num = nextclosure;
					}

					closure_sum++;//这个时候才确定产生了一个新的项目集
				}
				else
				{
					//cout << dupnum << endl;
					
					//此处构造ActionGoto表
					if (isV[X] == isVt)
					{
						ActionGoto[curclosure_num][X].kind = 's';
						ActionGoto[curclosure_num][X].num = dupnum;
					}
					else if (isV[X] == isVn)
					{
						//这里是构建goto
						ActionGoto[curclosure_num][X].kind = 'G';
						ActionGoto[curclosure_num][X].num = dupnum;
					}

					closure_eachlength[nextclosure] = 0;

				}


			}
			else
			{
				//如果X为空，那么就要构建Action表中的“归约”项rj
				for (int k = 0; k < curclosure[j].next.length(); k++)
				{
					ActionGoto[curclosure_num][curclosure[j].next[k]].kind = 'r';

					if (curclosure[j].prod == 0)
					{
						ActionGoto[curclosure_num][curclosure[j].next[k]].kind = 'a';
						ActionGoto[curclosure_num][curclosure[j].next[k]].num = curclosure[j].prod;
					}

					ActionGoto[curclosure_num][curclosure[j].next[k]].num = curclosure[j].prod;
				}
			}
		}

	}

	return;
}









void params::showClosures()
{
	for (int i = 0; i < closure_sum; i++)
	{
		cout << "I" << i << " : " << endl;

		for (int j = 0; j < closure_eachlength[i]; j++)
		{
			
			cout << G[closure[i][j].prod][0] << "->";

			int k = 0;
			for (; k < closure[i][j].dot; k++)
				cout << G[closure[i][j].prod][k + 1];

			cout << '.';

			while (G[closure[i][j].prod][k + 1] != 0)
			{
				cout << G[closure[i][j].prod][k + 1];
				k++;
			}


			cout << " , " << closure[i][j].next << " " << endl;
		}
	}

	return;
}


void params::showActionGoto()
{
	cout << "   ";

	for (int j = 0; j < Max_Vsign; j++)
	{
		if (j == myFinal || isV[j] != None)
			cout << "  " << (char)j << "  ";
	}
	cout << endl;

	for (int i = 0; i < closure_sum; i++)
	{
		cout <<' '<< i << " ";
		for (int j = 0; j < Max_Vsign; j++)
		{
			if (j == myFinal || isV[j] != None)
				cout <<' '<< ActionGoto[i][j].kind << ActionGoto[i][j].num << "  ";
		}

		cout << endl;
	}

	return;
}




int main()
{
	params param;
	param.getGrammar("H:\\编译原理\\大作业1\\Compiler-toy-for-c-like-grammar\\LexicalAnalysis\\testgrammar3.txt");
	param.showGrammar();

	cout << param.Vn_sum << "   " << param.Vt_sum << endl;

	param.set_First();
	param.showFirst();

	param.set_Closure_and_Action();
	param.showClosures();

	param.showActionGoto();


	return 0;
}
