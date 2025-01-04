#include "ChessGame.h"



//bool ChessGame::is_check(move)
//{
//
//	// 1. Simulate the move on a copy of the board
//	Board temp_board = board;
//	temp_board.apply_move(move);
//
//	// 2. Locate the king
//	Position king_pos = temp_board.find_king(move.player);
//
//	// 3. & 4. Check if any opponent piece attacks the king
//	for (const Piece& piece : temp_board.get_opponent_pieces(move.player)) {
//		if (piece.can_attack(king_pos, temp_board)) {
//			// 5. Move puts the king in check
//			return true;
//		}
//	}
//
//	// 6. Move does not put the king in check
//	return false;
//	
//}

ChessGame::ChessGame()
{
	
	boardImage.pieces = boardImage.mapPiecesToImages_andReturnPieces(); //stupid having to do this twice ...

	positionsToPieces = switchMapKeysAndValues(boardImage.piecesToPositions);

	getPiecesToMoves(); 


	string baseFolderOfNodeJS = "testingNodeJS/public/";
	string filename = baseFolderOfNodeJS + "chessboard.bmp"; //note that this will overwrite!
	boardImage.writeImageFile(filename);

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
					cout << currentPair.second.at(i) << "\n";
				}
			}

			else if (moveCount % 2 == 1 && currentPair.first.find("black") != string::npos)
			{
				cout << currentPair.first << " can move to:\n";
				for (int i = 0; i < currentPair.second.size(); ++i)
				{
					cout << currentPair.second.at(i) << "\n";
				}
			}
		}
	}
}

