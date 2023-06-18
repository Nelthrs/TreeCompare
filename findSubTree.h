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
#include <optional>
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
	bool isDescendant(const Node* searchedNode) const;
	Node* findFather(const Node* desiredChild);
	vector<Node*> getChildren() const;
	void setPatchNode(PatchNode* newPatchNode);
	PatchNode* getRelPatch() const;
	int buildDeltaTreeWrap(Node* cmpTree, unique_ptr<Node>& deltaTree);
	vector<PatchConnection*> getConnectionsForChildren(Patch* generalPatch);
	PatchConnection* getMinValidConnection(vector<PatchConnection*> connections);
	std::optional<vector<pair<Node*, PatchConnection*>>> getMinConnectionPairs(Patch* generalPatch);
	int buildDeltaTree(Patch* generalPatch);
	int buildDeltaTree(Node* mainTree, Node* deltaTree, Patch* generalPatch, PatchConnection* prevConnection) const;
	Node* getMirrorNode(Patch* generalPatch) const;
	PatchConnection* getMinPatchConnection();
	vector<pair<PatchConnection*, PatchNode*>> getIncomingConnections(Patch* generalPatch);
	void removeChild(Node* nodeToDelete);

protected:
	string name;
	vector<unique_ptr<Node>> children;
	PatchNode* relPatch;
};

unique_ptr<Node> parseOnTree(const string& content, const string& delimiters, int startIndex = 0);

class PatchConnection {
public:
	PatchConnection(int weight, Node* searchedSubTree, PatchNode* rootPatchNode);
	void setWeight(int weight);
	int getWeight();
	Node* getSearchedSubTree();
	PatchNode* getRootPatchNode();
protected:
	int weight;
	Node* searchedSubTree;
	PatchNode* rootPatchNode;
};

class PatchNode {
public:
	PatchNode();
	PatchNode(Node* rootSubTree);
	void addConnection(unique_ptr<PatchConnection> newConnection);
	void addConnection(int weight, Node* searchedSubTree);
	vector<PatchConnection*> getConnections();
	bool conectionsContains(PatchConnection* connection);
	Node* getRoot();
	int validConnectionsCount();
	void addChild(unique_ptr<PatchNode> child);
protected:
	Node* rootSubTree;
	vector<unique_ptr<PatchConnection>> connections;
	vector<unique_ptr<PatchNode>> children;
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
