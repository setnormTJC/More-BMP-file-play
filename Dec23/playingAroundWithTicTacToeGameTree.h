#pragma once

#include"TicTacToe.h"
#include"Tree.h"

void printFirstTwoLevelsOfTicTacToeGameTree()
{
	TicTacToe theGame{};
	TicTacToeTree ticTacToeGameTree{ theGame.boardData };


	//ticTacToeGameTree.generatePossibleFirstMoves(theGame.theBoard); 
	//cout << "\n\nLevel 1: \n";
	//ticTacToeGameTree.BFSPrintTree(); 


	//ticTacToeGameTree.generatePossibleSecondMoves();
	//cout << "\n\nLevel 2:\n";
	//ticTacToeGameTree.BFSPrintTree(); 


	//get the first level: 
	ticTacToeGameTree.generatePossibleFirstMoves(theGame.boardData);

	//get the second level: 
	ticTacToeGameTree.generatePossibleSecondMoves();


	ticTacToeGameTree.BFSPrintTree();

	cout << "\n\nTotal number of nodes: " << ticTacToeGameTree.totalNodeCount << "\n";
	cout << "Node count in level 1: " << ticTacToeGameTree.rootNode.childrenLinks.size() << "\n\n";
	int nodeCountLevel2 = 0;
	for (auto& level1Child : ticTacToeGameTree.rootNode.childrenLinks)
	{
		nodeCountLevel2 += level1Child->childrenLinks.size();
		//ALL children in a certain level SHOULD have same number of children but doing it this way anyhow
	}
	cout << "Node count in level 2: " << nodeCountLevel2 << "\n";

}

void generateFullTicTacToeGameTree()
{
	TicTacToe theGame{};

	TicTacToeTree ticTacToeGameTree{ theGame.boardData };

	ticTacToeGameTree.generateGameTreeRecursively(ticTacToeGameTree.rootNode, theGame.boardData, 0, 8);

	//print state of board at "leftmost" node in level 4: should be xox,oef,ghi
	cout << "\n\n\n\"leftmost\" node in level 4: \n"
		<< *(ticTacToeGameTree.rootNode.childrenLinks.at(0)
			->childrenLinks.at(0)->childrenLinks.at(0)->childrenLinks.at(0)) << "\n";

	//line below requires commenting out the OR condition in base case for `generateMovesRecursively`:
	//cout << "Total number of nodes in the (unpruned) Tic Tac Toe game tree: " 
	//	<< ticTacToeGameTree.totalNodeCount << "\n";

	cout << "Total number of nodes in the PRUNED (quit \"digging\" once win is reached) Tic Tac Toe game tree: "
		<< ticTacToeGameTree.totalNodeCount << "\n";

	cout << "Number of wins at max depth (AKA: on last move/nail biters): "
		<< ticTacToeGameTree.numberOfWinsAtMaxDepth << "\n";

	cout << "ROUGH Size of tic tac toe game tree "
		<< sizeof(TicTacToeBoardNode) * ticTacToeGameTree.totalNodeCount << " bytes\n";




	//int n = 9;
	//int totalNodes = 1; // Start with 1 for the root node
	//int factorial = 1;
	//int sum = 1; // Initialize with the root node

	//for (int k = 1; k <= n; ++k)
	//{
	//	factorial *= (n - k + 1); 
	//	sum += factorial;
	//	//LaTex equivalent: [ S = \sum_{k=0}^{9} \frac{9!}{(9-k)!} ]
	//	//see: https://latexeditor.lagrida.com/
	//}

	//cout << "For comparison - the number of nodes obtained using combinatorics equation is:";
	//cout << sum << std::endl;


}

void demoTicTacToeMinimax()
{

	TicTacToe theGame{};

	TicTacToeTree ticTacToeGameTree{ theGame.boardData };

	ticTacToeGameTree.generateGameTreeRecursively(ticTacToeGameTree.rootNode, theGame.boardData, 0, 8);

	//now that we've got the game tree, try to predict the next best move given a particular starting state: 
	cout << "\n\n\n";

	theGame.printBoard();



	std::cin.get();

	int moveCount = 0;
	while (!theGame.isGameOver())
	{
		int row, col;
		cout << "Enter row and col\n";

		std::cin >> row >> col;
		theGame.getMove(row, col);
		system("cls");

		theGame.printBoard();


		moveCount++;
		char currentPlayer = (moveCount % 2 == 1) ? 'O' : 'X';

		cout << "\n\nBest move for " << currentPlayer << " is "
			<< ticTacToeGameTree.getBestMove(theGame.boardData, currentPlayer).first
			<< ticTacToeGameTree.getBestMove(theGame.boardData, currentPlayer).second
			<< "\n\n";


	}

	cout << "Game is over\n";

}