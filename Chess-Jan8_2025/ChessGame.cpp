#include "ChessGame.h"


ChessGame::ChessGame()
{
	
	boardImage.pieces = boardImage.mapPiecesToImages_andReturnPieces(); //stupid having to do this twice ...

	for (auto& pieceToPosition : boardImage.piecesToPositions)
	{
		auto piece = pieceToPosition.first; 
		auto position = pieceToPosition.second; 

		positionsToPieces.insert({ position, piece });
	}

	getPiecesToMoves();


	string baseFolderOfNodeJS = "testingNodeJS/public/";
	string filename = baseFolderOfNodeJS + "chessboard.bmp"; //note that this will overwrite!
	boardImage.writeImageFile(filename);

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

string ChessGame::getPieceAtPosition(const ChessPosition& position) const
{

	auto possiblePositionIterator = positionsToPieces.find(position);

	if (possiblePositionIterator == positionsToPieces.end())
	{
		return string{};
	}

	else
	{
		return possiblePositionIterator->second;
	}
}

void ChessGame::getPiecesToMoves()
{
	piecesToMoves.clear(); //


	assert(boardImage.pieces.size() == boardImage.piecesToPositions.size());

	for (const auto& piece : boardImage.pieces)
	{

		ChessPosition position = boardImage.piecesToPositions[piece];
		char file = position.file;
		int rank = position.rank; 

		vector<ChessPosition> moves;

		if (piece.find("whitePawn") != string::npos)
		{
			WhitePawnMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("blackPawn") != string::npos)
		{
			BlackPawnMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("King") != string::npos)
		{
			KingMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("Knight") != string::npos)
		{
			KnightMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("Rook") != string::npos)
		{
			RookMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("Bishop") != string::npos)
		{
			BishopMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("Queen") != string::npos)
		{
			QueenMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else
		{
			cout << "What piece is this?\n";
			__debugbreak();
		}

	}

}

void ChessGame::showAllPossibleMoves()
{
	for (const auto& currentPair : piecesToMoves)
	{
		if (currentPair.second.size() != 0)
		{
			if (moveCount % 2 == 0 && currentPair.first.find("white") != string::npos)
			{
				cout << currentPair.first << " can move to:\n";
				for (int i = 0; i < currentPair.second.size(); ++i)
				{
					cout << currentPair.second.at(i).file 
						<< currentPair.second.at(i).rank
						<< "\n";
				}
			}

			else if (moveCount % 2 == 1 && currentPair.first.find("black") != string::npos)
			{
				cout << currentPair.first << " can move to:\n";
				for (int i = 0; i < currentPair.second.size(); ++i)
				{
					cout << currentPair.second.at(i).file
						<< currentPair.second.at(i).rank
						<< "\n";
				}
			}
		}
	}
}

array<ChessPosition, 2> ChessGame::getAndConfirmMove()
{
	array<pair<int, int>, 2> theTwoCoordinates = readFileAndReturnCoordinatesClickedInBrowser();

	int x0 = theTwoCoordinates.at(0).first;
	int y0 = theTwoCoordinates.at(0).second;

	ChessPosition firstPosition =
		boardImage.convertImageCoordinatesToPosition(x0, y0);

	int xf = theTwoCoordinates.at(1).first;
	int yf = theTwoCoordinates.at(1).second;

	ChessPosition secondPosition =
		boardImage.convertImageCoordinatesToPosition(xf, yf);

	cout << firstPosition.file << firstPosition.rank << "->"
		<< secondPosition.file << secondPosition.rank << "\n";


	cout << "Updating the board...\n";
	array<ChessPosition, 2> theTwoChosenPositions =
	{
		firstPosition,
		secondPosition
	};

	return theTwoChosenPositions;
}


void ChessGame::movePiece(const ChessPosition& oldPosition, const ChessPosition& newPosition)
{

	auto oldPositionIterator = positionsToPieces.find(oldPosition);

	if (oldPositionIterator != positionsToPieces.end())
	{
		string pieceName = oldPositionIterator->second;

		//system("cls");
		cout << "\n\n\n";

		if (!isThatColorTurn(pieceName))
		{
			return;
		}

		if (!isPieceOnBoard(pieceName))
		{
			cout << pieceName << " not found\n";
			return;
		}

		if (!ChessPosition::isPositionInBounds(newPosition.file, newPosition.rank))
		{
			cout << newPosition.file << newPosition.rank << " not a legitimate position (legal positions are A1 through H8)\n";
			return;
		}

		//confirm that newPosition is in the list of possible positions for the given piece:
		auto newPositionIterator = std::find(piecesToMoves.at(pieceName).begin(),
			piecesToMoves.at(pieceName).end(), newPosition);

		if (newPositionIterator == piecesToMoves.at(pieceName).end())
		{
			cout << newPosition.file << newPosition.rank << " is not an allowed position for " << pieceName
				<< " - the position is either occupied by a friend, or " << pieceName << " cannot move in that way\n";
			return;
		}

		//cout << "\nFIRST, a SIMULATION of the move..\n";
		//simulateMove(pieceName, newPosition, true, false);
		//cout << "\nEND of simulation ...\n";

		movePieceHelper(pieceName, newPosition);
	}

	else
	{
		cout << "No piece currently occupies the position: " << oldPosition.file << oldPosition.rank << "\n";
	}

}

void ChessGame::movePieceHelper(const string& piece, const ChessPosition& newPosition)
{
	auto oldPosition = boardImage.piecesToPositions.find(pieceName)->second;
	positionsToPieces.erase(oldPosition); //effectively makes the old square empty

	string takenPieceName;
	bool pieceTaken = false;

	//handle taking an opponent piece by deleting from vector<string> pieceNames!
	if (positionsToPieces.find(newPosition) != positionsToPieces.end())
	{
		//opponent must be there (UNLESS castling!) 
		takenPieceName = positionsToPieces.at(newPosition);
		takePiece(pieceName, newPosition);
		pieceTaken = true;
	}

	//insert the piece at the new position
	positionsToPieces.insert({ newPosition, pieceName });

	// Handle castling
	if (pieceName.find("King") != string::npos)
	{
		if (pieceName.find("white") != string::npos)
		{
			whiteKingMoved = true;
			if (newPosition == "G1")
			{
				// Kingside castling
				movePieceHelper("whiteRookKSide", "F1");
				whiteRookMoved[0] = true;
			}
			else if (newPosition == "C1")
			{
				// Queenside castling
				movePieceHelper("whiteRookQSide", "D1");
				whiteRookMoved[1] = true;
			}
		}
		else if (pieceName.find("black") != string::npos)
		{
			blackKingMoved = true;
			if (newPosition == "G8")
			{
				// Kingside castling
				movePieceHelper("blackRookKSide", "F8");
				blackRookMoved[0] = true;
			}
			else if (newPosition == "C8")
			{
				// Queenside castling
				movePieceHelper("blackRookQSide", "D8");
				blackRookMoved[1] = true;
			}
		}
	}
	else if (pieceName.find("Rook") != string::npos)
	{
		if (pieceName.find("white") != string::npos)
		{
			if (oldPosition == "A1")
			{
				whiteRookMoved[1] = true;
			}
			else if (oldPosition == "H1")
			{
				whiteRookMoved[0] = true;
			}
		}
		else if (pieceName.find("black") != string::npos)
		{
			if (oldPosition == "A8")
			{
				blackRookMoved[1] = true;
			}
			else if (oldPosition == "H8")
			{
				blackRookMoved[0] = true;
			}
		}
	}



	//clear piecesToPositions and then get its contents by using the "switch" function: 
	boardImage.piecesToPositions.clear();
	boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);

	//once a piece has moved, possible positions change for ALL pieces, so clear the possible positions map
	piecesToMoves.clear();
	getPiecesToMoves();

	/*Finally, check for check: */
	/*IF the move puts friendly king in check, REJECT the move (early return) and REPROMPT the user for a move*/
	if (checkForCheck(getPieceColor(pieceName)))
	{
		//reversion to previous state - disallow move: 
		positionsToPieces.erase(newPosition);
		positionsToPieces.insert({ oldPosition, pieceName });

		if (pieceTaken)
		{
			positionsToPieces.insert({ newPosition, takenPieceName });
			boardImage.pieces.push_back(takenPieceName);
		}

		boardImage.piecesToPositions.clear();
		boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);
		piecesToMoves.clear();
		getPiecesToMoves();

		cout << pieceName << " cannot move to " << newPosition << " because\n";

		cout << "AFTER updating, " << getPieceColor(pieceName) << " is in check!\n";
		cout << "- try another move!\n";

		isKingInCheck = true; //the MAIN function checks for this flag
		//if it is set, the `checkForMate` function is called (in main)

		return; //so moveCount will not increment and new board will not be drawn 
	}

	else
	{
		isKingInCheck = false;
	}

	moveCount++;


	/*Write the the image file with the updated board*/
	drawBoardHelper(oldPosition);


}

