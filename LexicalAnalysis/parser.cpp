#define _CRT_SECURE_NO_WARNINGS
#define None 0
#define isVt 1
#define isVn 2

#define Max_num 2000
#define Max_prodlength 20

#define Max_Vsign 128
#define Max_Vn 40

#define Vn_Start 'A'
#define Vn_end 'f'


#define Vt_Start 'p'
#define Vt_end 'z'

#define myEpsilon '@'
#define myFinal '#'

#define Noduplicate -1

#include<iostream>
#include<fstream>
#include<stack>
#include<queue>
#include<sstream>
#include"tree.h"
#include<map>
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
	elem : Action/Goto表每一格的动作。

	kind指明移进、规约、acc还是错误；
	num以下标指明项目集族。
	*/
	struct elem
	{
		char kind;
		int num;
	};

	/*
	ActionGoto表
		在构建closure的时候初始化，申请空间，并同时完成构建。
	*/
	elem* ActionGoto[Max_num];

	/*
用于建树：
*/
	vector<node*> nodes;
	vector<bool> available;
	int nodesum = 0;

	node* root;
	FILE* dotp;

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
	void showGrammar(const char* fname = NULL);
	/*
	符号对照表及构建
	*/
	map<char, string> trans;
	void setTrans(const char* filepath);
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
	void showFirst(const char* fname = NULL);



	/*
	计算项目集和Action表
	*/
	void set_Closure_and_Action();


	/*
	打印项目集族
	*/
	void showClosures(const char* fname = NULL);



	/*
	构造项目集：
		根据传入的i=curclosure，用closure[i]和closure_eachlength[i]来构造项目集；
		结果放入当前的closure[i]中，并刷新closure_eachlength，(还没做)重新申请适合长度的空间。
	*/
	void setClosure(int curclosure_num);


	/*
		根据buffer和输入的buf_projcnt来构造一个项目，结果存回buffer中，返回新的buf_projcnt。
		*未使用
	*/
	project buffer[Max_num];
	int getclosure(int buf_projcnt);


	/*
	查重1：
	将项目集src和当前项目集族内所有项目集相比较；
		如果没有重复，返回-1；
		如果重复（！！或被包含），返回重复项目集的下标。
	*/
	int dupcheck(int src);


	/*
	打印ActionGoto表。	
	*/
	void showActionGoto(const char* fname = NULL);

	/*
	获得项目的向前搜索符。
	*/
	string get_firstBa(project src);


	/*
	判断句子是否是该文法的语言
	*/
	int judge(const char* sentensepath);

};


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

void params::setTrans(const char* filepath)
{
	ifstream infile;
	infile.open(filepath, ios::in || ios::binary);

	char c;
	string str;

	while (infile.good())
	{
		infile >> c;
		infile >> str;
		if (!infile.good())
			break;
		trans[c] = str;
	}

	infile.close();
	return;
}

bool params::First_trans(char Vn, char Vt)
{
	if (Vn >= 'A' && Vn <= 'Z')
		return First[(int)Vn - 'A'][Vt];

	else
		return First[(int)(Vn - 'a') + 26][Vt];

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

void params::showGrammar(const char* fname )
{
	std::stringstream  ss;
	fstream fout(fname, ios::out | ios::binary);
	for (int i = 0; i < prodsum; i++)
	{
		int prodlen = strlen(G[i]);
		char* curprod = G[i];

		ss << curprod[0] << "->";
		for (int j = 1; j < prodlen; j++)
			ss << curprod[j];
		ss << endl;
	}
	cout << ss.str();
	if (fname != NULL)
		fout << ss.str();
	fout.close();
	return;
};

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

void params::showFirst(const char* fname )
{
	std::stringstream  ss;
	fstream fout(fname, ios::out | ios::binary);
	ss << "    ";
	for (int j = 0; j < Max_Vsign; j++)
	{
		if (isV[j] == isVt)
			ss << char(j) << " ";
	}
	ss << endl;
	
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
			ss << char(k) << " : ";



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
				ss << (int)First[i][j] << " ";
			}
		}

		ss << endl;
	}
	cout << ss.str();
	if (fname != NULL)
		fout << ss.str();
	fout.close();
	return;
}



