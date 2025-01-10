#include "AIChessGame.h"

/*define the static member variable of AIChessGame*/
size_t AIChessGame::alphaBetaMinimaxCallCounter = 0; 




AIChessGame::AIChessGame()
	:ChessGame()
{
	//again, some silly thing going on with calling parent constructor ChessGame, so I'm repeating some function calls here... 
	
	//for caching of moves: 
	for (const auto& piece : boardImage.pieces)
	{
		piecesToUpdate.insert(piece);
	}

	getPiecesToMoves();

	moveCount = 0;

}

void AIChessGame::play()
{
	do
	{
		callNodeJS();
		openPort3000_andDisplayChessBoard();

		displayGameOptions(); 

		array<ChessPosition, 2> chosenPositions = getAndConfirmMove();
		ChessPosition chosenStart = chosenPositions.at(0);
		ChessPosition chosenFinish = chosenPositions.at(1);

		movePiece(chosenStart, chosenFinish);

		if (isKingInCheck)
		{
			if (moveCount % 2 == 0)
			{
				isCheckMate = checkForMate("white");
			}

			else
			{
				isCheckMate = checkForMate("black");
			}
		}

		if (isCheckMate)
		{
			break;
		}
	} while (!isCheckMate);

	cout << "\n\n\nGame over!";
	if (moveCount % 2 == 0)
	{
		cout << "Black won!\n";
	}

	else
	{
		cout << "WHITE won!\n";
	}
}

