


//#include"playingAroundWithTicTacToeGameTree.h"
//#include "TicTacToe.h"

#include"ChessGame.h"
#include "NodeJSFunctions.h"
#include"AlgebraicMoveNotation.h"
#include"DemosOfVariousChessMoves.h"

//#include<d3d11>


//#include"Tree.h"


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
	

	ChessGame theGame{}; 

	while (!theGame.isGameOver())
	{
		callNodeJS();
		openPort3000_andDisplayChessBoard();

		cout << "Evaluation function returns : " << theGame.evaluateGameState() << "\n";
		if (theGame.moveCount % 2 == 0)
		{
			cout << "WHITE to move:\n\n";
		}

		else
		{
			cout << "BLACK to move:\n\n";
		}

		/*
		//display "best" move: 
		*/
		//if (theGame.moveCount > -1)
		//{



		int desiredDepth = 3; //the ALL-IMPORTANT quantity ... 




		//auto minimaxMove = theGame.getMinimaxMove(desiredDepth);
		//cout << "Minimax move (with depth =  " << desiredDepth << " is "
		//	<< minimaxMove.first << " to " << minimaxMove.second << "\n";
		cout << "Enable move suggestion (y/n)? \n";
		string moveSuggestionResponse; 
		
		
		//getline(std::cin, moveSuggestionResponse);
		//if (moveSuggestionResponse == "y") 
		

		pair<string, string> minimaxAlphaBetaMove; 

		if (moveSuggestionResponse == "") //goofy way of just getting suggestedMove w/o input

		//desirable to skip if, ex, playing "standard" opening moves 
		//ALSO desirable to skip if playing opponent (who will not take these suggestions) 
		{
			ofstream fout{ "howLongItTakes.txt", ios::app };
			auto start = std::chrono::system_clock::now(); 
			minimaxAlphaBetaMove = theGame.getMinimaxAlphaBetaMove(desiredDepth);


			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start; 

			fout << "With depth = " << desiredDepth << " getMinimaxAlphaBetaMove took "
				<< elapsed_seconds.count() << " (seconds?) \n";

			fout.close();
			setTerminalColor(TerminalColor::Red);

			cout << "MinimaxAlphaBeta move (with depth =  " << desiredDepth << " is "
				<< minimaxAlphaBetaMove.first << " to " << minimaxAlphaBetaMove.second << "\n";
			setTerminalColor(TerminalColor::Default);


		}

		//write the time it took for getMinimaxAlphaBetaMove with given depth out to file (for comparison/benchmarking)
	

		//string response;
		//displayOptions();
		//getline(std::cin, response);

		//while (response != "1")
		//{
		//	if (response == "2")
		//	{
		//		theGame.showAllPossibleMoves();
		//	}

		//	else
		//	{
		//		cout << "Unsupported option - enter 1 or 2 (for now)\n";
		//	}
		//	displayOptions();
		//	getline(std::cin, response);
		//}

		//auto theTwoChosenPositions = theGame.getAndConfirmMove();
		// 
		//get location of piece selected to move by minimax: 
		auto positionOfSuggestedPiece = theGame.boardImage.piecesToPositions.at(minimaxAlphaBetaMove.first);
		
		auto oldPositionAsCharAndInt = convertStringChessPositionToCharAndInt(positionOfSuggestedPiece); 
		auto newPositionAsCharAndInt = convertStringChessPositionToCharAndInt(minimaxAlphaBetaMove.second);

		theGame.movePiece(oldPositionAsCharAndInt.first,
			oldPositionAsCharAndInt.second,
			newPositionAsCharAndInt.first,
			newPositionAsCharAndInt.second); 

		std::this_thread::sleep_for(std::chrono::seconds(1)); 

		//theGame.movePiece(theTwoChosenPositions.at(0).first,
		//	theTwoChosenPositions.at(0).second,
		//	theTwoChosenPositions.at(1).first,
		//	theTwoChosenPositions.at(1).second);

		


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

		system("cls"); 
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

