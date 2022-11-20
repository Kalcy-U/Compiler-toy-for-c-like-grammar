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
���ڴ�ŷ��ű���Ŀ��ȱ�Ҫ����
*/
struct params
{
	/*
	��ʶ����
		����ָ��asciiֵ��Ӧ�ķ��� ���ս�������ս�����ʲôҲ���ǡ�
		��Ϊ0����ʾ�ⲻ��һ�����ţ�
		  Ϊ1(isVt)���������ս����
		  Ϊ2(isVn)�������Ƿ��ս����
		//�ݶ�A-zԤ�������ս��ʹ�á�

	e.g.:	isV['A'] = 2������A��һ�����ս����
	*/
	int isV[Max_Vsign];
	int Vn_sum;
	int Vt_sum;

	/*
	�ķ�����ʽ��
		���ڴ���ķ��Ĳ���ʽ��

		e.g : G[5] = "A(B);"��ʾ����������ʽ��"A->(B);"

	*/
	char G[Max_num][Max_prodlength];
	int prodsum;

	/*
	First��
		���ڴ��First��
		First[i]��ֵΪTrue�ķ�����First('A'+i)�С�

		ע���ս����First�����䱾����������ֻ������ս����First��
			����0-25�д���ս��A-Z��
			26��֮���a�Ժ�ķ��ս����
			���Խ��в��һ������ʱ��Ҫ������ֽ��߽�����Ӧ�Ĵ���
	*/
	bool First[Max_Vn][Max_Vsign];

	bool First_trans(char Vn, char Vt);


	/*
	��Ŀ��
		prod˵�������Ŀ�ǻ��ڵ�prod������ʽ��
		dot˵����һ���ַ��ǵڼ�����
		next����չ����(��)����һ��/������

		e.g. : prod = 0,dot = 0,next = '#'ָ������Ŀ"S'->.S , '#'"��
			G[P.prod][P.dot+1]ָ��ǰ.��ǰ����һ����
	*/
	struct project
	{
		int prod = 0;
		int dot = 0;

		string next = "";
	};

	/*
	��Ŀ��
	*/
	project* closure[Max_num];
	int closure_sum = 0;
	int closure_eachlength[Max_num];


	/*
	elem : Action/Goto��ÿһ��Ķ�����

	kindָ���ƽ�����Լ��acc���Ǵ���
	num���±�ָ����Ŀ���塣
	*/
	struct elem
	{
		char kind;
		int num;
	};

	/*
	ActionGoto��
		�ڹ���closure��ʱ���ʼ��������ռ䣬��ͬʱ��ɹ�����
	*/
	elem* ActionGoto[Max_num];

	/*
���ڽ�����
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
	��ʼ����
	*/
	params();

	/*
	��ȡ�ķ���
	��ɱ�ʶ����IsV�Ͳ���ʽ��G��
	*�����
	*/
	void getGrammar(const char* grammarfilepath);

	/*
	��ӡ�ķ���
	�пռӸ�������ĵ�
	*/
	void showGrammar(const char* fname = NULL);
	/*
	���Ŷ��ձ�����
	*/
	map<char, string> trans;
	void setTrans(const char* filepath);
	/*
	����First��
	*�����
	*/
	void set_First();
	struct pair
	{
		char Vn;
		char Vt;
	};
	void insert(stack<pair>* mystack,pair target);


	/*
	��ӡFirst��
	*/
	void showFirst(const char* fname = NULL);



	/*
	������Ŀ����Action��
	*/
	void set_Closure_and_Action();


	/*
	��ӡ��Ŀ����
	*/
	void showClosures(const char* fname = NULL);



	/*
	������Ŀ����
		���ݴ����i=curclosure����closure[i]��closure_eachlength[i]��������Ŀ����
		������뵱ǰ��closure[i]�У���ˢ��closure_eachlength��(��û��)���������ʺϳ��ȵĿռ䡣
	*/
	void setClosure(int curclosure_num);


	/*
		����buffer�������buf_projcnt������һ����Ŀ��������buffer�У������µ�buf_projcnt��
		*δʹ��
	*/
	project buffer[Max_num];
	int getclosure(int buf_projcnt);


	/*
	����1��
	����Ŀ��src�͵�ǰ��Ŀ������������Ŀ����Ƚϣ�
		���û���ظ�������-1��
		����ظ��������򱻰������������ظ���Ŀ�����±ꡣ
	*/
	int dupcheck(int src);


	/*
	��ӡActionGoto��	
	*/
	void showActionGoto(const char* fname = NULL);

	/*
	�����Ŀ����ǰ��������
	*/
	string get_firstBa(project src);


