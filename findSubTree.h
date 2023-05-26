#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <locale.h>
#include <list>
#include <set>
#include <map>
#include <algorithm>
using namespace std;


enum LEXEM_TYPE
{
	NODE, LEFT_BRACKET, RIGHT_BRACKET, UNKNOWN
};

enum NODE_TYPES {
	LEAFS, NODES, LEAF_NODE, NODE_LEAF, NILL
};

class PatchNode;

class Node {
public:
	Node(string data);
	int findDeltaPairs(const Node* cmpTree);
	int buildPatch(Node* cmpTree, vector<unique_ptr<PatchNode>>& patchNodes);
	vector<unique_ptr<PatchNode>> buildPatch(Node* searchedTree);
	int cmpNodes(Node* cmpNode);
	bool isNode();
	bool isLeaf() const;
	unique_ptr<Node> copy() const;
	void addChild(const string& newChildName);
	void addChild(unique_ptr<Node> newChild);
	int descendantsCount() const;
	string getName() const;
	void print(int level = 0) const;
protected:
	string name;
	vector<unique_ptr<Node>> children;
};

unique_ptr<Node> parseOnTree(const string& content, const string& delimiters, int startIndex = 0);

class PatchConnection {
public:
	PatchConnection(Node* searchedSubTree);
	PatchConnection(int weight, Node* searchedSubTree);
	void setWeight(int weight);
	int getWeight();
	Node* getSearchedSubTree();
protected:
	int weight;
	Node* searchedSubTree;
};

class PatchNode {
public:
	PatchNode();
	PatchNode(Node* rootSubTree);
	void addConnection(unique_ptr<PatchConnection> newConnection);
	void addConnection(int weight, Node* searchedSubTree);
	vector<unique_ptr<PatchConnection>> getConnections();
	Node* getRoot();
protected:
	Node* rootSubTree;
	vector<unique_ptr<PatchConnection>> connections;
};
