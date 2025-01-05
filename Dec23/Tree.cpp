#include "Tree.h"

/*Explicit instantiation for tic tac toe board*/
//template class Tree<std::array<std::array<char, 3>, 3>>;

//template<typename T>
//Tree<T>::Tree()
//{
//
//}




#pragma region TicTacToe
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



TicTacToeTree::TicTacToeTree() = default;

TicTacToeTree::TicTacToeTree(const array<array<char, 3>, 3>& boardData)
{
	rootNode.boardData = boardData; 
	depth = 0; 
	totalNodeCount = 1; 
	//rootNode.childrenLinks
}

void TicTacToeTree::insertNode(const array<array<char, 3>, 3>& boardData, int level, int childIndex)
{
	TicTacToeBoardNode* node = new TicTacToeBoardNode;

	node->boardData = boardData; 

	if (level == 0)
	{
		rootNode.childrenLinks.push_back(node);

	}

	else if (level == 1)
	{
		rootNode.childrenLinks.at(childIndex)->childrenLinks.push_back(node); 
	}

	totalNodeCount++; 

	//NOTE the use of std::move!
	//push_back expects a UNIQUE_PTR 
	//passing in `node` does copy-assignment 
	//std::unique_ptr sets operator = delete!

}

void TicTacToeTree::BFSPrintTree()
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

void TicTacToeTree::generatePossibleFirstMoves(array<array<char, 3>, 3>& boardData)
{
	for (size_t row = 0; row < 3; ++row)
	{
		for (size_t col = 0; col < 3; ++col)
		{
			//this->
			char copy = boardData.at(row).at(col);

			boardData.at(row).at(col) = 'X';

			insertNode(boardData, 0, 0); //First 0 is the level (or depth), Second 0 is the index of the child
										//root only has one index (poor explanation)

			//now reset to "original" (abc, def, ghi):
			boardData.at(row).at(col) = copy;

		}
	}
	depth += 1;

	//BFSPrintTree(); 
}

void TicTacToeTree::generatePossibleSecondMoves()
{
	//go one level deep: 
	//auto startNode = rootNode.childrenLinks.at(0); 

	//cout << "\n\n\nAt one level deep, 0th child contains:\n";
	//cout << *startNode << "\n";

	//add children to first child node at position 0 
	//ex: 
	//Xbc    X0c		Xb0				Xbc
	//def  =>def   and  def  .....		def
	//ghi	 ghi		ghi				gh0
	for (int childIndex = 0; childIndex < rootNode.childrenLinks.size(); ++childIndex)
	{
		auto startNode = rootNode.childrenLinks.at(childIndex); 

		for (size_t row = 0; row < 3; ++row)
		{
			for (size_t col = 0; col < 3; ++col)
			{
				if (startNode->boardData.at(row).at(col) != 'X') //don't overwrite X with 0 ...cheater 
				{
					auto copy = startNode->boardData.at(row).at(col);
					//change to O and insert into childrenLinks: 
					startNode->boardData.at(row).at(col) = 'O';

					insertNode(startNode->boardData, 1, childIndex);

					//reset 
					startNode->boardData.at(row).at(col) = copy;
				}
			}
		}
	}

	depth += 1; 
	

}


bool TicTacToeTree::checkWinCondition(const array<array<char, 3>, 3>& boardData, char currentPlayer)

{
	// Check rows, columns, and diagonals for a win
	for (size_t i = 0; i < 3; ++i)
	{
		if ((boardData[i][0] == currentPlayer && boardData[i][1] == currentPlayer && boardData[i][2] == currentPlayer) ||
			(boardData[0][i] == currentPlayer && boardData[1][i] == currentPlayer && boardData[2][i] == currentPlayer))
		{
			return true;
		}
	}
	if ((boardData[0][0] == currentPlayer && boardData[1][1] == currentPlayer && boardData[2][2] == currentPlayer) ||
		(boardData[0][2] == currentPlayer && boardData[1][1] == currentPlayer && boardData[2][0] == currentPlayer))
	{
		return true;
	}
	return false;
}