void ChessGame::drawBoardHelper(const ChessPosition& oldPosition)
{
	//the image file stuff: 
	boardImage.drawPieces();
	boardImage.fillPositionWithColor(oldPosition); 	// "paint" over the old position with the appropriate (light or dark) square: 
	string baseFolderOfNodeJS = "testingNodeJS/public/";
	string filename = baseFolderOfNodeJS + "chessboard.bmp"; //note that this will overwrite!
	boardImage.writeImageFile(filename);
}



#pragma region Abstract Move Rules

vector<ChessPosition> WhitePawnMoveRules::getMoves(char currentFile, int currentRank, const string& currentPiece, const ChessGame& game)
{
	vector<ChessPosition> moves;
	int newRank;
	newRank = currentRank + 1;

	ChessPosition newPosition{ currentFile, newRank };

	string pieceAtNewPosition = getPieceAtPosition(game, newPosition);

	if (pieceAtNewPosition == "")
	{
		moves.push_back(newPosition);
	}

	if (currentRank == 2)
	{
		if (pieceAtNewPosition == "")
			//ex: A2 pawn wants to move to A4 and A3 is EMPTY 
		{
			ChessPosition newPosition{ currentFile, (newRank + 1) };

			pieceAtNewPosition = getPieceAtPosition(game, newPosition);

			if (pieceAtNewPosition == "")
			{
				moves.push_back(newPosition);
			}
		}
	}

	//check for opponent on diagonal - 
	//ChessPosition{}
	ChessPosition up1Left1{ static_cast<char>(currentFile - 1), currentRank + 1 };
	//NOTE: cast is here because doing arithmetic on a char implicitly casts to an int, 
	//then back to char (potentially problematic if you go beyond 0 or 255) 
	ChessPosition up1Right1{ static_cast<char>(currentFile + 1), currentRank + 1 };

	string pieceAtUp1Left1 = getPieceAtPosition(game, up1Left1);
	if (pieceAtUp1Left1.find("black") != string::npos)
	{
		moves.push_back(up1Left1);
	}

	string pieceAtUp1Right1 = getPieceAtPosition(game, up1Right1);

	if (pieceAtUp1Right1.find("black") != string::npos)
	{
		moves.push_back(up1Right1);
	}


	//string currentPosition = convertCharAndIntChessPositionToString(currentFile, currentRank);
	//why? 

	return moves;


}

