
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <locale.h>
#include <list>



using namespace std;

enum LEXEM_TYPE
{
	NODE, LEFT_BRACKET, RIGHT_BRACKET, UNKNOWN
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

	void deleteNode(Node* node)
	{
		return;
	}

	bool sameNamed(const Node* leaf)
	{
		if (this->getName() != leaf->getName())
			return false;
		else
			return true;
	}

	bool isLeaf()
	{
		if (this->children.size() == 0)
			return true;

		return false;
	}

	bool isNode()
	{
		return !this->isLeaf();
	}

	int allDescendantsCount()
	{
		for (auto& child : children)
		{
			return children.size() + child->allDescendantsCount();
		}
		return 0;
	}

	int compare()
	{

	}

	/*! ����� ���������
	\param[in] subTree ������� ���������
	\return ����������� ���������
	*/
	unique_ptr<Node> findSubTree(const unique_ptr<Node>& subTree)
	{
		int subTreeRootEntry(1);

		Node* foundSubTreeRoot = this->searchNode(subTree->getName(), subTreeRootEntry++);

		while (foundSubTreeRoot != nullptr)
		{
			// ����� � ������ ����, �������� �������� ��������� � ��������� ��������� ���� � ������� ���������
			this->searchNode(subTree->getName(), subTreeRootEntry);

			// �������� ������� ��������� � ����������, ������ �������� �������� ��������� ����


			// ���� ��������� ��������� �������������, ������� � ���������� ��������� ��������� ���� ���������
			// ����� �������, ��� ��������� �������
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
		string msg = "������ ������������ ������ \'" + to_string(symbol) + "\'" + "� ����� \'" + filename + "\'";
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
		string msg = "� ����� \'" + filename + "\'" + "���������� ����� 1 ������";
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

	ifstream in(path); // �������� ���� ��� ������
	if (in.is_open())
	{
		// ��������� ��������� ���� � ���������� ��� � ���� ������
		while (getline(in, line))
		{
			*content += line;
		}
		success = true;
	}
	in.close();     // ��������� ����
	return success;
}

string extractWord(string str, unsigned startIndex, string delimiters)
{
	// ����� ������ ��������� ������ �� ������������
	int word_end = str.find_first_of(delimiters, startIndex);

	// �������, ��� ����� ���������, ����� �������� ������ �����������
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

// ���������
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

	tree->print();

}