array<pair<char, int>, 2> ChessGame::getAndConfirmMove()
{
	array<pair<int, int>, 2> theTwoCoordinates = readFileAndReturnCoordinatesClickedInBrowser();

	int x0 = theTwoCoordinates.at(0).first;
	int y0 = theTwoCoordinates.at(0).second;

	pair<char, int> firstPosition =
		boardImage.convertImageCoordinatesToPosition(x0, y0);

	int xf = theTwoCoordinates.at(1).first;
	int yf = theTwoCoordinates.at(1).second;

	pair<char, int> secondPosition =
		boardImage.convertImageCoordinatesToPosition(xf, yf);

	cout << firstPosition.first << firstPosition.second << "->"
		<< secondPosition.first << secondPosition.second << "\n";

	//string response;
	//while (true) {
	//	cout << "Do you accept these as your inputs (y/n)? \n";
	//	std::getline(std::cin, response);
	//	
	//	if (response.length() > 1) //a safety measure in case I accidentally enter "yes" 
	//	{
	//		setTerminalColor(TerminalColor::Red); 
	//		cout << "Careful - enter one letter response - y or n\n";
	//		continue; 
	//	}

	//	if (response != "y") {
	//		cout << "How many seconds would you like this program to wait while you select the piece"
	//			<< " and its new position? \n";
	//		int seconds;
	//		std::cin >> seconds;

	//		callNodeJS(); 
	//		openPort3000_andDisplayChessBoard(); 

	//		for (int i = 0; i < seconds; ++i)
	//		{
	//			cout << seconds - i << "...";
	//			std::this_thread::sleep_for(std::chrono::seconds{ 1});
	//		}

	//		// Clear the input buffer
	//		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	//		// Re-read the coordinates
	//		theTwoCoordinates = readFileAndReturnCoordinatesClickedInBrowser();

	//		x0 = theTwoCoordinates.at(0).first;
	//		y0 = theTwoCoordinates.at(0).second;

	//		firstPosition = boardImage.convertImageCoordinatesToPosition(x0, y0);

	//		xf = theTwoCoordinates.at(1).first;
	//		yf = theTwoCoordinates.at(1).second;

	//		secondPosition = boardImage.convertImageCoordinatesToPosition(xf, yf);

	//		cout << firstPosition.first << firstPosition.second << "->"
	//			<< secondPosition.first << secondPosition.second << "\n";
	//	}
	//	else {
	//		break;
	//	}
	//}

	cout << "Updating the board...\n";
	array<pair<char, int>, 2> theTwoChosenPositions =
	{
		firstPosition,
		secondPosition
	};

	return theTwoChosenPositions;
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

bool ChessGame::checkForMate(const string& colorToCheckForMate)
{
	//check for checkMATE: 
	bool isCheckMATE = true;

	//make a COPY? of piecesToMoves having only colorToCheckForChefor now? 
	map<string, vector<string>> colorPiecesToMoves; 
	for (const auto& currentPair : piecesToMoves)
	{
		if (currentPair.first.find(colorToCheckForMate) != string::npos)
		{
			colorPiecesToMoves.insert({currentPair.first, currentPair.second});
		}
	}

	for (const auto& singlePieceToMoves : colorPiecesToMoves)
	//for (int i = 0; i < piecesToMoves.size(); ++i)
	{
		if (isCheckMATE == false)
		{
			break;
		}

		const string& piece = singlePieceToMoves.first;
		//if (piece.find(colorToCheckForMate) != string::npos) 
		////obviously, do not check if an OPPONENT can move in such a way that the mate is REMOVED :) 
		//{
		for (const string& move : singlePieceToMoves.second)
		{
			movePieceHelper(piece, move);

			if (isKingInCheck == false)
			{
				isCheckMATE = false;
				break;
			}
		}
		//}
	}

	if (isCheckMATE)
	{
		cout << "\n\n\nCheck MATE!\n\n\n";
	}

	return isCheckMATE;
}

array<pair<char, int>, 2> ChessGame::getRandomMove()
{
	array<pair<char, int>, 2> randomMove;

	std::uniform_int_distribution<int> distribution{ 0, (int)boardImage.pieces.size() - 1 };

	string startingPosition;
	string endingPosition;
	string piece;

	// Random move for white:
	if (moveCount % 2 == 0)
	{
		while (true)
		{
			int randomIndex = distribution(engine);

			// Choose a random white piece
			piece = boardImage.pieces.at(randomIndex);

			if (piece.find("white") == string::npos)
			{
				continue;
			}

			startingPosition = boardImage.piecesToPositions.at(piece);

			// Check if the piece has any valid moves
			if (piecesToMoves.at(piece).empty())
			{
				continue;
			}

			// Choose a random valid move for the piece
			std::uniform_int_distribution<int> moveDistribution{ 0, (int)piecesToMoves.at(piece).size() - 1 };
			endingPosition = piecesToMoves.at(piece).at(moveDistribution(engine));
			break;
		}
	}
	else // Random move for black:
	{
		while (true)
		{
			int randomIndex = distribution(engine);

			// Choose a random black piece
			piece = boardImage.pieces.at(randomIndex);

			if (piece.find("black") == string::npos)
			{
				continue;
			}

			startingPosition = boardImage.piecesToPositions.at(piece);

			// Check if the piece has any valid moves
			if (piecesToMoves.at(piece).empty())
			{
				continue;
			}

			// Choose a random valid move for the piece
			std::uniform_int_distribution<int> moveDistribution{ 0, (int)piecesToMoves.at(piece).size() - 1 };
			endingPosition = piecesToMoves.at(piece).at(moveDistribution(engine));
			break;
		}
	}

	auto start = convertStringChessPositionToCharAndInt(startingPosition);
	auto end = convertStringChessPositionToCharAndInt(endingPosition);

	randomMove =
	{
		start,
		end
	};

	return randomMove;
}


bool ChessGame::isPieceOnBoard(const string& pieceName)
{
	auto pieceNameIterator = std::find(boardImage.pieces.begin(), boardImage.pieces.end(), pieceName); 
	//anticipate ERASING pieceName from vector `pieceNames` when pieces get "taken" in later functions...

	return (pieceNameIterator != boardImage.pieces.end());
}


/*PRECONDITION: this "helper" function only gets called once its caller, `movePiece`, has done certain 
verifications (ex: the position is on the board, it is that turn's color, and some other(s)
NOTE: this method writes a (potentially "large") IMAGE file out with filename = current system time*/
void ChessGame::movePieceHelper(const string& pieceName, const string& newPosition)
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

	//clear piecesToPositions and then get its contents by using the "switch" function: 
	boardImage.piecesToPositions.clear(); 
	boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);

	//once a piece has moved, possible positions change for ALL pieces, so clear the possible positions map
	piecesToMoves.clear();
	getPiecesToMoves();
	
	if (checkForCheck(getPieceColor(pieceName))) 
	{
		//reversion to previous state - disallow move: 
		positionsToPieces.erase(newPosition); 
		positionsToPieces.insert({ oldPosition, pieceName });
		
		if (pieceTaken)
		{
			/************************************************/
			positionsToPieces.insert({ newPosition, takenPieceName }); //REPLACE!
			//replace oldPosition with newPosition here!!!!!








			/************************************************/



			boardImage.pieces.push_back(takenPieceName); 
		}

		boardImage.piecesToPositions.clear(); 
		boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces); 
		piecesToMoves.clear(); 
		getPiecesToMoves(); 

		cout << pieceName << " cannot move to " << newPosition << " because\n";

		cout << "AFTER updating, " << getPieceColor(pieceName) << " is in check!\n";
		cout << "- try another move!\n";


		/*The two lines of code directly below are only a "one-time erasure" of the move from piecesToMoves!
		This obviously needs to be improved later ...
		*/
		//auto iteratorToNewPosition = std::find(
		//	piecesToMoves.at(pieceName).begin(), piecesToMoves.at(pieceName).end(), newPosition); 

		//piecesToMoves.at(pieceName).erase(iteratorToNewPosition); 


		isKingInCheck = true; 




		return; //so moveCount will not increment and new board will not be drawn 
	}

	else
	{
		isKingInCheck = false; 
	}

	moveCount++;

	drawBoardHelper(oldPosition);

}

