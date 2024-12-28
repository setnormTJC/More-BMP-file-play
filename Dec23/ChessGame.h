#pragma once



#include"ChessImageBMP.h"

#include<chrono>  
#include<thread>  //for sleeping thread before displaying updated chessboard after a move is made (or similar "events")


class ChessRule
{
public: 



	/*ex: bishop, knight, king, etc. - note that I stored this in alphabetical order ...*/
	vector<string> nonSpecificPieceNames =
	{
		"blackPawn",
		"bishop",
		"knight",
		"king",
		"queen",
		"rook",
		"whitePawn"
	};



};

class ChessGame
{
private: 
	/*Not considering pieces being "in the way", for now
	@return - nothing, modifies a member variable (a map <string, vector<string>>) of the `ChessRule` class
	@param currentPosition - a chess position such as A8, G4, etc.
	*/
	vector<string> getPossiblePositionsForCurrentPiece(const string& currentPieceName, const string& currentPosition);


	/*private in anticipation of only being called by other member functions of this class*/
	bool isPieceOnBoard(const string& pieceName); 
	

	map<string, string> positionsToPieces;// = switchMapKeysAndValues(boardImage.pieceNamesToPositions);

public: 
	/************************public member functions ***********************************/
	ChessGame();

	/*little logic for now, just simple checks*/
	void arbitrarilyMovePiece(const string& pieceName, const string& newPosition); 

	bool canOpponentPieceBeTaken(); 

	bool isPositionOccupiedByFriend(const string& currentPieceName, const string& currentPosition);

	/*NOTE: this method will not handle "takes" - as a result, for example, DIAGONAL pawn moves will not be generated here ...*/
	void generatePiecesToPossiblePositions();

	
	/************************public member VARIABLES ***********************************/
	/*Ex: pawns are worth 1, knights and bishops worth 3, rooks worth 5, etc*/
	int points;

	map <string, vector<string>> piecesToPossiblePositions;
	ChessImageBMP boardImage{};
};

