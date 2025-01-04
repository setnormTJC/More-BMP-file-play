#include "TicTacToe.h"

bool TicTacToe::checkForVerticalWin()
{
	for (size_t col = 0; col < 3; ++col)
	{
		if (theBoard.at(0).at(col) == theBoard.at(1).at(col)
			&&
			theBoard.at(1).at(col) == theBoard.at(2).at(col))
		{
			return true;
		}
	}
	return false;
}

bool TicTacToe::checkForHorizontalWin()
{
	for (size_t row = 0; row < 3; ++row)
	{
		if (theBoard.at(row).at(0) == theBoard.at(row).at(1)
			&&
			theBoard.at(row).at(1) == theBoard.at(row).at(2))
		{
			return true;
		}
	}
	return false;
}

bool TicTacToe::checkForDiagonalWin()
{
	if (theBoard.at(0).at(0) == theBoard.at(1).at(1)
		&&
		theBoard.at(1).at(1) == theBoard.at(2).at(2))
	{
		return true;
	}

	if (theBoard.at(0).at(2) == theBoard.at(1).at(1)
		&&
		theBoard.at(1).at(1) == theBoard.at(2).at(0))
	{
		return true;
	}

	return false;
}

TicTacToe::TicTacToe()
{
	TicTacToe::moveCount = 0; 
	
	int counter = 0; 

	for (size_t row = 0; row < 3; ++row)
	{
		for (size_t col = 0; col < 3; ++col)
		{
			TicTacToe::theBoard.at(row).at(col) = 'a' + counter; 
			//make initial board contain a- c, d - f, g - i
			counter++; 
		}
	}

	printBoard(); 
}

void TicTacToe::printBoard()
{
	for (size_t row = 0; row < 3; ++row)
	{
		for (size_t col = 0; col < 3; ++col)
		{
			cout << TicTacToe::theBoard.at(row).at(col); 
		}
		cout << "\n";
	}
}

void TicTacToe::getMove(const int row, const int col)
{
	assert(row < 3 && col < 3);

	if (theBoard.at(row).at(col) == 'X' || theBoard.at(row).at(col) == 'O')
	{
		cout << "Invalid move. Try again.\n";
		return;
	}

	if (moveCount % 2 == 0)
	{
		theBoard.at(row).at(col) = 'X'; 
	}

	else
	{
		theBoard.at(row).at(col) = 'O';
	}

	moveCount++; 

	cout << "Updated board: \n";
	printBoard(); 

	if (moveCount >= 5)
	{
		if (isGameOver())
		{
			cout << "Game Over! Player " << (moveCount % 2 == 0 ? "O" : "X") << " wins!\n";
		}
		else if (moveCount == 9)
		{
			cout << "Game Over! It's a draw!\n";
		}
	}

}

bool TicTacToe::isGameOver()
{
	bool isGameOver = false;

	//check for vertical win: 
	isGameOver = checkForVerticalWin() || checkForHorizontalWin() || checkForDiagonalWin(); 

	//check for horizontal win: 


	//check for diagonal win: 
	return isGameOver;
}

ostream& operator<<(ostream& os, const TicTacToe& theGame)
{
	for (size_t row = 0; row < 3; ++row)
	{
		for (size_t col = 0; col < 3; ++col)
		{
			os << theGame.theBoard.at(row).at(col);
		}
		os << "\n";
	}

	return os; 
}
