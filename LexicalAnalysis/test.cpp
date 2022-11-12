/*

  Name: LR(1)分析器的构造

  Author: wangrui

  Date: 07-06-07

　Description:对任意给定的文法G构造LR(1)项目集规范族和分析表,并对输入的　　　句子进行语法分析

*/

#include"iostream"

#include"fstream"

#include"stack"

#include"utility"

using namespace std;

 

char G[20][20];     //use a matrix to store grammar G

int  length[20];    //length use to store each formula's length

int  number = 0;

bool tempofinput[150];  //buffer of input

char str_vn[20];        //put all vn into it

int  size_vn = 0;      

char str_vt[150];       //put all vt into it

int  size_vt = 0;

bool first_vn[30][150];

char buffer[50];            //用来存放生成CLOSURE(I)时需要的first_set 也用来读入用户的输入串^_^

int  bsize = 0;

struct thri{

    int beg;

    int nex;

    char ch;

};

thri trans[200];

int  size_trans = 0;

 

//定义项目集的形式

struct proj{

    int formula_numb;

    int part;

    char expc;

};

/*项目集*/

proj    items[100][100];

int     Ccount = 0;

int     size_item[100];

 

/*状态转换表*/

struct action{

    char    ch;

    int     nxt_sta;

};

action    action_table[100][100];

int       size_act_table[100];

 

ifstream  G_ifile;

ifstream  input_ifile;

ofstream  items_ofile;

ofstream  act_ofile;

 

void Read_G()

{

    G_ifile >> number;   //user should give the number of formula first

    for(int i = 1; i <= number; i++){

        char temp;

        int j = 0;

        G_ifile >> temp;

        while(temp != '$'){

            tempofinput[temp] = true;

            G[i][j++] = temp;

            G_ifile >> temp;

        }

        length[i] = j;

    }

   

    G[0][0] = 'S';

    G[0][1] = G[1][0];

    length[0] = 2;

   

    for(int i = 0; i < 64; i++)

        if(tempofinput[i])

           str_vt[size_vt++] = i;

    for(int i = 91; i < 128; i++)

        if(tempofinput[i])

           str_vt[size_vt++] = i;

    for(int i = 65; i < 91; i++)

        if(tempofinput[i])

           str_vn[size_vn++] = i;

}

 

void get_first(){
    bool flag1;
    do{
        flag1 = false;

        for(int i = 1; i <= number; i++){

           int t = 1;

           bool flag2;

           do{

               flag2 = false;
               if (G[i][t] >= 'A' && G[i][t] <= 'Z'){
                      for(int k = 0; k < 64; k++)
                      if(first_vn[G[i][t]-'A'][k]==true&& !first_vn[G[i][0]-'A'][k]){

                             first_vn[G[i][0]-'A'][k] = true;

                             flag1 = true;

                          }

                      for(int k = 91; k < 128; k++)

                      if(first_vn[G[i][t]-'A'][k]==true&& !first_vn[G[i][0]-'A'][k]){

                             first_vn[G[i][0]-'A'][k] = true;

                             flag1 = true;

                          }

                      if(first_vn[G[i][t]-'A'][64] == true){

                          t++;

                          flag2 = true;

                      }

                }

                else if(!first_vn[G[i][0]-'A'][ G[i][t] ]){

                      first_vn[G[i][0]-'A'][ G[i][t] ] = true;

                      flag1 = true;

                }

           }while(flag2 && t < length[i]);

           if(t == length[i])

                first_vn[G[i][0]-'A'][26] = true;

        }

    }while(flag1);

}

/*判断项目数否在项目集里*/

bool is_in(proj temp,int T){

    for(int i = 0; i < size_item[T]; i++)

        if(temp.formula_numb == items[T][i].formula_numb && temp.part == items[T][i].part && temp.expc == items[T][i].expc)

                return true;

    return false;

}

 

void  gete_expc(proj temp){

    bsize = 0;   

    bool flag;

    int tt = temp.part;

    do{

        flag = false;

        if(tt+1 >= length[temp.formula_numb]){

                buffer[bsize++] = temp.expc;

                return;

        }

        else if(G[temp.formula_numb][tt+1] < 'A' || G[temp.formula_numb][tt+1] > 'Z'){

                buffer[bsize++] = G[temp.formula_numb][tt+1];

                return;

        }   

        else if(G[temp.formula_numb][tt+1] >= 'A' && G[temp.formula_numb][tt+1] <= 'Z'){

                for(int i = 0; i < 64; i++){

                   if(first_vn[ G[temp.formula_numb][tt+1]-'A' ][i])

                      buffer[bsize++] = i;

                }

                for(int i = 91; i < 128; i++){

                   if(first_vn[ G[temp.formula_numb][tt+1]-'A' ][i])

                      buffer[bsize++] = i;

                }

                if(first_vn[ G[temp.formula_numb][tt+1]-'A' ][64]){

                   tt++;

                   flag = true;

                }

        }

    }while(flag);

}

 

