#pragma once
#include<iostream>
#include<string>
#include<vector>
using namespace std;
struct node
{
	node* parent;
	std::vector<node*> son;

	char c;
};


void showTree(node* root)
{
	cout << root->c << ' ' << endl;

	for(int i = 0;i<root->son.size();i++)
	{
		showTree(root->son[i]);
	}

	return;
}
