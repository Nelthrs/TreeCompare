#include "findSubTree.h"

using namespace std;

const string GRAPHVIZ_PATH = "dot";

Node::Node(const string& data)
{
	this->name = data;
}

void Node::addChild(unique_ptr<Node> newChild)
{
	this->children.push_back(move(newChild));
}

Node* Node::addChild(const string& newChildName)
{
	Node* addedChild = nullptr;
	unique_ptr<Node> newChild = make_unique<Node>(newChildName);
	addedChild = newChild.get(); 
	this->addChild(move(newChild));
	return addedChild;
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

bool Node::isNode() const
{
	return !this->isLeaf();
}

bool Node::isChild(const Node* probablyChild) const {
	return any_of(children.begin(), children.end(), [probablyChild](const auto& child) -> bool {
		return probablyChild == child.get();
	});
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

vector<Node*> Node::getChildren() const {
	vector<Node*> result;
	for (const auto& child : children) {
		result.push_back(child.get());
	}
	return result;
}

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

bool Node::isDescendant(const Node* searchedNode) const 
{
	if (this == searchedNode)
		return true;

	for (auto& child : children) {
		if (child->isDescendant(searchedNode))
			return true;
	}

	return false;
}

int Node::buildDeltaTreeWrap(Node* cmpTree, unique_ptr<Node>& deltaTree)
{
	auto cmpTreeCopy = cmpTree->copy();
	auto patch = this->buildPatchWrap(cmpTreeCopy.get());

	if (patch->buildDeltaTree(cmpTreeCopy.get()) == -1) {
		deltaTree = nullptr;
		return -1;
	}
	
	if (cmpTreeCopy->descendantsCount() == 0) {
		deltaTree = nullptr;
	}
	else {
		deltaTree = move(cmpTreeCopy);
	}

	return 0;
}

void Node::removeChild(const Node* nodeToDelete)
{
	for (auto it = children.begin(); it < children.end(); ++it) {
		if (nodeToDelete == (*it).get()) {
			children.erase(it);
			return;
		}		
	}
}

unique_ptr<PatchNode> Node::buildPatchWrap(Node* cmpTree) {
	auto patch = make_unique<PatchNode>(this);
	int rootConWeight = this->buildPatch(cmpTree, patch.get());
	patch->addConnection(rootConWeight, cmpTree);
	return move(patch);
}

int Node::buildPatch(const Node* cmpTree, PatchNode* patch) const {
	unique_ptr<PatchNode> curPatchNode;
	int curWeight;
	for (const auto& mainChild : this->children) {
		curPatchNode = make_unique<PatchNode>(mainChild.get());
		for (const auto& cmpChild : cmpTree->children) {
			if (!(mainChild->getName() == cmpChild->getName())) {
				continue;
			}

			if (mainChild->isLeaf() && cmpChild->isLeaf()) {
				curWeight = 0;
			}
			else if (mainChild->isLeaf() && cmpChild->isNode()) {
				curWeight = cmpChild->descendantsCount();
			}
			else if (mainChild->isNode() && cmpChild->isLeaf()) {
				curWeight = -1;
			}
			else if (mainChild->isNode() && cmpChild->isNode()) {
				curWeight = mainChild->buildPatch(cmpChild.get(), curPatchNode.get());
			}

			curPatchNode->addConnection(curWeight, cmpChild.get());
		}

		// Если в главном дереве есть узлы, на которых не нашлось узла из cmpTree, то сравнение невозможно
		if (curPatchNode->getConnections().empty())
			return -1;

		patch->addChild(move(curPatchNode));
	}


	int currentMinConnectionIndex;
	int minSumConnections = 0;

	// Если количество детей patch не равно количеству детей в узле сравниваемого дерева
	if (patch->getChildren().size() < cmpTree->children.size()) {
		auto uncaughtChildren = patch->findUncaughtChildren(cmpTree);
		for (const auto& child : uncaughtChildren) {
			minSumConnections += 1 + child->descendantsCount();
		}
	}

	for (const auto& patchChild : patch->getChildren()) {
		currentMinConnectionIndex = patchChild->findMinValidConnection();

		if (currentMinConnectionIndex == -1)
			return -1;

		minSumConnections += patchChild->getConnections()[currentMinConnectionIndex].second;
	}

	return minSumConnections;
}


// this - cmpTree
int PatchNode::buildDeltaTree(Node* cmpTree) const
{
	int curMinConnectionIndex;
	vector<pair<Node*, int>> curConnections;

	for (auto patchChild : this->getChildren()) {
		curConnections = patchChild->getConnections();
		curMinConnectionIndex = patchChild->findMinValidConnection();

		if (curMinConnectionIndex == -1)
			return -1;

		
		if (curConnections[curMinConnectionIndex].second == 0) {
			cmpTree->removeChild(curConnections[curMinConnectionIndex].first);
		}
		else {
			if (patchChild->buildDeltaTree(curConnections[curMinConnectionIndex].first) == -1)
				return -1;
		}
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

PatchNode* PatchNode::addChild(unique_ptr<PatchNode> newChild)
{
	this->children.push_back(move(newChild));
	return (*(children.end() - 1)).get();
}

void PatchNode::addConnection(int weight, Node* searchedSubTree)
{
	auto newPair = make_pair(searchedSubTree, weight);
	for (auto conIt = this->connections.begin(); conIt < this->connections.end(); ++conIt) {
		if ((*conIt).second > weight) {
			this->connections.insert(conIt, newPair);
			return;
		}
	}
	this->connections.push_back(newPair);
}

Node* PatchNode::getRoot() const
{
	return this->rootSubTree;
}

vector<PatchNode*> PatchNode::getChildren() const
{
	vector<PatchNode*> resChildren;
	for (const auto& child : this->children) {
		resChildren.push_back(child.get());
	}
	return resChildren;
}

vector<pair<Node*, int>>::const_iterator PatchNode::findConnection(const Node* searchedNode) const
{
	for (auto i = this->connections.begin(); i < this->connections.end(); i++) {
		if ((*i).first == searchedNode) {
			return i;
		}
	}
	return this->connections.end();
}

// Найти соединение с минимальным весом(но не -1) и вернуть индекс этого соединения
int PatchNode::findMinValidConnection(int startIndex) const
{
	for (int i = startIndex; i < this->connections.size(); i++) {
		if (this->connections[i].second != -1)
			return i;
	}
	return -1;
}

vector<pair<Node*, int>> PatchNode::getConnections() const
{
	return connections;
}

// Возвращает массив детей дерева, на которых не нашлось узлов патча среди детей текущего patchNode
vector<Node*> PatchNode::findUncaughtChildren(const Node* treeNode) const
{
	vector<Node*> uncaughtChildren = treeNode->getChildren();
	for (const auto& patchChild : this->children) {
		for (const auto& patchChildConnection : patchChild->getConnections()) {
			if (!treeNode->isChild(patchChildConnection.first)) {
				continue;
			}
			auto it = std::find_if(uncaughtChildren.begin(), uncaughtChildren.end(), [patchChildConnection](const Node* child) {
				return patchChildConnection.first == child;
				});
			if (it != uncaughtChildren.end())
				uncaughtChildren.erase(it);
		}
	}
	return uncaughtChildren;
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
		string msg = "Íàéäåí íåäîïóñòèìûé ñèìâîë \'" + to_string(symbol) + "\'" + "â ôàéëå \'" + filename + "\'";
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
		string msg = "Â ôàéëå \'" + filename + "\'" + "ñîäåðæèòñÿ áîëåå 1 äåðåâà";
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



bool readFile(const string& path, string* content)
{
	bool success = false;
	string line;

	ifstream in(path); // îêðûâàåì ôàéë äëÿ ÷òåíèÿ
	if (in.is_open())
	{
		// ñ÷èòûâàåì òåêñòîâûé ôàéë è çàïèñûâàåì åãî â îäíó ñòðîêó
		while (getline(in, line))
		{
			*content += line;
		}
		success = true;
	}
	in.close();     // çàêðûâàåì ôàéë
	return success;
}

string extractWord(const string& str, unsigned startIndex, const string& delimiters)
{
	// Íàéòè ïåðâîå âõîæäåíèå îäíîãî èç ðàçäåëèòåëåé
	auto word_end = str.find_first_of(delimiters, startIndex);

	// Ñ÷èòàòü, ÷òî ñëîâî êîí÷èëîñü, êîãäà âñòðå÷åí ïåðâûé ðàçäåëèòåëü
	return str.substr(startIndex, word_end - startIndex);
}

vector<Lexem> strToLexems(const string& content, const string& delimiters)
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

// Картинки
void generateDotFile(const PatchNode* patch, std::ostream& dotStream) {
	dotStream << "digraph Patch {" << std::endl;

	std::set<Node*> visitedNodes;
	std::stack<const PatchNode*> nodeStack;

	// Рекурсивно обходите PatchNode и его детей
	nodeStack.push(patch);
	while (!nodeStack.empty()) {
		const PatchNode* currNode = nodeStack.top();
		nodeStack.pop();

		// Добавьте узел в файл DOT, если он еще не посещен
		if (visitedNodes.find(currNode->getRoot()) == visitedNodes.end()) {
			visitedNodes.insert(currNode->getRoot());
			dotStream << "    \"" << currNode->getRoot()->getName() << ": Patch\";" << std::endl;
		}

		// Добавьте соединения между текущим узлом и его детьми с подписями весов
		for (const auto& connection : currNode->getConnections()) {
			dotStream << "    \"" << currNode->getRoot()->getName() << ": Patch\" -> \"" << connection.first->getName() << "\" [label=\"" << connection.second << "\"];" << std::endl;
		}

		// Добавьте соединения между текущим узлом и его детьми PatchNode
		for (const auto& child : currNode->getChildren()) {
			dotStream << "    \"" << currNode->getRoot()->getName() << ": Patch\" -> \"" << child->getRoot()->getName() << ": Patch\" [style=dotted];" << std::endl;
		}

		// Поместите всех детей текущего узла в стек для обхода
		for (const auto& child : currNode->getChildren()) {
			nodeStack.push(child);
		}
	}

	dotStream << "}" << std::endl;
}


bool generatePngFromDotContent(const std::string& dotContent, const std::string& pngFileName) {
	// Создайте временный файл с содержимым DOT
	std::string dotFileName = "temp.dot";
	std::ofstream dotFile(dotFileName);
	dotFile << dotContent;
	dotFile.close();

	// Запустите команду dot для создания PNG из DOT
	std::string command = "dot -Tpng " + dotFileName + " -o " + pngFileName;
	int result = std::system(command.c_str());

	// Удалите временный файл DOT
	std::filesystem::remove(dotFileName);

	// Верните true, если конвертация прошла успешно
	return (result == 0);
}

bool generatePngFromPatch(const PatchNode* patch, const std::string& pngFileName) {
	std::stringstream dotStream;
	generateDotFile(patch, dotStream);
	std::string dotContent = dotStream.str();

	return generatePngFromDotContent(dotContent, pngFileName);
}


int main()
{
	setlocale(LC_ALL, "ru_RU.UTF-8");
	string content;
	string path{ R"(C:\Users\barten\Documents\eztree.txt)" };
	 
	readFile(path, &content);
	string delimiters = "() \t\n\r";
	cout << content << endl;

	//string c1 = "1(2(3 4) 5(6(7) 8) 9)";
	//string c2 = "1(2(3(4 5) 6) 5(8(9) 10) 11)";

	string c1 = "1(3(5 6) 3(5(7) 6) 4)";
	string c2 = "1(3(5 6) 3(5(7) 6) 4)";

	//string c1 = "tractor(steering_wheel (right_half left_half) who)";
	//string c2 = "tractor(steering_wheel (right_half left_half) who)";

	//string c1 = "1(4 3(13 14) 4(5))";
	//string c2 = "1(3(10 11 12 13 14) 4 4(5))";
	auto tree1 = parseOnTree(c1, delimiters);
	auto tree2 = parseOnTree(c2, delimiters);
	unique_ptr<Node> deltaTree;

	auto patch = tree1->buildDeltaTreeWrap(tree2.get(), deltaTree);
	if (patch != -1)
		deltaTree->print();
	cout << patch;
}