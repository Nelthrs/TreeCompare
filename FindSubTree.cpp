#include "findSubTree.h"

using namespace std;

const string GRAPHVIZ_PATH = "dot";

/**
 * Создать узел
 * \param[in] data Имя нового узла
 */
Node::Node(const string& data)
{
	this->name = data;
}

/**
 * Добавить ребёнка к заданному узлу
 * \param[in] this Родительский узел
 * \param[in] newChild Новый ребёнок
 */
void Node::addChild(unique_ptr<Node> newChild)
{
	this->children.push_back(move(newChild));
}

/**
 * Добавить ребёнка к заданному узлу
 * \param[in] this Родительский узел
 * \param[in] newChildName Имя нового ребёнка
 * \return Указатель на созданного ребёнка
 */
Node* Node::addChild(const string& newChildName)
{
	Node* addedChild = nullptr;
	unique_ptr<Node> newChild = make_unique<Node>(newChildName);
	addedChild = newChild.get(); 
	this->addChild(move(newChild));
	return addedChild;
}

/**
 * Создать копию дерева
 * \param[in] this Копируемое дерево
 * \return Скопированное дерево
 */
unique_ptr<Node> Node::copy() const
{
	if (this == nullptr)
		return nullptr;

	auto root = make_unique<Node>(this->name);

	for (auto& child : children)
	{
		root->addChild(child->copy());
	}

	return root;
}

/**
 * Узнать, является ли этот узел - листом
 * \param[in] this Узел
 * \return Логический флаг, является ли листом
 */
bool Node::isLeaf() const
{
	if (this == nullptr)
		return false;

	if (this->children.size() == 0)
		return true;

	return false;
}

/**
 * Узнать, есть ли у этого узла потомки
 * \param[in] this Узел
 * \return Логический флаг, есть ли потомки
 */
bool Node::isNode() const
{
	if (this == nullptr)
		return false;

	return !this->isLeaf();
}

/**
 * Узнать, является ли узел ребёнком по отношению к данному узлу
 * \param[in] this Узел
 * \param[in] probablyChild Узел, подозреваемый в качестве ребёнка
 * \return Логический флаг, является ли узел ребёнком
 */
bool Node::isChild(const Node* probablyChild) const {
	return any_of(children.begin(), children.end(), [probablyChild](const auto& child) -> bool {
		return probablyChild == child.get();
	});
}

/**
 * Узнать количество потомков узла
 * \param[in] this Узел
 * \return количество потомков
 */
int Node::descendantsCount() const
{
	int count = children.size();
	for (auto& child : children)
	{
		count += child->descendantsCount();
	}
	return count;
}

/**
 * Поиск всех детей данного узла
 * \param[in] this Узел
 * \return Дети данного узла
 */
vector<Node*> Node::getChildren() const {
	vector<Node*> result;
	for (const auto& child : children) {
		result.push_back(child.get());
	}
	return result;
}

/**
 * Выведать название данного узла
 * \param[in] this Узел
 * \return Название данного узла
 */
string Node::getName() const
{
	return this->name;
}

/**
 * Печать дерева в консоль.
 * \param[in] this Узел
 * \param[in] level Стартовый отступ от левого края консоли
 */
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

/**
 * Поиск всех потомков узла с заданным именем.
 * \param[in] this Корень дерева, в котором производится поиск
 * \param[in] searchedNodeName Наименование искомых потомков
 * \return Найденные потомки
 */
vector<const Node*> Node::findDescendants(const string& searchedNodeName) const
{
	vector<const Node*> foundNodes;
	vector<const Node*> foundInChild;

	if (this->getName() == searchedNodeName) {
		foundNodes.push_back(this);
	}

	for (const auto& child : children) {
		foundInChild = child->findDescendants(searchedNodeName);
		foundNodes.insert(foundNodes.end(), foundInChild.begin(), foundInChild.end());
	}

	return foundNodes;
}