pair<int, int> TicTacToeTree::getBestMove(array<array<char, 3>, 3>& boardData, char currentPlayer)
{

	pair<int, int> bestMove; 

	int bestScore = (currentPlayer == 'X') ? INT_MIN : INT_MAX;

	//get possible moves for the given board state (may be partially full) 
	for (size_t row = 0; row < 3; ++row)
	{
		for (size_t col = 0; col < 3; ++col)
		{
			if (boardData.at(row).at(col) != 'X' && boardData.at(row).at(col) != 'O')
				//spot is avaialable 
			{
				auto copy = boardData.at(row).at(col); 

				boardData.at(row).at(col) = currentPlayer;

				int score = minimax(boardData, 9, (currentPlayer == 'O'));

				boardData.at(row).at(col) = copy; 

				if ((currentPlayer == 'X' && score > bestScore) || (currentPlayer == 'O' && score < bestScore))
				{
					bestScore = score;
					bestMove = { row, col };
				}
			}
		}
	}

	return bestMove;
}

int TicTacToeTree::evaluateBoard(const array<array<char, 3>, 3>& boardData)
{
	if (checkWinCondition(boardData, 'X'))
		return 10;

	else if (checkWinCondition(boardData, 'O'))
		return -10;

	else //no winner or tie 
	{
		return 0;
	}
}

int TicTacToeTree::minimax(array<array<char, 3>, 3>& boardData, int depth, bool isMaximizing)
{
	int score = evaluateBoard(boardData);

	if (score == 10 || score == -10)
	{
		return score;
	}

	if (depth == 0)
	{
		return 0; //all 9 moves made and it was a draw
	}

	if (isMaximizing)
	{
		int best = INT_MIN; 

		for (size_t row = 0; row < 3; ++row)
		{
			for (size_t col = 0; col < 3; ++col)
			{
				// Check if cell is empty
				if (boardData.at(row).at(col) != 'X' && boardData.at(row).at(col) != 'O')
				{
					// Make the move
					char copy = boardData.at(row).at(col);
					boardData.at(row).at(col) = 'X';

					//the recursive call: 
					best = std::max(best, minimax(boardData, depth - 1, !isMaximizing));

					boardData.at(row).at(col) = copy; 

				}
			}
		}
		return best; 
	}

	else
	{
		int best = INT_MAX; 

		for (size_t row = 0; row < 3; ++row)
		{
			for (size_t col = 0; col < 3; ++col)
			{
				// Check if cell is empty
				if (boardData.at(row).at(col) != 'X' && boardData.at(row).at(col) != 'O')
				{
					// Make the move
					char copy = boardData.at(row).at(col);
					boardData.at(row).at(col) = 'O';

					best = std::min(best, minimax(boardData, depth - 1, !isMaximizing));

					boardData.at(row).at(col) = copy; 
				}
			}
		}
		
		return best; 
	}
}

void TicTacToeTree::generatePossibleMoves_toDepthN(array<array<char, 3>, 3>& boardData, size_t N)
{
	generateGameTreeRecursively(rootNode, boardData, 0, N);
}

void TicTacToeTree::generateGameTreeRecursively(TicTacToeBoardNode& parentNode, array<array<char, 3>, 3>& boardData, int currentDepth, int maxDepth)
{

	if (currentDepth == maxDepth  //base case
		||
		checkWinCondition(boardData, (currentDepth % 2 == 0) ? 'X' : 'O'))
	{
		if (checkWinCondition(boardData, (currentDepth % 2 == 0) ? 'X' : 'O'))
		{
			numberOfWinsAtMaxDepth++; 
		}
		return; //nothing - because method modifies member vars of class 
		//so, anticipate that recursive calls will not be returning anything - only modifying member vars 
	}

	//handle X->0 turn alternation: 
	char currentPlayer = (currentDepth % 2 == 0) ? 'X' : 'O'; 

	for (size_t row = 0; row < 3; ++row)
	{
		for (size_t col = 0; col < 3; ++col)
		{
			if (boardData.at(row).at(col) != 'X' && boardData.at(row).at(col) != 'O')
			{
				char copy = boardData.at(row).at(col); 
				boardData.at(row).at(col) = currentPlayer; 

				TicTacToeBoardNode* childNode = new TicTacToeBoardNode; 
				childNode->boardData = boardData; 
				

				//key bit!
				parentNode.childrenLinks.push_back(childNode); 
				totalNodeCount++; 

				//recursive call on child node - down to maxDepth 
				generateGameTreeRecursively(*childNode, boardData, currentDepth + 1, maxDepth);

				boardData.at(row).at(col) = copy; 
			}
		}
	}

	if (currentDepth == 0)
	{
		depth = maxDepth; 
	}
	
}




void TicTacToeTree::deleteNode(TicTacToeBoardNode* node)
{
	for (auto child : node->childrenLinks)
	{
		deleteNode(child); 

		delete child; 
	}
}

TicTacToeTree::~TicTacToeTree()
{
	deleteNode(&rootNode); 
}

#pragma endregion


