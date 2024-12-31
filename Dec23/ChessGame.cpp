#include "ChessGame.h"

map<string, string> ChessGame::positionsToPieces; //this MUST be here since positionsToPieces is STATIC
bool ChessGame::canTakeOpponentPiece = false;  //this also MUST be here - linker error occurs if not

#pragma region PieceRules

vector<string> PieceRules::getWhitePawnPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
{
	vector<string> possiblePositionsForWhitePawn;
	int newRank;
	newRank = currentRank + 1;

	string possiblePosition = currentFile + to_string(newRank);
	string contentsOfPossiblePosition = ChessGame::getContentsOfPossiblePosition(possiblePosition);

	if (contentsOfPossiblePosition == "")
	{
		possiblePositionsForWhitePawn.push_back(possiblePosition);
	}

	if (currentRank == 2)
	{
		if (contentsOfPossiblePosition == "")
			//ex: A2 pawn wants to move to A4 and A3 is EMPTY 
		{
			possiblePosition = currentFile + to_string(newRank + 1);
			contentsOfPossiblePosition = ChessGame::getContentsOfPossiblePosition(possiblePosition);

			if (contentsOfPossiblePosition == "")
			{
				possiblePositionsForWhitePawn.push_back(possiblePosition);
			}
		}
	}

	//check for opponent on diagonal - 
	string firstPossibleTakingPosition = convertCharAndIntChessPositionToString(currentFile - 1, currentRank + 1);
	string secondPossibleTakingPosition = convertCharAndIntChessPositionToString(currentFile + 1, currentRank + 1);

	string contentsOfFirstTakingPosition = ChessGame::getContentsOfPossiblePosition(firstPossibleTakingPosition);
	if (contentsOfFirstTakingPosition.find("black") != string::npos)
	{
		possiblePositionsForWhitePawn.push_back(firstPossibleTakingPosition);
	}

	string contentsOfSecondTakingPosition = ChessGame::getContentsOfPossiblePosition(secondPossibleTakingPosition);

	if (contentsOfSecondTakingPosition.find("black") != string::npos)
	{
		possiblePositionsForWhitePawn.push_back(secondPossibleTakingPosition);
	}

	//if (currentRank )
	return possiblePositionsForWhitePawn;
}

vector<string> PieceRules::getBlackPawnPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
{
	int newRank;
	vector<string> possiblePositionsForBlackPawn;

	//black pawns "usually" decrement rank by 1
	newRank = currentRank - 1;
	string possiblePosition = currentFile + to_string(newRank);
	string contentsOfPossiblePosition = ChessGame::getContentsOfPossiblePosition(possiblePosition);

	if (contentsOfPossiblePosition == "") //space is empty 
		//(not occupied by friend, and pawns cannot take unless on a diagonal) 
	{
		possiblePositionsForBlackPawn.push_back(possiblePosition);
	}

	//allow moving two positions only if rank is 7 (pawn has not yet moved): 
	if (currentRank == 7)
	{
		if (contentsOfPossiblePosition == "")
			//ex: B7 pawn wants to move to B5 and B6 is EMPTY 
		{
			possiblePosition = currentFile + to_string(newRank - 1);
			contentsOfPossiblePosition = ChessGame::getContentsOfPossiblePosition(possiblePosition);
			if (contentsOfPossiblePosition == "")
			{
				possiblePositionsForBlackPawn.push_back(possiblePosition);
			}
		}
	}

	//check for opponent on diagonal - 
	string firstPossibleTakingPosition = convertCharAndIntChessPositionToString(currentFile - 1, currentRank - 1);
	string secondPossibleTakingPosition = convertCharAndIntChessPositionToString(currentFile + 1, currentRank - 1);

	string contentsOfFirstTakingPosition = ChessGame::getContentsOfPossiblePosition(firstPossibleTakingPosition);
	if (contentsOfFirstTakingPosition.find("white") != string::npos)
	{
		possiblePositionsForBlackPawn.push_back(firstPossibleTakingPosition);
	}

	string contentsOfSecondTakingPosition = ChessGame::getContentsOfPossiblePosition(secondPossibleTakingPosition);

	if (contentsOfSecondTakingPosition.find("white") != string::npos)
	{
		possiblePositionsForBlackPawn.push_back(secondPossibleTakingPosition);
	}

	return possiblePositionsForBlackPawn;
}