void AIChessGame::displayGameOptions()
{
	string response;

	while (response != "4")
	{
		int desiredDepth = 5;
		cout << "1 - display suggested move with depth = " << desiredDepth << "\n";
		cout << "2 - display ALL available moves \n";
		cout << "3 - display suggested move with depth = " << desiredDepth - 1 << "\n";
		cout << "4 - CONTINUE, once you've clicked the piece and the spot to move it to:\n";

		getline(std::cin, response);

		if (response == "1")
		{

			auto suggestedMove = getMinimaxAlphaBetaMove(desiredDepth);
			sharedFunctions::setTerminalColor(TerminalColor::Red);
			cout << suggestedMove.first << " to " << suggestedMove.second.file << suggestedMove.second.rank << "\n";
			sharedFunctions::setTerminalColor(TerminalColor::Default); 
		}
		else if (response == "2")
		{
			ChessGame::showAllPossibleMoves();
		}

		else if (response == "3")
		{
			auto suggestedMove = getMinimaxAlphaBetaMove(desiredDepth - 1);

			sharedFunctions::setTerminalColor(TerminalColor::Red);
			cout << suggestedMove.first << " to " << suggestedMove.second.file << suggestedMove.second.rank << "\n";
			sharedFunctions::setTerminalColor(TerminalColor::Default);
		}

		else if (response == "4")
		{
			cout << "Continuing game ...\n";
		}

		else
		{
			cout << "Unrecognized response \n";
		}
	}
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

void AIChessGame::getGameTreeRecursively(Node& parentNode, unordered_map<string, vector<ChessPosition>>& piecesToMoves, int currentDepth, int maxDepth, int moveCount, int alpha, int beta)
{
	if (gameTreeNodeCount % 10'000 == 0) //expect very roughly 160'000 nodes at level 4 
	{
		cout << "Generating game tree ... currently, nodeCount = " << gameTreeNodeCount << "\n";
	}

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

		// Order the moves for the current piece
		vector<ChessPosition> orderedMoves = orderMoves(piecesToMoves.at(piece), piece);

		// Get all moves available to that piece
		for (const ChessPosition& newPosition : orderedMoves)
		{
			// Update board section
			auto oldPosition = boardImage.piecesToPositions.find(piece)->second;
			positionsToPieces.erase(oldPosition); //effectively makes the old square empty

			string takenPieceName;
			bool pieceTaken = false;

			// Handle taking an opponent piece by deleting from vector<string> pieceNames!
			if (positionsToPieces.find(newPosition) != positionsToPieces.end())
			{
				// Opponent must be there (UNLESS castling!)
				takenPieceName = positionsToPieces.at(newPosition);
				takePiece(piece, newPosition);
				pieceTaken = true;
			}

			// Insert the piece at the new position
			positionsToPieces.insert({ newPosition, piece });

			// Clear piecesToPositions and then get its contents by using the "switch" function
			boardImage.piecesToPositions.clear();
			boardImage.piecesToPositions = switchPositionsAndPieces(positionsToPieces);

			// Once a piece has moved, possible positions change for ALL pieces, so clear the possible positions map
			piecesToMoves.clear();
			getPiecesToMoves();

			// Insertion of new node into GameTree section
			auto childNode = make_unique<Node>();
			childNode->piecesToMoves = piecesToMoves;

			parentNode.childrenLinks.push_back(std::move(childNode)); // Note the std::move!!
			gameTreeNodeCount++;

			// Recursive call with alpha-beta pruning
			getGameTreeRecursively(*parentNode.childrenLinks.back(), piecesToMoves, currentDepth + 1, maxDepth, moveCount + 1, alpha, beta);

			// Backtrack (undo the updates to the board state)
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

			// Alpha-beta pruning
			if (moveCount % 2 == 0) // Maximizing player (white)
			{
				alpha = std::max(alpha, evaluateGameState());
				if (beta <= alpha)
				{
					break; // Beta cut-off
				}
			}
			else // Minimizing player (black)
			{
				beta = std::min(beta, evaluateGameState());
				if (beta <= alpha)
				{
					break; // Alpha cut-off
				}
			}
		}
	}
}

void AIChessGame::parallelGetGameTreeRecursively(Node& parentNode, unordered_map<string, vector<ChessPosition>>& piecesToMoves, int currentDepth, int maxDepth, int moveCount, int alpha, int beta)
{
	if (gameTreeNodeCount % 10'000 == 0) //expect very roughly 160'000 nodes at level 4 
	{
		cout << "Generating game tree ... currently, nodeCount = " << gameTreeNodeCount << "\n";
	}

	if (currentDepth == maxDepth)
	{
		return;
	}

	string currentPlayer = (moveCount % 2 == 0) ? "white" : "black";

	std::vector<std::future<void>> futures;
	mutex mtx; 


	for (int pieceIndex = 0; pieceIndex < boardImage.pieces.size(); ++pieceIndex)
	{
		string piece = boardImage.pieces.at(pieceIndex);
		if (piece.find(currentPlayer) == string::npos)
		{
			continue;
		}

		// Order the moves for the current piece
		vector<ChessPosition> orderedMoves = orderMoves(piecesToMoves.at(piece), piece);

		// Get all moves available to that piece
		for (const ChessPosition& newPosition : orderedMoves)
		{
			futures.push_back(std::async(std::launch::async, 
				[this, &parentNode, &piecesToMoves, currentDepth, maxDepth, moveCount, &alpha, &beta, piece, newPosition, &mtx]()
				{
					
					std::lock_guard<std::mutex> lock(mtx); //exception will throw if this is not here ...
					//cout << std::this_thread::get_id() << " started\n"; 
					//uncomment to verify that multi-threading is used 
					
					// Update board section
					auto oldPosition = boardImage.piecesToPositions.find(piece)->second;
					positionsToPieces.erase(oldPosition); //effectively makes the old square empty

					string takenPieceName;
					bool pieceTaken = false;

					// Handle taking an opponent piece by deleting from vector<string> pieceNames!
					if (positionsToPieces.find(newPosition) != positionsToPieces.end())
					{
						// Opponent must be there (UNLESS castling!)
						takenPieceName = positionsToPieces.at(newPosition);
						takePiece(piece, newPosition);
						pieceTaken = true;
					}

					// Insert the piece at the new position
					positionsToPieces.insert({ newPosition, piece });

					// Clear piecesToPositions and then get its contents by using the "switch" function
					boardImage.piecesToPositions.clear();
					boardImage.piecesToPositions = switchPositionsAndPieces(positionsToPieces);

					// Once a piece has moved, possible positions change for ALL pieces, so clear the possible positions map
					piecesToMoves.clear();
					getPiecesToMoves();

					// Insertion of new node into GameTree section
					auto childNode = make_unique<Node>();
					childNode->piecesToMoves = piecesToMoves;

					parentNode.childrenLinks.push_back(std::move(childNode)); // Note the std::move!!
					gameTreeNodeCount++;

					// Recursive call with alpha-beta pruning
					getGameTreeRecursively(*parentNode.childrenLinks.back(), piecesToMoves, currentDepth + 1, maxDepth, moveCount + 1, alpha, beta);

					// Backtrack (undo the updates to the board state)
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

					// Alpha-beta pruning
					if (moveCount % 2 == 0) // Maximizing player (white)
					{
						alpha = std::max(alpha, evaluateGameState());
						if (beta <= alpha)
						{
							return; // Beta cut-off
						}
					}
					else // Minimizing player (black)
					{
						beta = std::min(beta, evaluateGameState());
						if (beta <= alpha)
						{
							return; // Alpha cut-off
						}
					}
				}));
		}
	}

	for (auto& future : futures)
	{
		future.get();
	}

}


pair<string, ChessPosition> AIChessGame::getMinimaxAlphaBetaMove(const short desiredDepth)
{
	pair<string, ChessPosition> minimaxAlphaBetaMove;

	Tree gameTree{ piecesToMoves }; 

	// Initialize alpha and beta
	int alpha = INT_MIN;
	int beta = INT_MAX;

	//getGameTreeRecursively(*gameTree.root, piecesToMoves, 0, desiredDepth, moveCount); 
	//previous approach -> depth = 6 overwhelms RAM and my time ... without alpha beta in tree generation

	//getGameTreeRecursively(*gameTree.root, piecesToMoves, 0, desiredDepth, moveCount, alpha, beta);

	parallelGetGameTreeRecursively(*gameTree.root, piecesToMoves, 0, desiredDepth, moveCount, alpha, beta);

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
					/*int */alpha = INT_MIN;
					/*int */beta = INT_MAX;

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
	//const int KING_VALUE = 200;

	//const int CHECK_PENALTY = 1; //changed from FIFTY (suggested by copilot) when AI suggested I blunder a queen ...


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

		// Check for forks
		if (piece.find("Pawn") != string::npos
			||
			piece.find("Knight") != string::npos)
		{
			int forkBonus = calculateForkBonus(position, piece);
			score += (piece[0] == 'w') ? forkBonus : -forkBonus;
		}

		// Check for pins
		//if (piece.find("Queen") != string::npos
		//	||
		//	piece.find("Bishop") != string::npos
		//	||
		//	piece.find("Rook") != string::npos)
		//{
		//	int pinBonus = calculatePinBonus(position, piece);
		//	score += (piece[0] == 'w') ? pinBonus : -pinBonus;
		//}


		//if (piece.find("Pawn"))
		//{
		//	unsigned short doubledPawnPenalty = calculatePenaltyForDoubledPawn(position, piece); 
		//}

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
			score += /*KING_VALUE + */kingTable[rank][file]; //commented out KING_VALUE for now 
															//attackers "think" they can take it too easily 

			//if (checkForCheck("white"))
			//{
			//	score -= CHECK_PENALTY;
			//}
		}
		else if (piece.find("blackKing") != string::npos)
		{
			score -= /*KING_VALUE + */kingTable[7 - rank][file];

			//if (checkForCheck("black"))
			//{
			//	score += CHECK_PENALTY;
			//}
		}
	}

	return score;
}

