#pragma once
#include <array>
#include<iostream> 

#include<cassert>

using std::array, std::cout, std::ostream; 

/*My main intention with this class is to test creation of a 
GameTree with a "manageable" number of nodes (ChessGame node count will
become unmanageable once a certain depth is reached)*/
class TicTacToe
{

	size_t moveCount;

	bool checkForVerticalWin(); 
	bool checkForHorizontalWin(); 
	bool  checkForDiagonalWin(); 

public: 
	TicTacToe(); 

	void printBoard(); 

	void getMove(const int row, const int col); 

	bool isGameOver(); 


	//friend ostream& operator << (ostream& os, const TicTacToe& theGame);

	array<array<char, 3>, 3> boardData;

};

