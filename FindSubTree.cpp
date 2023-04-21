
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
		if (this->child == nullptr)
			this->child = move(newChild);
		else
			this->child->addSibling(move(newChild));
	}

	void addSibling(unique_ptr<Node> newSibling)
	{

		if (this->sibling == nullptr)
			this->sibling = move(newSibling);
		else
			this->sibling->addSibling(move(newSibling));
	}

	Node* searchNode(const string& searchData, int entry) {
		queue<Node*> q;
		q.push(this);
		int curEntry = 0;

		// Пока очередь не пуста
		while (!q.empty()) {
			// Изъять первый узел из очереди
			Node* current = q.front();
			q.pop();

			// Если название текущего и искомого узла совпали, считать этот узел очередным вхождением
			if (current->getName() == searchData) {
				curEntry++;
				// Считать, что узел найден, если текущее вхождение = запрашиваемому вхождению
				if (curEntry == entry) {
					return current;
				}
			}
			// Добавить потомков в очередь, если они есть
			if (current->child != nullptr) {
				q.push(current->child.get());
			}
			if (current->sibling != nullptr) {
				q.push(current->sibling.get());
			}
		}
		return nullptr;
	}

	Node* findInSiblings(const string& targetName)
	{
		while (this->sibling != nullptr)
		{
			if (this->sibling->getName() == targetName)
				return this;
			this->sibling->findInSiblings(targetName);
		}
		return nullptr;
	}

	unique_ptr<Node> copy() const
	{
		auto root = make_unique<Node>(this->name);

		if (this->child != nullptr) {
			root->addChild(this->child->copy());
		}
		if (this->sibling != nullptr) {
			root->addSibling(this->sibling->copy());
		}

		return root;
	}

	void print(int level = 0) const
	{
		for (int i = 0; i < level; ++i)
			cout << "-";
		cout << this->name << endl;

		if (this->child != nullptr)
			child->print(level + 1);
		if (this->sibling != nullptr)
			sibling->print(level);

	}

	void deleteNode(Node* node)
	{
		return;
	}

	// Возвращает список всех ближайших детей
	list<Node*> getNearestChildren()
	{
		list<Node*> children;
		Node* curNode = this;
		if (curNode->child != nullptr)
		{
			curNode = curNode->child.get();
			children.push_back(curNode);
			curNode = curNode->sibling.get();
			while (curNode != NULL)
			{
				children.push_back(curNode);
				curNode = curNode->sibling.get();
			}
		}
		return children;
	}

	


	int compare(unique_ptr<Node>& subTree, unique_ptr<Node> deltaTree)
	{

	}



	/*! Найти поддерево
	\param[in] subTree Искомое поддерево
	\return Недостающее поддерево
	*/
	unique_ptr<Node> findSubTree(const unique_ptr<Node>& subTree)
	{
		int subTreeRootEntry(1);

		Node* foundSubTreeRoot = this->searchNode(subTree->getName(), subTreeRootEntry++);

		while (foundSubTreeRoot != nullptr)
		{
			// Найти в дереве узел, название которого совпадает с названием корневого узла в искомом поддереве
			this->searchNode(subTree->getName(), subTreeRootEntry);

			// Сравнить искомое поддерево с поддеревом, корнем которого является найденный узел


			// Если результат сравнения отрицательный, перейти к следующему вхождению корневого узла поддерева
			// Иначе считать, что поддерево найдено
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
	unique_ptr<Node> child;
	unique_ptr<Node> sibling;
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

vector<Lexem> parseOnLexems(string content, string delimiters)
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

	tree->print();

}