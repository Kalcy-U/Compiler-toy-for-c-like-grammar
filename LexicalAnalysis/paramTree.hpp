#pragma once
#include<iostream>
#include<vector>

class treeNode {
	int id;
	std::vector<treeNode*>child;//�ӽڵ�id
	char content;//�ڵ�����
};
class paramTree {
	std::vector<treeNode*>nodes;
	treeNode root;//��������nodes�����һ��
};