vector<string> PieceRules::getKingPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
{
	vector<string> possiblePositionsForKing;

	int newRank;
	char newFile;

	//white king can move one square in any direction as long as friend is not there (and not off of board)
	for (int row = -1; row <= 1; ++row)
	{
		for (int col = -1; col <= 1; ++col)
		{
			newFile = currentFile + col; //N.B. newFile is a CHARACTER type
			newRank = currentRank + row;

			//exclude "self-check" (do not list current position in list of possible positions to move to)
			if (row == 0 && col == 0)
			{
				continue;
			}

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = newFile + to_string(newRank);
				string contentsOfPossiblePosition = ChessGame::getContentsOfPossiblePosition(possiblePosition);

				if (contentsOfPossiblePosition == "") //possible position is free to move to
				{
					possiblePositionsForKing.push_back(possiblePosition);
				}

				else if ((contentsOfPossiblePosition.find("white") != string::npos
					&&
					currentPieceName.find("white") != string::npos)
					||
					(contentsOfPossiblePosition.find("black") != string::npos
						&&
						currentPieceName.find("black") != string::npos)) //friend is already there
				{
					//do not add to list of possible positions yet - 
					//not handling "castling" just yet 
					if (contentsOfPossiblePosition.find("rook"))
					{
						//...need to check for clear spaces between that rook and the king ...
					}
				}

				else //must be opponent ("black" 
				{
					ChessGame::canTakeOpponentPiece = true; //not sure what to do with this yet 
					possiblePositionsForKing.push_back(possiblePosition);
				}
			}
		}
	}

	return possiblePositionsForKing;

}

vector<string> PieceRules::getKnightPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
{
	vector<string> possiblePositionsForKnight;
	int newRank;
	char newFile; //potentially need to cast to a char later, but vector below perhaps suggest using int for now...

	vector<pair<int, int>> listOfDeltaRanksAndFilesForKnight =
	{
		{+1, +2}, //up 1 rank, right 2 files 
		{+1, -2}, //up 1, left 2
		{+2, +1}, //up 2, right 1 
		{+2, -1}, //up 2, left 1 

		{-1, +2}, //now the "downie equivalents" 
		{-1, -2},
		{-2, +1},
		{-2, -1},
	};

	for (const auto& currentPair : listOfDeltaRanksAndFilesForKnight)
	{
		newRank = currentRank + currentPair.first;
		newFile = currentFile + currentPair.second;

		if (!isPositionInBounds(newFile, newRank))
		{
			continue; //move on to the next pair if out of bounds 
		}

		string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

		string contentsOfPossiblePosition = ChessGame::getContentsOfPossiblePosition(possiblePosition);

		string relationship = ChessGame::getFriendOrFoeOrNeutral(currentPieceName, contentsOfPossiblePosition);

		if (relationship == "Neutral" || relationship == "Foe")
		{
			possiblePositionsForKnight.push_back(possiblePosition);
		}
		//else do nothing
	}

	return possiblePositionsForKnight;
}

vector<string>PieceRules::getRookPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
{

	vector<string> possiblePositionsForRook;

	vector<string> upMoves = lookForVerticalMoves(currentRank, currentFile, currentPieceName, true);
	vector<string> downMoves = lookForVerticalMoves(currentRank, currentFile, currentPieceName, false);

	possiblePositionsForRook.insert(possiblePositionsForRook.end(), upMoves.begin(), upMoves.end());
	possiblePositionsForRook.insert(possiblePositionsForRook.end(), downMoves.begin(), downMoves.end());

	vector<string> rightMoves = lookForHorizontalMoves(currentRank, currentFile, currentPieceName, true); 
	vector<string> leftMoves = lookForHorizontalMoves(currentRank, currentFile, currentPieceName, false);

	possiblePositionsForRook.insert(possiblePositionsForRook.end(), rightMoves.begin(), rightMoves.end()); 
	possiblePositionsForRook.insert(possiblePositionsForRook.end(), leftMoves.begin(), leftMoves.end()); 

	return possiblePositionsForRook;
}