int AIChessGame::calculateForkBonus(const ChessPosition& position, const string& piece)
{
	int forkBonus = 0; 
	unordered_set<string> attackedPieces; 

	vector<ChessPosition> possibleMoves = piecesToMoves[piece];

	for (const ChessPosition& move : possibleMoves)
	{
		if (positionsToPieces.find(move) != positionsToPieces.end())
		{
			string attackedPiece = positionsToPieces[move]; 

			if (attackedPiece[0] != piece[0]) //true implies pieces are different colors ('w' != 'b')
			{
				attackedPieces.insert(attackedPiece);
			}
		}
	}

	if (attackedPieces.size() > 1) //'tis a fork! (probably rare to evaluate this so not too costly) 
	{
		
		//cout << "Fork possible!\n"; //consider moving this to parent class, ChessGame, for non-cheating play helper 
		
		int minPieceValue = INT_MAX;
		for (const string& attackedPiece : attackedPieces)
		{
			int pieceValue = sharedFunctions::findPieceValue(attackedPiece);
			if (pieceValue < minPieceValue)
			{
				minPieceValue = pieceValue;
			}
		}
		forkBonus = minPieceValue;
	}

	return forkBonus; 
}


int AIChessGame::calculatePinBonus(const ChessPosition& position, const string& piece)
{
	// Only queens, rooks, and bishops can pin
	if (piece.find("Queen") == string::npos && piece.find("Rook") == string::npos && piece.find("Bishop") == string::npos)
	{
		return 0;
	}

	// Determine the direction of movement for the piece
	vector<pair<int, int>> directions;
	if (piece.find("Queen") != string::npos || piece.find("Rook") != string::npos)
	{
		// Horizontal and vertical directions for queen and rook
		directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
	}
	if (piece.find("Queen") != string::npos || piece.find("Bishop") != string::npos)
	{
		// Diagonal directions for queen and bishop
		directions.insert(directions.end(), { {1, 1}, {-1, -1}, {1, -1}, {-1, 1} });
	}

	int pinBonus = 0;
	for (const auto& direction : directions)
	{
		ChessPosition currentPos = position;
		bool foundOpponentPiece = false;
		string opponentPiece;
		while (true)
		{
			currentPos.file += direction.first;
			currentPos.rank += direction.second;

			if (!ChessPosition::isPositionInBounds(currentPos.file, currentPos.rank))
			{
				break;
			}

			if (positionsToPieces.find(currentPos) != positionsToPieces.end())
			{
				if (!foundOpponentPiece)
				{
					// First piece found in the direction
					opponentPiece = positionsToPieces[currentPos];
					if (opponentPiece[0] != piece[0]) // Opponent's piece
					{
						foundOpponentPiece = true;
					}
					else // Friendly piece
					{
						break;
					}
				}
				else //an opponent was found previously in this direction 
				{
					// Second piece found in the direction
					if (positionsToPieces[currentPos][0] != piece[0]) // Friendly piece
					{
						// Check if the opponent's piece is pinned
						pinBonus += sharedFunctions::findPieceValue(opponentPiece);
					}
					break;
				}
			}
		}
	}

	return pinBonus;
}

unsigned short AIChessGame::calculatePenaltyForDoubledPawn(const ChessPosition& position, const string& piece)
{
	//doubled pawn occurs if two pawns of the same color
	//1 are on same file
	//2 are within 1 rank of one another

	for (const auto& currentPiece : boardImage.pieces)
	{
		//value is fractional... continue later if desired 
	}
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