void ChessGame::takePiece(const string& pieceName, const string& newPosition)
{
	string opponentPieceName; 

	auto vectorIteratorOfPieceToTake =
		std::find(boardImage.pieces.begin(), boardImage.pieces.end(),
			positionsToPieces.find(newPosition)->second);

	opponentPieceName = *vectorIteratorOfPieceToTake; //note the dereferenced iterator

	//erase opponent from list of pieces: 
	boardImage.pieces.erase(vectorIteratorOfPieceToTake);

	//erase opponent from map
	positionsToPieces.erase(newPosition);

	size_t pieceValue = findPieceValue(opponentPieceName); 


	if (pieceName.find("black") != string::npos)
	{
		setTerminalColor(TerminalColor::Red);
		cout << "Black gained " << pieceValue << "\n";
		setTerminalColor(TerminalColor::Default);
		blackScore += pieceValue;
	}

	else
	{
		setTerminalColor(TerminalColor::Red);
		cout << "White gained " << pieceValue << "\n";
		setTerminalColor(TerminalColor::Default);
		whiteScore += pieceValue;
	}

	cout << "Score is now (White v Black) " << whiteScore << " to " << blackScore << "\n";
}


void ChessGame::drawBoardHelper(const string& oldPosition)
{
	//the image file stuff: 
	boardImage.drawPieces();
	boardImage.fillPositionWithColor(oldPosition); 	// "paint" over the old position with the appropriate (light or dark) square: 
	string baseFolderOfNodeJS = "testingNodeJS/public/";
	string filename = baseFolderOfNodeJS + "chessboard.bmp"; //note that this will overwrite!
	boardImage.writeImageFile(filename);
}

