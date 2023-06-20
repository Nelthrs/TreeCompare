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
#include <filesystem>
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
	explicit Node(const string& data);
	bool isNode() const;
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
	int buildDeltaTreeWrap(Node* cmpTree, unique_ptr<Node>& deltaTree) const;
	vector<PatchConnection*> getConnectionsForChildren(const Patch* generalPatch) const;
	PatchConnection* getMinValidConnection(vector<PatchConnection*> connections);
	std::optional<vector<pair<Node*, PatchConnection*>>> getMinConnectionPairs(Patch* generalPatch) const;
	int buildDeltaTree(Patch* generalPatch, int counter);
	vector<pair<PatchConnection*, PatchNode*>> getIncomingConnections(const Patch* generalPatch) const;
	void removeChild(const Node* nodeToDelete);

protected:
	string name;
	vector<unique_ptr<Node>> children;
	PatchNode* relPatch;
};

void visualizeTree(const Node* root, const string filename);
unique_ptr<Node> parseOnTree(const string& content, const string& delimiters, int startIndex = 0);

class PatchConnection {
public:
	PatchConnection(int weight, Node* searchedSubTree, PatchNode* rootPatchNode);
	void setWeight(int weight);
	int getWeight() const;
	Node* getSearchedSubTree();
	PatchNode* getRootPatchNode() const;
protected:
	int weight;
	Node* searchedSubTree;
	PatchNode* rootPatchNode;
};

class PatchNode {
public:
	PatchNode();
	explicit PatchNode(Node* rootSubTree);
	void addConnection(unique_ptr<PatchConnection> newConnection);
	void addConnection(int weight, Node* searchedSubTree);
	vector<PatchConnection*> getConnections() const;
	bool conectionsContains(PatchConnection* connection);
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
	vector<pair<PatchNode*, PatchConnection*>> findPointingNode(const Node* pointNode) const;
	int countDeltaNodes(const Node* pointTree, const Node* sourceTree) const;
	vector<PatchNode*> findLeafPatches(const Node* sourceTree) const;
	void deleteReferences(const  Node*  excludedNode, const optional<vector<PatchConnection*>> reservedConnections);
	void addNode(unique_ptr<PatchNode> newNode);
protected:
	vector<unique_ptr<PatchNode>> patchNodes;
};
