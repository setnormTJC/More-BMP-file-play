#pragma once



#include"ChessImageBMP.h"

#include<chrono>  
#include<thread>  //for sleeping thread before displaying updated chessboard after a move is made (or similar "events")

#include"NodeJSFunctions.h"

#include<random> //for goofiness 

#include<memory> //for smart pointers in chess game tree nodes (using new and delete result in interesting heap error (double deletion?)

using std::unique_ptr, std::make_unique; 

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
	//Tree gameTree;

	map<string, string> positionsToPieces;	
	/*ex: "whitePawnE2" will be mapped to {E3, E4} initially*/
	map <string, vector<string>> piecesToMoves;

	size_t blackScore = 0;
	size_t whiteScore = 0;

	size_t gameTreeNodeCount = 0; 

	std::random_device device{};
	std::mt19937 engine{ device() };

	/*********************************************private member functions***********************************/
	bool isThatColorTurn(const string& pieceName);
	bool isPieceOnBoard(const string& pieceName);

	string getPieceAtPosition(const string& position) const;

	void getPiecesToMoves();

	bool checkForCheck(const string& colorToCheckForCheck);

	/*Anticipate that this "helper" function will only be called by `movePiece`*/
	void movePieceHelper(const string& piece, const string& newPosition);

	void takePiece(const string& piece, const string& newPosition);

	/*Simulate move updates the board state, checks for certain conditions (checks, forks, others), and then sets the board back to previous state*/
	void simulateMove(const string& piece, const string& newPosition, bool determineIfSelfCheck, bool determineIfFork);

	void drawBoardHelper(const string& oldPosition);

	void getGameTreeRecursively(Node& parentNode, map <string, vector<string>>& data, int currentDepth, int maxDepth, int moveCount);



public:
	/************************public member functions ***********************************/
	ChessGame();
	
	ChessGame(const ChessGame& other); //copy constructor 

	void showAllPossibleMoves();

	/*this method gets input from localhost::3000
	through the input file that MUST be at relative filepath: testingNodeJS/public/clickCoordinates.txt
	*/
	array<pair<char, int>, 2> getAndConfirmMove();

	void movePiece(const char oldFile, const int oldRank, const char newFile, const int newRank);

	bool isGameOver();

	bool checkForMate(const string& colorToCheckForMate);

	array<pair<char, int>, 2> getRandomMove(); 

	/*Return value is found using a formula involving piece values from the Shannon paper
	* @returns the score for the current board state - if > 0, WHITE is winning (arbitrary convention), if < 0, BLACK is winning
	*/
	int evaluateGameState();

	void updateBoard(const string& piece, const string& newPosition); 

public:
	/************************public member VARIABLES ***********************************/

	size_t moveCount; 
	
	bool isKingInCheck = false; 

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










