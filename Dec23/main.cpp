


#include"ChessGame.h"
#include "NodeJSFunctions.h"
#include"AlgebraicMoveNotation.h"
#include"DemosOfVariousChessMoves.h"
#include "TicTacToe.h"
#include"Tree.h"


/*"Too much" means > 100 MB*/
void soundAlertIfImagesTakingUpTooMuchSpace()
{
	auto currentPath = std::filesystem::current_path(); 

	size_t sumOfFileSizes = 0; 

	for (auto& entry : std::filesystem::directory_iterator(currentPath))
	{
		sumOfFileSizes += entry.file_size(); 
	}

	if (sumOfFileSizes > 100'000'000) //100 MB (about 50 images with 720 x 720 p with 32 bit depth)
	{
		cout << "file size over 9000!!!! (actually 100 MB)\n";
		while (true)
		{
			cout << "\a";
		}
	}

	//cout.imbue(std::locale{""}); //add commas every 3 digits for readability 
	//cout << "\n\nSum of file sizes: " << sumOfFileSizes << "\n";
}

/*Displays chessboard.bmp image file in browser, allows user to click piece, then next position
...until checkmate*/
void playChessWithMouseClicks(ChessGame & theGame)
{
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

		soundAlertIfImagesTakingUpTooMuchSpace();
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

/*Anticipate this function using `AlgebraicMoveNotation` class*/
void playChessReadingPGNFile(ChessGame& theGame, const string& filename)
{
	AlgebraicMoveNotation algebraicMoveNotation{};
	algebraicMoveNotation.readPortableGameNotationFile("resources/foolsMate.pgn");
	algebraicMoveNotation.cleanUpFileData();


	//lots more to do here ...
}

int main()
{

	TicTacToe theGame{};
	Tree ticTacToeGameTree{ theGame.theBoard };
	

	ticTacToeGameTree.generatePossibleFirstMoves(theGame.theBoard); 


	ticTacToeGameTree.BFSPrintTree(); 

	
	ticTacToeGameTree.generatePossibleSecondMoves();

	//for (auto secondLevel : ticTacToeGameTree.rootNode.childrenLinks.at(0))

	std::cin.get(); 


	while (!theGame.isGameOver())
	{
		int row, col;
		cout << "Enter row and col\n";

		std::cin >> row >> col; 
		theGame.getMove(row, col); 
	}

	cout << "Game is over\n";

	

	//ChessGame theGame{}; 
	//playChessWithMouseClicks(theGame); 



	return 0; 

}

