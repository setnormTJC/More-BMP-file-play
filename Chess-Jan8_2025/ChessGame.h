#pragma once


#include"ChessImageBMP.h"

#include<chrono>  
#include<thread>  //for sleeping thread before displaying updated chessboard after a move is made (or similar "events")

#include"NodeJSFunctions.h"

#include<memory>

#include<unordered_set>

using std::unique_ptr, std::make_unique;
using std::unordered_set;

/*This "basic" ChessGame class allows playing without move suggestion*/
class ChessGame
{
	/*********************************************private member variables***********************************/

	bool hasWhiteKingMoved = false;
	bool hasBlackKingMoved = false;
	bool hasWhiteRookMoved[2] = { false, false }; // [0] for kingside, [1] for queenside
	bool hasBlackRookMoved[2] = { false, false }; // [0] for kingside, [1] for queenside


	bool isKingInCheck = false;
	bool isCheckMate = false;



	/*********************************************private member functions***********************************/
	bool isThatColorTurn(const string& pieceName);
	bool isPieceOnBoard(const string& pieceName);

	/*this method gets input from localhost::3000
	through the input file that MUST be at relative filepath: testingNodeJS/public/clickCoordinates.txt
	*/
	array<ChessPosition, 2> getAndConfirmMove();

	/*Note: movePiece calls movePieceHelper - which will potentially update isKingInCheck*/
	void movePiece(const ChessPosition& oldPosition, const ChessPosition& newPosition);

	bool checkForMate(const string& colorToCheckForMate);


	/*Note: this is the only method that modifies has..KingMoved, has...RookMoved*/
	void handleCastling(const string& piece, const ChessPosition& oldPosition, const ChessPosition& newPosition);

	/*Anticipate that this "helper" function will only be called by `movePiece`*/
	void movePieceHelper(const string& piece, const ChessPosition& newPosition);
	void drawBoardHelper(const ChessPosition& oldPosition);




	/****PROTECTED member vars - accessible to AIChessGame child class*******************/
protected: 
	/*unsigned short - UNLIKELY that game exceeds 32K moves ...*/
	unsigned short moveCount;

	ChessImageBMP boardImage;

	unordered_map<ChessPosition, string, ChessPositionHash> positionsToPieces;
	///*ex: "whitePawnE2" will be mapped to {E3, E4} initially*/
	unordered_map <string, vector<ChessPosition>> piecesToMoves;

	unordered_map<string, vector<ChessPosition>> cachedMoves;
	unordered_set<string> piecesToUpdate;


	/****protected functions - accessible to AIChessGame child class*******************/
	void takePiece(const string& piece, const ChessPosition& newPosition);
	unordered_map<string, ChessPosition> switchPositionsAndPieces(const unordered_map<ChessPosition, string, ChessPositionHash>& positionsToPieces);
	void getPiecesToMoves();

	string getPieceAtPosition(const ChessPosition& position) const;

	bool checkForCheck(const string& colorToCheckForCheck);

	void updatePieceMoves(const string& piece);
	void invalidateCache();


public:
	/************************public member VARIABLES ***********************************/
	
	//...none for now 

	/************************public member functions ***********************************/
	ChessGame(); 


	void showAllPossibleMoves();

	void play(); 

	/*Applies "diagonal rule" to bishops, "L rule" to knights, etc.*/
	friend class AbstractMoveRules;
	/*Also give KingMoveRules friendship (for access to has...KingMoved and has...RookMoved*/
	friend class KingMoveRules; 
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
