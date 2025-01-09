#include "AIChessGame.h"

/*define the static member variable of AIChessGame*/
size_t AIChessGame::alphaBetaMinimaxCallCounter = 0; 




AIChessGame::AIChessGame()
	:ChessGame()
{

}

void AIChessGame::getGameTreeRecursively(Node& parentNode, unordered_map<string, vector<ChessPosition>>& piecesToMoves, int currentDepth, int maxDepth, int moveCount)
{

	if (gameTreeNodeCount % 10'000 == 0) //expect very roughly 160'000 nodes at level 4 
	{
		cout << "Generating game tree ... currently, nodeCount = " << gameTreeNodeCount << "\n";
	}

	//(because ~20 moves per "ply" and 20 ^4 = 160'000

	if (currentDepth == maxDepth)
	{
		return;
	}

	string currentPlayer = (moveCount % 2 == 0) ? "white" : "black";

	for (int pieceIndex = 0; pieceIndex < boardImage.pieces.size(); ++pieceIndex)
	{
		string piece = boardImage.pieces.at(pieceIndex);
		if (piece.find(currentPlayer) == string::npos)
		{
			continue;
		}

		//sort possible moves by any move that TAKES an opponent piece with highest value
		//this will help alpha beta pruning 
		vector<ChessPosition> orderedMoves = orderMoves(piecesToMoves.at(piece), piece);

		//else get all moves available to that piece: 
		//for (int moveIndex = 0; moveIndex < piecesToMoves.at(piece).size(); ++moveIndex)
		for (const ChessPosition& newPosition : orderedMoves)
		{
			//ChessPosition newPosition = piecesToMoves.at(piece).at(moveIndex);

			/********************************Update board section************************************************/
			auto oldPosition = boardImage.piecesToPositions.find(piece)->second;
			positionsToPieces.erase(oldPosition); //effectively makes the old square empty

			string takenPieceName;
			bool pieceTaken = false;

			//handle taking an opponent piece by deleting from vector<string> pieceNames!
			if (positionsToPieces.find(newPosition) != positionsToPieces.end())
			{
				//opponent must be there (UNLESS castling!) 
				takenPieceName = positionsToPieces.at(newPosition);
				takePiece(piece, newPosition);
				pieceTaken = true;
			}

			//insert the piece at the new position
			positionsToPieces.insert({ newPosition, piece });

			//clear piecesToPositions and then get its contents by using the "switch" function: 
			boardImage.piecesToPositions.clear();
			boardImage.piecesToPositions = switchPositionsAndPieces(positionsToPieces);

			//once a piece has moved, possible positions change for ALL pieces, so clear the possible positions map
			piecesToMoves.clear();
			getPiecesToMoves();


			/****************Insertion of new node into GameTree section***************************************** */
			auto childNode = make_unique<Node>();
			childNode->piecesToMoves = piecesToMoves;

			parentNode.childrenLinks.push_back(std::move(childNode)); //note the std::move!!
			gameTreeNodeCount++;

			//recursive call: 
			//getGameTreeRecursively(*childNode, piecesToMoves, currentDepth + 1, maxDepth, moveCount + 1); //previous approach w/o smart ptrs
			getGameTreeRecursively(*parentNode.childrenLinks.back(),
				piecesToMoves, currentDepth + 1, maxDepth, moveCount + 1);


			/***********************BACKTRACK(undo the updates to the board state)**************************/
			positionsToPieces.erase(newPosition);
			positionsToPieces.insert({ oldPosition, piece });

			if (pieceTaken)
			{
				positionsToPieces.insert({ newPosition, takenPieceName });
				boardImage.pieces.push_back(takenPieceName);
			}

			boardImage.piecesToPositions.clear();
			boardImage.piecesToPositions = switchPositionsAndPieces(positionsToPieces);
			piecesToMoves.clear();
			getPiecesToMoves();

		}

	}

}

pair<string, ChessPosition> AIChessGame::getMinimaxAlphaBetaMove(const short desiredDepth)
{
	pair<string, ChessPosition> minimaxAlphaBetaMove;

	Tree gameTree{ piecesToMoves }; 
	getGameTreeRecursively(*gameTree.root, piecesToMoves, 0, desiredDepth, moveCount);

	int bestScore = (moveCount % 2 == 0) ? INT_MIN : INT_MAX; // White maximizes, Black minimizes
	bool isMaximizingPlayer = (moveCount % 2 == 0); // True if White's turn, False if Black's turn

	for (const unique_ptr<Node>& child : gameTree.root->childrenLinks)
	{
		for (const auto& pieceMoves : child->piecesToMoves)
		{
			const string& piece = pieceMoves.first; 

			////sort possible moves by any move that TAKES an opponent piece with highest value
			////this will help alpha beta pruning 
			//vector<ChessPosition> orderedMoves = orderMoves(pieceMoves.second, piece); 

			for (const ChessPosition& move : pieceMoves.second)
			{
				if (isMoveValid(piece, move))
				{
					ChessPosition oldPosition{}; //these three vars will be MODIFIED by `updateBoardState`
					string takenPieceName{}; //to be clear - this will remain the empty string in many cases 
					bool pieceTaken;

					if (!updateBoardState(piece, move, oldPosition, takenPieceName, pieceTaken))
					{
						continue; //skip this move if it results in friendly check 
					}

					// Evaluate the board state after the move using minimax with alpha-beta pruning
					int alpha = INT_MIN;
					int beta = INT_MAX;

					int score = minimaxAlphaBeta(*child, desiredDepth - 1, !isMaximizingPlayer, alpha, beta);


					// If the score is better than the best score, update the best score and best move
					if ((isMaximizingPlayer && score > bestScore) || (!isMaximizingPlayer && score < bestScore))
					{
						bestScore = score;
						minimaxAlphaBetaMove = { piece, move };
					}

					undoUpdateBoardState(piece, move, oldPosition, takenPieceName, pieceTaken);
				}
			}
		}
	}


	//reset number of nodes in game tree to 0 (because it will/may be regenerated on next move) 
	gameTreeNodeCount = 0;
	//also reset alphaBetaMinimaxCallCounter
	alphaBetaMinimaxCallCounter = 0;

	return minimaxAlphaBetaMove; 

}


vector<ChessPosition> AIChessGame::orderMoves(const vector<ChessPosition>& moves, const string& piece)
{
	
	vector<ChessPosition> orderedMoves = moves;

	sort(orderedMoves.begin(), orderedMoves.end(),
		[this, &piece](const ChessPosition& a, const ChessPosition& b) //better names for a and b? 
		{
			string pieceAtA = getPieceAtPosition(a);
			string pieceAtB = getPieceAtPosition(b);

			return sharedFunctions::findPieceValue(pieceAtA) > sharedFunctions::findPieceValue(pieceAtB);
		});

	return orderedMoves;

}

bool AIChessGame::isMoveValid(const string& piece, const ChessPosition& move)
{
	/*Copied code from isThatColorTurn (avoiding the print statemtent in recursion here)*/
	if (moveCount % 2 == 0)
	{
		//white's turn:
		if (piece.find("white") == string::npos)
		{
			return false;
		}
	}

	else //black's turn
	{
		if (piece.find("black") == string::npos)
		{
			return false;
		}
	}

	// Check if the move is within the list of possible moves for the piece
	auto it = piecesToMoves.find(piece);
	if (it != piecesToMoves.end())
	{
		const vector<ChessPosition>& possibleMoves = it->second;
		if (std::find(possibleMoves.begin(), possibleMoves.end(), move) != possibleMoves.end())
		{
			return true;
		}
	}
	return false;
}

bool AIChessGame::updateBoardState(const string& piece, const ChessPosition& move, ChessPosition& oldPosition, 
	string& takenPieceName, bool& pieceTaken)
{
	oldPosition = boardImage.piecesToPositions.find(piece)->second;

	takenPieceName;
	pieceTaken = false;

	// Update the board state
	positionsToPieces.erase(oldPosition);
	if (positionsToPieces.find(move) != positionsToPieces.end())
	{
		takenPieceName = positionsToPieces.at(move);
		takePiece(piece, move);
		pieceTaken = true;
	}
	positionsToPieces.insert({ move, piece });
	boardImage.piecesToPositions.clear();

	boardImage.piecesToPositions = switchPositionsAndPieces(positionsToPieces);

	piecesToMoves.clear();

	getPiecesToMoves();

	/*Don't allow checks!*/
	if (checkForCheck(sharedFunctions::getPieceColor(piece)))
	{

		//undo changes and continue: 
		positionsToPieces.erase(move);
		positionsToPieces.insert({ oldPosition, piece });
		if (pieceTaken)
		{
			positionsToPieces.insert({ move, takenPieceName });
			boardImage.pieces.push_back(takenPieceName);
		}
		boardImage.piecesToPositions.clear();
		boardImage.piecesToPositions = switchPositionsAndPieces(positionsToPieces);
		piecesToMoves.clear();
		getPiecesToMoves();

		return false;
	}

	return true; 
}

void AIChessGame::undoUpdateBoardState(const string& piece, const ChessPosition& move, const ChessPosition& oldPosition,
	const string& takenPieceName, const bool& pieceTaken)
{
	positionsToPieces.erase(move);
	positionsToPieces.insert({ oldPosition, piece });
	if (pieceTaken)
	{
		positionsToPieces.insert({ move, takenPieceName });
		boardImage.pieces.push_back(takenPieceName);
	}
	boardImage.piecesToPositions.clear();
	boardImage.piecesToPositions = switchPositionsAndPieces(positionsToPieces);
	piecesToMoves.clear();
	getPiecesToMoves();
}

int AIChessGame::minimaxAlphaBeta(Node& node, int depth, bool isMaximizingPlayer, int alpha, int beta)
{

	// Increment the counter
	alphaBetaMinimaxCallCounter++;

	if (alphaBetaMinimaxCallCounter % 1'000 == 0)
	{
		std::cout << "alphaBetaMinimax call count: " << alphaBetaMinimaxCallCounter << std::endl;
	}

	string boardHash = hashBoardState();
	if (cacheTable.find(boardHash) != cacheTable.end()) //"look" no further ... 
	{
		return cacheTable[boardHash];
	}

	if (depth == 0 || node.childrenLinks.empty())
	{
		int eval = evaluateGameState();
		cacheTable[boardHash] = eval;

		return eval;
	}

	if (isMaximizingPlayer)
	{
		int maxEval = INT_MIN;
		for (const auto& child : node.childrenLinks)
		{
			int eval = minimaxAlphaBeta(*child, depth - 1, false, alpha, beta);
			maxEval = std::max(maxEval, eval);
			alpha = std::max(alpha, eval);
			if (beta <= alpha)
			{
				break; // Beta cut-off
			}
		}
		cacheTable[boardHash] = maxEval;
		return maxEval;
	}
	else
	{
		int minEval = INT_MAX;
		for (const auto& child : node.childrenLinks)
		{
			int eval = minimaxAlphaBeta(*child, depth - 1, true, alpha, beta);
			minEval = std::min(minEval, eval);
			beta = std::min(beta, eval);
			if (beta <= alpha)
			{
				break; // Alpha cut-off
			}
		}
		cacheTable[boardHash] = minEval;
		return minEval;
	}
}

string AIChessGame::hashBoardState()
{
	/*don't dwell on the details here
	- just wanting a unique key to corresponding board state for an unordered map */
	std::stringstream ss;
	for (const auto& entry : positionsToPieces) {
		const ChessPosition& position = entry.first;
		const string& piece = entry.second;
		ss << piece << position.file << position.rank;
	}
	return ss.str();
}

int AIChessGame::evaluateGameState()
{
	// Piece values
	const int PAWN_VALUE = 1;
	const int KNIGHT_VALUE = 3;
	const int BISHOP_VALUE = 3;
	const int ROOK_VALUE = 5;
	const int QUEEN_VALUE = 9;
	const int KING_VALUE = 200;

	const int CHECK_PENALTY = 50;


	// Piece-square tables
	const float pawnTable[8][8] = {
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f },
		{ 1.0f, 1.0f, 2.0f, 3.0f, 3.0f, 2.0f, 1.0f, 1.0f },
		{ 0.5f, 0.5f, 1.0f, 2.5f, 2.5f, 1.0f, 0.5f, 0.5f },
		{ 0.0f, 0.0f, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.0f },
		{ 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -1.0f, -0.5f, 0.5f },
		{ 0.5f, 1.0f, 1.0f, -2.0f, -2.0f, 1.0f, 1.0f, 0.5f },
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
	};

	const float knightTable[8][8] = {
		{ -5.0f, -4.0f, -3.0f, -3.0f, -3.0f, -3.0f, -4.0f, -5.0f },
		{ -4.0f, -2.0f, 0.0f, 0.0f, 0.0f, 0.0f, -2.0f, -4.0f },
		{ -3.0f, 0.0f, 1.0f, 1.5f, 1.5f, 1.0f, 0.0f, -3.0f },
		{ -3.0f, 0.5f, 1.5f, 2.0f, 2.0f, 1.5f, 0.5f, -3.0f },
		{ -3.0f, 0.0f, 1.5f, 2.0f, 2.0f, 1.5f, 0.0f, -3.0f },
		{ -3.0f, 0.5f, 1.0f, 1.5f, 1.5f, 1.0f, 0.5f, -3.0f },
		{ -4.0f, -2.0f, 0.0f, 0.5f, 0.5f, 0.0f, -2.0f, -4.0f },
		{ -5.0f, -4.0f, -3.0f, -3.0f, -3.0f, -3.0f, -4.0f, -5.0f }
	};

	const float bishopTable[8][8] = {
		{ -2.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -2.0f },
		{ -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
		{ -1.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.5f, 0.0f, -1.0f },
		{ -1.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f, -1.0f },
		{ -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f },
		{ -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f },
		{ -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, -1.0f },
		{ -2.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -2.0f }
	};

	const float rookTable[8][8] = {
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f },
		{ -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f },
		{ -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f },
		{ -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f },
		{ -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f },
		{ -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f },
		{ 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f }
	};

	const float queenTable[8][8] = {
		{ -2.0f, -1.0f, -1.0f, -0.5f, -0.5f, -1.0f, -1.0f, -2.0f },
		{ -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
		{ -1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, -1.0f },
		{ -0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, -0.5f },
		{ 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, -0.5f },
		{ -1.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, -1.0f },
		{ -1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
		{ -2.0f, -1.0f, -1.0f, -0.5f, -0.5f, -1.0f, -1.0f, -2.0f }
	};

	const float kingTable[8][8] = {
		{ -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f },
		{ -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f },
		{ -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f },
		{ -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f },
		{ -2.0f, -3.0f, -3.0f, -4.0f, -4.0f, -3.0f, -3.0f, -2.0f },
		{ -1.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -1.0f },
		{ 2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 2.0f },
		{ 2.0f, 3.0f, 1.0f, 0.0f, 0.0f, 1.0f, 3.0f, 2.0f }
	};

	int score = 0;

	for (const auto& piece : boardImage.pieces)
	{
		ChessPosition position = boardImage.piecesToPositions.at(piece);
		char file = position.file;
		int rank = position.rank;

		//penalize for blunders: 
		if (piece[0] == 'w')
		{
			if (isPieceThreatened(position, 'b'))
			{
				score -= sharedFunctions::findPieceValue(piece);
			}
		}

		else
		{
			if (isPieceThreatened(position, 'w'))
			{
				score += sharedFunctions::findPieceValue(piece);
			}
		}



		if (piece.find("whitePawn") != string::npos)
		{
			score += PAWN_VALUE + pawnTable[rank][file];
		}
		else if (piece.find("blackPawn") != string::npos)
		{
			score -= PAWN_VALUE + pawnTable[7 - rank][file];
		}
		else if (piece.find("whiteKnight") != string::npos)
		{
			score += KNIGHT_VALUE + knightTable[rank][file];
		}
		else if (piece.find("blackKnight") != string::npos)
		{
			score -= KNIGHT_VALUE + knightTable[7 - rank][file];
		}
		else if (piece.find("whiteBishop") != string::npos)
		{
			score += BISHOP_VALUE + bishopTable[rank][file];
		}
		else if (piece.find("blackBishop") != string::npos)
		{
			score -= BISHOP_VALUE + bishopTable[7 - rank][file];
		}
		else if (piece.find("whiteRook") != string::npos)
		{
			score += ROOK_VALUE + rookTable[rank][file];
		}
		else if (piece.find("blackRook") != string::npos)
		{
			score -= ROOK_VALUE + rookTable[7 - rank][file];
		}
		else if (piece.find("whiteQueen") != string::npos)
		{
			score += QUEEN_VALUE + queenTable[rank][file];
		}
		else if (piece.find("blackQueen") != string::npos)
		{
			score -= QUEEN_VALUE + queenTable[7 - rank][file];
		}
		else if (piece.find("whiteKing") != string::npos)
		{
			score += KING_VALUE + kingTable[rank][file];

			if (checkForCheck("white"))
			{
				score -= CHECK_PENALTY;
			}
		}
		else if (piece.find("blackKing") != string::npos)
		{
			score -= KING_VALUE + kingTable[7 - rank][file];

			if (checkForCheck("black"))
			{
				score += CHECK_PENALTY;
			}
		}
	}

	return score;
}

bool AIChessGame::isPieceThreatened(const ChessPosition& position, char firstLetterOfPieceName)
{
	for (const auto& [opponentPiece, moves] : piecesToMoves)
	{
		if (opponentPiece[0] == firstLetterOfPieceName)
		{
			if (std::find(moves.begin(), moves.end(), position) != moves.end())
			{
				return true;
			}
		}
	}
	return false;
}



#pragma region Tree

Node::Node() = default; 

Tree::Tree()
	:root(make_unique<Node>())
{
	depth = 1;
}

Tree::Tree(const unordered_map <string, vector<ChessPosition>>& piecesToMoves)
{

	//root.data = data; //former approach 
	root = make_unique<Node>();
	root->piecesToMoves = piecesToMoves;

	depth = 0;
}

Tree::~Tree() = default;


#pragma endregion 