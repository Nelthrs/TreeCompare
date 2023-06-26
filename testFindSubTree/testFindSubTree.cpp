#include "pch.h"
#include "CppUnitTest.h"
#include "../FindSubTree/findSubTree.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

bool compareTrees(const Node* tree1, const Node* tree2) {
	// Если оба указателя равны nullptr, деревья считаются равными
	if (tree1 == nullptr && tree2 == nullptr) {
		return true;
	}

	// Если только один из указателей равен nullptr, деревья считаются разными
	if (tree1 == nullptr || tree2 == nullptr) {
		return false;
	}

	// Сравниваем имена
	if (tree1->getName() != tree2->getName())
		return false;

	// Сравниваем кол-во детей
	if (tree1->getChildren().size() != tree2->getChildren().size())
		return false;

	for (size_t i = 0; i < tree1->getChildren().size(); i++) {
		if (!compareTrees(tree1->getChildren()[i], tree2->getChildren()[i]))
			return false;
	}
	
	return true;
}

namespace testsNode
{
	TEST_CLASS(testBuildDeltaTree)
	{
		TEST_METHOD(OneDifference)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("2");
			auto child3_1 = tree1Root->addChild("3");

			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("3");

			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child3_delta = desiredDeltaTreeRoot->addChild("2");

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
		TEST_METHOD(NoDifference)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("2");
			auto child3_1 = tree1Root->addChild("3");

			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("2");
			auto child3_2 = tree2Root->addChild("3");

			auto desiredDeltaTreeRoot = nullptr;

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot));
		}
		TEST_METHOD(OneDifferenceDeep)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("2");
				auto child4_1 = child2_1->addChild("4");
					auto child5_1 = child4_1->addChild("5");
					auto child6_1 = child4_1->addChild("6");
			auto child3_1 = tree1Root->addChild("3");

			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("2");
				auto child4_2 = child2_2->addChild("4");
					auto child5_2 = child4_2->addChild("5");
			auto child3_2 = tree2Root->addChild("3");

			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child2_desired = desiredDeltaTreeRoot->addChild("2");
				auto child4_desired = child2_desired->addChild("4");
					auto child6_desired = child4_desired->addChild("6");

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
		TEST_METHOD(MissingDescendancesInOneNode)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("2");
				auto child4_1 = child2_1->addChild("4");
				auto child5_1 = child2_1->addChild("5");
				auto child6_1 = child2_1->addChild("6");
			auto child3_1 = tree1Root->addChild("3");

			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("2");
			auto child3_2 = tree2Root->addChild("3");

			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child2_desired = desiredDeltaTreeRoot->addChild("2");
				auto child4_desired = child2_desired->addChild("4");
				auto child5_desired = child2_desired->addChild("5");
				auto child6_desired = child2_desired->addChild("6");

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
		TEST_METHOD(MissingOneNodeWithDescendance)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("2");
				auto child4_1 = child2_1->addChild("4");
				auto child5_1 = child2_1->addChild("5");
				auto child6_1 = child2_1->addChild("6");
			auto child3_1 = tree1Root->addChild("3");
			auto child7_1 = tree1Root->addChild("7");

			auto tree2Root = make_unique<Node>("1");
			auto child3_2 = tree2Root->addChild("3");
			auto child7_2 = tree2Root->addChild("7");

			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child2_desired = desiredDeltaTreeRoot->addChild("2");
			auto child4_desired = child2_desired->addChild("4");
			auto child5_desired = child2_desired->addChild("5");
			auto child6_desired = child2_desired->addChild("6");

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
		TEST_METHOD(MissingThreeLeaves)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("2");
				auto child4_1 = child2_1->addChild("4");
				auto child5_1 = child2_1->addChild("5");
				auto child6_1 = child2_1->addChild("6");
			auto child3_1 = tree1Root->addChild("3");
			auto child7_1 = tree1Root->addChild("7");
			auto child8_1 = tree1Root->addChild("8");

			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("2");
				auto child4_2 = child2_2->addChild("4");
				auto child5_2 = child2_2->addChild("5");
				auto child6_2 = child2_2->addChild("6");

			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child3_desired = desiredDeltaTreeRoot->addChild("3");
			auto child7_desired = desiredDeltaTreeRoot->addChild("7");
			auto child8_desired = desiredDeltaTreeRoot->addChild("8");

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
		TEST_METHOD(MissingAllExceptRoot)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("2");
			auto child4_1 = child2_1->addChild("4");
			auto child5_1 = child2_1->addChild("5");
			auto child6_1 = child2_1->addChild("6");
			auto child3_1 = tree1Root->addChild("3");
			auto child7_1 = tree1Root->addChild("7");
			auto child8_1 = tree1Root->addChild("8");

			auto tree2Root = make_unique<Node>("1");

			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child2_d = desiredDeltaTreeRoot->addChild("2");
			auto child4_d = child2_d->addChild("4");
			auto child5_d = child2_d->addChild("5");
			auto child6_d = child2_d->addChild("6");
			auto child3_d = desiredDeltaTreeRoot->addChild("3");
			auto child7_d = desiredDeltaTreeRoot->addChild("7");
			auto child8_d = desiredDeltaTreeRoot->addChild("8");

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
		TEST_METHOD(MissingOneNodeAndTwoLeaves)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("2");
				auto child4_1 = child2_1->addChild("4");
				auto child5_1 = child2_1->addChild("5");
				auto child6_1 = child2_1->addChild("6");
			auto child3_1 = tree1Root->addChild("3");
				auto child9_1 = child3_1->addChild("9");
				auto child10_1 = child3_1->addChild("10");
			auto child7_1 = tree1Root->addChild("7");
			auto child8_1 = tree1Root->addChild("8");
			auto child11_1 = tree1Root->addChild("11");
			

			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("2");
				auto child4_2 = child2_2->addChild("4");
				auto child5_2 = child2_2->addChild("5");
				auto child6_2 = child2_2->addChild("6");
			auto child11_2 = tree2Root->addChild("11");

			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child3_d = desiredDeltaTreeRoot->addChild("3");
				auto child9_d = child3_d->addChild("9");
				auto child10_d = child3_d->addChild("10");
			auto child7_d = desiredDeltaTreeRoot->addChild("7");
			auto child8_d = desiredDeltaTreeRoot->addChild("8");
			

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
		TEST_METHOD(MissingOneNodeDeep)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("2");
				auto child4_1 = child2_1->addChild("4");
				auto child5_1 = child2_1->addChild("5");
				auto child6_1 = child2_1->addChild("6");
					auto child8_1 = child6_1->addChild("8");
					auto child9_1 = child6_1->addChild("9");
						auto child10_1 = child9_1->addChild("10");	
						auto child11_1 = child9_1->addChild("11");
			auto child3_1 = tree1Root->addChild("3");
			auto child7_1 = tree1Root->addChild("7");



			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("2");
				auto child4_2 = child2_2->addChild("4");
				auto child5_2 = child2_2->addChild("5");
				auto child6_2 = child2_2->addChild("6");
					auto child8_2 = child6_2->addChild("8");
			auto child3_2 = tree2Root->addChild("3");
			auto child7_2 = tree2Root->addChild("7");

			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child2_d = desiredDeltaTreeRoot->addChild("2");
				auto child6_d = child2_d->addChild("6");
					auto child9_d = child6_d->addChild("9");
						auto child10_d = child9_d->addChild("10");
						auto child11_d = child9_d->addChild("11");

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
		TEST_METHOD(NoDiffNodeAndLeafWithSimmilarNames)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("2");
				auto child4_1 = child2_1->addChild("4");
				auto child5_1 = child2_1->addChild("5");
				auto child6_1 = child2_1->addChild("6");
			auto child8_1 = tree1Root->addChild("2");



			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("2");
				auto child4_2 = child2_2->addChild("4");
				auto child5_2 = child2_2->addChild("5");
				auto child6_2 = child2_2->addChild("6");
			auto child8_2 = tree2Root->addChild("2");

			auto desiredDeltaTreeRoot = nullptr;

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot));
		}
		TEST_METHOD(DifferenceChildrenOrder)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child8_1 = tree1Root->addChild("2");
			auto child2_1 = tree1Root->addChild("2");
			auto child6_1 = child2_1->addChild("6");
			auto child4_1 = child2_1->addChild("4");
			auto child5_1 = child2_1->addChild("5");



			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("2");
			auto child4_2 = child2_2->addChild("4");
			auto child5_2 = child2_2->addChild("5");
			auto child6_2 = child2_2->addChild("6");
			auto child8_2 = tree2Root->addChild("2");

			auto desiredDeltaTreeRoot = nullptr;

			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot));
		}
		TEST_METHOD(NoDifferenceInSimmilarNamedNodes)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("3");
				auto child7_1 = child2_1->addChild("7");
				auto child8_1 = child2_1->addChild("5");
				auto child9_1 = child2_1->addChild("9");
			auto child3_1 = tree1Root->addChild("3");
				auto child4_1 = child3_1->addChild("4");
				auto child6_1 = child3_1->addChild("5");

			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("3");
				auto child4_2 = child2_2->addChild("4");
				auto child5_2 = child2_2->addChild("5");
			auto child3_2 = tree2Root->addChild("3");
				auto child7_2 = child3_2->addChild("7");
				auto child8_2 = child3_2->addChild("5");
				auto child9_2 = child3_2->addChild("9");


			unique_ptr<Node> realDeltaTreeRoot;
			tree2Root->buildDeltaTreeWrap(tree1Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), nullptr));
		}

		TEST_METHOD(DifferenceInSimmilarNamedNodes)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("3");
				auto child7_1 = child2_1->addChild("7");
				auto child8_1 = child2_1->addChild("5");
			auto child3_1 = tree1Root->addChild("3");
				auto child4_1 = child3_1->addChild("4");

			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("3");	
				auto child5_2 = child2_2->addChild("5");
				auto child4_2 = child2_2->addChild("4");
			auto child3_2 = tree2Root->addChild("3");
				auto child8_2 = child3_2->addChild("5");
				auto child7_2 = child3_2->addChild("7");
				auto child9_2 = child3_2->addChild("9");

			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child3_delta = desiredDeltaTreeRoot->addChild("3");
				auto child4_delta = child3_delta->addChild("5");
			auto child5_delta = desiredDeltaTreeRoot->addChild("3");
				auto child6_delta = child5_delta->addChild("9");

			unique_ptr<Node> realDeltaTreeRoot;
			tree1Root->buildDeltaTreeWrap(tree2Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}

		TEST_METHOD(ExtraNodesInMainTree)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child2_1 = tree1Root->addChild("3");
				auto child7_1 = child2_1->addChild("7");
				auto child8_1 = child2_1->addChild("5");
				auto child9_1 = child2_1->addChild("4");
			auto child3_1 = tree1Root->addChild("3");
				auto child4_1 = child3_1->addChild("4");

			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("3");
				auto child4_2 = child2_2->addChild("4");
				auto child5_2 = child2_2->addChild("5");
			auto child3_2 = tree2Root->addChild("3");
				auto child7_2 = child3_2->addChild("4");

			unique_ptr<Node> realDeltaTreeRoot;
			Assert::IsTrue(tree1Root->buildDeltaTreeWrap(tree2Root.get(), realDeltaTreeRoot) == -1);
			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), nullptr));
		}
		TEST_METHOD(MissingOneNodeSimmilarNamed)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child3_1 = tree1Root->addChild("3");
				auto child9_1 = child3_1->addChild("9");
				auto child10_1 = child3_1->addChild("10");
			auto child7_1 = tree1Root->addChild("7");


			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("3");
				auto child4_2 = child2_2->addChild("4");
				auto child5_2 = child2_2->addChild("5");
				auto child6_2 = child2_2->addChild("6");
			auto child3_2 = tree2Root->addChild("3");
				auto child9_2 = child3_2->addChild("9");
				auto child10_2 = child3_2->addChild("10");
			auto child7_2 = tree2Root->addChild("7");


			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child3_d = desiredDeltaTreeRoot->addChild("3");
				auto child4_d = child3_d->addChild("4");
				auto child5_d = child3_d->addChild("5");
				auto child6_d = child3_d->addChild("6");


			unique_ptr<Node> realDeltaTreeRoot;
			tree1Root->buildDeltaTreeWrap(tree2Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
		TEST_METHOD(MissingOneSimmilarNode)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child3_1 = tree1Root->addChild("3");
				auto child9_1 = child3_1->addChild("9");
				auto child10_1 = child3_1->addChild("10");
			

			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("3");
				auto child9_2 = child2_2->addChild("9");
				auto child10_2 = child2_2->addChild("10");
			auto child3_2 = tree2Root->addChild("3");
				auto child91_2 = child3_2->addChild("9");
				auto child101_2 = child3_2->addChild("10");


			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child3_d = desiredDeltaTreeRoot->addChild("3");
			auto child4_d = child3_d->addChild("9");
			auto child5_d = child3_d->addChild("10");



			unique_ptr<Node> realDeltaTreeRoot;
			tree1Root->buildDeltaTreeWrap(tree2Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
		TEST_METHOD(MissingAllNodesDescendants)
		{
			auto tree1Root = make_unique<Node>("1");
			auto child3_1 = tree1Root->addChild("3");
			auto child4_1 = tree1Root->addChild("3");
			auto child7_1 = tree1Root->addChild("7");


			auto tree2Root = make_unique<Node>("1");
			auto child2_2 = tree2Root->addChild("3");
				auto child4_2 = child2_2->addChild("4");
				auto child5_2 = child2_2->addChild("5");
				auto child6_2 = child2_2->addChild("6");
			auto child7_2 = tree2Root->addChild("7");
			auto child3_2 = tree2Root->addChild("3");
				auto child9_2 = child3_2->addChild("9");
				auto child10_2 = child3_2->addChild("10");
			


			auto desiredDeltaTreeRoot = make_unique<Node>("1");
			auto child3_d = desiredDeltaTreeRoot->addChild("3");
				auto child4_d = child3_d->addChild("4");
				auto child5_d = child3_d->addChild("5");
				auto child6_d = child3_d->addChild("6");
			auto child2_d = desiredDeltaTreeRoot->addChild("3");
				auto child9_d = child2_d->addChild("9");
				auto child10_d = child2_d->addChild("10");


			unique_ptr<Node> realDeltaTreeRoot;
			tree1Root->buildDeltaTreeWrap(tree2Root.get(), realDeltaTreeRoot);

			Assert::IsTrue(compareTrees(realDeltaTreeRoot.get(), desiredDeltaTreeRoot.get()));
		}
	};
	TEST_CLASS(testFindSubTree)
	{
		TEST_METHOD(SearchedTreeIsMainTree)
		{
			string delimiters = "() ";
			string mainTreeNote = "1(3(4 5 6) 2(9 10))";
			string searchedTreeNote = "1(3(4 5 6) 2(9 10))";

			auto mainTree = parseOnTree(mainTreeNote, delimiters);
			auto searchedTree = parseOnTree(searchedTreeNote, delimiters);

			unique_ptr<Node> realDeltaTree;
			int result = mainTree->findSubTree(searchedTree.get(), realDeltaTree);

			Assert::IsTrue(result == 0);
			Assert::IsTrue(realDeltaTree.get() == nullptr);
		}
		TEST_METHOD(SearchedTreeInMainRootChildren)
		{
			string delimiters = "() ";
			string mainTreeNote = "1(3(4 5 6) 2(9 10))";
			string searchedTreeNote = "3(4 5 6)";

			auto mainTree = parseOnTree(mainTreeNote, delimiters);
			auto searchedTree = parseOnTree(searchedTreeNote, delimiters);

			unique_ptr<Node> realDeltaTree;
			int result = mainTree->findSubTree(searchedTree.get(), realDeltaTree);

			Assert::IsTrue(realDeltaTree.get() == nullptr);
		}
		TEST_METHOD(InChildrenMissingOneLeaf)
		{
			string delimiters = "() ";
			string mainTreeNote = "1(3(4 5) 2(9 10))";
			string searchedTreeNote = "3(4 5 6)";
			string desiredDeltaTreeNote = "1(3(6))";

			auto mainTree = parseOnTree(mainTreeNote, delimiters);
			auto searchedTree = parseOnTree(searchedTreeNote, delimiters);

			unique_ptr<Node> realDeltaTree;
			int result = mainTree->findSubTree(searchedTree.get(), realDeltaTree);

			auto desiredDeltaTree = parseOnTree(desiredDeltaTreeNote, delimiters);

			Assert::IsTrue(compareTrees(realDeltaTree.get(), desiredDeltaTree.get()));
		}
		TEST_METHOD(InChildrenWithSimmilars)
		{
			string delimiters = "() ";
			string mainTreeNote = "1(3(4 5 6) 3(4 5))";
			string searchedTreeNote = "3(4 5 6)";
			string desiredDeltaTreeNote = "1(3(6))";

			auto mainTree = parseOnTree(mainTreeNote, delimiters);
			auto searchedTree = parseOnTree(searchedTreeNote, delimiters);

			unique_ptr<Node> realDeltaTree;
			int result = mainTree->findSubTree(searchedTree.get(), realDeltaTree);

			Assert::IsTrue(result == 0);
			Assert::IsTrue(realDeltaTree.get() == nullptr);
		}
		TEST_METHOD(InDescendantsSearchedLeaf)
		{
			string delimiters = "() ";
			string mainTreeNote = "1(3(4 5) 3(4 5) 3)";
			string searchedTreeNote = "3(7)";
			string desiredDeltaTreeNote = "1(3(7))";

			auto mainTree = parseOnTree(mainTreeNote, delimiters);
			auto searchedTree = parseOnTree(searchedTreeNote, delimiters);

			unique_ptr<Node> realDeltaTree;
			int result = mainTree->findSubTree(searchedTree.get(), realDeltaTree);
			
			auto desiredDeltaTree = parseOnTree(desiredDeltaTreeNote, delimiters);

			Assert::IsTrue(result == 1);
			Assert::IsTrue(compareTrees(realDeltaTree.get(), desiredDeltaTree.get()));
		}
		TEST_METHOD(InDescentantsSearchedNode)
		{
			string delimiters = "() ";
			string mainTreeNote = "1(3(4 5) 3(4 5))";
			string searchedTreeNote = "4(1 2 3)";
			string desiredDeltaTreeNote = "1(3(4(1 2 3)))";

			auto mainTree = parseOnTree(mainTreeNote, delimiters);
			auto searchedTree = parseOnTree(searchedTreeNote, delimiters);

			unique_ptr<Node> realDeltaTree;
			int result = mainTree->findSubTree(searchedTree.get(), realDeltaTree);

			auto desiredDeltaTree = parseOnTree(desiredDeltaTreeNote, delimiters);

			Assert::IsTrue(result == 3);
			Assert::IsTrue(compareTrees(realDeltaTree.get(), desiredDeltaTree.get()));
		}
		TEST_METHOD(NoSearchedTree)
		{
			string delimiters = "() ";
			string mainTreeNote = "1(3(4 5 6) 3(4 5))";
			string searchedTreeNote = "9(4 5 6)";

			auto mainTree = parseOnTree(mainTreeNote, delimiters);
			auto searchedTree = parseOnTree(searchedTreeNote, delimiters);

			unique_ptr<Node> realDeltaTree;
			int result = mainTree->findSubTree(searchedTree.get(), realDeltaTree);

			Assert::IsTrue(result == -1);
			Assert::IsTrue(realDeltaTree.get() == nullptr);
		}
		TEST_METHOD(ManySimmilars)
		{
			string delimiters = "() ";
			string mainTreeNote = "1(3(1(3 3) 5 6) 3 3(4 5))";
			string searchedTreeNote = "1(3 3)";

			auto mainTree = parseOnTree(mainTreeNote, delimiters);
			auto searchedTree = parseOnTree(searchedTreeNote, delimiters);

			unique_ptr<Node> realDeltaTree;
			int result = mainTree->findSubTree(searchedTree.get(), realDeltaTree);

			Assert::IsTrue(result == 0);
			Assert::IsTrue(realDeltaTree.get() == nullptr);
		}
		TEST_METHOD(MissingOneLeafDeep)
		{
			string delimiters = "() ";
			string mainTreeNote = "1(3(1(3 3) 5 6) 3 3(4 5))";
			string searchedTreeNote = "1(3 3(5))";
			string desiredDeltaTreeNote = "1(3(1(3(5))))";

			auto mainTree = parseOnTree(mainTreeNote, delimiters);
			auto searchedTree = parseOnTree(searchedTreeNote, delimiters);

			unique_ptr<Node> realDeltaTree;
			int result = mainTree->findSubTree(searchedTree.get(), realDeltaTree);

			auto desiredDeltaTree = parseOnTree(desiredDeltaTreeNote, delimiters);

			Assert::IsTrue(compareTrees(realDeltaTree.get(), desiredDeltaTree.get()));
		}
		TEST_METHOD(MissingNodesDeep)
		{
			string delimiters = "() ";
			string mainTreeNote = "1(3(1(3 3) 5 6) 3 3(4 5))";
			string searchedTreeNote = "1(3 3(5 6(7 8)))";
			string desiredDeltaTreeNote = "1(3(1(3(5 6(7 8)))))";

			auto mainTree = parseOnTree(mainTreeNote, delimiters);
			auto searchedTree = parseOnTree(searchedTreeNote, delimiters);

			unique_ptr<Node> realDeltaTree;
			int result = mainTree->findSubTree(searchedTree.get(), realDeltaTree);

			auto desiredDeltaTree = parseOnTree(desiredDeltaTreeNote, delimiters);

			Assert::IsTrue(compareTrees(realDeltaTree.get(), desiredDeltaTree.get()));
		}

	};

	TEST_CLASS(copyTests)
	{
		TEST_METHOD(SmallTree)
		{
			auto treeRoot = make_unique<Node>("1");
			treeRoot->addChild("3");
			treeRoot->addChild("3");
			treeRoot->addChild("7");

			Assert::IsTrue(compareTrees(treeRoot.get(), treeRoot->copy().get()));
		}
		TEST_METHOD(MediumTree)
		{
			auto treeRoot = make_unique<Node>("tractor");
			auto child1 = treeRoot->addChild("wheel");
			auto child1_1 = child1->addChild("bolts");
			auto child2 = treeRoot->addChild("wheel");
			auto child2_1 = child2->addChild("bolts");
			auto child2_2 = child2->addChild("tire");
			auto child2_3 = child2->addChild("screws");
			auto child3 = treeRoot->addChild("cabin");

			Assert::IsTrue(compareTrees(treeRoot.get(), treeRoot->copy().get()));
		}
		TEST_METHOD(LargeTree)
		{
			auto treeRoot = make_unique<Node>("tractor");
			auto child1 = treeRoot->addChild("wheel");
			auto child1_1 = child1->addChild("bolts");
			auto child2 = treeRoot->addChild("wheel");
			auto child2_1 = child2->addChild("bolts");
			auto child2_3 = child2->addChild("screws");
			auto child3 = treeRoot->addChild("cabin");
			auto child4 = treeRoot->addChild("pilot");
			auto child4_1 = child4->addChild("eyes");
			auto child4_2 = child4->addChild("nose");
			auto child1_2 = child1->addChild("axle");
			auto child3_3 = child3->addChild("steering wheel");
			auto child4_5 = child4->addChild("legs");
			auto child4_6 = child4->addChild("mouth");
			auto child1_2_1 = child1_2->addChild("bearings");
			auto child4_5_1 = child4_5->addChild("teeth");
			auto child4_5_2 = child4_5->addChild("tongue");

			Assert::IsTrue(compareTrees(treeRoot.get(), treeRoot->copy().get()));
		}
		TEST_METHOD(NullTree)
		{
			unique_ptr<Node> treeRoot = nullptr;
			Assert::IsTrue(treeRoot->copy().get() == nullptr);
		}
		TEST_METHOD(OneLeaf)
		{
			auto treeRoot = make_unique<Node>("tractor");
			Assert::IsTrue(compareTrees(treeRoot.get(), treeRoot->copy().get()));
		}

	};

	TEST_CLASS(isLeafTests)
	{
		TEST_METHOD(NodeIsLeaf)
		{
			auto node = make_unique<Node>("1");
			Assert::IsTrue(node->isLeaf());
		}
		TEST_METHOD(NodeIsNode)
		{
			auto node = make_unique<Node>("1");
			node->addChild("2");
			node->addChild("3");
			Assert::IsFalse(node->isLeaf());
		}
		TEST_METHOD(NodeIsNull)
		{
			unique_ptr<Node> node;
			Assert::IsFalse(node->isLeaf());
		}
	};

	TEST_CLASS(isNodeTests)
	{
		TEST_METHOD(NodeIsLeaf)
		{
			auto node = make_unique<Node>("1");
			Assert::IsFalse(node->isNode());
		}
		TEST_METHOD(NodeIsNode)
		{
			auto node = make_unique<Node>("1");
			node->addChild("2");
			node->addChild("3");
			Assert::IsTrue(node->isNode());
		}
		TEST_METHOD(NodeIsNull)
		{
			unique_ptr<Node> node;
			Assert::IsFalse(node->isNode());
		}
	};

	TEST_CLASS(isChildTests)
	{
		TEST_METHOD(NodeHasNoChildren)
		{
			auto node = make_unique<Node>("1");
			auto child = make_unique<Node>("2");

			Assert::IsFalse(node->isChild(child.get()));
		}
		TEST_METHOD(NodeHasThisChild)
		{
			auto node = make_unique<Node>("1");
			auto nodeChild1 = node->addChild("3");
			auto nodeChild2 = node->addChild("4");
			auto nodeChild3 = node->addChild("5");
			auto nodeChild4 = node->addChild("2");

			Assert::IsTrue(node->isChild(nodeChild3));
		}
		TEST_METHOD(NodeHasntThisChild)
		{
			auto node = make_unique<Node>("1");
			auto nodeChild = node->addChild("2");
			auto child = make_unique<Node>("2");

			Assert::IsFalse(node->isChild(child.get()));
		}
		TEST_METHOD(NodeHasItButInSecondKnee)
		{
			auto node = make_unique<Node>("1");
			auto nodeChild1 = node->addChild("3");
			auto nodeChild2 = node->addChild("4");
			auto nodeChildDeep = nodeChild2->addChild("6");
			auto nodeChild3 = node->addChild("5");
			auto nodeChild4 = node->addChild("2");

			Assert::IsFalse(node->isChild(nodeChildDeep));
		}

	};

	TEST_CLASS(descendantsCountTests)
	{
		TEST_METHOD(NoDescendants)
		{
			auto node = make_unique<Node>("1");

			Assert::IsTrue(node->descendantsCount() == 0);
		}
		TEST_METHOD(TwoChildren)
		{
			auto node = make_unique<Node>("1");
			node->addChild("2");
			node->addChild("3");

			Assert::IsTrue(node->descendantsCount() == 2);
		}
		TEST_METHOD(FiveDescendants)
		{
			auto node = make_unique<Node>("1");
			auto child1 = node->addChild("2");
			auto child1_1 = child1->addChild("4");
			auto child2 = node->addChild("3");
			auto child1_2 = child2->addChild("5");
			auto child2_2 = child2->addChild("6");

			Assert::IsTrue(node->descendantsCount() == 5);
		}
	};

	TEST_CLASS(findDescendants)
	{
		TEST_METHOD(NoDescendants)
		{
			auto node = make_unique<Node>("human");

			Assert::IsTrue(node->findDescendants("Eye").empty());
		}
		TEST_METHOD(OneSuitableDescendant)
		{
			auto human = make_unique<Node>("human");
			auto head = human->addChild("Head");
			auto eyes = head->addChild("Eyes");
			auto hand = human->addChild("Hand");
			auto finger = hand->addChild("Finger");

			auto foundDescendants = human->findDescendants("Finger");

			Assert::IsTrue(foundDescendants.size() == 1);
			Assert::IsTrue(foundDescendants[0] == finger);
		}
		TEST_METHOD(ThreeSuitableDescendants)
		{
			auto tractor = make_unique<Node>("tractor");
			auto firstWheel = tractor->addChild("Wheel");
			auto boltsFirstWheel = firstWheel->addChild("Bolts");

			auto secondWheel = tractor->addChild("Wheel");
			auto boltsSecondWheel = secondWheel->addChild("Bolts");
			auto metalCrap = secondWheel->addChild("metal Crap");

			auto thirdWheel = tractor->addChild("Wheel");
			auto boltsThirdWheel = thirdWheel->addChild("Bolts");

			auto fourthWheel = tractor->addChild("Wheel");
			auto tire = fourthWheel->addChild("Tire");
			auto boltsFourthWheel = fourthWheel->addChild("Bolts");

			auto foundDescendants = tractor->findDescendants("Bolts");
			vector<Node*> desiredDescendants = { boltsFirstWheel, boltsSecondWheel, boltsThirdWheel, boltsFourthWheel };

			Assert::IsTrue(foundDescendants.size() == desiredDescendants.size());

			for (size_t i = 0; i < foundDescendants.size(); i++) {
				Assert::IsTrue(foundDescendants[i] == desiredDescendants[i]);
			}
		}
	};

}
