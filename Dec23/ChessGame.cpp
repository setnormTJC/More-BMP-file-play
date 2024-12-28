#include "ChessGame.h"


ChessGame::ChessGame()
{
	//boardImage

	positionsToPieces = switchMapKeysAndValues(boardImage.piecesToPositions);
}



bool ChessGame::isPieceOnBoard(const string& pieceName)
{
	auto pieceNameIterator = std::find(boardImage.pieceNames.begin(), boardImage.pieceNames.end(), pieceName); 
	//anticipate ERASING pieceName from vector `pieceNames` when pieces get "taken" in later functions...

	return (pieceNameIterator != boardImage.pieceNames.end());
}


/*NOTE: this method writes a (potentially "large") IMAGE file out with filename = current system time*/
void ChessGame::arbitrarilyMovePiece(const string& pieceName, const string& newPosition)
{
	if (!isPieceOnBoard(pieceName))
	{
		cout << pieceName << " not found\n";
		return;
	}

	//also update the "convenient flipped map": 
	auto oldPosition = boardImage.piecesToPositions.find(pieceName)->second;
	positionsToPieces.erase(oldPosition); //keys are unique, so maybe no need for `erase`?
	positionsToPieces.insert({ newPosition, pieceName });

	//map piece's new position: 
	boardImage.piecesToPositions[pieceName] = newPosition; 
	
	//draw piece in new position
	boardImage.drawPieces();

	//finally, "paint" over the old position with the appropriate (light or dark) square: 
	boardImage.fillPositionWithColor(oldPosition); 

	//add sleep then display new file with filename = current hour and second: 
	string currentHourAndMinute = getCurrentHourAndMinute();
	string filename = currentHourAndMinute + ".bmp"; 
	boardImage.writeImageFile(filename); //get current time with command later...

	std::this_thread::sleep_for(std::chrono::seconds{ 2 });
	system(filename.c_str());
}

bool ChessGame::canOpponentPieceBeTaken()
{
	return false;
}

bool ChessGame::isPositionOccupiedByFriend(const string& currentPieceName, const string& possiblePosition)
{
	string pieceColor;
	if (currentPieceName.find("black") != string::npos)
	{
		pieceColor = "black";
	}

	else
	{
		pieceColor = "white";
	}

	auto possiblePositionIterator = positionsToPieces.find(possiblePosition); 
	if (possiblePositionIterator == positionsToPieces.end())
	{
		//cout << "No pieces at that position\n";
		return false;
	}

	else//piece IS at that position - and it is either black or white 
	{
		if (possiblePositionIterator->second.find(pieceColor) != string::npos)
		{
			return true;
		}
		else
		{
			return false;
		}
	}


}

void ChessGame::generatePiecesToPossiblePositions()
{

	for (int i = 0; i < boardImage.pieceNames.size(); ++i)
	{
		string currentPieceName = boardImage.pieceNames.at(i);

		string currentPosition = boardImage.piecesToPositions.find(currentPieceName)->second;

		vector<string> possiblePositionsForCurrentPiece
			= getPossiblePositionsForCurrentPiece(currentPieceName, currentPosition);

		piecesToPossiblePositions.insert({ currentPieceName, possiblePositionsForCurrentPiece });
	}
}


vector<string> ChessGame::getPossiblePositionsForCurrentPiece(const string& currentPieceName, const string& currentPosition)
{
	vector<string> possiblePositionsForCurrentPiece;


	int currentRank = (int)(currentPosition.at(1) - 48); //ASCII value for character '0' is 48
	char currentFile = currentPosition.at(0);

	int newRank; 
	char newFile; 

	if (currentPieceName.find("blackPawn") != string::npos)
	{
		//black pawns "usually" decrement rank by 1
		newRank = currentRank - 1;
		string possiblePosition = currentFile + to_string(newRank);
		possiblePositionsForCurrentPiece.push_back(possiblePosition);

		//allow moving two positions only if rank is 7 (pawn has not yet moved): 
		if (currentRank == 7)
		{
			possiblePositionsForCurrentPiece.push_back(currentFile + to_string(newRank - 1));
		}
	}

	else if (currentPieceName.find("whitePawn") != string::npos)
	{
		newRank = currentRank + 1; 
		string possiblePosition = currentFile + to_string(newRank); 
		possiblePositionsForCurrentPiece.push_back(possiblePosition); 

		if (currentRank == 2)
		{
			possiblePositionsForCurrentPiece.push_back(currentFile + to_string(newRank + 1));
		}
	}

	//IGNORING placing self in check for now ....
	else if (currentPieceName.find("whiteKing") != string::npos)
	{
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

				if (isPositionInBounds(newRank, newFile))
				{
					string possiblePosition = newFile + to_string(newRank);
					if (!isPositionOccupiedByFriend(currentPieceName, possiblePosition))
					{
						possiblePositionsForCurrentPiece.push_back(possiblePosition); 
					}
				}
			}
		}
	}

	return possiblePositionsForCurrentPiece;
}