string params::get_firstBa(project src)
{
	string rst = "";

	if (G[src.prod][src.dot + 2] == 0)//.后面的第二个字符是空
		return src.next;
	else if (isV[G[src.prod][src.dot + 2]] == isVt)//.后面的第二个字符是终结符
	{
		rst += G[src.prod][src.dot + 2];
		return rst;
	}
	else
	{
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
	int* isfound = new int[closure_eachlength[src]];

	for (int i = 0; i < closure_sum; i++)
	{
		if (i == src)
			continue;
		for (int j = 0; j < closure_eachlength[src]; j++)
			isfound[j] = 0;
		int curproj = 0;
		project* curclosure = closure[i];

		bool cur_isdup = true;
		for (int k = 0; k < closure_eachlength[src]; k++)
		{
			for (int m = 0; m < closure_eachlength[i]; m++)
			{
				if (isfound[k] == true)
					continue;

				if (srcclosure[k].prod == curclosure[m].prod && srcclosure[k].dot == curclosure[m].dot && curclosure[m].next.find(srcclosure[k].next) != curclosure[m].next.npos)
				{
					isfound[k] = true;
				}
			}
		}

		for (int j = 0; j < closure_eachlength[src]; j++)
		{
			if (isfound[j] == false)
				cur_isdup = false;
		}

		if (cur_isdup == true)
		{
			delete[]isfound;
			return i;
		}
	}
	
	delete[]isfound;

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

					//要查重
					bool is_dup = false;
					for (int m = 0; m < projcnt; m++)
					{
						if (curclosure[m].prod == tempproj.prod && curclosure[m].dot == tempproj.dot)//如果重了，那么就把新的后继符号集添加进原来的里面，然后继续下一个项目。
						{
							is_dup = true;
							for (int n = 0; n < tempproj.next.length(); n++)
							{
								if (curclosure[m].next.find(tempproj.next[n]) == curclosure[m].next.npos)
									curclosure[m].next += tempproj.next[n];
							}
						}
					}
					if (is_dup == true)
						continue;

					curclosure[projcnt] = tempproj;
					projcnt++;

				}

			}
		}
	}

	closure_eachlength[curclosure_num] = projcnt;

	return;
}

