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
Node* Node::addChild(unique_ptr<Node> newChild)
{
	Node* addedChild = newChild.get();
	this->children.push_back(move(newChild));
	return addedChild;
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

/**
 * Вставка в дерево одного потомка вместо другого.
 * \param[in,out] this Дерево, в которое производится вставка
 * \param[in] removingChild Удаляемый узел
 * \param[in] insertingNode Замена удаленному узлу
 * \return Указатель на новый узел
 */
Node* Node::insertDescendant(const Node* removingChild, unique_ptr<Node>& insertingNode)
{
	Node* insertedChild = nullptr;
	for (const auto& child : this->children) {
		if (child.get() == removingChild) {
			this->removeChild(child.get());
			return this->addChild(move(insertingNode));
		}
		insertedChild = child->insertDescendant(removingChild, insertingNode);
		if (insertedChild != nullptr)
			return insertedChild;
	}
	return nullptr;
}

/**
 * Строит дерево разности, содержащее узлы, которых не хватает главному дереву для появления в нем поддерева, совпадающего с искомым деревом
 * \param[in] this Главное дерево
 * \param[in] cmpTree Искомое дерево
 * \param[out] deltaTree Дерево разности
 * \return Количество нехватающих узлов в главном дереве
 */
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

	return patch->getConnections()[0].second;
}

/**
 * Создание родословной узла(пути по дереву от потомка до самого старшего родителя).
 * \param[in] this Дерево, в котором ищется родословная
 * \param[in] searchedChild Узел, для которого составляется родословная
 * \return Родословная узла
 */
