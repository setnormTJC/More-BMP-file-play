#pragma once



#include"ChessImageBMP.h"

#include<chrono>  
#include<thread>  //for sleeping thread before displaying updated chessboard after a move is made (or similar "events")

#include"NodeJSFunctions.h"

#include<random> //for goofiness 

#include<memory> //for smart pointers in chess game tree nodes (using new and delete result in interesting heap error (double deletion?)

#include<algorithm> //for std::sort -> called to sort pieces by pieceValue -> this has a positive effect on GameTree pruning ...

#include<unordered_map> //for cache table! prevents game tree from "retreading" previously-visited territory (vague) 

#include<future> //...MULTI-THREADING

using std::unique_ptr, std::make_unique; 
using std::sort; 
using std::unordered_map;

using std::future; //again- MULTI-THREADING 



/*Tree class will be made of nodes*/
struct Node
{
	/*anticipate that "data" will be piecesToMoves for ChessGame - since updating the board requires `pieceName`*/
	map <string, vector<string>> data;

	//vector<Node*> childrenLinks; //former, manual memory mgmt approach
	vector<unique_ptr<Node>> childrenLinks; 

	Node();

};

class Tree
{

public:
	//Node root; //former manual memory mgmt approach
	unique_ptr<Node> root; 
	int depth;

	Tree();
	/*anticipate that "data" will be piecesToMoves for ChessGame - since updating the board requires `pieceName`*/
	Tree(const map <string, vector<string>>& data);



	/*Implementation is similar to a DFS algorithm - note that BFS with a queue is also an option (not included here) */
	//void generateGameTreeRecursively(Node& parentNode, vector<string>& data, int currentDepth, int maxDepth);

	//void deleteNode(Node* node); //former, manual memory mgmt approach 

	~Tree();
};

class ChessGame
{

	/*********************************************private member variables***********************************/
	ChessImageBMP boardImage;
	//Tree gameTree; //don't need to STORE this for move prediction (and it will be a memory hog for sufficient tree depth) 

	map<string, string> positionsToPieces;	
	/*ex: "whitePawnE2" will be mapped to {E3, E4} initially*/
	map <string, vector<string>> piecesToMoves;

	size_t blackScore = 0;
	size_t whiteScore = 0;



	size_t gameTreeNodeCount = 0; 

	static size_t recursionCounter; 

	unordered_map<string, int> transpositionTable;


	std::random_device device{};
	std::mt19937 engine{ device() };

	/*********************************************private member functions***********************************/
	bool isThatColorTurn(const string& pieceName);
	bool isPieceOnBoard(const string& pieceName);
	bool isMoveValid(const string& piece, const string& move);



	void getPiecesToMoves();



	/*Anticipate that this "helper" function will only be called by `movePiece`*/
	void movePieceHelper(const string& piece, const string& newPosition);
	
	
	void takePiece(const string& piece, const string& newPosition);

	/*Simulate move updates the board state, checks for certain conditions (checks, forks, others), and then sets the board back to previous state*/
	void simulateMove(const string& piece, const string& newPosition, bool determineIfSelfCheck, bool determineIfFork);

	void drawBoardHelper(const string& oldPosition);

	/*Call this at the end of movePieceHelper if you want to see the gametree - not needed for good move prediction*/
	void getGameTreeRecursively(Node& parentNode, map <string, vector<string>>& data, int currentDepth, int maxDepth, int moveCount);

	int minimax(Node& node, int depth, bool isMaximizingPlayer);

	int minimaxAlphaBeta(Node& node, int depth, bool isMaximizingPlayer, int alpha, int beta);

	vector<string> orderMoves(const vector<string>& moves, const string& piece);


	/*Nice!*/
	string hashBoardState();

	int minimaxAlphaBetaParallel(Node& node, int depth, bool isMaximizingPlayer, int alpha, int beta);

public:
	/************************public member functions ***********************************/
	ChessGame();
	
	ChessGame(const ChessGame& other); //copy constructor 

	void showAllPossibleMoves();

	/*"Strong-ish" is depth-dependent*/
	pair<string, string> getStrongishMove(int desiredDepth);

	pair<string, string> getMinimaxMove(int desiredDepth);

	pair<string, string> getMinimaxAlphaBetaMove(int desiredDepth);

	/*this method gets input from localhost::3000
	through the input file that MUST be at relative filepath: testingNodeJS/public/clickCoordinates.txt
	*/
	array<pair<char, int>, 2> getAndConfirmMove();
	string getPieceAtPosition(const string& position) const;

	void movePiece(const char oldFile, const int oldRank, const char newFile, const int newRank);

	bool isGameOver();
	bool checkForCheck(const string& colorToCheckForCheck);
	bool checkForMate(const string& colorToCheckForMate);

	array<pair<char, int>, 2> getRandomMove(); 

	/*Return value is found using a formula involving piece values from the Shannon paper
	* ONLY considers "material balance" 
	* @returns the score for the current board state - if > 0, WHITE is winning (arbitrary convention), if < 0, BLACK is winning
	*/
	int simplestEvaluateGameState();

	/*Maybe medium complexity - factors in weakness and strengths of piece location using "piece tables"
	ex: if knight is in corner, it can only move to two spots (rather than max of 8 spots in middle of board) 
	*/
	int evaluateGameState(); 
public:
	/************************public member VARIABLES ***********************************/

	size_t moveCount; 
	
	bool isKingInCheck = false; 


	bool whiteKingMoved = false;
	bool blackKingMoved = false;
	bool whiteRookMoved[2] = { false, false }; // [0] for kingside, [1] for queenside
	bool blackRookMoved[2] = { false, false }; // [0] for kingside, [1] for queenside


	/********************************friend class(es)*******************************/
	/*Applies "diagonal rule" to bishops, "L rule" to knights, etc.*/
	friend class AbstractMoveRules;
	//friend class Tree;
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










