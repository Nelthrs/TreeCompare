#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <locale.h>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <optional>
#include <filesystem>
#include <cstdlib>
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
	explicit Node(const string& data);
	bool isNode() const;
	bool isChild(const Node* probablyChild) const;
	bool isLeaf() const;
	unique_ptr<Node> copy() const;
	Node* addChild(const string& newChildName);
	void addChild(unique_ptr<Node> newChild);
	void removeChild(const Node* nodeToDelete);
	int descendantsCount() const;
	string getName() const;
	void print(int level = 0) const;
	vector<const Node*> findDescendants(const string& searchedNodeName) const;
	vector<Node*> getChildren() const;
	unique_ptr<Node> buildPedigree(const Node* child) const;
	int findSubTree(const Node* cmpTree, unique_ptr<Node>& deltaTree);
	unique_ptr<PatchNode> buildPatchWrap(Node* cmpTree) const;
	int buildPatch(const Node* cmpTree, PatchNode* patch) const;
	int buildDeltaTreeWrap(const Node* cmpTree, unique_ptr<Node>& deltaTree) const;
private:
	string name;
	vector<unique_ptr<Node>> children;
};

unique_ptr<Node> parseOnTree(const string& content, const string& delimiters, int startIndex = 0);

class PatchNode {
public:
	PatchNode();
	explicit PatchNode(const Node* rootSubTree);
	explicit PatchNode(Node* rootSubTree);
	PatchNode* addChild(unique_ptr<PatchNode> newChild);
	void addConnection(int weight, Node* searchedSubTree);
	vector<pair<Node*, int>> getConnections() const;
	vector<Node*> findUncaughtChildren(const Node* treeNode) const;
	Node* getRoot() const;
	vector<PatchNode*> getChildren() const;
	vector<pair<Node*, int>>::const_iterator findConnection(const Node* searchedNode) const;
	int findMinValidConnection(int startIndex = 0) const;
	int buildDeltaTree(Node* cmpTree) const;
private:
	Node* rootSubTree;
	vector<pair<Node*, int>> connections;
	vector<unique_ptr<PatchNode>> children;
};

