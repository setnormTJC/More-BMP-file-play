#include "Tree.h"

/*Explicit instantiation for tic tac toe board*/
//template class Tree<std::array<std::array<char, 3>, 3>>;

//template<typename T>
//Tree<T>::Tree()
//{
//
//}

TicTacToeBoardNode::TicTacToeBoardNode() = default;

ostream& operator<<(ostream& os, const TicTacToeBoardNode& theNode)
{
	for (size_t row = 0; row < 3; ++row)
	{
		for (size_t col = 0; col < 3; ++col)
		{
			os << theNode.boardData.at(row).at(col); 
		}
		os << "\n";
	}

	return os;
}



Tree::Tree() = default;

Tree::Tree(const array<array<char, 3>, 3>& boardData)
{
	rootNode.boardData = boardData; 
	depth = 0; 
	//rootNode.childrenLinks
}

void Tree::insertNode(const array<array<char, 3>, 3>& boardData, int level)
{
	TicTacToeBoardNode* node = new TicTacToeBoardNode;

	node->boardData = boardData; 

	if (level == 0)
	{
		rootNode.childrenLinks.push_back(node);
	}

	else if (level == 1)
	{
		rootNode.childrenLinks.at(0)->childrenLinks.push_back(node); 
	}


	//NOTE the use of std::move!
	//push_back expects a UNIQUE_PTR 
	//passing in `node` does copy-assignment 
	//std::unique_ptr sets operator = delete!

}

void Tree::BFSPrintTree()
{
	//start at root: 
	cout << "ROOT:\n";
	cout << rootNode << "\n";

	auto children = rootNode.childrenLinks; 

	for (int depthCounter = 0; depthCounter < depth; ++depthCounter)
	{

		for (int i = 0; i < children.size(); ++i)
		{
			cout << *children.at(i) << "\n";
		}
		//move to deeper children ... 
		children = children.at(0)->childrenLinks; 
		//NOTE the 0 here - I think this is making the assumption that all nodes have same number of children 
		//ex: level 0 of tic tac toe (root) has NINE children 
		//level 1 has EIGHT children (8 moves remain before board is full) 
		//level 2 has SEVEN, etc. 
	}

}

void Tree::generatePossibleFirstMoves(array<array<char, 3>, 3>& boardData)
{
	for (size_t row = 0; row < 3; ++row)
	{
		for (size_t col = 0; col < 3; ++col)
		{
			//this->
			char copy = boardData.at(row).at(col);

			boardData.at(row).at(col) = 'X';

			insertNode(boardData, 0);

			//now reset to "original" (abc, def, ghi):
			boardData.at(row).at(col) = copy;

		}
	}
	depth += 1;

	//BFSPrintTree(); 
}

void Tree::generatePossibleSecondMoves()
{
	//go one level deep: 
	auto startNode = rootNode.childrenLinks.at(0); 

	//cout << "\n\n\nAt one level deep, 0th child contains:\n";
	//cout << *startNode << "\n";

	//add children to first child node at position 0 
	//ex: 
	//Xbc    X0c		Xb0				Xbc
	//def  =>def   and  def  .....		def
	//ghi	 ghi		ghi				gh0

	for (size_t row = 0; row < 3; ++row)
	{
		for (size_t col = 0; col < 3; ++col)
		{
			if (startNode->boardData.at(row).at(col) != 'X') //don't overwrite X with 0 ...cheater 
			{
				auto copy = startNode->boardData.at(row).at(col); 
				//change to O and insert into childrenLinks: 
				startNode->boardData.at(row).at(col) = 'O'; 

				insertNode(startNode->boardData, 1); 

				//reset 
				startNode->boardData.at(row).at(col) = copy; 
			}
		}
	}

	depth += 1; 


}

void Tree::deleteNode(TicTacToeBoardNode* node)
{
	for (auto child : node->childrenLinks)
	{
		deleteNode(child); 

		delete child; 
	}
}

Tree::~Tree()
{
	deleteNode(&rootNode); 
}


