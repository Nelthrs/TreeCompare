
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

class Node {
public:

	Node(string data)
	{
		this->name = data;
	}

	void addChild(unique_ptr<Node> newChild)
	{
		this->children.push_back(move(newChild));
	}

	void addChild(const string& newChildName)
	{
		unique_ptr<Node> newChild = make_unique<Node>(newChildName);
		this->addChild(move(newChild));
	}

	Node* findNode(const string& targetName, int& count)
	{
		if (this == nullptr)
			return nullptr;
		if (this->name == targetName)
		{
			if (count == 1)
				return this;

			count--;
		}
		for (auto& child : children)
		{
			Node* foundNode = child->findNode(targetName, count);
			if (foundNode != nullptr)
				return foundNode;
		}
		return nullptr;
	}

	Node* findInChildren(const string& targetName)
	{
		for (auto& child : children)
		{
			if (child->name == targetName)
				return child.get();
		}
		return nullptr;
	}

	unique_ptr<Node> copy() const
	{
		auto root = make_unique<Node>(this->name);

		for (auto& child : children)
		{
			root->addChild(child->copy());
		}

		return root;
	}

	void print(int level = 0) const
	{
		for (int i = 0; i < level; ++i)
			cout << "-";
		cout << this->name << endl;

		level++;
		for (auto& child : children)
		{
			child->print(level);
		}
	}

	bool sameNamed(const Node* leaf) const
	{
		if (this->getName() != leaf->getName())
			return false;
		else
			return true;
	}

	bool isLeaf() const
	{
		if (this->children.size() == 0)
			return true;

		return false;
	}

	bool isSimilarLeaves(const Node* node)
	{
		if (this->isLeaf() && node->isLeaf())
		{
			if (this->sameNamed(node))
				return true;
			else
				return false;
		}
		return false;
	}

	bool isNode()
	{
		return !this->isLeaf();
	}

	int descendantsCount()
	{
		for (auto& child : children)
		{
			return children.size() + child->descendantsCount();
		}
		return 0;
	}

	void writeInSet(set<Node*>& set)
	{
		if (this == nullptr)
			return;

		set.insert(this);

		if (this->isNode())
		{
			for (auto& child : children)
			{
				child->writeInSet(set);
			}
		}
	}

	NODE_TYPES cmpNodes(Node* cmpNode) {
		if (this->isLeaf() && cmpNode->isLeaf()) {
			return LEAFS;
		}
		// ≈сли в первом дереве - лист, а во втором - узел, считать разность равной кол-ву потомков узла + 1
		if (this->isLeaf() && cmpNode->isNode()) {
			return LEAF_NODE;
		}
		// ≈сли в первом дереве - узел, а во втором лист, считать сравнение невозможным.
		if (this->isNode() && cmpNode->isLeaf()) {
			return NODE_LEAF;
		}
		// ≈сли эти дети - узлы, найти их разность
		if (this->isNode() && cmpNode->isNode()) {
			return NODES;
		}
		return NILL;
	}
	/*! ”дал€ет в вызываемом дереве поддеревь€, которые совпадают с поддеревь€ми из задаваемого дерева

	
	bool deleteSimilarSubTrees(unique_ptr<Node>& cmpTree)
	{
		bool nodeDeleted;
		for (auto cmpTreeChild = cmpTree->children.begin(); cmpTreeChild != cmpTree->children.end(); )
		{
			nodeDeleted = false;
			for (auto& treeChild : this->children)
			{
				if (treeChild->isSimilarLeaves(cmpTreeChild->get()))
				{
					cmpTreeChild = cmpTree->children.erase(cmpTreeChild);
					nodeDeleted = true;
					break;
				}
				else if (treeChild->isNode()&&)
					treeChild->deleteSimilarSubTrees(*cmpTreeChild);
			}
			if (nodeDeleted == false)
				++cmpTreeChild;
		}
	}

	int compare(const unique_ptr<Node>& subTree, unique_ptr<Node>& deltaTree)
	{
		// ”далить текущий узел поддерева если он 
		// (не совпадает по имени с текущим узлом сравниваемого дерева или с одним из его братьев)
		// и (среди его потомков нет недостающих узлов)


		if (!this->sameNamed(subTree.get()))
			return subTree->descendantsCount() + 1;

		// ≈сли рассматриваемые узлы - два однаковых листа, считать что ненайденных узлов - 0, если два разных листа, то 1
		if (this->isLeaf() && subTree->isLeaf())
		{
			if (this->sameNamed(subTree.get()))
				return 0;
			else
				return 1;
		}
		// ≈сли у рассматриваемого узла вызываемого дерева есть дети, а у рассматриваемого узла сравниваемого - нет, считать что дополнение невозможно
		else if (this->isNode() && subTree->isLeaf())
			return -1;

		int minDelta = subTree->descendantsCount() + 1;
		int delta = minDelta;
		Node* minDeltaTree;


		for (auto subTreeChild = subTree->children.begin(); subTreeChild != subTree->children.end(); )
		{
			for (auto treeChild = this->children.begin(); treeChild != this->children.end(); )
			{
				// —равнить поддеревь€
				delta = (*subTreeChild)->compare(*treeChild);

				if (delta == -1)
					return -1;

				// «аписать новое минимальное дерево
				if (delta < minDelta)
				{
					minDelta = delta;
					minDeltaTree = (*subTreeChild).get();
				}

				if (minDelta == 0)
				{

				}
			}
			if (delta == 0)
			{
				subTreeChild.reset();
			}
		}

		return subTree->descendantsCount() + 1;
	}

	int findSubTree(const unique_ptr<Node>& subTree, unique_ptr<Node>& deltaTree);
	*/