vector<string> PieceRules::lookForVerticalMoves(int currentRank, char currentFile, const string& currentPieceName, 
	bool isUp)
{

	vector<string> possibleVerticalMoves; 

	if (isUp)
	{
		int newRank = currentRank;
		char newFile = currentFile;

		//first look for positions upwards: 
		bool isUpMoveAvailable = true;

		for (int i = 1; i < 9; ++i)
		{
			if (isUpMoveAvailable == false)
			{
				break;
			}
			newRank = currentRank + 1 * i; //NOTE the multiplication by loop counter 

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = ChessGame::getContentsOfPossiblePosition(possiblePosition);

				string relationship = ChessGame::getFriendOrFoeOrNeutral(currentPieceName, contentsOfPossiblePosition);

				int previousNumberOfPossibilities = possibleVerticalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleVerticalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
						isUpMoveAvailable = false; 
					}
				}

				//no need to consider castling since this is vertical moving (castling is horizontal) 

				if (possibleVerticalMoves.size() != previousNumberOfPossibilities + 1) //ex: A1 rook and A2 pawn is still at A2 - no up moves possible
				{
					isUpMoveAvailable = false; //waste no more loop iterations looking 
				}
			}
		}
	}

	else
	{
		int newRank = currentRank;
		char newFile = currentFile;

		bool isDownMoveAvailable = true;
		for (int i = 1; i < 9; ++i)
		{
			if (isDownMoveAvailable == false)
			{
				break;
			}
			newRank = currentRank - 1 * i; //NOTE the negative 1

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = ChessGame::getContentsOfPossiblePosition(possiblePosition);

				string relationship = ChessGame::getFriendOrFoeOrNeutral(currentPieceName, contentsOfPossiblePosition);

				int previousNumberOfPossibilities = possibleVerticalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleVerticalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
						isDownMoveAvailable = false;
					}
				}

				if (possibleVerticalMoves.size() != previousNumberOfPossibilities + 1) //ex: A1 rook and A2 pawn is still at A2 - no up moves possible
				{
					isDownMoveAvailable = false; //waste no more loop iterations looking 
				}
			}
		}
	}

	return possibleVerticalMoves; 
}

vector<string> PieceRules::lookForHorizontalMoves(int currentRank, char currentFile, const string& currentPieceName, bool isRight)

{
	vector<string> possibleHorizontalMoves;

	if (isRight)
	{
		int newFile = currentFile;
		char newRank = currentRank;

		//first look for positions upwards: 
		bool isRightMoveAvailable = true;

		for (int i = 1; i < 9; ++i)
		{
			if (isRightMoveAvailable == false)
			{
				break;
			}
			newFile = currentFile + 1 * i; //NOTE the multiplication by loop counter 

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = ChessGame::getContentsOfPossiblePosition(possiblePosition);

				string relationship = ChessGame::getFriendOrFoeOrNeutral(currentPieceName, contentsOfPossiblePosition);

				int previousNumberOfPossibilities = possibleHorizontalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleHorizontalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
						isRightMoveAvailable = false;
					}
				}

				//handle castling: 
				else //relationship == "Friend"
				{
					if (contentsOfPossiblePosition.find("King") != string::npos)
					{
						//check if spaces between the rook and king are empty

						//nested if - check if rook and king movecount == 0

						//if so, add king's position to possible position for rook 
						//AND add rook's position for king 
						
					}
				}

				
				if (possibleHorizontalMoves.size() != previousNumberOfPossibilities + 1)
					//ex: A1 rook and B2 knight is still at B2 - no right moves possible
				{
					isRightMoveAvailable = false; //waste no more loop iterations looking 
				}
			}
		}
	}

	else
	{
		int newRank = currentRank;
		char newFile = currentFile;

		bool isLeftMoveAvailable = true;
		for (int i = 1; i < 9; ++i)
		{
			if (isLeftMoveAvailable == false)
			{
				break;
			}
			newFile = currentFile - 1 * i; //NOTE the negative 1

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = ChessGame::getContentsOfPossiblePosition(possiblePosition);

				string relationship = ChessGame::getFriendOrFoeOrNeutral(currentPieceName, contentsOfPossiblePosition);

				int previousNumberOfPossibilities = possibleHorizontalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleHorizontalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
						isLeftMoveAvailable = false;
					}
				}

				//handle castling: 
				else //relationship == "Friend"
				{
					if (contentsOfPossiblePosition.find("King") != string::npos)
					{
						//check if spaces between the rook and king are empty

						//nested if - check if rook and king movecount == 0

						//if so, add king's position to possible position for rook 
						//AND add rook's position for king 

					}
				}


				if (possibleHorizontalMoves.size() != previousNumberOfPossibilities + 1) 
				{
					isLeftMoveAvailable = false; //waste no more loop iterations looking 
				}
			}
		}
	}

	return possibleHorizontalMoves;
}