vector<ChessPosition> BlackPawnMoveRules::getMoves(char currentFile, int currentRank, const string& piece, const ChessGame& game)
{
	int newRank;
	vector<ChessPosition> moves;

	//black pawns "usually" decrement rank by 1
	newRank = currentRank - 1;
	ChessPosition possiblePosition{ currentFile,(newRank) };
	string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

	if (contentsOfPossiblePosition == "") //space is empty 
		//(not occupied by friend, and pawns cannot take unless on a diagonal) 
	{
		moves.push_back(possiblePosition);
	}

	//allow moving two positions only if rank is 7 (pawn has not yet moved): 
	if (currentRank == 7)
	{
		if (contentsOfPossiblePosition == "")
			//ex: B7 pawn wants to move to B5 and B6 is EMPTY 
		{
			possiblePosition = ChessPosition{ currentFile ,(newRank - 1) };
			contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);
			if (contentsOfPossiblePosition == "")
			{
				moves.push_back(possiblePosition);
			}
		}
	}

	//check for opponent on diagonal - 
	ChessPosition firstPossibleTakingPosition { static_cast<char>(currentFile - 1), currentRank - 1 };
	ChessPosition secondPossibleTakingPosition { static_cast<char>(currentFile + 1), currentRank - 1};

	string contentsOfFirstTakingPosition = getPieceAtPosition(game, firstPossibleTakingPosition);
	if (contentsOfFirstTakingPosition.find("white") != string::npos)
	{
		moves.push_back(firstPossibleTakingPosition);
	}

	string contentsOfSecondTakingPosition = getPieceAtPosition(game, secondPossibleTakingPosition);

	if (contentsOfSecondTakingPosition.find("white") != string::npos)
	{
		moves.push_back(secondPossibleTakingPosition);
	}

	return moves;
}