	pair<int, Node*> findMinDeltaTree(const map<Node*, vector<pair<int, Node*>>> patch) 
	{
		auto it = patch.find(this);
		// ≈сли узел найден
		if (it != patch.end()) {
			vector<pair<int, Node*>> pairs = it->second;
			// ≈сли дл€ этого узла есть пары
			if (!pairs.empty()) {
				// Ќайти минимальную пару среди них
				auto min_it = std::min_element(pairs.begin(), pairs.end(), 
					[](const auto& pair1, const auto& pair2) {
						if (pair1.first >= 0 && pair2.first >= 0) {
							return pair1.first < pair2.first;
						}
					});
				pair<int, Node*> minPair = *min_it;
				return minPair;
			}
		}

		return(make_pair(-1, nullptr));
	}

	int findDelta(unique_ptr<Node>& cmpTree) 
	{
		unique_ptr<Node> firstTree = this->copy();
		unique_ptr<Node> secondTree = cmpTree->copy();
		return firstTree->removeCommonNodes(secondTree);
	}

	int removeCommonNodes(const unique_ptr<Node>& cmpTree) 
	{
		map<Node*, vector<pair<int, Node*>>> patch;
		int curDelta = -1;
		Node* curTreeNode = nullptr;
		vector<pair<int, Node*>> similarTrees;
		// ƒл€ каждой пары детей в первом и втором дереве
		for (auto& treeNode : this->children) {
			curTreeNode = treeNode.get();
			similarTrees.clear();
			for (auto& cmpTreeNode : cmpTree->children) {
				// ≈сли у детей одинаковые имена
				if (treeNode->getName() == cmpTreeNode->getName()) {
					switch (cmpNodes(cmpTreeNode.get())){
					case LEAFS:
						curDelta = 0;
						break;
					case NODES:
						curDelta = treeNode->removeCommonNodes(cmpTreeNode);
						break;
					case LEAF_NODE:
						curDelta = cmpTreeNode->descendantsCount();
						break;
					default:
						curDelta = -1;
						break;
					}
					similarTrees.push_back(make_pair(curDelta, cmpTreeNode.get()));
				}
			}
			patch[curTreeNode] = similarTrees;
		}

		for (auto& treeNode : this->children) {
			treeNode
		}
		

	}

	string getName() const
	{
		return this->name;
	}

	~Node()
	{

	}
protected:
	string name;
	vector<unique_ptr<Node>> children;
};

bool compareNodes(const Node* Node1, const Node* Node2)
{
	return Node1->getName() < Node2->getName();
}

class ExcForbiddenSymbol : public std::exception
{
public:
	ExcForbiddenSymbol(unsigned char symbol)
	{
		this->symbol = symbol;
	}

	const char* what() const noexcept override
	{
		string msg = "Ќайден недопустимый символ \'" + to_string(symbol) + "\'" + "в файле \'" + filename + "\'";
		return msg.c_str();
	}

	void setFilename(string filename)
	{
		this->filename = filename;
	}
protected:
	unsigned char symbol;
	string filename;
};

