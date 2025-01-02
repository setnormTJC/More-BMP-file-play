#pragma once



#include"ChessImageBMP.h"

#include<chrono>  
#include<thread>  //for sleeping thread before displaying updated chessboard after a move is made (or similar "events")

#include"NodeJSFunctions.h"

class PieceRules
{
	static vector<string> getWhitePawnMoves(int currentRank, char currentFile, const string& currentPiece);
	static vector<string> getBlackPawnPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);
	static vector<string> getKingPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);
	static vector<string> getKnightPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);
	
	static vector<string> getRookPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);
	static vector<string> getBishopPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);
	static vector<string> getQueenPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);


	/*Anticipate queen and rook calling this function*/
	static vector<string> lookForVerticalMoves(int currentRank, char currentFile, const string& currentPieceName, bool isUp);
	/*Anticipate queen and rook calling this function*/
	static vector<string> lookForHorizontalMoves(int currentRank, char currentFile, const string& currentPieceName, bool isRight);

	static vector<string> lookForDiagonalMoves(int currentRank, char currentFile, const string& currentPieceName);


public:
	/*Static so that ChessGame class can call without having to instantiate an object*/
	static vector<string> getPossiblePositionsForCurrentPiece(const string& currentPieceName, const string& currentPosition);



	PieceRules();


};




class ChessGame
{
private: //member functions
	bool isThatColorTurn(const string& pieceName);
	bool isPieceOnBoard(const string& pieceName);

	/*
	@returns 3 possibilites -
	1) the empty string (square is unoccupied)
	2) a friend piece
	3) an opponent's piece
	*/
	static string getPieceAtPosition(const string& possiblePosition);

	static string getPieceRelationship(const string& currentPiece, const string& contentsOfNewPosition);

	/*modifies: piecesToPossiblePositions
	* reads:
	*/
	static void getPiecesToMoves();
	

	/*Trying to be cute with the function name here
	This method looks through boardImage.piecesToPositions for the king's location
	Then it looks through piecesToPossiblePositions to see if any (enemy) pieces 
	*/
	bool checkForCheck(const string& colorToCheckForCheck);

	/*Anticipate that this "helper" function will only be called by `movePiece`*/
	void movePieceHelper(const string& piece, const string& newPosition);

	void takePiece(const string& piece, const string& newPosition);

	void drawBoardHelper(const string& oldPosition);

	/*TEMPORARILY modifies board state, checks for check, then resets the board state */
	static bool doesMoveResultInSelfCheck(const string& piece, const string& oldPosition, const string& newPosition, const string& color);

private: //member variables  (also private)
	
	static map<string, string> positionsToPieces;

	/*ex: "whitePawnE2" will be mapped to {E3, E4} initially*/
	static map <string, vector<string>> piecesToMoves;

	/*antipcated way of calling this function - 
	ex: at END of getWhitePawnMoves, just before returning whitePawnMoves: 

	loop through whitePawnMoves
	if is_check(move), 

	then remove from the list of whitePawnMoves 

	*/
	bool is_check(const string& move); 
public: 
	/************************public member functions ***********************************/
	ChessGame();

	void showAllPossibleMoves(); 

	array<pair<char, int>, 2> getAndConfirmMove();

	/*an overloaded that allows user to specify start pos and end pos (probably more friendly)
	@returns nothing - but NOTE that this overload of movePiece calls the OTHER overload of movePiece (which does all of the work) 
	*/
	void movePiece(const char oldFile, const int oldRank, const char newFile, const int newRank);

	bool isGameOver();
public: 
	/************************public member VARIABLES ***********************************/
	/*Ex: pawns are worth 1, knights and bishops worth 3, rooks worth 5, etc*/
	size_t blackScore = 0;
	//ex: if whiteScore > blackScore, white is "winnning" 
	size_t whiteScore = 0; 

	static size_t moveCount; // initialized to 0 in ChessGame.cpp at top of file

	static ChessImageBMP boardImage;

	friend class PieceRules; 

};



