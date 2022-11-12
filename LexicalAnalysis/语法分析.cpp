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


	/*
	��Ŀ��
		prod˵�������Ŀ�ǻ��ڵ�prod������ʽ��
		dot˵����һ���ַ��ǵڼ�����
		next����չ����(��)����һ����

		e.g. : prod = 0,dot = 0,next = '#'ָ������Ŀ"S'->.S , '#'"��
	*/
	struct project
	{
		int prod;
		int dot;

		char next;
	};

	/*
	��Ŀ��

	//��Ļ����Ǻ����
	ÿһ�д�һ����Ŀ����Ȼ���ٿ�һ�������ÿһ����Ŀ���ĳ��ȣ�
	������

	*/
	project closure[Max_num][Max_num];
	int closure_sum = 0;
	int closure_length[Max_num];

	/*
	Action/Goto��ÿһ��Ķ�����

	kindָ���ƽ�����Լ��acc���Ǵ���
	next���±�ָ����Ŀ���塣

	*/
	struct elem
	{
		char kind;
		int next;
	};



	/*
	Action��
	*/
	elem Action[Max_num][Max_Vsign];

	/*
	Goto��
	*/
	elem Goto[Max_num][Max_Vsign];


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
	void showGrammar();

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
	void showFirst();




};





/*
������Ŀ����Closure��
*/
//��û���

void get_closure(params* param);


/*
����Action���Goto��
*/
void get_Action();


/* ��ʵ��֪��Ҫ��Ҫ����Щȫ��д�ɽṹ������ĺ���*/


/*
�жϾ����Ƿ��Ǹ��ķ�������
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
* �Ҹо����յ��Ǹ�����̫�鷳�� ����ѭ���˶� �������ϵ�дһд��һ��
void params::set_First()
{
	bool done = false;

	while (!done)//��һ��ѭ��
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
							First[cur[0]][k] = true;//�Ҳ�������

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
	param.getGrammar("H:\\����ԭ��\\����ҵ1\\Compiler-toy-for-c-like-grammar\\LexicalAnalysis\\testgrammar2.txt");
	param.showGrammar();

	cout << param.Vn_sum << "   " << param.Vt_sum << endl;

	param.set_First();
	param.showFirst();
	return 0;
}