class ExcSeveralTrees : public std::exception
{
public:
	const char* what() const noexcept override
	{
		string msg = "¬ файле \'" + filename + "\'" + "содержитс€ более 1 дерева";
		return msg.c_str();
	}
protected:
	string filename;
};

class Lexem {
public:
	Lexem(LEXEM_TYPE type)
	{
		this->nodeType = type;
	}
	Lexem(LEXEM_TYPE type, string nodeName)
	{
		this->nodeType = type;
		this->nodeName = nodeName;
	}
	string name()
	{
		if (nodeType == NODE)
			return this->nodeName;
		else if (nodeType == LEFT_BRACKET)
			return "LEFT_BRACKET";
		else if (nodeType == RIGHT_BRACKET)
			return "RIGHT_BRACKET";
		return "Unknown";
	}
	LEXEM_TYPE type()
	{
		return this->nodeType;
	}
protected:
	LEXEM_TYPE nodeType;
	string nodeName;
};

bool readFile(string path, string* content)
{
	bool success = false;
	string line;

	ifstream in(path); // окрываем файл дл€ чтени€
	if (in.is_open())
	{
		// считываем текстовый файл и записываем его в одну строку
		while (getline(in, line))
		{
			*content += line;
		}
		success = true;
	}
	in.close();     // закрываем файл
	return success;
}

string extractWord(string str, unsigned startIndex, string delimiters)
{
	// Ќайти первое вхождение одного из разделителей
	int word_end = str.find_first_of(delimiters, startIndex);

	// —читать, что слово кончилось, когда встречен первый разделитель
	return str.substr(startIndex, word_end - startIndex);
}

vector<Lexem> parseOnLexems(const string& content, string delimiters)
{
	vector<Lexem> lexems;
	int i = 0;
	for (i; i < content.length(); i++)
	{
		const char curSymbol = content[i];
		if (curSymbol == '(')
		{
			lexems.push_back(Lexem(LEFT_BRACKET));
		}
		else if (curSymbol == ')')
		{
			lexems.push_back(Lexem(RIGHT_BRACKET));
		}
		else if (isalnum(curSymbol))
		{
			string nodeName = extractWord(content, i, delimiters);
			lexems.push_back(Lexem(NODE, nodeName));
			i += nodeName.length() - 1;
		}
	}
	return lexems;
}

unique_ptr<Node> sexpToTree(vector<Lexem>& lexems, int& index) {

	auto root = make_unique<Node>(lexems[index++].name());
	while (index < lexems.size()) {
		Lexem curLexem = lexems[index];
		Lexem nextLexem(UNKNOWN);
		if (index < lexems.size() - 1)
			nextLexem = lexems[index + 1];

		if (curLexem.type() == NODE)
		{
			unique_ptr<Node> child;

			if (nextLexem.type() == LEFT_BRACKET)
				child = sexpToTree(lexems, index);
			else
				child = make_unique<Node>(curLexem.name());

			root->addChild(move(child));
			index++;
		}
		else if (curLexem.type() == RIGHT_BRACKET)
			return root;
		else
			index++;
	}
	return root;
}

// проверить
list<Node*> listsDiff(list<Node*> list1, list<Node*> list2) {
	std::list<Node*> difference = list2;
	for (auto it1 = list1.begin(); it1 != list2.end();)
	{
		bool found = false;
		auto it2 = list2.begin();
		while (it2 != list2.end())
		{
			if ((*it1)->getName() == (*it2)->getName())
			{
				found = true;
				it2 = list2.erase(it2);
			}
			else
			{
				++it2;
			}
		}
		if (found)
		{
			it1 = list1.erase(it1);
		}
		else
		{
			++it1;
		}
	}
}

int main()
{

	setlocale(LC_ALL, "Russian");
	string content;
	string path{ R"(C:\Users\barten\Documents\eztree.txt)" };

	readFile(path, &content);
	string delimiters = "() \t\n\r";
	cout << content << endl;
	vector<Lexem> lexems = parseOnLexems(content, delimiters);

	int startIndex = 0;

	unique_ptr<Node> tree = sexpToTree(lexems, startIndex);

	auto newTree = tree->copy();
	auto newChild = make_unique<Node>("1");
	newChild->addChild("2");
	newChild->addChild("3");
	newTree->addChild(move(newChild));
	


	tree->print();

}
