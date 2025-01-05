#pragma once

#include<vector> 
#include<iostream> 
#include<array> 
 

using std::vector, std::cout, std::array, std::ostream, std::pair, std::string; 



	
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
class TicTacToeTree
{

public: 
	size_t depth;
	TicTacToeBoardNode rootNode;

	size_t numberOfWinsAtMaxDepth = 0;
	size_t totalNodeCount;

	TicTacToeTree(); 
	TicTacToeTree(const array<array<char, 3>, 3>& boardData);


	void insertNode(const array<array<char, 3>, 3>& boardData, int level, int childIndex);

	/*breadth first search*/
	void BFSPrintTree();

	void generatePossibleFirstMoves(array<array<char, 3>, 3>& boardData);

	void generatePossibleSecondMoves();
	
	void generatePossibleMoves_toDepthN(array<array<char, 3>, 3>& boardData, size_t N);

	/*Implementation is similar to a DFS algorithm - note that BFS with a queue is also an option (not included here) */
	void generateGameTreeRecursively(TicTacToeBoardNode& parentNode, array<array<char, 3>, 3>& boardData, int currentDepth, int maxDepth);

	/*the implementation was "hardcoded" by Copilot*/
	bool checkWinCondition(const array<array<char, 3>, 3>& boardData, char currentPlayer);

	pair<int, int> getBestMove(array<array<char, 3>, 3>& boardData, char currentPlayer);
	
	int minimax(array<array<char, 3>, 3>& boardData, int depth, bool isMaximizing);

	int evaluateBoard(const array<array<char, 3>, 3>& boardData);

	void deleteNode(TicTacToeBoardNode* node);

	~TicTacToeTree(); 

};




/*plan for this to hold tic tac toe boards*/
//extern template class Tree<std::array<std::array<char, 3>, 3>>;
/*An alternative to the use of EXTERN:*/
//#include"Tree.cpp" -> odd