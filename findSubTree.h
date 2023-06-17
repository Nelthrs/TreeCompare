#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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

class Patch;
class PatchNode;
class PatchConnection;

class Node {
public:
	Node(string data);
	bool isNode();
	bool isLeaf() const;
	unique_ptr<Node> copy() const;
	Node* addChild(const string& newChildName);
	void addChild(unique_ptr<Node> newChild);
	int descendantsCount() const;
	int buildPatch(Node* cmpTree, Patch* generalPatch) const;
	string getName() const;
	void print(int level = 0) const;
	bool isDescendant(Node* searchedNode) const;
	vector<Node*> getChildren() const;
	void setPatchNode(PatchNode* newPatchNode);
	PatchNode* getRelPatch() const;
	int buildDeltaTreeWrap(Node* cmpTree, unique_ptr<Node>& deltaTree) const;
	int buildDeltaTree(Node* deltaTree, Patch* generalPatch) const;
	Node* getMirrorNode(Patch* generalPatch) const;
	PatchConnection* getMinPatchConnection();
	vector<pair<PatchConnection*, PatchNode*>> getIncomingConnections(Patch* generalPatch);
protected:
	string name;
	vector<unique_ptr<Node>> children;
	PatchNode* relPatch;
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
	vector<PatchConnection*> getConnections();
	Node* getRoot();
	int validConnectionsCount();
protected:
	Node* rootSubTree;
	vector<unique_ptr<PatchConnection>> connections;
};

class Patch {
public:
	Patch();
	vector<PatchNode*> getPatch() const;
	int countLeafPatches(Node* tree);
	vector<pair<PatchNode*, PatchConnection*>> findPointingNode(Node* pointNode) const;
	int countDeltaNodes(const Node* pointTree, const Node* sourceTree) const;
	vector<PatchNode*> findLeafPatches(Node* sourceTree);
	void deleteAllReferences(const  Node*  excludedNode);
	void addNode(unique_ptr<PatchNode> newNode);
protected:
	vector<unique_ptr<PatchNode>> patchNodes;
};