int Node::buildDeltaTreeWrap(const Node* cmpTree, unique_ptr<Node>& deltaTree) const
{
	unique_ptr<Node> cmpTreeCopy = cmpTree->copy();
	unique_ptr<PatchNode> patch = this->buildPatchWrap(cmpTreeCopy.get());

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

/**
 * Создание родословной узла(пути по дереву от потомка до самого старшего родителя).
 * \param[in] this Дерево, в котором ищется родословная
 * \param[in] searchedChild Узел, для которого составляется родословная
 * \return Родословная узла
 */
unique_ptr<Node> Node::buildPedigree(const Node* searchedChild) const
{
	unique_ptr<Node> foundNode;
	unique_ptr<Node> pedigree = make_unique<Node>(this->getName());
	
	if (this == searchedChild) {
		return move(pedigree);
	}

	for (const auto& treeChild : children) {
		foundNode = treeChild->buildPedigree(searchedChild);
		if (foundNode != nullptr) {
			pedigree->addChild(move(foundNode));
			return move(pedigree);
		}
	} 

	return nullptr;
}

/**
 * Функция удаляет указанного ребёнка узла
 * \param[in] this Узел
 * \param[in] nodeToDelete Удаляемый ребёнок узла
 */
void Node::removeChild(const Node* nodeToDelete)
{
	for (auto it = children.begin(); it < children.end(); ++it) {
		if (nodeToDelete == (*it).get()) {
			children.erase(it);
			return;
		}		
	}
}

/**
 * Инициализировать корневой Patch-узел и построить Patch дерево
 * \param[in] this Главное дерево
 * \param[in] cmpTree Сравниваемое дерево
 * \return Patch
 */
unique_ptr<PatchNode> Node::buildPatchWrap(Node* cmpTree) const {
	auto patch = make_unique<PatchNode>(this);
	int rootConWeight = this->buildPatch(cmpTree, patch.get());
	patch->addConnection(rootConWeight, cmpTree);
	return move(patch);
}

/**
 * Построить Patch дерево, с заданным корневым узлом
 * \param[in] this Главное дерево
 * \param[in] cmpTree Сравниваемое дерево
 * \param[in,out] patch Patch-дерево
 * \return Минимальное количество дополнительных узлов в главном дереве для полного совпадения со сравниваемым
 */
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
	if (this->getChildren().size() < cmpTree->children.size()) {
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

/**
 * Поиск поддерева и построение дерева разности.
 * \param[in] this Главное дерево, в котором проводится поиск
 * \param[in] cmpTree Искомое дерево
 * \param[out] deltaTree Дерево разности, содержающее узлы, которых не хватает главному дереву для появления в нем поддерева, совпадающего с искомым деревом
 * \return Количество узлов, которые необходимо добавить к главному дереву
 */
int Node::findSubTree(const Node* cmpTree, unique_ptr<Node>& deltaTree) {
	vector<const Node*> probableCmpTrees = this->findDescendants(cmpTree->getName());
	int curDeltaValue;
	const Node* minTree = nullptr;
	unique_ptr<Node> curDeltaTree;
	unique_ptr<Node> minDeltaTree;
	int minDelta = INT_MAX;

	for (const auto& tree : probableCmpTrees) {
		curDeltaValue = tree->buildDeltaTreeWrap(cmpTree, curDeltaTree);
		if (curDeltaValue != -1 && curDeltaValue < minDelta) {
			minTree = tree;
			minDelta = curDeltaValue;
			minDeltaTree = move(curDeltaTree);
		}
	}

	if (minDelta == INT_MAX) {
		deltaTree = nullptr;
		return -1;
	}

	
	auto parents = this->buildPedigree(minTree);

	auto curNode = parents.get();

	if (curNode->getChildren().empty()) {
		deltaTree = move(minDeltaTree);
		return minDelta;
	}

	while (!curNode->getChildren().empty() && !curNode->getChildren()[0]->getChildren().empty()) {
		curNode = curNode->getChildren()[0];
	}
	curNode->removeChild(curNode->getChildren()[0]);

	curNode->addChild(move(minDeltaTree));
	
	deltaTree = curNode->copy();

	return minDelta;
}

/**
 * Построение дерева разности на основе заданного Patch-дерева.
 * \param[in] this Patch-дерево 
 * \param[in, out] cmpTree Искомое дерево
 * \return Успешность построения дерева разности
 */
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
	return 0;
}


PatchNode::PatchNode()
{
	this->rootSubTree = nullptr;
}

PatchNode::PatchNode(Node* rootSubTree)
{
	this->rootSubTree = rootSubTree;
}

PatchNode::PatchNode(const Node* rootSubTree)
{
	this->rootSubTree = const_cast<Node*>(rootSubTree);
}

Node* PatchNode::getRoot() const
{
	return this->rootSubTree;
}

vector<pair<Node*, int>> PatchNode::getConnections() const
{
	return connections;
}

/**
 * Добавить ребёнка к patch-узлу
 * \param[in] this Patch-узел
 * \param[in] newChild Новый ребёнок
 * \return Указатель на добавленного ребёнка
 */
PatchNode* PatchNode::addChild(unique_ptr<PatchNode> newChild)
{
	this->children.push_back(move(newChild));
	return (*(children.end() - 1)).get();
}

/**
 * Добавить соединение к patch-узлу
 * \param[in] this Patch-узел
 * \param[in] weight Вес нового соединения
 * \param[in] searchedSubTree Целевое дерево
 */
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

/**
 * Получить список всех детей данного patch-узла
 * \param[in] this patch-узел
 * \return Список всех детей patch-узла
 */
vector<PatchNode*> PatchNode::getChildren() const
{
	vector<PatchNode*> resChildren;
	for (const auto& child : this->children) {
		resChildren.push_back(child.get());
	}
	return resChildren;
}

/**
 * Поиск исходящего из patch-узла соединения, указывающего на заданный узел дерева
 * \param[in] this patch-узел
 * \param[in] searchedNode Узел дерева
 * \return Соединения, связывающее patch-узел и узел дерева
 */
vector<pair<Node*, int>>::const_iterator PatchNode::findConnection(const Node* searchedNode) const
{
	for (auto i = this->connections.begin(); i < this->connections.end(); i++) {
		if ((*i).first == searchedNode) {
			return i;
		}
	}
	return this->connections.end();
}

/**
 * Поиск наименьшего валидного(с неотрицательным весом) соединения
 * \param this patch-узел, среди соединений которого производится поиск
 * \param startIndex Начальная позиция поиска
 * \return Индекс наименьшего валидного соедниения среди соединений patch-узла
 */
int PatchNode::findMinValidConnection(int startIndex) const
{
	for (int i = startIndex; i < this->connections.size(); i++) {
		if (this->connections[i].second != -1)
			return i;
	}
	return -1;
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
	setlocale(LC_ALL, "Russian");
	string content;
	string path{ R"(C:\Users\barten\Documents\eztree.txt)" };
	 
	readFile(path, &content);
	string delimiters = "() \t\n\r";
	cout << content << endl;

	//string treeMain = "1(2(3 4) 5(6(7) 8) 9)";
	//string cmpTree = "1(2(3(4 5) 6) 5(8(9) 10) 11)";

	//string treeMain = "1(3(5 6) 3(5(7) 6) 4)";
	//string cmpTree = "1(3(5 6) 3(5(7) 6) 4)";

	//string treeMain = "tractor(steering_wheel (right_half left_half) who)";
	//string cmpTree = "tractor(steering_wheel (right_half left_half) who)";

	// string treeMain = "1(4 3(13 14) 4(5))";
	// string cmpTree = "1(3(10 11 12 13 14) 4 4(5))";

	// wstring wstr = L"трактор";
	// wcout << wstr << endl;

	string treeMain = "tractor(wheel(bolts metal) wheel(tire bolts) wheel(metal crap bolts))";
	string cmpTree = "tractor(wheel(bolts metal tire) wheel(metal crap bolts tire) wheel(tire bolts))";

	auto tree1 = parseOnTree(treeMain, delimiters);
	auto tree2 = parseOnTree(cmpTree, delimiters);

	unique_ptr<Node> deltaTree;
	int delta = tree1->findSubTree(tree2.get(), deltaTree);
	cout << delta << endl;
	deltaTree->print();
	
}