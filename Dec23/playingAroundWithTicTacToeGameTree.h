#pragma once

#include"TicTacToe.h"
#include"Tree.h"

void printFirstTwoLevelsOfTicTacToeGameTree()
{
	TicTacToe theGame{};
	Tree ticTacToeGameTree{ theGame.boardData };


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