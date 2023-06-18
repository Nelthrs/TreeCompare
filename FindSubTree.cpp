#include "findSubTree.h"

using namespace std;

const string GRAPHVIZ_PATH = "dot";

Node::Node(string data)
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

vector<Node*> Node::getChildren() const {
	vector<Node*> result;
	for (const auto& child : children) {
		result.push_back(child.get());
	}
	return result;
}

void Node::setPatchNode(PatchNode* newPatchNode)
{
	this->relPatch = newPatchNode;
}

PatchNode* Node::getRelPatch() const
{
	return this->relPatch;
}

int Node::buildPatch(Node* cmpTree, Patch* generalPatch) const
{
	int curConnectionDelta;
	int sumConnectionDelta = 0;
	std::unique_ptr<PatchNode> curPatch;
	std::unique_ptr<PatchConnection> curConnection;

	for (auto& mainChild : this->children) {
		curPatch = make_unique<PatchNode>(mainChild.get());
		mainChild->setPatchNode(curPatch.get());
		for (auto& cmpChild : cmpTree->children) {
			if (mainChild->getName() == cmpChild->getName()) {
				if (mainChild->isLeaf() && cmpChild->isLeaf()) {
					curConnectionDelta = 0;
				}
				else if (mainChild->isLeaf() && cmpChild->isNode()) {
					curConnectionDelta = cmpChild->descendantsCount();
				}
				else if (mainChild->isNode() && cmpChild->isLeaf()) {
					curConnectionDelta = -1;
				}
				else if(mainChild->isNode() && cmpChild->isNode()) {
					curConnectionDelta = mainChild->buildPatch(cmpChild.get(), generalPatch);
				}

				curConnection = make_unique<PatchConnection>(curConnectionDelta, cmpChild.get(), curPatch.get());
				curPatch->addConnection(move(curConnection));
			}
		}
		if (curPatch->getConnections().size() == 0) {
			return -1;
		}
		generalPatch->addNode(move(curPatch));
	}
	sumConnectionDelta += generalPatch->countDeltaNodes(cmpTree, this);
	return sumConnectionDelta;
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

vector<PatchConnection*> Node::getConnectionsForChildren(Patch* generalPatch) {
	vector<PatchConnection*> connections;
	for (auto patchNode : generalPatch->getPatch()) {
		for (auto con : patchNode->getConnections()) {
			for (auto& child : this->children) {
				if (con->getSearchedSubTree() == child.get())
					connections.push_back(con);
			}
		}
	}
	return connections;
}

// Найти минимальное валидное(не == -1) соединение, входящее в this
PatchConnection* Node::getMinValidConnection(vector<PatchConnection*> connections) 
{
	if (connections.size() < 1) {
		return nullptr;
	}

	PatchConnection* minCon = connections[0];
	for (auto con : connections) {
		if (minCon->getWeight() > con->getWeight() || minCon->getWeight() == -1)
			minCon = con;
	}

	return minCon;
}

bool compareByWeight(PatchConnection* conn1, PatchConnection* conn2) 
{
	return conn1->getWeight() < conn2->getWeight();
}

std::optional<vector<pair<Node*, PatchConnection*>>> Node::getMinConnectionPairs(Patch* generalPatch)
{
	auto connections = this->getConnectionsForChildren(generalPatch);
	std::sort(connections.begin(), connections.end(), compareByWeight);
	set<Node*> sortedChildren;												// Отсортированные дети в порядке возрастания весов входящих соединений
	pair<set<Node*>::iterator, bool> insertionResult;
	
	vector<pair<Node*, PatchConnection*>> conPairs;

	for (auto& con : connections) {
		if (con->getWeight() != -1) {
			insertionResult = sortedChildren.insert(con->getSearchedSubTree());
			if (insertionResult.second) {
				conPairs.push_back(make_pair(con->getSearchedSubTree(), con));
			}
		}
			
	}

	if (sortedChildren.size() != this->children.size()) {
		return nullopt;
	}

	return move(conPairs);
}

int Node::buildDeltaTreeWrap(Node* cmpTree, unique_ptr<Node>& deltaTree)
{
	unique_ptr<Patch> patch = make_unique<Patch>();
	int minPatch = this->buildPatch(cmpTree, patch.get());

	if (minPatch == -1)
		return -1;

	deltaTree = make_unique<Node>(this->getName());
	minPatch = cmpTree->buildDeltaTree(patch.get());
	return minPatch;
}

int Node::buildDeltaTree(Patch* generalPatch) {
	auto minAddPairs = this->getMinConnectionPairs(generalPatch);

	if (!minAddPairs.has_value())
		return -1;

	for (auto& pair : minAddPairs.value()) {
		generalPatch->deleteAllReferences(pair.first);
		if (pair.second->getWeight() == 0) {
			this->removeChild(pair.first);
		}
		else {
			pair.first->buildDeltaTree(generalPatch);
		}

	}
}

vector<pair<PatchConnection*, PatchNode*>> Node::getIncomingConnections(Patch* generalPatch)
{
	vector<pair<PatchConnection*, PatchNode*>> result;

	for (auto patchNode : generalPatch->getPatch()) {
		for (auto con : patchNode->getConnections()) {
			if (con->getSearchedSubTree() == this && con->getWeight() != -1) {
				result.push_back(make_pair(con, patchNode));
			}
		}
	}
	
	return result;
}

void Node::removeChild(Node* nodeToDelete)
{
	for (auto it = children.begin(); it < children.end(); ++it) {
		if (nodeToDelete == (*it).get()) {
			children.erase(it);
			return;
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

void PatchNode::addConnection(unique_ptr<PatchConnection> newConnection)
{
	this->connections.push_back(move(newConnection));
}

void PatchNode::addConnection(int weight, Node* searchedSubTree)
{
	auto newConnection = make_unique<PatchConnection>(weight, searchedSubTree, this);
	this->connections.push_back(move(newConnection));
}

Node* PatchNode::getRoot()
{
	return this->rootSubTree;
}

void PatchNode::addChild(unique_ptr<PatchNode> child)
{
	children.push_back(move(child));
}

vector<PatchConnection*> PatchNode::getConnections()
{
	vector<PatchConnection*> cons;
	for (auto& connection : this->connections) {
		cons.push_back(connection.get());
	}
	return cons;
}




PatchConnection::PatchConnection(int weight, Node* searchedSubTree, PatchNode* rootPatchNode)
{
	this->weight = weight;
	this->searchedSubTree = searchedSubTree;
	this->rootPatchNode = rootPatchNode;
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

PatchNode* PatchConnection::getRootPatchNode()
{
	return this->rootPatchNode;
}



Patch::Patch() {
	this->patchNodes.clear();
}

vector<PatchNode*> Patch::getPatch() const
{
	vector<PatchNode*> patch;
	for (auto& patchNode : this->patchNodes) {
		patch.push_back(patchNode.get());
	}
	return patch;
}

void Patch::deleteAllReferences(const Node* excludedNode) 
{
	vector<PatchConnection*> curCons;
	for (auto patchIter = this->patchNodes.begin(); patchIter < this->patchNodes.end(); ) {
		curCons = (*patchIter)->getConnections();

		// Удалить все соединения, указывающие на исключаемый узел
		for (auto i = curCons.begin(); i < curCons.end();) {
			if((*i)->getSearchedSubTree() == excludedNode) {
				i = curCons.erase(i);
			}
			else {
				++i;
			}
		}

		// Удалить текущий PatchNode если он не указывает ни на один узел искомого дерева
		if ((*patchIter)->getConnections().size() == 0)
			patchIter = this->patchNodes.erase(patchIter);
		else
			++patchIter;
	}
}

void Patch::addNode(unique_ptr<PatchNode> newNode)
{
	this->patchNodes.push_back(move(newNode));
}

int Patch::countDeltaNodes(const Node* pointTree, const Node* sourceTree) const {
	int sum = 0;
	bool patchFound = false;
	vector<pair<PatchNode*, PatchConnection*>> pointingNodes;

	// Для каждого ребенка узла смотрим, указывает ли на него какой-то Patch
	for (auto& child : pointTree->getChildren()) {
		patchFound = false;
		pointingNodes = this->findPointingNode(child);
		for (auto& ptrNode : pointingNodes) {
			// Если корень Patch находится в исходном дереве, то прибавляем вес соотвествующего соединения к общей сумме 
			if (sourceTree->isDescendant(ptrNode.first->getRoot())) {
				sum += ptrNode.second->getWeight();
				patchFound = true;
			}
		}
		// Если для данного ребенка нет соответсвующего Patch, то прибавляем к общем сумме 1 + кол-во его потомков
		if(!patchFound)
			sum += 1 + child->descendantsCount();

	}

	return sum;
}

vector<PatchNode*> Patch::findLeafPatches(Node* sourceTree ) {
	vector<PatchNode*> leafNodes;
	for (auto& node : this->patchNodes) {
		if (node->getConnections().size() == 0 && sourceTree->isDescendant(node->getRoot())) {
			leafNodes.push_back(node.get());
		}
	}
	return leafNodes;
}

// Проверяет, есть ли в Patch элементы(PatchNode) которые указывают на этот treeNode
vector<pair<PatchNode*, PatchConnection*>> Patch::findPointingNode(Node* pointNode) const {
	vector<pair<PatchNode*, PatchConnection*>> pointingNodes;
	vector<PatchConnection*> curCons;
	
	for (auto& node : this->patchNodes) {
		curCons = node->getConnections();
		for (auto& con : curCons) {
			if (pointNode == con->getSearchedSubTree())
				pointingNodes.push_back(make_pair(node.get(), con));
		}
	}

	return pointingNodes;
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



bool readFile(string path, string* content)
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

string extractWord(string str, unsigned startIndex, string delimiters)
{
	// Íàéòè ïåðâîå âõîæäåíèå îäíîãî èç ðàçäåëèòåëåé
	int word_end = str.find_first_of(delimiters, startIndex);

	// Ñ÷èòàòü, ÷òî ñëîâî êîí÷èëîñü, êîãäà âñòðå÷åí ïåðâûé ðàçäåëèòåëü
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

void generateDotFile(const Node* node, std::ofstream& file) {
	static int counter = 0;
	int current = counter++;

	file << "node" << current << " [label=\"" << node->getName() << "\"];" << std::endl;

	const std::vector<Node*>& children = node->getChildren();
	for (const auto& child : children) {
		int childNode = counter;
		generateDotFile(child, file);
		file << "node" << current << " -> node" << childNode << ";" << std::endl;
	}
}

void visualizeTree(const Node* root, const string filename = "tree.png") {
	std::ofstream file("tree.dot");
	file << "digraph Tree {" << std::endl;
	generateDotFile(root, file);
	file << "}" << std::endl;
	file.close();

	std::string command = GRAPHVIZ_PATH + " -Tpng tree.dot -o " + filename;
	system(command.c_str());

	std::cout << "Tree visualization generated: " + filename << std::endl;
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
	string c2 = "1(3(5(7 8) 6) 3(5(7) 6) 4)";

	// string c1 = "tractor(steering wheel (right_half left_half) who)";
	// string c2 = "tractor(steering wheel (right_half left_half) who)";

	auto tree1 = parseOnTree(c1, delimiters);
	auto tree2 = parseOnTree(c2, delimiters);
	unique_ptr<Node> deltaTree;

	visualizeTree(tree1.get(), "tree1.png");
	visualizeTree(tree2.get(), "tree2.png");
	cout << tree1->buildDeltaTreeWrap(tree2.get(), deltaTree) << endl;
	deltaTree->print();
	visualizeTree(tree2.get(), "deltaTree.png");

}