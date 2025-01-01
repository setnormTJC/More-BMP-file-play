#pragma once



#include"ChessImageBMP.h"



#include<chrono>  
#include<thread>  //for sleeping thread before displaying updated chessboard after a move is made (or similar "events")

#include"NodeJSFunctions.h"

class PieceRules
{


	static vector<string> getWhitePawnPossiblePositions(int currentRank, char currentFile, const string& currentPieceName);
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
private: 
	bool isThatColorTurn(const string& pieceName);
	bool isPieceOnBoard(const string& pieceName);


	void generatePiecesToPossiblePositions();


	/*
	@returns 3 possibilites -
	1) the empty string (square is unoccupied)
	2) a friend piece
	3) an opponent's piece
	*/
	static string getContentsOfPossiblePosition(const string& possiblePosition);

	static string getFriendOrFoeOrNeutral(const string& contentsOfFirstPosition, const string& contentsOfSecondPosition);
	
	/*Trying to be cute with the function name here
	This method looks through boardImage.positionsToPieces for the king's location
	Then it looks through piecesToPossiblePositions to see if any (enemy) pieces 
	*/
	bool checkForCheck(const string& colorToCheckForCheck);

	/*Anticipate that this "helper" function will only be called by `movePiece`*/
	void movePieceHelper(const string& pieceName, const string& newPosition);


	void takePiece(const string& pieceName, const string& newPosition);

	size_t findPieceValue(string fullPieceName);

	void drawBoardHelper(const string& oldPosition);

private: //just using the "indentation" here to "separate" member variables and member functions 
	
	map<string, size_t> piecesToValues;

	static map<string, string> positionsToPieces;// = switchMapKeysAndValues(boardImage.pieceNamesToPositions);

	map <string, vector<string>> piecesToPossiblePositions;

	size_t moveCount = 0; 

	//static to allow PieceRules class access without having to instantate ChessGame object 
	static bool canTakeOpponentPiece;// = false; 

	//static bool isKingInCheck; 

	//static bool isWhiteKingInCheck; '




public: 
	/************************public member functions ***********************************/
	ChessGame();

	void showAllPossibleMoves(); 

	array<pair<char, int>, 2> getAndConfirmChessMove();


	/*an overloaded that allows user to specify start pos and end pos (probably more friendly)
	@returns nothing - but NOTE that this overload of movePiece calls the OTHER overload of movePiece (which does all of the work) 
	*/
	void movePiece(const char oldPositionFile, const int oldPositionRank, const char newPositionFile, const int newPositionRank);


	bool isGameOver();
public: 
	/************************public member VARIABLES ***********************************/
	/*Ex: pawns are worth 1, knights and bishops worth 3, rooks worth 5, etc*/
	size_t blackScore = 0;
	//ex: if whiteScore > blackScore, white is "winnning" 
	size_t whiteScore = 0; 

	ChessImageBMP boardImage{};

	friend class PieceRules; 

};



