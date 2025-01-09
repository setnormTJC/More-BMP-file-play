#pragma once


#include"ChessImageBMP.h"

#include<chrono>  
#include<thread>  //for sleeping thread before displaying updated chessboard after a move is made (or similar "events")

#include"NodeJSFunctions.h"



/*This "basic" ChessGame class allows playing without move suggestion*/
class ChessGame
{
	/*********************************************private member variables***********************************/
	unordered_map<ChessPosition, string, ChessPositionHash> positionsToPieces;
	///*ex: "whitePawnE2" will be mapped to {E3, E4} initially*/
	unordered_map <string, vector<ChessPosition>> piecesToMoves;




	/*********************************************private member functions***********************************/
	bool isThatColorTurn(const string& pieceName);
	bool isPieceOnBoard(const string& pieceName);
	void getPiecesToMoves();




public:
	/************************public member VARIABLES ***********************************/

	/*unsigned short - UNLIKELY that game exceeds 32K moves ...*/
	unsigned short moveCount;

	bool isKingInCheck = false;



	bool whiteKingMoved = false;
	bool blackKingMoved = false;
	bool whiteRookMoved[2] = { false, false }; // [0] for kingside, [1] for queenside
	bool blackRookMoved[2] = { false, false }; // [0] for kingside, [1] for queenside


	ChessImageBMP boardImage;


	/*Anticipate that this "helper" function will only be called by `movePiece`*/
	void movePieceHelper(const string& piece, const ChessPosition& newPosition);
	void drawBoardHelper(const ChessPosition& oldPosition);

public:
	/************************public member functions ***********************************/
	ChessGame(); 

	string getPieceAtPosition(const ChessPosition& position) const;

	void showAllPossibleMoves();

	/*this method gets input from localhost::3000
	through the input file that MUST be at relative filepath: testingNodeJS/public/clickCoordinates.txt
	*/
	array<ChessPosition, 2> getAndConfirmMove();

	void movePiece(const ChessPosition& oldPosition, const ChessPosition& newPosition);


	/*Applies "diagonal rule" to bishops, "L rule" to knights, etc.*/
	friend class AbstractMoveRules;


};


class AbstractMoveRules
{
public:
	virtual vector<ChessPosition> getMoves(char file, int rank, const string& currentPiece, const ChessGame& game) = 0;
	virtual ~AbstractMoveRules() = default;

protected:
	/*Note that this method is for AbstractMoveRules class and is defined in the HEADER FILE - due to problems if trying to define in cpp file*/
	static string getPieceAtPosition(const ChessGame& game, const ChessPosition& possiblePosition)
	{
		return(game.getPieceAtPosition(possiblePosition));
	}

	vector<ChessPosition> lookForVerticalMoves(char file, int rank, const string& piece, bool isUp, const ChessGame& game);
	vector<ChessPosition> lookForHorizontalMoves(char file, int rank, const string& piece, bool isRight, const ChessGame& game);
	vector<ChessPosition> lookForDiagonalMoves(char file, int rank, const string& piece, const ChessGame& game);
};

class WhitePawnMoveRules : public AbstractMoveRules
{
public:
	vector<ChessPosition> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class BlackPawnMoveRules : public AbstractMoveRules
{
public:
	vector<ChessPosition> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class KingMoveRules : public AbstractMoveRules
{
public:
	vector<ChessPosition> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class KnightMoveRules : public AbstractMoveRules
{
public:
	vector<ChessPosition> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class RookMoveRules : public AbstractMoveRules
{
public:
	vector<ChessPosition> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class BishopMoveRules : public AbstractMoveRules
{
public:
	vector<ChessPosition> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};

class QueenMoveRules : public AbstractMoveRules
{
public:
	vector<ChessPosition> getMoves(char file, int rank, const string& piece, const ChessGame& game) override;
};
