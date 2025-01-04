


#include"ChessGame.h"
#include "NodeJSFunctions.h"
#include"AlgebraicMoveNotation.h"

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

void displayOptions()
{
	cout << "1 - to continue, once you've clicked your piece and the position to move it to\n";
	cout << "2 - to show all possible moves\n";
}

int main()
{
	AlgebraicMoveNotation algebraicMoveNotation{}; 
	algebraicMoveNotation.readPortableGameNotationFile("resources/foolsMate.pgn");
	algebraicMoveNotation.cleanUpFileData(); 

	//std::cin.get(); 


	ChessGame theGame{}; 


	//make first four moves for fool's mate (so I don't have to keep giving input): 
	//theGame.movePiece('F', 2, 'F', 4); //white
	//theGame.movePiece('E', 7, 'E', 5); //black
	//theGame.movePiece('G', 2, 'G', 4);
	//theGame.movePiece('D', 8, 'H', 4); //the white queen in for the kill 


	//now test scholar's mate: 
	// 1.e4 e5 2.Bc4 (targeting f7) Nc6 3.Qh5 (adding another attacker to the f7-pawn) Nf6?? 4.Qxf7#.
	//scholar's mate also detected correctly 


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

	return 0; 

}