vector<string> PieceRules::getPossiblePositionsForCurrentPiece(const string& currentPieceName, const string& currentPosition)
{

	vector<string> possiblePositionsForCurrentPiece;

	int currentRank = (int)(currentPosition.at(1) - 48); //ASCII value for character '0' is 48
	char currentFile = currentPosition.at(0);

	//int newRank;
	//char newFile;


	if (currentPieceName.find("blackPawn") != string::npos)
	{
		possiblePositionsForCurrentPiece =
			getBlackPawnPossiblePositions(currentRank, currentFile, currentPieceName);
	}

	else if (currentPieceName.find("whitePawn") != string::npos)
	{
		possiblePositionsForCurrentPiece =
			getWhitePawnPossiblePositions(currentRank, currentFile, currentPieceName);

	}

	//IGNORING placing self in check for now ....
	else if (currentPieceName.find("King") != string::npos)
	{
		possiblePositionsForCurrentPiece =
			getKingPossiblePositions(currentRank, currentFile, currentPieceName);

	}

	else if (currentPieceName.find("Knight") != string::npos)
	{
		possiblePositionsForCurrentPiece =
			getKnightPossiblePositions(currentRank, currentFile, currentPieceName);

	}

	else if (currentPieceName.find("Rook") != string::npos)
	{
		possiblePositionsForCurrentPiece =
			getRookPossiblePositions(currentRank, currentFile, currentPieceName);
	}
	


	//else
	//{
	//	cout << "What piece is that? \n";
	//	__debugbreak(); 
	//}
	return possiblePositionsForCurrentPiece;
}

PieceRules::PieceRules()
{
	//no member vars and methods are static - does this default constructor even need to exist? 
}


#pragma endregion


ChessGame::ChessGame()
{

	positionsToPieces = switchMapKeysAndValues(boardImage.piecesToPositions);

	generatePiecesToPossiblePositions(); 

	string baseFolderOfNodeJS = "testingNodeJS/public/";
	string filename = baseFolderOfNodeJS + "chessboard.bmp"; //note that this will overwrite!
	boardImage.writeImageFile(filename);

}

array<pair<char, int>, 2> ChessGame::getAndConfirmChessMove()
{
	//std::this_thread::sleep_for(std::chrono::milliseconds{ 10'000 });
	
	//5 second pause for mouse clicks on chessboard image in browser 
	// before trying to read coordinates file (may be much longer delay for actual game playing)

	array<pair<int, int>, 2> theTwoCoordinates = readFileAndReturnCoordinatesClickedInBrowser();

	int x0 = theTwoCoordinates.at(0).first;
	int y0 = theTwoCoordinates.at(0).second;

	pair<char, int> firstPosition =
		boardImage.convertImageCoordinatesToPosition(x0, y0);

	cout << "As a chess position, first coordinate is: " << firstPosition.first
		<< firstPosition.second << "\n";

	int xf = theTwoCoordinates.at(1).first;
	int yf = theTwoCoordinates.at(1).second;

	pair<char, int> secondPosition =
		boardImage.convertImageCoordinatesToPosition(xf, yf);

	cout << "\nAnd the second coordinate you chose is: " << secondPosition.first
		<< secondPosition.second << "\n";

	cout << "Do you accept these as your inputs (yes/no)? \n";
	string response; 
	std::getline(std::cin, response);

	if (response != "yes")
	{
		cout << "How many seconds would you like this program to wait while you select the piece"
			<< " and its new position? \n";
		int seconds; 
		std::cin >> seconds; 

		cout << "Pausing for" << seconds <<"\n";
		std::this_thread::sleep_for(std::chrono::seconds{ seconds });

		getAndConfirmChessMove(); // Recursively call the function to try again
	}
	else /*if (response == "yes")*/
	{
		cout << " updating the board ...\n";
		array<pair<char, int>, 2> theTwoChosenPositions =
		{
			firstPosition,
			secondPosition
		};

		return theTwoChosenPositions;
	}
}