void ChessGame::simulateMove(const string& piece, const string& newPosition, bool determineIfSelfCheck, bool determineIfFork)
{
	/**************************UPDATE THE BOARD STATE SECTION**************/

	//first, check if an enemy occupies the new position
	string contents = getPieceAtPosition(newPosition); 
	bool opponentTaken = false; 
	if (getPieceRelationship(piece, contents) == "Foe")
	{
		opponentTaken = true; 
		//if an enemy is there, remove enemy from list of pieces 
		auto pos = std::find(boardImage.pieces.begin(), boardImage.pieces.end(), contents);
		boardImage.pieces.erase(pos);

		//also remove from the positions maps: 
		boardImage.piecesToPositions.erase(contents); 
		positionsToPieces.erase(newPosition); 
	}

	//first get a copy of the currentPosition of `piece`: 
	string currentPosition = boardImage.piecesToPositions.at(piece); 

	//now update the positions maps with piece at newPosition: 
	boardImage.piecesToPositions.erase(piece); 
	boardImage.piecesToPositions.insert({ piece, newPosition });

	positionsToPieces.erase(currentPosition);
	positionsToPieces.insert({ newPosition, piece });

	//NOW update the piecesToMoves map: 
	getPiecesToMoves(); 

	//*************************perform the checks of interest (look for self check, enemy check, fork, etc)

	if (determineIfSelfCheck)
	{
		if (checkForCheck(getPieceColor(piece)))
		{
			cout << "SIMULATION says piece is in check !\n";
		}
	}


	/**************************RESET TO THE ORIGINAL BOARD STATE SECTION**************/
	if (opponentTaken)
	{
		//put him back in the list and in the maps: 
		boardImage.pieces.push_back(contents); 
		boardImage.piecesToPositions.insert({ contents, newPosition });
		positionsToPieces.insert({ newPosition, contents });
	}

	//put piece back in current (old/previous) position 
	boardImage.piecesToPositions.erase(piece); 
	boardImage.piecesToPositions.insert({ piece, currentPosition });

	positionsToPieces.erase(newPosition);
	positionsToPieces.insert({ currentPosition, piece });

	//don't forget to regenerate piecesToMoves 
	getPiecesToMoves(); 
}

bool ChessGame::checkForFork()
{
	//true if a piece can move to two different locations which BOTH have enemies 
	return false;
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
		auto newPositionIterator = std::find(piecesToMoves.at(pieceName).begin(),
			piecesToMoves.at(pieceName).end(), newPosition);

		if (newPositionIterator == piecesToMoves.at(pieceName).end())
		{
			cout << newPosition << " is not an allowed position for " << pieceName
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
		cout << "No piece currently occupies the position: " << oldPosition << "\n";
	}

}


