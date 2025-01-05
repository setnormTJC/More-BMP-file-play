#pragma once

#include<vector> 
#include<iostream> 
#include<array> 
 

using std::vector, std::cout, std::array, std::ostream; 

	
//template<typename T> 
struct TicTacToeBoardNode
{
	array<array<char, 3>, 3> boardData; 
	/*NOTE: unique_ptr used here - `insertNode` might be inclined to use NEW so that no dangling pointers, don't want to deal with manual DELETE*/
	vector<TicTacToeBoardNode*> childrenLinks;

	//functions: 
	TicTacToeBoardNode(); 

	friend ostream& operator << (ostream& os, const TicTacToeBoardNode& theNode);

};

//template<typename T> 
class Tree
{

public: 
	size_t depth;
	TicTacToeBoardNode rootNode;

	size_t totalNodeCount;

	Tree(); 
	Tree(const array<array<char, 3>, 3>& boardData);


	void insertNode(const array<array<char, 3>, 3>& boardData, int level, int childIndex);

	/*breadth first search*/
	void BFSPrintTree();

	void generatePossibleFirstMoves(array<array<char, 3>, 3>& boardData);

	void generatePossibleSecondMoves();
	
	void generatePossibleMoves_toDepthN(array<array<char, 3>, 3>& boardData, size_t N);

	void generateMovesRecursively(TicTacToeBoardNode& parentNode, array<array<char, 3>, 3>& boardData, int currentDepth, int maxDepth);

	void deleteNode(TicTacToeBoardNode* node);

	~Tree(); 

};

/*plan for this to hold tic tac toe boards*/
//extern template class Tree<std::array<std::array<char, 3>, 3>>;
/*An alternative to the use of EXTERN:*/
//#include"Tree.cpp" -> odd