void e_closure(int T){

     for(int i = 0; i < size_item[T]; i++){ 

       proj temp;

       if(G[items[T][i].formula_numb][items[T][i].part] >= 'A' && G[items[T][i].formula_numb][items[T][i].part] <= 'Z'){

           for(int j = 0; j < 20; j++)

               if(G[j][0] == G[items[T][i].formula_numb][items[T][i].part]){

                 gete_expc(items[T][i]);

                 for(int k = 0; k < bsize; k++){

                         temp.formula_numb = j;

                         temp.part = 1;

                         temp.expc = buffer[k];

                         if(!is_in(temp,T))

                           items[T][size_item[T]++] = temp;

                 }

                 bsize = 0;

               }

       }

     }

     return ;

}

 

int is_contained()

{

    for(int i = 0; i < Ccount; i++){

       int s = 0;        //记录有多少是匹配的

       if(size_item[i] == size_item[Ccount])

                for(int j = 0; j < size_item[Ccount]; j++){

                   for(int k = 0; k < size_item[i]; k++)

                    if((items[Ccount][j].formula_numb==items[i][k].formula_numb)&&(items[Ccount][j].part == items[i][k].part) && (items[Ccount][j].expc == items[i][k].expc)){

                                 s++;

                                 break;

                    }

                }

       if(s == size_item[Ccount])

                return i;

    }

    return 0;

}

 

void go(){

    proj init;

    init.expc = '#';

    init.formula_numb = 0;

    init.part = 1;

    items[0][0] = init;

    size_item[0]++;

   

    e_closure(0);

    items_ofile << "I0:" << endl;

    for(int i = 0; i < size_item[0]; i++)

           items_ofile << items[0][i].formula_numb << " , " << items[0][i].part << " , " << items[0][i].expc << endl;

     items_ofile << "***************************************" << endl;

   

    for(int index = 0; index <= Ccount ; index++){

        for(int j = 0; j < size_vt; j++){

                proj    buf[50];

                int     buf_size = 0;

                proj    tp;

                for(int p = 0; p < size_item[index]; p++){

if((items[index][p].part<length[items[index][p].formula_numb])&&( G[ items[index][p].formula_numb ][ items[index][p].part ] == str_vt[j]) ){

                           tp.formula_numb = items[index][p].formula_numb;

                           tp.expc = items[index][p].expc;

                           tp.part = items[index][p].part+1;

                           buf[buf_size++] = tp;

                   }

                }

                if(buf_size  != 0){

                   Ccount++;

                   for(int t = 0; t < buf_size; t++){

                       items[Ccount][ size_item[Ccount]++ ] = buf[t];

                   }

                   e_closure(Ccount);

                   int  next_state = is_contained();        //看生成的项目集是否已经在项目集族中了

                   if(next_state != 0){

                       size_item[Ccount] = 0;

                       Ccount--;

                       trans[size_trans].beg = index;

                       trans[size_trans].nex = next_state;

                       trans[size_trans].ch = str_vt[j];

                       size_trans++;

                   }

                   else{

                       items_ofile << "I" << Ccount << ":" << endl;

                       for(int i = 0; i < size_item[Ccount]; i++)

                           items_ofile << items[Ccount][i].formula_numb << " , " << items[Ccount][i].part << " , " << items[Ccount][i].expc << endl;

                       items_ofile << "***************************************" << endl;

                       trans[size_trans].beg = index;

                       trans[size_trans].nex = Ccount;

                       trans[size_trans].ch = str_vt[j];

                       size_trans++;          

                   }

                }

        }                //对文法的每一个终结符

 

        for(int j = 0; j < size_vn; j++){

                proj    buf[50];

                int     buf_size = 0;

                proj    tp;

                for(int p = 0; p < size_item[index]; p++){

if((items[index][p].part<length[items[index][p].formula_numb])&&( G[ items[index][p].formula_numb ][ items[index][p].part ] == str_vn[j]) ){

                           tp.formula_numb = items[index][p].formula_numb;

                           tp.expc = items[index][p].expc;

                           tp.part = items[index][p].part+1;

                           buf[buf_size++] = tp;

                       }

                }

                if(buf_size  != 0){

                   Ccount++;

                   for(int t = 0; t < buf_size; t++){

                       items[Ccount][ size_item[Ccount]++ ] = buf[t];

                   }

                   e_closure(Ccount);

                   int  next_state = is_contained();       //看生成的项目集是否已经在项目集族中了

                  

                   if(next_state != 0){

                       size_item[Ccount] = 0;

                       Ccount--;

                       trans[size_trans].beg = index;

                       trans[size_trans].nex = next_state;

                       trans[size_trans].ch = str_vn[j];

                       size_trans++;

                   }

                   else{

                       items_ofile << "I" << Ccount << ":" << endl;

                       for(int i = 0; i < size_item[Ccount]; i++)

                           items_ofile << items[Ccount][i].formula_numb << " , " << items[Ccount][i].part << " , " << items[Ccount][i].expc << endl;

                       items_ofile << "***************************************" << endl;

                       trans[size_trans].beg = index;

                       trans[size_trans].nex = Ccount;

                       trans[size_trans].ch = str_vn[j];

                       size_trans++;          

                   }                  

                }

        }                //对文法的每一个非终结符

    }

}

