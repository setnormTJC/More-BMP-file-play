#pragma once
#include "ChessGame.h"

#include<memory> //for smart pointers in chess game tree nodes (using new and delete result in interesting heap error (double deletion?)
#include<algorithm> //for std::sort -> called to sort pieces by pieceValue -> this has a positive effect on GameTree pruning ...

#include<sstream> //Copilot suggested use of stringstream for hashing board state 



using std::sort; 

/*Tree class will be made of nodes*/
struct Node
{
	/*anticipate that "data" will be piecesToMoves for ChessGame - since updating the board requires `pieceName`*/
	unordered_map <string, vector<ChessPosition>> piecesToMoves;

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
	Tree(const unordered_map <string, vector<ChessPosition>>& piecesToMoves);

	~Tree();
};

/*AIChessGame will incorporate GameTree and minimax methods*/
class AIChessGame :public ChessGame
{
	/*overflows in the QUINT-illions ~game depth of 18 
	- I have zero hopes of going this deep (so no worry about overflow) */
	size_t gameTreeNodeCount = 0;

	/*used as a way of observing tree pruning effectiveness 
	(if final gameTreeNodeCount >> alphaBetaMinimaxCallCounter, pruning is effective)*/
	static size_t alphaBetaMinimaxCallCounter;

	/*board state will be hashed to a string (the key) - and the VALUE is the score of the current board state*/
	unordered_map<string, int> cacheTable;

	/******************private functions*************************/
	vector<ChessPosition> orderMoves(const vector<ChessPosition>& moves, const string& piece);

	/*Used by getMinimaxAlphaBetaMove - might consider moving this func. to ChessGame parent class ...*/
	bool isMoveValid(const string& piece, const ChessPosition& move);

	/*NOTE that this method takes by NON-CONST reference the takenPieceName and pieceTaken vars - and might UPDATE them!
	@returns FALSE if the move puts a friendly king in check 
	*/
	bool updateBoardState(const string& piece, const ChessPosition& move, ChessPosition& oldPosition,
		string& takenPieceName, bool& pieceTaken);

	void undoUpdateBoardState(const string& piece, const ChessPosition& move, const ChessPosition& oldPosition,
		const string& takenPieceName, const bool& pieceTaken);

	int minimaxAlphaBeta(Node& node, int depth, bool isMaximizingPlayer, int alpha, int beta);

	string hashBoardState();
	
	/*Maybe medium complexity - factors in weakness and strengths of piece location using "piece tables"
	ex: if knight is in corner, it can only move to two spots (rather than max of 8 spots in middle of board)
	*/
	int evaluateGameState();

	/*called by `evaluateGameState`
	* @params firstLetterOfPieceName will be either 'w' or 'b' -> this is hopefully a fast way to do this...
	*/
	bool isPieceThreatened(const ChessPosition& position, char firstLetterOfPieceName);

public: 
	AIChessGame(); 


	/*public for now - move to private later*/
	void getGameTreeRecursively(Node& parentNode, unordered_map <string, vector<ChessPosition >> &piecesToMoves, 
		int currentDepth, int maxDepth, int moveCount);

	pair<string, ChessPosition> getMinimaxAlphaBetaMove(const short desiredDepth);

};