bool ChessGame::isThatColorTurn(const string& pieceName)
{
	if (moveCount % 2 == 0)
	{
		//white's turn:
		if (pieceName.find("white") == string::npos)
		{
			cout << pieceName << " cannot be moved - it is white's turn:\n";
			return false;
		}
	}

	else //black's turn
	{
		if (pieceName.find("black") == string::npos)
		{
			cout << pieceName << " cannot be moved - it is black's turn:\n";
			return false;
		}
	}

	return true;
}

bool ChessGame::isGameOver()
{
	return false;
}


bool ChessGame::isPieceOnBoard(const string& pieceName)
{
	auto pieceNameIterator = std::find(boardImage.pieceNames.begin(), boardImage.pieceNames.end(), pieceName); 
	//anticipate ERASING pieceName from vector `pieceNames` when pieces get "taken" in later functions...

	return (pieceNameIterator != boardImage.pieceNames.end());
}


/*NOTE: this method writes a (potentially "large") IMAGE file out with filename = current system time*/
void ChessGame::movePiece(const string& pieceName, const string& newPosition)
{
	system("cls"); 

	if (!isThatColorTurn(pieceName))
	{
		return; 
	}

	if (!isPieceOnBoard(pieceName))
	{
		cout << pieceName << " not found\n";
		return;
	}

	auto positionAsCharAndInt = convertStringChessPositionToCharAndInt(newPosition); 

	if (!isPositionInBounds(positionAsCharAndInt.first, positionAsCharAndInt.second))
	{
		cout << newPosition << " not a legitimate position (legal positions are A1 through H8)\n";
		return; 
	}

	//confirm that newPosition is in the list of possible positions for the given piece:
	auto newPositionIterator = std::find(piecesToPossiblePositions.at(pieceName).begin(), 
		piecesToPossiblePositions.at(pieceName).end(), newPosition);

	if (newPositionIterator == piecesToPossiblePositions.at(pieceName).end())
	{
		cout << newPosition << " is not an allowed position for " << pieceName
			<< " - the position is either occupied by a friend, or " << pieceName << " cannot move in that way\n";
		return; 
	}

	//also update the "convenient flipped map": 
	auto oldPosition = boardImage.piecesToPositions.find(pieceName)->second;
	positionsToPieces.erase(oldPosition); //effectively makes the old square empty

	//handle taking an opponent piece by deleting from vector<string> pieceNames!
	
	string opponentPieceName;
	if (positionsToPieces.find(newPosition) != positionsToPieces.end())
	//opponent must be there, considering the logic that I used for generating possible positions
	{
		auto vectorIteratorOfPieceToTake =
			std::find(boardImage.pieceNames.begin(), boardImage.pieceNames.end(), 
				positionsToPieces.find(newPosition)->second);

		opponentPieceName = *vectorIteratorOfPieceToTake; //dereferenced iterator

		//erase opponent from list of pieces: 
		boardImage.pieceNames.erase(vectorIteratorOfPieceToTake);

		//erase opponent from map
		positionsToPieces.erase(newPosition); 


		int pieceValue = 1; //use a map of pieceNames to point values later!

		if (pieceName.find("black") != string::npos)
		{
			blackScore += pieceValue;
		}

		else
		{
			whiteScore += pieceValue; 
		}
	}
		
	//and insert the piece at the new position
	positionsToPieces.insert({ newPosition, pieceName });

	assert(positionsToPieces.size() == boardImage.pieceNames.size());

	//clear piecesToPositions and then get its contents by using the "switch" function: 
	boardImage.piecesToPositions.clear(); 
	boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);

	//draw piece in new position
	boardImage.drawPieces();

	//finally, "paint" over the old position with the appropriate (light or dark) square: 
	boardImage.fillPositionWithColor(oldPosition); 
	

	
	string baseFolderOfNodeJS = "testingNodeJS/public/";

	
	string filename = baseFolderOfNodeJS + "chessboard.bmp"; //note that this will overwrite!

	boardImage.writeImageFile(filename); 
	

	//system(filename.c_str());

	//string currentHourAndMinute = getCurrentHourAndMinute();

	//std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });


	//once a piece has moved, possible positions change for ALL pieces, so clear the possible positions map
	piecesToPossiblePositions.clear(); 
	//update (and display) possible moves for all pieces that have possibleMoveCount > 0 

	moveCount++; //for preventing black to move when it is white's turn and vice versa  

	string OTHERfilename = "move" + to_string(moveCount) + ".bmp";
	boardImage.writeImageFile(OTHERfilename);

	//find the new possible positions for all pieces (but display only the color whose turn it is)
	generatePiecesToPossiblePositions(); 

}


