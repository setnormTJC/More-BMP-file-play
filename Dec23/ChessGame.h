#pragma once



#include"ChessImageBMP.h"

class ChessGame : public ChessImageBMP
{

	/*private in anticipation of only being called by other member functions of this class*/
	bool isPieceOnBoard(const string& pieceName); 
	
	auto getChessPositionCoordinates(const string& position);

public: 

	ChessGame();

	/*little logic for now, just simple checks*/
	void arbitrarilyMovePiece(const string& pieceName, const string& newPosition); 


};