//get action table based on item set and trans array

void get_action(){

    for(int i = 0; i < 100; i++)

        size_act_table[i] = 0;

       

    for(int i = 0; i <= Ccount; i++)         //*************  i must <= Ccount !!!!!!!!!!!!!! ***********

        for(int j = 0; j < size_item[i]; j++)

                if(items[i][j].part == length[ items[i][j].formula_numb ] ){

                    action_table[i][ size_act_table[i] ].ch = items[i][j].expc;

                    action_table[i][ size_act_table[i]++ ].nxt_sta = items[i][j].formula_numb*(-1);

                }

    for(int i = 0; i < size_trans; i++){

        int    t1 = trans[i].beg;

        int    t2 = trans[i].nex;

        char   tp = trans[i].ch;

        action_table[t1][ size_act_table[t1] ].ch = tp;

        action_table[t1][ size_act_table[t1]++ ].nxt_sta = t2;

    }

}

 

int main(){

   for(int i = 0; i< 150; i++)

        tempofinput[i] = false;

    for(int i= 0; i < 100; i++)

        size_item[i] = 0;

    for(int i = 0; i < 30; i++)

        for(int j = 0; j < 150; j++)

                first_vn[i][j] = false;

 

    G_ifile.open("d://grammar.txt");

    input_ifile.open("d://input.txt");

    items_ofile.open("d://items.txt");

    act_ofile.open("d://action_table.txt");

 

    Read_G();        //read G and put the number of formula into count

    get_first();     //each vn's first_set

    go();

    get_action();

    for(int i = 0; i < Ccount; i++)

        for(int j = 0; j < size_act_table[i]; j++){

            char   tp = action_table[i][j].ch;

            int    t  = action_table[i][j].nxt_sta;

            act_ofile << "(" << i << "," << tp << "," << t << ")" << endl;

        }

  

    bsize = 0;

    do{

        input_ifile >> buffer[bsize];

    }while(buffer[ bsize++ ] != '#');

    stack<pair<int,char> > s;    //语法检查栈

    int    work_sta = 0;

    int    index_buf = 0;

    bool   err;

    bool   done = false;

    s.push(make_pair(0,'#'));

    do{

        work_sta = s.top().first;

        err =  true;

        for(int i= 0; i < size_act_table[work_sta]; i++)

            if(action_table[work_sta][i].ch == buffer[index_buf]){

                    err = false;

                    if(action_table[work_sta][i].nxt_sta == 0){

                                    cout << "Accept!!!" << endl;

                                    done = true;

                                    break;

                    }

                    else if(action_table[work_sta][i].nxt_sta > 0){

                       s.push(make_pair(action_table[work_sta][i].nxt_sta,action_table[work_sta][i].ch));

                                    index_buf++;

                                    break;

                    }

                    else{

                       int tp = action_table[work_sta][i].nxt_sta*(-1);    //用来归约的产生式编号

                                    for(int k = 0; k < length[tp]-1; k++)

                                          s.pop();

                                    --index_buf;

                                    buffer[index_buf] = G[tp][0];

                                    break;

                    }

            }

    }while(done == false && err == false);

    if(!done)

        cout << "请检查输入串!!!" << endl;

    G_ifile.close();

    input_ifile.close();

    items_ofile.close();

act_ofile.close();

    return 0;

}