void ChessGame::movePiece
	(const char oldPositionFile, const int oldPositionRank, const char newPositionFile, const int newPositionRank)
{
	string oldPosition = convertCharAndIntChessPositionToString(oldPositionFile, oldPositionRank); 
	string newPosition = convertCharAndIntChessPositionToString(newPositionFile, newPositionRank);

	auto oldPositionIterator = positionsToPieces.find(oldPosition);

	if (oldPositionIterator != positionsToPieces.end())
	{
		string pieceName = oldPositionIterator->second; 
		movePiece(pieceName, newPosition); //NOTE that this is not a recursive call ...
	}

	else
	{
		cout << "No piece currently occupies the position: " << oldPosition << "\n";
	}

}


void ChessGame::generatePiecesToPossiblePositions()
{

	for (int i = 0; i < boardImage.pieceNames.size(); ++i)
	{
		string currentPieceName = boardImage.pieceNames.at(i);

		string currentPosition = boardImage.piecesToPositions.find(currentPieceName)->second;

		//vector<string> possiblePositionsForCurrentPiece
		//	= getPossiblePositionsForCurrentPiece(currentPieceName, currentPosition);

		//Call method of FRIEND CLASS - PieceRules - to get possible positions for current piece 
		//(this is an attempt to reduce the number of visible functions specific to `ChessGame` class)
		vector<string> possiblePositionsForCurrentPiece =
			PieceRules::getPossiblePositionsForCurrentPiece(currentPieceName, currentPosition);

		if (possiblePositionsForCurrentPiece.size() != 0)
		{
			if (moveCount % 2 == 0 && currentPieceName.find("white") != string::npos)
			{
				//if (currentPieceName.find("white") != string::npos)
				//{
					cout << currentPieceName << " can move to:\n";
					for (int i = 0; i < possiblePositionsForCurrentPiece.size(); ++i)
					{
						cout << possiblePositionsForCurrentPiece.at(i) << "\n";
					}
				//}
			}

			else if (moveCount % 2 == 1 && currentPieceName.find("black") != string::npos)
			{
				//if (currentPieceName.find("black") != string::npos)
				//{
					cout << currentPieceName << " can move to:\n";
					for (int i = 0; i < possiblePositionsForCurrentPiece.size(); ++i)
					{
						cout << possiblePositionsForCurrentPiece.at(i) << "\n";
					}
				//}
			}

			else
			{
				//impossible to reach unless logic errors? 
			}

		}


		piecesToPossiblePositions.insert({ currentPieceName, possiblePositionsForCurrentPiece });
	}

}



string ChessGame::getContentsOfPossiblePosition(const string& possiblePosition)
{

	auto possiblePositionIterator = positionsToPieces.find(possiblePosition); 

	if (possiblePositionIterator == positionsToPieces.end())
	{
		return string{}; 
	}

	else
	{
		return possiblePositionIterator->second; 
	}
}

string ChessGame::getFriendOrFoeOrNeutral(const string& contentsOfFirstPosition, const string& contentsOfSecondPosition)
{

	if (contentsOfFirstPosition == "" || contentsOfSecondPosition == "")
	{
		return "Neutral"; // if either position does not contain a piece, neither a friend nor a foe is there
	}

	else if (contentsOfFirstPosition.find("white") != string::npos
		&&
		contentsOfSecondPosition.find("white") != string::npos
		)
	{
		return "Friend";
	}

	else if (contentsOfFirstPosition.find("black") != string::npos
		&&
		contentsOfSecondPosition.find("black") != string::npos
		)
	{
		return "Friend";
	}

	else //IS the only other possibility that one is white and the other is black? 
	{
		return "Foe";
	} 
}