void ChessGame::getPiecesToMoves()
{
	piecesToMoves.clear(); //


	assert(boardImage.pieces.size() == boardImage.piecesToPositions.size());

	for (const auto& piece : boardImage.pieces)
	{

		string position = boardImage.piecesToPositions[piece];
		char file = position.at(0);
		int rank = position.at(1) - '0'; //'0' is 48 

		vector<string> moves;

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

string ChessGame::getPieceAtPosition(const string& position) const
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



bool ChessGame::checkForCheck(const string& colorToCheckForCheck)
{
	string kingToCheckForCheck = colorToCheckForCheck + "King";
	//first, find the king's position: 

	//if any white piece has a possible position equal to the black king's position, black king is in check 

	auto kingIterator = boardImage.piecesToPositions.find(kingToCheckForCheck); 

	auto positionOfKing = kingIterator->second;

	for (const auto& currentPair : piecesToMoves)
	{
		for (const auto& possiblePosition : currentPair.second)
		{
			if (possiblePosition == positionOfKing 
				&& 
				currentPair.first.find(colorToCheckForCheck) == string::npos) //ex: white cannot put white in check 
			{
				//cout << "\033[31m"; //RED!
				setTerminalColor(TerminalColor::Red);
				cout << currentPair.first << " has " << kingToCheckForCheck << " in check\n";
				//cout << "\033[0m";
				setTerminalColor(TerminalColor::Default);

				return true; 
			}
		}
	}

	return false;
}

vector<string> WhitePawnMoveRules::getMoves(char currentFile, int currentRank, const string& currentPiece, const ChessGame& game)
{
	vector<string> moves;
	int newRank;
	newRank = currentRank + 1;

	string newPosition = currentFile + to_string(newRank);
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
			newPosition = currentFile + to_string(newRank + 1);
				
			pieceAtNewPosition = getPieceAtPosition(game, newPosition);

			if (pieceAtNewPosition == "")
			{
				moves.push_back(newPosition);
			}
		}
	}

	//check for opponent on diagonal - 
	string up1Left1 = convertCharAndIntChessPositionToString(currentFile - 1, currentRank + 1);
	string up1Right1 = convertCharAndIntChessPositionToString(currentFile + 1, currentRank + 1);

	string pieceAtUp1Left1 = getPieceAtPosition(game, up1Left1);
	if (pieceAtUp1Left1.find("black") != string::npos)
	{
		if (game.moveCount % 2 == 1) //only display if piece can be taken to the color about to move 
		{
			displayThatAPieceCanBeTaken(currentPiece, pieceAtUp1Left1);
		}
		moves.push_back(up1Left1);
	}

	string pieceAtUp1Right1 = getPieceAtPosition(game, up1Right1);

	if (pieceAtUp1Right1.find("black") != string::npos)
	{
		if (game.moveCount % 2 == 1) //only display if piece can be taken to the color about to move 
		{
			displayThatAPieceCanBeTaken(currentPiece, pieceAtUp1Right1);
		}
		moves.push_back(up1Right1);
	}


	string currentPosition = convertCharAndIntChessPositionToString(currentFile, currentRank);

	return moves;
	

}

vector<string> BlackPawnMoveRules::getMoves(char currentFile, int currentRank, const string& piece, const ChessGame& game)
{
	int newRank;
	vector<string> moves;

	//black pawns "usually" decrement rank by 1
	newRank = currentRank - 1;
	string possiblePosition = currentFile + to_string(newRank);
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
			possiblePosition = currentFile + to_string(newRank - 1);
			contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);
			if (contentsOfPossiblePosition == "")
			{
				moves.push_back(possiblePosition);
			}
		}
	}

	//check for opponent on diagonal - 
	string firstPossibleTakingPosition = convertCharAndIntChessPositionToString(currentFile - 1, currentRank - 1);
	string secondPossibleTakingPosition = convertCharAndIntChessPositionToString(currentFile + 1, currentRank - 1);

	string contentsOfFirstTakingPosition = getPieceAtPosition(game, firstPossibleTakingPosition);
	if (contentsOfFirstTakingPosition.find("white") != string::npos)
	{
		if (game.moveCount % 2 == 0)
		{
			displayThatAPieceCanBeTaken(piece, contentsOfFirstTakingPosition);
		}
		moves.push_back(firstPossibleTakingPosition);
	}

	string contentsOfSecondTakingPosition = getPieceAtPosition(game, secondPossibleTakingPosition);

	if (contentsOfSecondTakingPosition.find("white") != string::npos)
	{
		if (game.moveCount % 2 == 0)
		{
			displayThatAPieceCanBeTaken(piece, contentsOfSecondTakingPosition);
		}
		moves.push_back(secondPossibleTakingPosition);
	}

	return moves;
}

vector<string> KingMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)