vector<ChessPosition> KingMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)

{
	vector<ChessPosition> moves;

	int newRank;
	char newFile;

	//king can move one square in any direction as long as friend is not there (and not off of board)
	for (int row = -1; row <= 1; ++row)
	{
		for (int col = -1; col <= 1; ++col)
		{
			newFile = file + col; 
			newRank = rank + row;

			//exclude "self-check" (do not list current position in list of possible positions to move to)
			if (row == 0 && col == 0)
			{
				continue;
			}

			if (ChessPosition::isPositionInBounds(newFile, newRank))
			{
				ChessPosition possiblePosition{ newFile,(newRank) };
				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				if (contentsOfPossiblePosition == "") //possible position is free to move to
				{
					moves.push_back(possiblePosition);
				}

				else if ((contentsOfPossiblePosition.find("white") != string::npos
					&&
					piece.find("white") != string::npos)
					||
					(contentsOfPossiblePosition.find("black") != string::npos
						&&
						piece.find("black") != string::npos)) //friend is already there
				{
					//do nothing... castling handled down below 
				}

				else //must be opponent ("black" 
				{
					moves.push_back(possiblePosition);
				}
			}
		}
	}

	// Castling moves
	if (piece.find("white") != string::npos && !game.whiteKingMoved)
	{
		// Kingside castling
		if (!game.whiteRookMoved[0] && game.getPieceAtPosition(ChessPosition{'F', 1}) == "" 
			&& game.getPieceAtPosition(ChessPosition{'G', 1}) == "")
		{
			//
			//if (!game.checkForCheck("white")) /*&&*//* !game.simulateMoveForCheck("E1", "F1", "white") && !game.simulateMoveForCheck("E1", "G1", "white")*/
			//{
			moves.push_back(ChessPosition{ 'G', 1 });
			//}
		}
		// Queenside castling
		if (!game.whiteRookMoved[1] && game.getPieceAtPosition(ChessPosition{ 'B', 1 }) == ""
			&& game.getPieceAtPosition(ChessPosition{ 'C', 1 }) == "" && game.getPieceAtPosition(ChessPosition{ 'D', 1 }) == "")
		{
			//if (!game.checkForCheck("white"))// && !game.simulateMoveForCheck("E1", "D1", "white") && !game.simulateMoveForCheck("E1", "C1", "white"))
			//{
			moves.push_back(ChessPosition{ 'C', 1 });
			//}
		}
	}
	else if (piece.find("black") != string::npos && !game.blackKingMoved)
	{
		// Kingside castling
		if (!game.blackRookMoved[0] && game.getPieceAtPosition(ChessPosition{ 'F', 8 }) == "" 
			&& game.getPieceAtPosition(ChessPosition{ 'G', 8 }) == "")
		{
			//if (!game.checkForCheck("black"))// && !game.simulateMoveForCheck("E8", "F8", "black") && !game.simulateMoveForCheck("E8", "G8", "black"))
			//{
			moves.push_back(ChessPosition{ 'G', 8 });
			//}
		}
		// Queenside castling
		if (!game.blackRookMoved[1] && game.getPieceAtPosition(ChessPosition{ 'B', 8 }) == ""
			&& game.getPieceAtPosition(ChessPosition{ 'C', 8 }) == "" && 
			game.getPieceAtPosition(ChessPosition{ 'D', 8 }) == "")
		{
			//if (!game.checkForCheck("black"))// && !game.simulateMoveForCheck("E8", "D8", "black") && !game.simulateMoveForCheck("E8", "C8", "black"))
			//{
			moves.push_back(ChessPosition{ 'C', 8 });
			//}
		}
	}


	return moves;

}

