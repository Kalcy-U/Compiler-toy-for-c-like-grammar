#pragma once
#include<iostream>
#include<vector>

class treeNode {
	int id;
	std::vector<treeNode*>child;//子节点id
	char content;//节点内容
};
class paramTree {
	std::vector<treeNode*>nodes;
	treeNode root;//理论上是nodes的最后一个
};