{
	vector<string> moves;

	int newRank;
	char newFile;

	//white king can move one square in any direction as long as friend is not there (and not off of board)
	for (int row = -1; row <= 1; ++row)
	{
		for (int col = -1; col <= 1; ++col)
		{
			newFile = file + col; //N.B. newFile is a CHARACTER type
			newRank = rank + row;

			//exclude "self-check" (do not list current position in list of possible positions to move to)
			if (row == 0 && col == 0)
			{
				continue;
			}

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = newFile + to_string(newRank);
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
					//do not add to list of possible positions yet - 
					//not handling "castling" just yet 
					if (contentsOfPossiblePosition.find("rook"))
					{
						//...need to check for clear spaces between that rook and the king ...
					}
				}

				else //must be opponent ("black" 
				{
					displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
					//ChessGame::canTakeOpponentPiece = true; //not sure what to do with this yet 
					moves.push_back(possiblePosition);
				}
			}
		}
	}

	return moves;

}

vector<string> KnightMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<string> moves;
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

		if (!isPositionInBounds(newFile, newRank))
		{
			continue; //move on to the next pair if out of bounds 
		}

		string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

		string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

		string relationship = getPieceRelationship(piece, contentsOfPossiblePosition);

		if (relationship == "Neutral" || relationship == "Foe")
		{
			moves.push_back(possiblePosition);

			if (relationship == "Foe")
			{
				displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
			}
		}
		//else do nothing
	}

	return moves;
}

vector<string> RookMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<string> moves;

	vector<string> upMoves = lookForVerticalMoves(file, rank, piece, true, game);
	vector<string> downMoves = lookForVerticalMoves(file, rank, piece, false, game);

	moves.insert(moves.end(), upMoves.begin(), upMoves.end());
	moves.insert(moves.end(), downMoves.begin(), downMoves.end());

	vector<string> rightMoves = lookForHorizontalMoves(file, rank, piece, true, game);
	vector<string> leftMoves = lookForHorizontalMoves(file, rank, piece, false, game);

	moves.insert(moves.end(), rightMoves.begin(), rightMoves.end());
	moves.insert(moves.end(), leftMoves.begin(), leftMoves.end());

	return moves;
}


vector<string> BishopMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<string> possiblePositionsForBishop = lookForDiagonalMoves(file, rank, piece, game);


	return possiblePositionsForBishop;
}


vector<string> QueenMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<string> upMoves = lookForVerticalMoves(file, rank, piece, true, game);
	vector<string> downMoves = lookForVerticalMoves(file, rank, piece, false, game);

	vector<string> rightMoves = lookForHorizontalMoves(file, rank, piece, true, game);
	vector<string> leftMoves = lookForHorizontalMoves(file, rank, piece, false, game);

	vector<string> diagonalMoves = lookForDiagonalMoves(file, rank, piece, game);

	vector<string> possiblePositionsForQueen;

	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), upMoves.begin(), upMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), downMoves.begin(), downMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), rightMoves.begin(), rightMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), leftMoves.begin(), leftMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), diagonalMoves.begin(), diagonalMoves.end());

	//= upMoves + downMoves + rightMoves + leftMoves + diagonalMoves; //not a supported operator for std::vector

	return possiblePositionsForQueen;

}


vector<string> AbstractMoveRules::lookForVerticalMoves(char file, int rank, const string& piece, bool isUp, const ChessGame& game)
{

	vector<string> possibleVerticalMoves;

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

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleVerticalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleVerticalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);

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

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleVerticalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleVerticalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
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

vector<string> AbstractMoveRules::lookForHorizontalMoves(char file, int rank, const string& piece, bool isRight, const ChessGame& game)

{
	vector<string> possibleHorizontalMoves;

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

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleHorizontalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleHorizontalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
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

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleHorizontalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleHorizontalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
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

vector<string> AbstractMoveRules::lookForDiagonalMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	char newFile = file;
	int newRank = rank;

	vector<string> diagonalMoves;

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

					if (isPositionInBounds(newFile, newRank))
					{
						string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

						string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

						string relationship =getPieceRelationship(piece, contentsOfPossiblePosition);

						if (relationship == "Neutral" || relationship == "Foe")
						{
							diagonalMoves.push_back(possiblePosition);
							if (relationship == "Foe")
							{
								displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
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