vector<ChessPosition> KnightMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<ChessPosition> moves;
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
		newRank = rank + currentPair.first;
		newFile = file + currentPair.second;

		if (!ChessPosition::isPositionInBounds(newFile, newRank))
		{
			continue; //move on to the next pair if out of bounds 
		}

		ChessPosition possiblePosition = ChessPosition{ newFile, newRank };

		string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

		string relationship = sharedFunctions::getPieceRelationship(piece, contentsOfPossiblePosition);

		if (relationship == "Neutral" || relationship == "Foe")
		{
			moves.push_back(possiblePosition);
		}
		//else do nothing
	}

	return moves;
}

vector<ChessPosition> RookMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<ChessPosition> moves;

	vector<ChessPosition> upMoves = lookForVerticalMoves(file, rank, piece, true, game);
	vector<ChessPosition> downMoves = lookForVerticalMoves(file, rank, piece, false, game);

	moves.insert(moves.end(), upMoves.begin(), upMoves.end());
	moves.insert(moves.end(), downMoves.begin(), downMoves.end());

	vector<ChessPosition> rightMoves = lookForHorizontalMoves(file, rank, piece, true, game);
	vector<ChessPosition> leftMoves = lookForHorizontalMoves(file, rank, piece, false, game);

	moves.insert(moves.end(), rightMoves.begin(), rightMoves.end());
	moves.insert(moves.end(), leftMoves.begin(), leftMoves.end());

	return moves;
}


vector<ChessPosition> BishopMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<ChessPosition> possiblePositionsForBishop = lookForDiagonalMoves(file, rank, piece, game);

	return possiblePositionsForBishop;
}


vector<ChessPosition> QueenMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<ChessPosition> upMoves = lookForVerticalMoves(file, rank, piece, true, game);
	vector<ChessPosition> downMoves = lookForVerticalMoves(file, rank, piece, false, game);

	vector<ChessPosition> rightMoves = lookForHorizontalMoves(file, rank, piece, true, game);
	vector<ChessPosition> leftMoves = lookForHorizontalMoves(file, rank, piece, false, game);

	vector<ChessPosition> diagonalMoves = lookForDiagonalMoves(file, rank, piece, game);

	vector<ChessPosition> possiblePositionsForQueen;

	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), upMoves.begin(), upMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), downMoves.begin(), downMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), rightMoves.begin(), rightMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), leftMoves.begin(), leftMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), diagonalMoves.begin(), diagonalMoves.end());

	//= upMoves + downMoves + rightMoves + leftMoves + diagonalMoves; //not a supported operator for std::vector

	return possiblePositionsForQueen;

}


