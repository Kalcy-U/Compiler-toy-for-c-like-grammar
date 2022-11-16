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
	Action/Goto��ÿһ��Ķ�����

	kindָ���ƽ�����Լ��acc���Ǵ���
	next���±�ָ����Ŀ���塣

	*/
	struct elem
	{
		char kind;
		int num;
	};

	/*
	Action��
		�ڹ���closure��ʱ���ʼ��������ռ䣬��һ��������
	*/
	elem* ActionGoto[Max_num];



	/*
	Goto��
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



	/*
	������Ŀ����Action��
	*/
	void set_Closure_and_Action();


	/*
	��ӡ��Ŀ����
	*/
	void showClosures();



	/*
	�ѱ����
	���ݴ����i=curclosure����closure[i]��closure_eachlength[i]��������Ŀ����
	������뵱ǰ��closure[i]�У���ˢ��closure_eachlength��(��ѡ)���������ʺϳ��ȵĿռ䡣
	*/
	void setClosure(int curclosure_num);



	/*
		����buffer�������buf_projcnt������һ����Ŀ��������buffer�У������µ�buf_projcnt��
	*/
	project buffer[Max_num];
	int getclosure(int buf_projcnt);


	/*
	����1��
	����Ŀ��src�͵�ǰ��Ŀ������������Ŀ����Ƚϣ�
		���û���ظ�������-1��
		����ظ��������ظ���Ŀ�����±ꡣ
	*/
	int dupcheck(int src);


	/*
	��ӡActionGoto��	
	*/
	void showActionGoto();

	/*
	�����ǰ��������
	*/
	string get_firstBa(project src);


};




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
	if (G[src.prod][src.dot + 2] == 0)//.����ĵڶ����ַ��ǿ�
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

		if (isV[G[curproj.prod][curproj.dot + 1]] == isVn)//�����ǰ��Ŀ.֮����Ǹ������Ƿ��ս��
		{
			for (int k = 0; k < prodsum; k++)//�������������Ϊ�󲿵���Ŀ
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


/*
void params::set_Closure_and_Action()
{

	project* quebuf[Max_num];
	bool quebuf_used[Max_num];
	for (int i = 0; i < Max_num; i++)
		quebuf_used[i] = false;

	queue<project*> buf_que;
	queue<int> bufcnt_que;

	//����Ҫ������Ŀ��0:
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

		if (G[buffer[0].prod][buffer[0].dot + 1] == 0)//��������Ŀ���Ѿ����ս�״̬
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

			for (int k = 0; k < buf_projcnt && isused[k] == false; k++)//ʹ��ĳ���ַ����������������Ŀ�ܹ�ʹ�ø��ַ�����.��ǰ��һ������ô��ҲҪ���������±հ���
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


				for (int k = 0; k != j && k < closure_eachlength[curclosure_num]; k++)//�������.����ǿ��Һ�X��ͬ����Ŀ��ҲҪ��������Ŀ����
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
	param.getGrammar("H:\\����ԭ��\\����ҵ1\\Compiler-toy-for-c-like-grammar\\LexicalAnalysis\\testgrammar3.txt");
	param.showGrammar();

	cout << param.Vn_sum << "   " << param.Vt_sum << endl;

	param.set_First();
	param.showFirst();

	param.set_Closure_and_Action();
	param.showClosures();

	param.showActionGoto();


	return 0;
}
