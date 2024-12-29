#pragma once



#include"ChessImageBMP.h"

#include<chrono>  
#include<thread>  //for sleeping thread before displaying updated chessboard after a move is made (or similar "events")


class ChessGame
{
private: 
	bool isThatColorTurn(const string& pieceName);
	bool isPieceOnBoard(const string& pieceName);


	void generatePiecesToPossiblePositions();
	/*Not considering pieces being "in the way", for now
	@return - nothing, modifies a member variable (a map <string, vector<string>>) of the `ChessRule` class
	@param currentPosition - a chess position such as A8, G4, etc.
	*/
	vector<string> getPossiblePositionsForCurrentPiece(const string& currentPieceName, const string& currentPosition);

	vector<string> getWhitePawnPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);
	vector<string> getBlackPawnPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);
	vector<string> getKingPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);
	vector<string> getKnightPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);


	/*
	@returns 3 possibilites -
	1) the empty string (square is unoccupied)
	2) a friend piece
	3) an opponent's piece
	*/
	string getContentsOfPossiblePosition(const string& possiblePosition);

	string getFriendOrFoeOrNeutral(const string& contentsOfFirstPosition, const string& contentsOfSecondPosition);
	

private: //just using the "indentation" here to "separate" member variables and member functions 
	map<string, string> positionsToPieces;// = switchMapKeysAndValues(boardImage.pieceNamesToPositions);

	map <string, vector<string>> piecesToPossiblePositions;

	size_t moveCount = 0; 

	bool canTakeOpponentPiece = false; 

public: 
	/************************public member functions ***********************************/
	ChessGame();

	/*little logic for now, just simple checks*/
	void movePiece(const string& pieceName, const string& newPosition); 

	/*an overloaded that allows user to specify start pos and end pos (probably more friendly)
	@returns nothing - but NOTE that this overload of movePiece calls the OTHER overload of movePiece (which does all of the work) 
	*/
	void movePiece(const char oldPositionFile, const int oldPositionRank, const char newPositionFile, const int newPositionRank);


	bool isGameOver();
public: 
	/************************public member VARIABLES ***********************************/
	/*Ex: pawns are worth 1, knights and bishops worth 3, rooks worth 5, etc*/
	int blackScore = 0;
	//ex: if whiteScore > blackScore, white is "winnning" 
	int whiteScore = 0; 

	ChessImageBMP boardImage{};
};