vector<ChessPosition> AbstractMoveRules::lookForVerticalMoves(char file, int rank, const string& piece, bool isUp, const ChessGame& game)
{

	vector<ChessPosition> possibleVerticalMoves;

	if (isUp)
	{
		int newRank = rank;
		char newFile = file;

		//first look for positions upwards: 
		bool isUpMoveAvailable = true;

		for (int i = 1; i < 9; ++i)
		{
			if (isUpMoveAvailable == false)
			{
				break;
			}
			newRank = rank + 1 * i; //NOTE the multiplication by loop counter 

			if (ChessPosition::isPositionInBounds(newFile, newRank))
			{
				ChessPosition possiblePosition{ newFile, newRank };

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = sharedFunctions::getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleVerticalMoves.size();
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
		int newRank = rank;
		char newFile = file;

		bool isDownMoveAvailable = true;
		for (int i = 1; i < 9; ++i)
		{
			if (isDownMoveAvailable == false)
			{
				break;
			}
			newRank = rank - 1 * i; //NOTE the negative 1

			if (ChessPosition::isPositionInBounds(newFile, newRank))
			{
				ChessPosition possiblePosition{ newFile, newRank };

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = sharedFunctions::getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleVerticalMoves.size();
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

vector<ChessPosition> AbstractMoveRules::lookForHorizontalMoves(char file, int rank, const string& piece, bool isRight, const ChessGame& game)

{
	vector<ChessPosition> possibleHorizontalMoves;

	if (isRight)
	{
		int newFile = file;
		char newRank = rank;

		//first look for positions upwards: 
		bool isRightMoveAvailable = true;

		for (int i = 1; i < 9; ++i)
		{
			if (isRightMoveAvailable == false)
			{
				break;
			}
			newFile = file + 1 * i; //NOTE the multiplication by loop counter 

			if (ChessPosition::isPositionInBounds(newFile, newRank))
			{
				ChessPosition possiblePosition{ static_cast<char>(newFile), newRank };

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = sharedFunctions::getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleHorizontalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleHorizontalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
						isRightMoveAvailable = false;
					}
				}
				//else ... 
				//handled castling elsewhere...: 

				if (possibleHorizontalMoves.size() != previousNumberOfPossibilities + 1)
					//ex: A1 rook and B2 knight is still at B2 - no right moves possible
				{
					isRightMoveAvailable = false; //waste no more loop iterations looking 
				}
			}
		}
	}

	else //isLeft
	{
		int newRank = rank;
		char newFile = file;

		bool isLeftMoveAvailable = true;
		for (int i = 1; i < 9; ++i)
		{
			if (isLeftMoveAvailable == false)
			{
				break;
			}
			newFile = file - 1 * i; //NOTE the negative 1

			if (ChessPosition::isPositionInBounds(newFile, newRank))
			{
				ChessPosition possiblePosition{ newFile, newRank };

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = sharedFunctions::getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleHorizontalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleHorizontalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
						isLeftMoveAvailable = false;
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

vector<ChessPosition> AbstractMoveRules::lookForDiagonalMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	char newFile = file;
	int newRank = rank;

	vector<ChessPosition> diagonalMoves;

	//diagonal move possiblilities are: 
	//1) up N squares, right N squares 
	//2) up N squares, left N squares 
	//3) down N, right N 
	//4) down and left N squares each 

	vector<pair<int, int>> diagonalTypeMultipliers =
	{
		{1, 1}, //up and right (deltaRank and deltaFile multiplied by +1
		{1, -1},  //up and left (deltaRank multiplied by +1, deltaFile by -1)
		{-1, 1}, //down and right 
		{-1, -1} //down and left 
	};

	bool stopLookingAlongThatDiagonal;

	for (const auto& currentPair : diagonalTypeMultipliers)
	{
		stopLookingAlongThatDiagonal = false;

		for (int deltaRank = 1; deltaRank < 8; ++deltaRank)
		{
			if (stopLookingAlongThatDiagonal)
			{
				break; //breaks to outermost loop - in which diagonalTypeMultiplier is updated
			}
			for (int deltaFile = 1; deltaFile < 8; ++deltaFile)
			{
				if (stopLookingAlongThatDiagonal)
				{
					break; //breaks to middlemost loop
				}

				if (deltaRank == deltaFile)
				{


					newFile = file + deltaFile * currentPair.second;
					newRank = rank + deltaRank * currentPair.first;

					if (ChessPosition::isPositionInBounds(newFile, newRank))
					{
						ChessPosition possiblePosition{ newFile, newRank };

						string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

						string relationship = sharedFunctions::getPieceRelationship(piece, contentsOfPossiblePosition);

						if (relationship == "Neutral" || relationship == "Foe")
						{
							diagonalMoves.push_back(possiblePosition);
							if (relationship == "Foe")
							{
								//set flag to stop looking further along "that" diagonal 
								stopLookingAlongThatDiagonal = true;
							}
						}

						else //relationship is Friend
						{
							stopLookingAlongThatDiagonal = true;
						}
					}

					else //if we've gone out of bounds, no reason to continue further 
					{
						stopLookingAlongThatDiagonal = true;
					}
				}
			}
		}
	}
	return diagonalMoves;

}


#pragma endregion 

