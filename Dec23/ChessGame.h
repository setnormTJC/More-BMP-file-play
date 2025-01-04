#pragma once



#include"ChessImageBMP.h"

#include<chrono>  
#include<thread>  //for sleeping thread before displaying updated chessboard after a move is made (or similar "events")

#include"NodeJSFunctions.h"

#include<random> //for goofiness 

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
	string getPieceAtPosition(const string& position) const;


	/*modifies: piecesToPossiblePositions
	* reads:
	*/
	void getPiecesToMoves();


	/*Trying to be cute with the function name here
	This method looks through boardImage.piecesToPositions for the king's location
	Then it looks through piecesToPossiblePositions to see if any (enemy) pieces
	*/
	bool checkForCheck(const string& colorToCheckForCheck);

	/*Anticipate that this "helper" function will only be called by `movePiece`*/
	void movePieceHelper(const string& piece, const string& newPosition);

	void takePiece(const string& piece, const string& newPosition);

	void drawBoardHelper(const string& oldPosition);

	/*Simulate move updates the board state, checks for certain conditions (checks, forks, others), and then sets the board back to previous state*/
	void simulateMove(const string& piece, const string& newPosition, bool determineIfSelfCheck, bool determineIfFork); 


	bool checkForFork(); 

private: //member variables  (also private)

	map<string, string> positionsToPieces;


	/*ex: "whitePawnE2" will be mapped to {E3, E4} initially*/
	map <string, vector<string>> piecesToMoves;
	
	std::random_device device{};
	std::mt19937 engine{ device() };



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

	bool checkForMate(const string& colorToCheckForMate);

	array<pair<char, int>, 2> getRandomMove(); 


public:
	/************************public member VARIABLES ***********************************/
	/*Ex: pawns are worth 1, knights and bishops worth 3, rooks worth 5, etc*/
	size_t blackScore = 0;
	//ex: if whiteScore > blackScore, white is "winnning" 
	size_t whiteScore = 0;

	size_t moveCount; // initialized to 0 in ChessGame.cpp at top of file

	ChessImageBMP boardImage;
	
	bool isKingInCheck = false; 


	friend class AbstractMoveRules;
};

class AbstractMoveRules
{
public: 
	virtual vector<string> getMoves(char currentFile, int currentRank, const string& currentPiece, const ChessGame& game) = 0;
	virtual ~AbstractMoveRules() = default; 

protected:
	/*Note that this method is for AbstractMoveRules class and is defined in the HEADER FILE - due to problems if trying to define in cpp file*/
	static string getPieceAtPosition(const ChessGame& game, const string& possiblePosition)
	{
		return( game.getPieceAtPosition(possiblePosition));
	}

	vector<string> lookForVerticalMoves(char file, int rank, const string& piece, bool isUp, const ChessGame& game);
	vector<string> lookForHorizontalMoves(char file, int rank, const string& piece, bool isRight, const ChessGame& game);
	vector<string> lookForDiagonalMoves(char file, int rank, const string& piece, const ChessGame& game);
};

class WhitePawnMoveRules : public AbstractMoveRules
{
public: 
	vector<string> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class BlackPawnMoveRules : public AbstractMoveRules
{
public:
	vector<string> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class KingMoveRules : public AbstractMoveRules
{
public:
	vector<string> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class KnightMoveRules : public AbstractMoveRules
{
public:
	vector<string> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class RookMoveRules : public AbstractMoveRules
{
public:
	vector<string> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class BishopMoveRules : public AbstractMoveRules
{
public:
	vector<string> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class QueenMoveRules : public AbstractMoveRules
{
public:
	vector<string> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};










