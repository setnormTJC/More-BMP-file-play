#include "DemosOfVariousChessMoves.h"

void displayOptions()
{
	cout << "1 - to continue, once you've clicked your piece and the position to move it to\n";
	cout << "2 - to show all possible moves\n";
}


void demoAGameOfRandomMoves(ChessGame& theGame)
{
	while (!theGame.isGameOver())
	{
		callNodeJS();
		openPort3000_andDisplayChessBoard();

		/*make RANDOM moves for fun until checkmate*/
		array<pair<char, int>, 2> aRandomStartAndEndPosition = theGame.getRandomMove();


		theGame.movePiece(aRandomStartAndEndPosition.at(0).first,
			aRandomStartAndEndPosition.at(0).second,
			aRandomStartAndEndPosition.at(1).first,
			aRandomStartAndEndPosition.at(1).second);

		//cout << "\n\nMove count now: " << theGame.moveCount << "\n";

		bool isCheckMate = false;

		if (theGame.isKingInCheck)
		{
			if (theGame.moveCount % 2 == 0)
			{
				isCheckMate = theGame.checkForMate("white");
			}

			else
			{
				isCheckMate = theGame.checkForMate("black");
			}
		}

		if (isCheckMate)
		{
			break; //replace this with better approach in the ChessGame class later of course
		}

		std::this_thread::sleep_for(std::chrono::seconds(3)); //sleep is for random move dealy 
	}

	cout << "\n\n\nGame over!";
	if (theGame.moveCount % 2 == 0)
	{
		cout << "Black won!\n";
	}

	else
	{
		cout << "WHITE won!\n";
	}

}

//If you want to test SCHOLAR'S mate instead: 
// 1.e4 e5 2.Bc4 Nc6 3.Qh5 Nf6?? 4.Qxf7#.
void demoFoolsMate(ChessGame& theGame)
{
	//make first four moves for fool's mate (so I don't have to keep giving input): 
	theGame.movePiece('F', 2, 'F', 4); //white
	theGame.movePiece('E', 7, 'E', 5); //black
	theGame.movePiece('G', 2, 'G', 4);
	theGame.movePiece('D', 8, 'H', 4); //the white queen in for the kill 


	while (!theGame.isGameOver())
	{
		callNodeJS();
		openPort3000_andDisplayChessBoard();

		string response;

		displayOptions(); 
		getline(std::cin, response);

		while (response != "1")
		{
			if (response == "2")
			{
				theGame.showAllPossibleMoves();
			}

			else
			{
				cout << "Unsupported option - enter 1 or 2 (for now)\n";
			}
			displayOptions();
			getline(std::cin, response);
		}

		auto theTwoChosenPositions = theGame.getAndConfirmMove();

		theGame.movePiece(theTwoChosenPositions.at(0).first,
			theTwoChosenPositions.at(0).second,
			theTwoChosenPositions.at(1).first,
			theTwoChosenPositions.at(1).second);


		bool isCheckMate = false;

		if (theGame.isKingInCheck)
		{
			if (theGame.moveCount % 2 == 0)
			{
				isCheckMate = theGame.checkForMate("white");
			}

			else
			{
				isCheckMate = theGame.checkForMate("black");
			}
		}

		if (isCheckMate)
		{
			break; //replace this with better approach in the ChessGame class later of course
		}

		//soundAlertIfImagesTakingUpTooMuchSpace();
	}


	cout << "\n\n\nGame over!";
	if (theGame.moveCount % 2 == 0)
	{
		cout << "Black won!\n";
	}

	else
	{
		cout << "WHITE won!\n";
	}
}