unique_ptr<Node> Node::buildPedigree(const Node* searchedChild, Node** deepestChild) const
{
	unique_ptr<Node> foundNode;
	unique_ptr<Node> pedigree = make_unique<Node>(this->getName());
	
	if (this == searchedChild) {
		*deepestChild = pedigree.get();
		return move(pedigree);
	}

	for (const auto& treeChild : children) {
		foundNode = treeChild->buildPedigree(searchedChild, deepestChild);
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

			// Считать что узлы идентичны, если они являются
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
int Node::findSubTree(const Node* cmpTree, unique_ptr<Node>& deltaTree) const {
	vector<const Node*> probableCmpTrees = this->findDescendants(cmpTree->getName());
	int curDeltaValue;
	const Node* minTree = nullptr;
	unique_ptr<Node> curDeltaTree;
	unique_ptr<Node> minDeltaTree;
	int minDelta = INT_MAX;

	// Найти минимальное дерево разности среди узлов, имя котороых совпадает с именем корня искомого дерева
	for (const auto& tree : probableCmpTrees) {
		curDeltaValue = tree->buildDeltaTreeWrap(cmpTree, curDeltaTree);
		if (curDeltaValue != -1 && curDeltaValue < minDelta) {
			minTree = tree;
			minDelta = curDeltaValue;
			minDeltaTree = move(curDeltaTree);
		}
	}

	// Если ни одного дерева разности не было найдено, считать результат поиска отрицательным
	if (minDelta == INT_MAX) {
		deltaTree = nullptr;
		return -1;
	}

	// Если минимальное дерево разности не содержит ни одного узла, считать поиск успешным
	if (minDeltaTree.get() == nullptr) {
		return minDelta;
	}

	// Иначе от корня минимально дерева разности построить родословную до корня главного дерева
	Node* removingChild = nullptr;
	auto parents = this->buildPedigree(minTree, &removingChild);

	parents->insertDescendant(removingChild, minDeltaTree);
	
	deltaTree = move(parents);

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

	for (auto patchChild : this->getChildren()) {
		auto& curConnections = patchChild->connections;
		curMinConnectionIndex = patchChild->findMinValidConnection();

		if (curMinConnectionIndex == -1)
			return -1;

		auto curConnectionToDelete = curConnections[curMinConnectionIndex];
		if (curConnectionToDelete.second == 0) {
			this->deleteAllChildReferences(curConnectionToDelete.first);
			cmpTree->removeChild(curConnectionToDelete.first);
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
 * Удаляет все соединения ведущие к указанному узлу из дочерних patch-узлов.
 * \param this Родительский patch-узел
 * \param selectedNode Выбранный узел
 * \return Количество удаленных соединений
 */
int PatchNode::deleteAllChildReferences(const Node* selectedNode) const
{
	int deletedConnectionsCount = 0;
	for (const auto& patchChild : this->children) {
		auto& curConnections = patchChild->connections;
		for (auto conIt = curConnections.begin(); conIt != curConnections.end();) {
			if ((*conIt).first == selectedNode) {
				conIt = curConnections.erase(conIt);
				deletedConnectionsCount++;
			}
			else {
				++conIt;
			}
		}
	}
	return deletedConnectionsCount;
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
	explicit ExcForbiddenSymbol(unsigned char symbol)
	{
		this->symbol = symbol;
	}

	const char* what() const noexcept override
	{
		string msg = "Detected invalid character \'" + to_string(symbol) + "\'" + "in the file \'" + filename + "\'";
		return msg.c_str();
	}

	void setFilename(const string& filename)
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
		string msg = "There are more than one tree in the file \'" + filename + "\'";
		return msg.c_str();
	}
protected:
	string filename;
};

class ExcBadBrackets : public std::exception
{
public:
	explicit ExcBadBrackets(int bracketBalance)
	{
		this->bracketBalance = bracketBalance;
	}
	const char* what() const noexcept override
	{
		msg = "The balance of brackets is off: ";

		if (bracketBalance > 0) {
			msg.append("Opening brackets are ");
			msg.append(to_string(bracketBalance));
		}
		else {
			msg.append("Closing brackets are ");
			msg.append(to_string(abs(bracketBalance)));
		}
		msg.append(" more");

		return msg.c_str();
	}
protected:
	int bracketBalance;
	mutable std::string msg;
};
class Lexem {
public:
	explicit Lexem(LexemType type)
	{
		this->nodeType = type;
	}
	Lexem(LexemType type, const string& nodeName)
	{
		this->nodeType = type;
		this->nodeName = nodeName;
	}
	string getName() const
	{
		if (nodeType == LexemType::Node)
			return this->nodeName;
		else if (nodeType == LexemType::LeftBracket)
			return "LEFT_BRACKET";
		else if (nodeType == LexemType::RightBracket)
			return "RIGHT_BRACKET";
		return "Unknown";
	}
	LexemType getType() const
	{
		return this->nodeType;
	}
protected:
	LexemType nodeType;
	string nodeName;
};



bool readFile(const string& path, string& content)
{
	bool success = false;
	string line;

	ifstream in(path); // îêðûâàåì ôàéë äëÿ ÷òåíèÿ
	if (in.is_open())
	{
		// ñ÷èòûâàåì òåêñòîâûé ôàéë è çàïèñûâàåì åãî â îäíó ñòðîêó
		while (getline(in, line))
		{
			content += line;
		}
		success = true;
	}
	in.close();     // çàêðûâàåì ôàéë
	return success;
}

string extractWord(const string& str, unsigned startIndex, const string& delimiters)
{
	// Найти конец слова
	auto word_end = str.find_first_of(delimiters, startIndex);

	// Вырезать слово
	return str.substr(startIndex, word_end - startIndex);
}

int countLexemOfType(const vector<Lexem>& lexems, LexemType targetType) 
{
	int targetLexemsCount = 0;
	for (const auto& lexem : lexems) {
		if (lexem.getType() == targetType)
			targetLexemsCount++;
	}
	return targetLexemsCount;
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
			lexems.emplace_back(LexemType::LeftBracket);
		}
		else if (curSymbol == ')')
		{
			lexems.emplace_back(LexemType::RightBracket);
		}
		else if (isalnum(curSymbol))
		{
			string nodeName = extractWord(content, i, delimiters);
			lexems.emplace_back(LexemType::Node, nodeName);
			i += nodeName.length() - 1;
		}
		else if (delimiters.find(curSymbol) == std::string::npos) 
		{
			ExcForbiddenSymbol exception(curSymbol);
			throw exception;
		}
		
	}
	int bracketBalance = countLexemOfType(lexems, LexemType::LeftBracket) - countLexemOfType(lexems, LexemType::RightBracket);
	if (bracketBalance != 0) {
		ExcBadBrackets exception(bracketBalance);
		throw exception;
	}
	return lexems;
}

unique_ptr<Node> sexpToTree(vector<Lexem>& lexems, int& index) {

	auto root = make_unique<Node>(lexems[index].getName());
	index++;
	while (index < lexems.size()) {
		Lexem curLexem = lexems[index];
		Lexem nextLexem(LexemType::Unknown);
		if (index < lexems.size() - 1)
			nextLexem = lexems[index + 1];

		if (curLexem.getType() == LexemType::Node)
		{
			unique_ptr<Node> child;

			if (nextLexem.getType() == LexemType::LeftBracket)
				child = sexpToTree(lexems, index);
			else
				child = make_unique<Node>(curLexem.getName());

			root->addChild(move(child));
			index++;
		}
		else if (curLexem.getType() == LexemType::RightBracket)
			return root;
		else
			index++;
	}
	return root;
}

unique_ptr<Node> parseOnTree(const string& content, const string& delimiters, int startIndex)
{
	unique_ptr<Node> builtTree;
	try {
		vector<Lexem> lexems = strToLexems(content, delimiters);
		builtTree = sexpToTree(lexems, startIndex);
	}
	catch (ExcBadBrackets& bracketException) {
		throw bracketException;
	}
	catch (ExcForbiddenSymbol& symbolException) {
		throw symbolException;
	}
	catch (...) {
		throw "Unknown error";
	}
	return builtTree;
}


int main(int argc, char* argv[])
{
	if (argc != 3) {
		cout << "There must be 2 command-line arguments(recieved "<< to_string(argc - 1) <<") : \n\t1.path to main tree \n\t2.path to searched tree \n\t3.path to result delta tree";
		return -1;
	}
	

	string mainTreeNote, searchedTreeNote;
	string mainTreePath = argv[1];
	string searchedTreePath = argv[2];
	
	readFile(mainTreePath, mainTreeNote);
	readFile(searchedTreePath, searchedTreeNote);
	string delimiters = "() \t\n\r";

	unique_ptr<Node> mainTree, searchedTree, deltaTree;
	try {
		mainTree = parseOnTree(mainTreeNote, delimiters);
	}
	catch (ExcBadBrackets& bracketException) {
		cout << bracketException.what() << endl;
	}
	catch (ExcForbiddenSymbol& symbolException) {
		symbolException.setFilename(mainTreePath);
		cout << symbolException.what() << endl;
	}
	catch (...) {
		cout << "Can't parse file '" + mainTreePath + "'" << endl;
		return -1;
	}

	try {
		searchedTree = parseOnTree(searchedTreeNote, delimiters);
	}
	catch (ExcBadBrackets& bracketException) {
		cout << bracketException.what() << endl;
	}
	catch (ExcForbiddenSymbol& symbolException) {
		symbolException.setFilename(searchedTreePath);
		cout << symbolException.what() << endl;
	}
	catch (...) {
		cout << "Can't parse file '" + searchedTreePath + "'" << endl;
		return -1;
	}

	int delta = mainTree->findSubTree(searchedTree.get(), deltaTree);
	if (delta != -1 && deltaTree.get() == nullptr) {
		cout << "The searched tree is completely contained in the given tree.";
	}
	else if (delta == -1 && deltaTree.get() == nullptr) {
		cout << "The searched tree is not in the given tree.";
	}
	else if (deltaTree.get() != nullptr) {
		cout << "The searched tree is partially contained in the given tree. \nCorresponding delta tree:" << endl;
		cout << delta << endl;
		deltaTree->print();
	}

}