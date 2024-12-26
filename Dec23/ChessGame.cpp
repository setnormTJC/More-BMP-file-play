#include "ChessGame.h"


ChessGame::ChessGame()
	:ChessImageBMP()
{

}


bool ChessGame::isPieceOnBoard(const string& pieceName)
{
	auto pieceNameIterator = std::find(pieceNames.begin(), pieceNames.end(), pieceName); 

	return (pieceNameIterator != pieceNames.end());
}

auto ChessGame::getChessPositionCoordinates(const string& position)
{

	auto positionIterator = positionsToImageCoordinates.find(position); 
	
	if (positionIterator != positionsToImageCoordinates.end())
	{
		return pair<int, int> {positionIterator->second.first, positionIterator->second.second};
	}

	else
	{
		__debugbreak(); 
	}
}


void ChessGame::arbitrarilyMovePiece(const string& pieceName, const string& newPosition)
{
	if (!isPieceOnBoard(pieceName))
	{
		cout << pieceName << " not found\n";
		return; 
	}

	//erase the old record for the current piece: 
	pieceNamesToPositions.erase(pieceName);
	//map piece's new position: 
	pieceNamesToPositions.insert({ pieceName, newPosition });

	//get new coordinates
	//auto coordinates = getChessPositionCoordinates(newPosition); 

	//get corresponding image: 
	//ImageBMP pieceImage =  pieceNamesToImages.at(pieceName);
	
	//draw piece in new position
	drawPieces(); 

	//finally, "paint" over the old position with the appropriate (light or dark) square: 
	
	//add sleep then display new file: 
	writeImageFile("currentTime.bmp"); //get current time with command later...


	system("currentTime.bmp"); 
}