	/*
	�жϾ����Ƿ��Ǹ��ķ�������
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
		G[i][0] = line[0];//����ʽ����

		int j = 3;
		while (line[j] != 0)//����ʽ���Ҳ�
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

	if (G[src.prod][src.dot + 2] == 0)//.����ĵڶ����ַ��ǿ�
		return src.next;
	else if (isV[G[src.prod][src.dot + 2]] == isVt)//.����ĵڶ����ַ����ս��
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

		if (isV[G[curproj.prod][curproj.dot + 1]] == isVn)//�����ǰ��Ŀ.֮����Ǹ������Ƿ��ս��
		{
			for (int k = 0; k < prodsum; k++)//�������������Ϊ�󲿵���Ŀ
			{
				if (G[k][0] == G[curproj.prod][curproj.dot + 1])
				{
					tempproj.prod = k;
					tempproj.dot = 0;
					tempproj.next = get_firstBa(curproj);

					//Ҫ����
					bool is_dup = false;
					for (int m = 0; m < projcnt; m++)
					{
						if (curclosure[m].prod == tempproj.prod && curclosure[m].dot == tempproj.dot)//������ˣ���ô�Ͱ��µĺ�̷��ż���ӽ�ԭ�������棬Ȼ�������һ����Ŀ��
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

/*������*/
int params::getclosure(int buf_projcnt)
{
	project curproj;
	project tempproj;

	//project* curclosure = closure[curclosure_num];
	int projcnt = buf_projcnt;

	for (int i = 0; i < projcnt; i++)
	{
		curproj = buffer[i];

		if (isV[G[curproj.prod][curproj.dot + 1]] == isVn)//�����ǰ��Ŀ.֮����Ǹ������Ƿ��ս��
		{
			for (int k = 0; k < prodsum; k++)//�������������Ϊ�󲿵���Ŀ
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
	//ΪActionGoto������ռ䲢��ʼ��
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
	һ�����У��������ִ���ǰ���ڴ����closure�±ꡣ
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
		����Ϊ���ݵ�ǰclosure�����µ�closure���ף���ͬʱ��ActionGoto��
		*/
		project* curclosure = closure[curclosure_num];
		for (int j = 0; j < closure_eachlength[curclosure_num]; j++)
		{
			char X = G[curclosure[j].prod][curclosure[j].dot + 1];
			if (X != 0)//���.����ǿ�
			{
				int nextclosure = closure_sum;
				closure[nextclosure][0] = curclosure[j];
				closure[nextclosure][0].dot++;
				closure_eachlength[nextclosure]++;


				for (int k = 0;k < closure_eachlength[curclosure_num]; k++)//�������.����ǿ��Һ�X��ͬ����Ŀ��ҲҪ��������Ŀ����
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
					//�˴�����ActionGoto��
					if (isV[X] == isVt)
					{
						ActionGoto[curclosure_num][X].kind = 's';
						ActionGoto[curclosure_num][X].num = nextclosure;
					}
					else if (isV[X] == isVn)
					{
						//�����ǹ���goto
						ActionGoto[curclosure_num][X].kind = 'G';
						ActionGoto[curclosure_num][X].num = nextclosure;
					}

					closure_sum++;//���ʱ���ȷ��������һ���µ���Ŀ��
				}
				else
				{
					//cout << dupnum << endl;
					
					//�˴�����ActionGoto��
					if (isV[X] == isVt)
					{
						ActionGoto[curclosure_num][X].kind = 's';
						ActionGoto[curclosure_num][X].num = dupnum;
					}
					else if (isV[X] == isVn)
					{
						//�����ǹ���goto
						ActionGoto[curclosure_num][X].kind = 'G';
						ActionGoto[curclosure_num][X].num = dupnum;
					}

					closure_eachlength[nextclosure] = 0;

				}


			}
			else
			{
				//���XΪ�գ���ô��Ҫ����Action���еġ���Լ����rj
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
	������׼��
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
			cout << "�ƽ���Լ�� : " << SRstr << "            " << "���봮 : " << instr << "         " << "״̬ : ";
			stack<int> tempstack = status;
			for (int i = 0; i < status.size(); i++)
			{
				cout << tempstack.top() << ' ';
				tempstack.pop();
			}
			cout << endl;
		}

		if (ActionGoto[cur][instr[0]].kind == 'a')//����״̬
		{
			isdone = true;

			/*�����ڵ�*/
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
		else if (ActionGoto[cur][instr[0]].kind == 's')//�ƽ�
		{
			status.push(ActionGoto[cur][instr[0]].num);
			SRstr += instr[0];
			instr.erase(instr.begin(), instr.begin() + 1);
		}
		else if (ActionGoto[cur][instr[0]].kind == 'r')//��Լ
		{

			int prodBlength = 0;//���õĲ���ʽ���Ҳ�����
			int rprod = ActionGoto[cur][instr[0]].num;
			while (G[rprod][prodBlength + 1] != 0)
				prodBlength++;

			/*�����ڵ�*/
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
	//param.getGrammar("H:\\����ԭ��\\����ҵ1\\Compiler-toy-for-c-like-grammar\\LexicalAnalysis\\testgrammar3.txt");
	param.setTrans("���ձ�new.txt");
	param.showGrammar("./output/grammar.txt");

	cout << param.Vn_sum << "   " << param.Vt_sum << endl;

	param.set_First();
	param.showFirst("./output/First.txt");

	param.set_Closure_and_Action();
	param.showClosures("./output/Closures.txt");

	param.showActionGoto("./output/ActionGo.txt");

	//cout << param.judge("H:\\����ԭ��\\����ҵ1\\Compiler-toy-for-c-like-grammar\\LexicalAnalysis\\testprogram.cpp");
	int pres = param.judge(fsentence);
	cout << ( pres== 1 ? "success\n" : "fail\n");
	
	if (pres) {
		system("cd bin && dot -Tpng ../try.dot -o ../output/tree.png\n");
		system("cd output && tree.png\n");
	}
	
	return 0;
}
