#include "findSubTree.h"

using namespace std;

Node::Node(string data)
{
	this->name = data;
}

void Node::addChild(unique_ptr<Node> newChild)
{
	this->children.push_back(move(newChild));
}

void Node::addChild(const string& newChildName)
{
	unique_ptr<Node> newChild = make_unique<Node>(newChildName);
	this->addChild(move(newChild));
}

unique_ptr<Node> Node::copy() const
{
	auto root = make_unique<Node>(this->name);

	for (auto& child : children)
	{
		root->addChild(child->copy());
	}

	return root;
}

bool Node::isLeaf() const
{
	if (this->children.size() == 0)
		return true;

	return false;
}

bool Node::isNode()
{
	return !this->isLeaf();
}

int Node::descendantsCount() const
{
	int count = children.size();
	for (auto& child : children)
	{
		count += child->descendantsCount();
	}
	return count;
}


/*

int Node::cmpNodes(Node* cmpNode) {
	if (this->isLeaf() && cmpNode->isLeaf()) {
		return 0;
	}
	// Если в первом дереве - лист, а во втором - узел, считать разность равной кол-ву потомков узла
	if (this->isLeaf() && cmpNode->isNode()) {
		return cmpNode->descendantsCount();
	}
	// Если в первом дереве - узел, а во втором лист, считать сравнение невозможным.
	if (this->isNode() && cmpNode->isLeaf()) {
		return -1;
	}
	// Если эти дети - узлы, найти их разность
	if (this->isNode() && cmpNode->isNode()) {
		return this->findDeltaPairs(cmpNode);
	}
	return -1;
}
*/

/*! Нахождение пары поддеревьев с минимальной разностью
\param[in] cmpTree Сравниваемое дерево
\return Пары поддеревьев
*/

/*
int Node::findDeltaPairs(const Node* cmpTree)
{
	map<Node*, vector<pair<int, Node*>>> patch;
	int curDelta = -1;
	Node* curCmpChild = nullptr;
	vector<pair<int, Node*>> similarTrees;
	if (this->descendantsCount() > cmpTree->descendantsCount()) {
		return -1;
	}
	// Для каждой пары детей в первом и втором дереве
	for (auto& cmpTreeChild : cmpTree->children) {
		similarTrees.clear();
		for (auto& treeChild : this->children) {
			// Если у детей одинаковые имена, вычислить их разность
			if (treeChild->getName() == cmpTreeChild->getName()) {
				curDelta = treeChild->cmpNodes(cmpTreeChild.get());
				similarTrees.push_back(make_pair(curDelta, treeChild.get()));
			}
		}
		if (similarTrees.empty()) {
			curDelta = -1;
			similarTrees.push_back(make_pair(curDelta, nullptr));
		}
		// Соотнести вычисленные разности каждого поддерева сравниваемого дерева
		// и текущее поддерево главного дерева
		if (patch.find(cmpTreeChild.get()) != patch.end()) {
			vector<pair<int, Node*>>& existingVector = patch[cmpTreeChild.get()];
			vector<pair<int, Node*>> newElements = similarTrees;
			existingVector.insert(existingVector.end(), newElements.begin(), newElements.end());
		}
		else {
			// curTreeNode отсутствует в словаре patch, создаем новую запись
			patch[cmpTreeChild.get()] = similarTrees;
		}
	}

	for (auto& cmpTreeChild : cmpTree->children) {
		curDelta += cmpTreeChild->findMinDeltaPatch(patch).first;
	}

	return curDelta;
}
*/





string Node::getName() const
{
	return this->name;
}

void Node::print(int level) const
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

PatchNode::PatchNode()
{
	this->rootSubTree = nullptr;
}

PatchNode::PatchNode(Node* rootSubTree)
{
	this->rootSubTree = rootSubTree;
}

PatchConnection::PatchConnection(Node* searchedSubTree) {
	this->searchedSubTree = searchedSubTree;
	this->weight = -2;
}

PatchConnection::PatchConnection(int weight, Node* searchedSubTree)
{
	this->weight = weight;
	this->searchedSubTree = searchedSubTree;
}

void PatchConnection::setWeight(int weight) {
	this->weight = weight;
}

int PatchConnection::getWeight()
{
	return this->weight;
}

Node* PatchConnection::getSearchedSubTree()
{
	return this->searchedSubTree;
}

void PatchNode::addConnection(unique_ptr<PatchConnection> newConnection)
{
	this->connections.push_back(move(newConnection));
}

void PatchNode::addConnection(int weight, Node* searchedSubTree)
{
	auto newConnection = make_unique<PatchConnection>(weight, searchedSubTree);
	this->connections.push_back(move(newConnection));
}

vector<unique_ptr<PatchConnection>> PatchNode::getConnections()
{
	return move(this->connections);
}


Node* PatchNode::getRoot()
{
	return this->rootSubTree;
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
		string msg = "Найден недопустимый символ \'" + to_string(symbol) + "\'" + "в файле \'" + filename + "\'";
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
		string msg = "В файле \'" + filename + "\'" + "содержится более 1 дерева";
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

	ifstream in(path); // окрываем файл для чтения
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
	// Найти первое вхождение одного из разделителей
	int word_end = str.find_first_of(delimiters, startIndex);

	// Считать, что слово кончилось, когда встречен первый разделитель
	return str.substr(startIndex, word_end - startIndex);
}

vector<Lexem> strToLexems(const string& content, string delimiters)
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

unique_ptr<Node> parseOnTree(const string& content, const string& delimiters, int startIndex)
{
	vector<Lexem> lexems = strToLexems(content, delimiters);
	return sexpToTree(lexems, startIndex);
}

int main()
{
	setlocale(LC_ALL, "Russian");
	string content;
	string path{ R"(C:\Users\barten\Documents\eztree.txt)" };

	readFile(path, &content);
	string delimiters = "() \t\n\r";
	cout << content << endl;

	//string c1 = "1(2(3 4) 5(6(7) 8) 9)";
	//string c2 = "1(2(3(4 5) 6) 5(8(9) 10) 11)";

	string c1 = "1(3(5 6) 3(5(7) 6) 4)";
	string c2 = "1(3(5(7 8) 6) 3(5(7) 6) 4)";

	auto tree1 = parseOnTree(c1, delimiters);
	auto tree2 = parseOnTree(c2, delimiters);

	vector<unique_ptr<PatchNode>> patchNodes;
	cout << tree1->buildPatch(tree2.get(), patchNodes) << endl;

	for (auto& el : patchNodes) {
		cout << "For root " + el->getRoot()->getName() << endl;
		cout << "\t Connections:" << endl;
		vector<unique_ptr<PatchConnection>>curConnections = el->getConnections();
		for (auto& con : curConnections) {
			cout << "\t\t Weight is " << con->getWeight() << " comparing with " << con->getSearchedSubTree()->getName() << endl;
		}
	}
}