/*已弃用*/
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

		//showClosures();
		
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


				for (int k = 0;k < closure_eachlength[curclosure_num]; k++)//如果还有.后面非空且和X相同的项目，也要加入新项目集中
				{
					if (k == j)
						continue;
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


void params::showClosures(const char* fname)
{

	std::stringstream  ss;
	fstream fout(fname, ios::out | ios::binary);
	for (int i = 0; i < closure_sum; i++)
	{
		ss << "I" << i << " : " << endl;

		for (int j = 0; j < closure_eachlength[i]; j++)
		{
			
			ss << G[closure[i][j].prod][0] << "->";

			int k = 0;
			for (; k < closure[i][j].dot; k++)
				ss << G[closure[i][j].prod][k + 1];

			ss << '.';

			while (G[closure[i][j].prod][k + 1] != 0)
			{
				ss << G[closure[i][j].prod][k + 1];
				k++;
			}


			ss << " , " << closure[i][j].next << " " << endl;
		}
	}
	cout << ss.str();
	if (fname != NULL)
		fout << ss.str();
	fout.close();
	return;
}


void params::showActionGoto(const char* fname)
{

	std::stringstream  ss;
	fstream fout(fname, ios::out | ios::binary);
	ss << "   ";

	for (int j = 0; j < Max_Vsign; j++)
	{
		if (j == myFinal || isV[j] != None)
			ss << "	" << (char)j;
	}
	ss <<"	"<< endl;

	for (int i = 0; i < closure_sum; i++)
	{
		ss << i << "	";
		for (int j = 0; j < Max_Vsign; j++)
		{
			if (j == myFinal || isV[j] != None)
				ss << ActionGoto[i][j].kind << ActionGoto[i][j].num<<'	' ;
		}

		ss << endl;
	}
	//cout << ss.str();
	if (fname != NULL)
		fout << ss.str();
	fout.close();
	return;
}



int params::judge(const char* sentensepath)
{


	stack<int> status;
	string SRstr = "";
	string instr;

	ifstream infile;
	infile.open(sentensepath, ios::in);
	infile >> instr;

	instr += myFinal;

	status.push(0);

	/*
	建树的准备
	vector<node*> nodes;
	vector<int> available;
	int nodesum;
	*/
	/*-------------------------*/
	dotp = fopen("try.dot", "w");
	fprintf(dotp, "digraph G\n{\nnode[fontname=\"FangSong\"]");
	for (int i = 0; i < instr.length() - 1; i++)
	{
		node* temp = new node();
		temp->c = instr[i];
		nodes.push_back(temp);
		available.push_back(true);
		nodesum++;
	}


	bool isdone = false;
	while (!isdone)
	{
		int cur = status.top();

		if (1)
		{
			cout << "移进归约串 : " << SRstr << "            " << "输入串 : " << instr << "         " << "状态 : ";
			stack<int> tempstack = status;
			for (int i = 0; i < status.size(); i++)
			{
				cout << tempstack.top() << ' ';
				tempstack.pop();
			}
			cout << endl;
		}

		if (ActionGoto[cur][instr[0]].kind == 'a')//接受状态
		{
			isdone = true;

			/*建树节点*/
			node* newnode = new node();
			newnode->c = G[0][0];
			newnode->parent = NULL;
			for (int i = 0; i < nodesum; i++)
				if (available[i] == true)
				{
					newnode->son.push_back(nodes[i]);
					nodes[i]->parent = newnode;

					fprintf(dotp, "%s%d->%s%d;\n", trans[G[0][0]].c_str(), nodesum, trans[nodes[i]->c].c_str(), i);
				}

			nodes.push_back(newnode);
			available.push_back(true);
			nodesum++;
			root = newnode;
			fprintf(dotp, "\n}\n");
			fclose(dotp);
			/*---------------*/
			return 1;
		}
		else if (ActionGoto[cur][instr[0]].kind == 's')//移进
		{
			status.push(ActionGoto[cur][instr[0]].num);
			SRstr += instr[0];
			instr.erase(instr.begin(), instr.begin() + 1);
		}
		else if (ActionGoto[cur][instr[0]].kind == 'r')//归约
		{

			int prodBlength = 0;//调用的产生式的右部长度
			int rprod = ActionGoto[cur][instr[0]].num;
			while (G[rprod][prodBlength + 1] != 0)
				prodBlength++;

			/*建树节点*/
			node* newnode = new node();
			for (int i = 0; i < prodBlength; i++)
			{
				bool isdone = false;
				for (int j = 0; j < nodesum; j++)
				{
					if (available[j] == false)
						continue;
					if (isdone)
						break;
					if (nodes[j]->c == G[rprod][i + 1])
					{
						available[j] = false;
						nodes[j]->parent = newnode;
						newnode->son.push_back(nodes[j]);
						isdone = true;
						fprintf(dotp, "%s%d->%s%d;\n", trans[G[rprod][0]].c_str(), nodesum, trans[nodes[j]->c].c_str(), j);
					}
				}
			}

			newnode->c = G[rprod][0];
			nodes.push_back(newnode);
			available.push_back(true);
			nodesum++;
			/*---------------*/


			for (int i = 0; i < prodBlength; i++)
				status.pop();

			int statusbefore = status.top();
			status.push(ActionGoto[statusbefore][G[rprod][0]].num);
			SRstr.erase(SRstr.end() - prodBlength, SRstr.end());
			SRstr += G[rprod][0];
		}
		else
			return -1;
	}

	infile.close();

	return -1;
}


int get_param(const char* fgramm,const char* fsentence)
{
	params param;
	param.getGrammar(fgramm);
	//param.getGrammar("H:\\编译原理\\大作业1\\Compiler-toy-for-c-like-grammar\\LexicalAnalysis\\testgrammar3.txt");
	param.setTrans("对照表new.txt");
	param.showGrammar("./output/grammar.txt");

	cout << param.Vn_sum << "   " << param.Vt_sum << endl;

	param.set_First();
	param.showFirst("./output/First.txt");

	param.set_Closure_and_Action();
	param.showClosures("./output/Closures.txt");

	param.showActionGoto("./output/ActionGo.txt");

	//cout << param.judge("H:\\编译原理\\大作业1\\Compiler-toy-for-c-like-grammar\\LexicalAnalysis\\testprogram.cpp");
	int pres = param.judge(fsentence);
	cout << ( pres== 1 ? "success\n" : "fail\n");
	
	if (pres) {
		system("cd bin && dot -Tpng ../try.dot -o ../output/tree.png\n");
		system("cd output && tree.png\n");
	}
	
	return 0;
}
