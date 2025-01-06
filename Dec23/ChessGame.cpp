#include "ChessGame.h"



size_t ChessGame::alphaBetaCallCounter = 0; 


ChessGame::ChessGame()
{
	
	boardImage.pieces = boardImage.mapPiecesToImages_andReturnPieces(); //stupid having to do this twice ...

	positionsToPieces = switchMapKeysAndValues(boardImage.piecesToPositions);

	getPiecesToMoves(); 


	string baseFolderOfNodeJS = "testingNodeJS/public/";
	string filename = baseFolderOfNodeJS + "chessboard.bmp"; //note that this will overwrite!
	boardImage.writeImageFile(filename);

}

ChessGame::ChessGame(const ChessGame& other) 
	: positionsToPieces(other.positionsToPieces),
	piecesToMoves(other.piecesToMoves),
	moveCount(other.moveCount)
{
	// Copy any other necessary member variables here

	//cout << "Copy constructor for ChessGame called! Beware!\n";
	//std::cin.get(); 
}

void ChessGame::showAllPossibleMoves()
{
	for (const auto& currentPair : piecesToMoves)
	{
		if (currentPair.second.size() != 0)
		{
			if (moveCount % 2 == 0 && currentPair.first.find("white") != string::npos)
			{
				cout << currentPair.first << " can move to:\n";
				for (int i = 0; i < currentPair.second.size(); ++i)
				{
					cout << currentPair.second.at(i) << "\n";
				}
			}

			else if (moveCount % 2 == 1 && currentPair.first.find("black") != string::npos)
			{
				cout << currentPair.first << " can move to:\n";
				for (int i = 0; i < currentPair.second.size(); ++i)
				{
					cout << currentPair.second.at(i) << "\n";
				}
			}
		}
	}
}

pair<string, string> ChessGame::getStrongishMove(int desiredDepth)
{
	Tree gameTree{ piecesToMoves };

	getGameTreeRecursively(*gameTree.root, piecesToMoves, 0, desiredDepth, moveCount); //note the dereferencing operator
	

	int bestScore = INT_MIN; // Initialize to a very low value
	pair<string, string> bestMove;

	for (const auto& child : gameTree.root->childrenLinks)
	{
		for (const auto& pieceMoves : child->data)
		{
			const string& piece = pieceMoves.first;
			for (const string& move : pieceMoves.second)
			{
				if (isMoveValid(piece, move))
				{
					// Save the current state
					auto oldPosition = boardImage.piecesToPositions.find(piece)->second;
					string takenPieceName;
					bool pieceTaken = false;

					// Update the board state
					positionsToPieces.erase(oldPosition); //effectively makes the old square empty
					if (positionsToPieces.find(move) != positionsToPieces.end())
					{
						// Opponent must be there (UNLESS castling!)
						takenPieceName = positionsToPieces.at(move);
						takePiece(piece, move);
						pieceTaken = true;
					}
					positionsToPieces.insert({ move, piece });
					boardImage.piecesToPositions.clear();
					boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);
					piecesToMoves.clear();
					getPiecesToMoves();

					// Evaluate the board state after the move
					int score = evaluateGameState();

					// If the score is better than the best score, update the best score and best move
					if (score > bestScore)
					{
						bestScore = score;
						bestMove = { piece, move };
					}

					// Revert the board state
					positionsToPieces.erase(move);
					positionsToPieces.insert({ oldPosition, piece });
					if (pieceTaken)
					{
						positionsToPieces.insert({ move, takenPieceName });
						boardImage.pieces.push_back(takenPieceName);
					}
					boardImage.piecesToPositions.clear();
					boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);
					piecesToMoves.clear();
					getPiecesToMoves();

				}
			}
		}
	}

	return bestMove;
}

pair<string, string> ChessGame::getMinimaxMove(int desiredDepth)

{
	Tree gameTree{ piecesToMoves };

	getGameTreeRecursively(*gameTree.root, piecesToMoves, 0, desiredDepth, moveCount); //note the dereferencing operator


	int bestScore = (moveCount % 2 == 0) ? INT_MIN : INT_MAX; // White maximizes, Black minimizes
	bool isMaximizingPlayer = (moveCount % 2 == 0); // True if White's turn, False if Black's turn

	pair<string, string> bestMove;

	for (const auto& child : gameTree.root->childrenLinks)
	{
		for (const auto& pieceMoves : child->data)
		{
			const string& piece = pieceMoves.first;
			for (const string& move : pieceMoves.second)
			{
				if (isMoveValid(piece, move))
				{
					// Save the current state
					auto oldPosition = boardImage.piecesToPositions.find(piece)->second;
					string takenPieceName;
					bool pieceTaken = false;

					// Update the board state
					positionsToPieces.erase(oldPosition); //effectively makes the old square empty
					if (positionsToPieces.find(move) != positionsToPieces.end())
					{
						// Opponent must be there (UNLESS castling!)
						takenPieceName = positionsToPieces.at(move);
						takePiece(piece, move);
						pieceTaken = true;
					}
					positionsToPieces.insert({ move, piece });
					boardImage.piecesToPositions.clear();
					boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);
					piecesToMoves.clear();
					getPiecesToMoves();

					// Evaluate the board state after the move using minimax
					int score = minimax(*child, desiredDepth - 1, !isMaximizingPlayer);

					// If the score is better than the best score, update the best score and best move
					if ((isMaximizingPlayer && score > bestScore) || (!isMaximizingPlayer && score < bestScore))
					{
						bestScore = score;
						bestMove = { piece, move };
					}

					// Revert the board state
					positionsToPieces.erase(move);
					positionsToPieces.insert({ oldPosition, piece });
					if (pieceTaken)
					{
						positionsToPieces.insert({ move, takenPieceName });
						boardImage.pieces.push_back(takenPieceName);
					}
					boardImage.piecesToPositions.clear();
					boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);
					piecesToMoves.clear();
					getPiecesToMoves();

				}
			}
		}
	}

	return bestMove;
}

string ChessGame::hashBoardState()
{
	/*don't dwell on the details here 
	- just wanting a unique key to corresponding board state for an unordered map */
	string hash; 
	for (const auto& piece : boardImage.pieces)
	{
		hash += piece + boardImage.piecesToPositions.at(piece); //concatenation 
	}

	return hash; 
}

int ChessGame::minimaxAlphaBetaParallel(Node& node, int depth, bool isMaximizingPlayer, int alpha, int beta)
{
	if (depth == 0 || node.childrenLinks.empty())
	{
		return evaluateGameState();
	}

	//LIMIT! 
	const int maxConcurrentTasks = std::thread::hardware_concurrency(); 
	vector<future<int>> futures; //ridiculous!
	std::mutex mtx; //mutual exclusion!

	if (isMaximizingPlayer)
	{
		int maxEval = INT_MIN; 

		for (const auto& child : node.childrenLinks)
		{
			if (futures.size() >= maxConcurrentTasks)
			{
				for (auto& future : futures)
				{
					int eval = future.get();
					maxEval = std::max(maxEval, eval);

					alpha = std::max(alpha, eval);

					if (beta <= alpha)
					{
						break; //"beta cutoff"...
					}
				}
				futures.clear(); 
			}
			futures.push_back(
				std::async(std::launch::async, //the madness begins
					[&]()
					{
						std::lock_guard<std::mutex> lock(mtx); 
						return minimaxAlphaBetaParallel(*child, depth - 1, false, alpha, beta);
					}
				));
		}

		for (auto& future : futures)
		{
			int eval = future.get();
			maxEval = std::max(maxEval, eval);
			alpha = std::max(alpha, eval);
			if (beta <= alpha)
			{
				break; // Beta cut-off
			}
		}
		return maxEval; 
	}

	else
	{
		int minEval = INT_MAX;

		for (const auto& child : node.childrenLinks)
		{
			if (futures.size() >= maxConcurrentTasks)
			{
				for (auto& future : futures)
				{
					int eval = future.get();
					minEval = std::min(minEval, eval);
					beta = std::min(beta, eval);
					if (beta <= alpha)
					{
						break; // Alpha cut-off
					}
				}
				futures.clear();
			}

			futures.push_back(
				std::async(std::launch::async,
					[&]() {
						std::lock_guard<std::mutex> lock(mtx);
						return minimaxAlphaBetaParallel(*child, depth - 1, true, alpha, beta);
					}
				)
			);
		}

		for (auto& future : futures)
		{
			int eval = future.get();
			minEval = std::min(minEval, eval);
			beta = std::min(beta, eval);
			if (beta <= alpha)
			{
				break; // Alpha cut-off
			}
		}

		return minEval;
	}

}

bool ChessGame::isPieceThreatened(const string& position, char opponentColor) 
{
	for (const auto& [opponentPiece, moves] : piecesToMoves) 
	{
		if (opponentPiece[0] == opponentColor) 
		{
			if (std::find(moves.begin(), moves.end(), position) != moves.end()) 
			{
				return true;
			}
		}
	}
	return false;
}

int ChessGame::minimaxAlphaBeta(Node& node, int depth, bool isMaximizingPlayer, int alpha, int beta)
{

	//alphaBetaCallCounter = 0; // Static counter to track the number of recursive calls

	// Increment the counter
	alphaBetaCallCounter++;

	// Print a message every 10,000 iterations
	//if (alphaBetaCallCounter % 10'000 == 0)
	//{
	//	std::cout << "minimaxAlphaBeta call count: " << alphaBetaCallCounter << std::endl;
	//}

	if (alphaBetaCallCounter % 1'000 == 0)
	{
		std::cout << "minimaxAlphaBeta call count: " << alphaBetaCallCounter << std::endl;
	}

	string boardHash = hashBoardState();
	if (transpositionTable.find(boardHash) != transpositionTable.end()) //"look" no further ... 
	{
		return transpositionTable[boardHash];
	}

	if (depth == 0 || node.childrenLinks.empty())
	{
		int eval = evaluateGameState(); 
		transpositionTable[boardHash] = eval; 

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
		transpositionTable[boardHash] = maxEval;
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
		transpositionTable[boardHash] = minEval;
		return minEval;
	}
}

pair<string, string> ChessGame::getMinimaxAlphaBetaMove(int desiredDepth)
{
	
	Tree gameTree{ piecesToMoves };

	getGameTreeRecursively(*gameTree.root, piecesToMoves, 0, desiredDepth, moveCount); //note the dereferencing operator

	//reset recursion counter 
	//recursionCounter = 0;

	int bestScore = (moveCount % 2 == 0) ? INT_MIN : INT_MAX; // White maximizes, Black minimizes
	bool isMaximizingPlayer = (moveCount % 2 == 0); // True if White's turn, False if Black's turn
	pair<string, string> bestMove;


	for (const auto& child : gameTree.root->childrenLinks)
	{
		for (const auto& pieceMoves : child->data)
		{
			const string& piece = pieceMoves.first;
			
			vector<string> orderedMoves = orderMoves(pieceMoves.second, piece); 

			for (const string& move : pieceMoves.second)
			{
				if (isMoveValid(piece, move))
				{
					// Save the current state
					auto oldPosition = boardImage.piecesToPositions.find(piece)->second;
					string takenPieceName;
					bool pieceTaken = false;

					// Update the board state
					positionsToPieces.erase(oldPosition); //effectively makes the old square empty
					if (positionsToPieces.find(move) != positionsToPieces.end())
					{
						// Opponent must be there (UNLESS castling!)
						takenPieceName = positionsToPieces.at(move);
						takePiece(piece, move);
						pieceTaken = true;
					}
					positionsToPieces.insert({ move, piece });
					boardImage.piecesToPositions.clear();
					boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);
					piecesToMoves.clear();
					getPiecesToMoves();

					/*Don't allow checks!*/
					if (checkForCheck(getPieceColor(piece)))
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
						boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);
						piecesToMoves.clear();
						getPiecesToMoves();

						continue; 
					}

					// Evaluate the board state after the move using minimax with alpha-beta pruning
					int alpha = INT_MIN;
					int beta = INT_MAX;
					
					int score = minimaxAlphaBeta(*child, desiredDepth - 1, !isMaximizingPlayer, alpha, beta);

					//call below uses MULTI-THREADING!
					//int score = minimaxAlphaBetaParallel(*child, desiredDepth - 1, !isMaximizingPlayer, alpha, beta);

					// If the score is better than the best score, update the best score and best move
					if ((isMaximizingPlayer && score > bestScore) || (!isMaximizingPlayer && score < bestScore))
					{
						bestScore = score;
						bestMove = { piece, move };
					}

					// Revert the board state
					positionsToPieces.erase(move);
					positionsToPieces.insert({ oldPosition, piece });
					if (pieceTaken)
					{
						positionsToPieces.insert({ move, takenPieceName });
						boardImage.pieces.push_back(takenPieceName);
					}
					boardImage.piecesToPositions.clear();
					boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);
					piecesToMoves.clear();
					getPiecesToMoves();
				}
			}
		}
	}

	//reset number of nodes in game tree to 0 (because it will/may be regenerated on next move) 
	gameTreeNodeCount = 0; 
	//also reset alphaBetaCallCounter
	alphaBetaCallCounter = 0; 

	return bestMove;
}

int ChessGame::minimax(Node& node, int depth, bool isMaximizingPlayer)
{
	if (depth == 0 || node.childrenLinks.empty())
	{
		return evaluateGameState(); 
	}

	if (isMaximizingPlayer)
	{
		int maxEval = INT_MIN; 
		for (const auto& child : node.childrenLinks)
		{
			int eval = minimax(*child, depth - 1, false);
			maxEval = std::max(maxEval, eval);
		}
		return maxEval;
	}

	else
	{
		int minEval = INT_MAX; 
		for (const auto& child : node.childrenLinks)
		{
			int eval = minimax(*child, depth - 1, true);
			minEval = std::min(minEval, eval);
		}
		return minEval; 
	}
}


array<pair<char, int>, 2> ChessGame::getAndConfirmMove()
{
	array<pair<int, int>, 2> theTwoCoordinates = readFileAndReturnCoordinatesClickedInBrowser();

	int x0 = theTwoCoordinates.at(0).first;
	int y0 = theTwoCoordinates.at(0).second;

	pair<char, int> firstPosition =
		boardImage.convertImageCoordinatesToPosition(x0, y0);

	int xf = theTwoCoordinates.at(1).first;
	int yf = theTwoCoordinates.at(1).second;

	pair<char, int> secondPosition =
		boardImage.convertImageCoordinatesToPosition(xf, yf);

	cout << firstPosition.first << firstPosition.second << "->"
		<< secondPosition.first << secondPosition.second << "\n";


	cout << "Updating the board...\n";
	array<pair<char, int>, 2> theTwoChosenPositions =
	{
		firstPosition,
		secondPosition
	};

	return theTwoChosenPositions;
}


bool ChessGame::isThatColorTurn(const string& pieceName)
{
	if (moveCount % 2 == 0)
	{
		//white's turn:
		if (pieceName.find("white") == string::npos)
		{
			cout << pieceName << " cannot be moved - it is white's turn:\n";
			return false;
		}
	}

	else //black's turn
	{
		if (pieceName.find("black") == string::npos)
		{
			cout << pieceName << " cannot be moved - it is black's turn:\n";
			return false;
		}
	}

	return true;
}

bool ChessGame::isGameOver()
{
	return false;
}

bool ChessGame::checkForMate(const string& colorToCheckForMate)
{
	//check for checkMATE: 
	bool isCheckMATE = true;

	//make a COPY? of piecesToMoves having only colorToCheckForChefor now? 
	map<string, vector<string>> colorPiecesToMoves; 
	for (const auto& currentPair : piecesToMoves)
	{
		if (currentPair.first.find(colorToCheckForMate) != string::npos)
		{
			colorPiecesToMoves.insert({currentPair.first, currentPair.second});
		}
	}

	for (const auto& singlePieceToMoves : colorPiecesToMoves)
	//for (int i = 0; i < piecesToMoves.size(); ++i)
	{
		if (isCheckMATE == false)
		{
			break;
		}

		const string& piece = singlePieceToMoves.first;
		//if (piece.find(colorToCheckForMate) != string::npos) 
		////obviously, do not check if an OPPONENT can move in such a way that the mate is REMOVED :) 
		//{
		for (const string& move : singlePieceToMoves.second)
		{
			movePieceHelper(piece, move);

			if (isKingInCheck == false)
			{
				isCheckMATE = false;
				break;
			}
		}
		//}
	}

	if (isCheckMATE)
	{
		cout << "\n\n\nCheck MATE!\n\n\n";
	}

	return isCheckMATE;
}

array<pair<char, int>, 2> ChessGame::getRandomMove()
{
	array<pair<char, int>, 2> randomMove;

	std::uniform_int_distribution<int> distribution{ 0, (int)boardImage.pieces.size() - 1 };

	string startingPosition;
	string endingPosition;
	string piece;

	// Random move for white:
	if (moveCount % 2 == 0)
	{
		while (true)
		{
			int randomIndex = distribution(engine);

			// Choose a random white piece
			piece = boardImage.pieces.at(randomIndex);

			if (piece.find("white") == string::npos)
			{
				continue;
			}

			startingPosition = boardImage.piecesToPositions.at(piece);

			// Check if the piece has any valid moves
			if (piecesToMoves.at(piece).empty())
			{
				continue;
			}

			// Choose a random valid move for the piece
			std::uniform_int_distribution<int> moveDistribution{ 0, (int)piecesToMoves.at(piece).size() - 1 };
			endingPosition = piecesToMoves.at(piece).at(moveDistribution(engine));
			break;
		}
	}
	else // Random move for black:
	{
		while (true)
		{
			int randomIndex = distribution(engine);

			// Choose a random black piece
			piece = boardImage.pieces.at(randomIndex);

			if (piece.find("black") == string::npos)
			{
				continue;
			}

			startingPosition = boardImage.piecesToPositions.at(piece);

			// Check if the piece has any valid moves
			if (piecesToMoves.at(piece).empty())
			{
				continue;
			}

			// Choose a random valid move for the piece
			std::uniform_int_distribution<int> moveDistribution{ 0, (int)piecesToMoves.at(piece).size() - 1 };
			endingPosition = piecesToMoves.at(piece).at(moveDistribution(engine));
			break;
		}
	}

	auto start = convertStringChessPositionToCharAndInt(startingPosition);
	auto end = convertStringChessPositionToCharAndInt(endingPosition);

	randomMove =
	{
		start,
		end
	};

	return randomMove;
}


bool ChessGame::isPieceOnBoard(const string& pieceName)
{
	auto pieceNameIterator = std::find(boardImage.pieces.begin(), boardImage.pieces.end(), pieceName); 
	//anticipate ERASING pieceName from vector `pieceNames` when pieces get "taken" in later functions...

	return (pieceNameIterator != boardImage.pieces.end());
}


/*PRECONDITION: this "helper" function only gets called once its caller, `movePiece`, has done certain 
verifications (ex: the position is on the board, it is that turn's color, and some other(s)
NOTE: this method writes a (potentially "large") IMAGE file out with filename = current system time*/
void ChessGame::movePieceHelper(const string& pieceName, const string& newPosition)
{

	auto oldPosition = boardImage.piecesToPositions.find(pieceName)->second;
	positionsToPieces.erase(oldPosition); //effectively makes the old square empty

	string takenPieceName; 
	bool pieceTaken = false; 

	//handle taking an opponent piece by deleting from vector<string> pieceNames!
	if (positionsToPieces.find(newPosition) != positionsToPieces.end())
	{
		//opponent must be there (UNLESS castling!) 
		takenPieceName = positionsToPieces.at(newPosition);
		takePiece(pieceName, newPosition);
		pieceTaken = true; 
	}
		
	//insert the piece at the new position
	positionsToPieces.insert({ newPosition, pieceName });

	// Handle castling
	if (pieceName.find("King") != string::npos)
	{
		if (pieceName.find("white") != string::npos)
		{
			whiteKingMoved = true;
			if (newPosition == "G1")
			{
				// Kingside castling
				movePieceHelper("whiteRookKSide", "F1");
				whiteRookMoved[0] = true;
			}
			else if (newPosition == "C1")
			{
				// Queenside castling
				movePieceHelper("whiteRookQSide", "D1");
				whiteRookMoved[1] = true;
			}
		}
		else if (pieceName.find("black") != string::npos)
		{
			blackKingMoved = true;
			if (newPosition == "G8")
			{
				// Kingside castling
				movePieceHelper("blackRookKSide", "F8");
				blackRookMoved[0] = true;
			}
			else if (newPosition == "C8")
			{
				// Queenside castling
				movePieceHelper("blackRookQSide", "D8");
				blackRookMoved[1] = true;
			}
		}
	}
	else if (pieceName.find("Rook") != string::npos)
	{
		if (pieceName.find("white") != string::npos)
		{
			if (oldPosition == "A1")
			{
				whiteRookMoved[1] = true;
			}
			else if (oldPosition == "H1")
			{
				whiteRookMoved[0] = true;
			}
		}
		else if (pieceName.find("black") != string::npos)
		{
			if (oldPosition == "A8")
			{
				blackRookMoved[1] = true;
			}
			else if (oldPosition == "H8")
			{
				blackRookMoved[0] = true;
			}
		}
	}



	//clear piecesToPositions and then get its contents by using the "switch" function: 
	boardImage.piecesToPositions.clear(); 
	boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);

	//once a piece has moved, possible positions change for ALL pieces, so clear the possible positions map
	piecesToMoves.clear();
	getPiecesToMoves();
	
	/*Finally, check for check: */
	/*IF the move puts friendly king in check, REJECT the move (early return) and REPROMPT the user for a move*/
	if (checkForCheck(getPieceColor(pieceName))) 
	{
		//reversion to previous state - disallow move: 
		positionsToPieces.erase(newPosition); 
		positionsToPieces.insert({ oldPosition, pieceName });
		
		if (pieceTaken)
		{
			positionsToPieces.insert({ newPosition, takenPieceName }); 
			boardImage.pieces.push_back(takenPieceName); 
		}

		boardImage.piecesToPositions.clear(); 
		boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces); 
		piecesToMoves.clear(); 
		getPiecesToMoves(); 

		cout << pieceName << " cannot move to " << newPosition << " because\n";

		cout << "AFTER updating, " << getPieceColor(pieceName) << " is in check!\n";
		cout << "- try another move!\n";

		isKingInCheck = true; //the MAIN function checks for this flag
							//if it is set, the `checkForMate` function is called (in main)

		return; //so moveCount will not increment and new board will not be drawn 
	}

	else
	{
		isKingInCheck = false; 
	}

	moveCount++;


	/*Write the the image file with the updated board*/
	drawBoardHelper(oldPosition);

	///*generate game tree: */
	//Tree gameTree{ piecesToMoves };
	//int desiredDepth = 3; 

	//getGameTreeRecursively(*gameTree.root, piecesToMoves, 0, desiredDepth, moveCount); //note the dereferencing operator 

	////reset recursive move count to 0
	//recursionCounter = 0; 
}

bool ChessGame::isMoveValid(const string& piece, const string& move)
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
		const vector<string>& possibleMoves = it->second;
		if (std::find(possibleMoves.begin(), possibleMoves.end(), move) != possibleMoves.end())
		{
			return true;
		}
	}
	return false;
}


void ChessGame::takePiece(const string& pieceName, const string& newPosition)
{
	string opponentPieceName; 

	auto vectorIteratorOfPieceToTake =
		std::find(boardImage.pieces.begin(), boardImage.pieces.end(),
			positionsToPieces.find(newPosition)->second);

	opponentPieceName = *vectorIteratorOfPieceToTake; //note the dereferenced iterator

	//erase opponent from list of pieces: 
	boardImage.pieces.erase(vectorIteratorOfPieceToTake);

	//erase opponent from map
	positionsToPieces.erase(newPosition);

	size_t pieceValue = findPieceValue(opponentPieceName); 


	//if (pieceName.find("black") != string::npos)
	//{
	//	setTerminalColor(TerminalColor::Red);
	//	if (pieceValue >= 5)
	//	{
	//		cout << "Black gained " << pieceValue << "\n";
	//		cout << "This happened with recursion counter = " << recursionCounter << "\n";
	//	}
	//	setTerminalColor(TerminalColor::Default);
	////	blackScore += pieceValue;
	//}

	//else
	//{
	//	
	//	setTerminalColor(TerminalColor::Red);
	//	if (pieceValue >= 5)
	//	{
	//		cout << "White gained " << pieceValue << "\n";
	//		cout << "This happened with recursion counter = " << recursionCounter << "\n";
	//	}

	//	setTerminalColor(TerminalColor::Default);
	//	whiteScore += pieceValue;
	//}

	//cout << "Score is now (White v Black) " << whiteScore << " to " << blackScore << "\n";
}


void ChessGame::drawBoardHelper(const string& oldPosition)
{
	//the image file stuff: 
	boardImage.drawPieces();
	boardImage.fillPositionWithColor(oldPosition); 	// "paint" over the old position with the appropriate (light or dark) square: 
	string baseFolderOfNodeJS = "testingNodeJS/public/";
	string filename = baseFolderOfNodeJS + "chessboard.bmp"; //note that this will overwrite!
	boardImage.writeImageFile(filename);
}

void ChessGame::getGameTreeRecursively(Node& parentNode, map <string, vector<string>>& data, 
	int currentDepth, int maxDepth, int moveCount)
{
	//recursionCounter++; 
	//display an update every 10K nodes ("progress report" so I'm not left in the lurch) 
	if (gameTreeNodeCount % 10'000 == 0) //expect ~160'000 nodes at level 4 
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

		//else get all moves available to that piece: 
		for (int moveIndex = 0; moveIndex < piecesToMoves.at(piece).size(); ++moveIndex)
		{
			string newPosition = piecesToMoves.at(piece).at(moveIndex); 
			
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
			boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);

			//once a piece has moved, possible positions change for ALL pieces, so clear the possible positions map
			piecesToMoves.clear();
			getPiecesToMoves();


			/****************Insertion of new node into GameTree section***************************************** */ 
			auto childNode = make_unique<Node>(); 
			childNode->data = piecesToMoves; 

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
			boardImage.piecesToPositions = switchMapKeysAndValues(positionsToPieces);
			piecesToMoves.clear();
			getPiecesToMoves();

		}

	}


}

//int ChessGame::simplestEvaluateGameState()
//{
//
//	//K is number of WHITE kings (only every equal to 0 or 1, but not so for pawns, bishops, etc.)
//	int K = 0;
//	//K_prime is number of BLACK kings 
//	int K_prime = 0;
//	
//	int Q = 0, Q_prime = 0, R = 0, R_prime = 0;
//	int B = 0, B_prime = 0, N = 0, N_prime = 0, P = 0, P_prime = 0;
//	int D = 0, D_prime = 0, S = 0, S_prime = 0, I = 0, I_prime = 0;
//	int M = 0, M_prime = 0;
//	
//	// Count pieces and other factors for both players
//	for (const auto& piece : boardImage.pieces) 
//	{
//		if (piece.find("whiteKing") != string::npos) K++;
//		if (piece.find("blackKing") != string::npos) K_prime++;
//		if (piece.find("whiteQueen") != string::npos) Q++;
//		if (piece.find("blackQueen") != string::npos) Q_prime++;
//		if (piece.find("whiteRook") != string::npos) R++;
//		if (piece.find("blackRook") != string::npos) R_prime++;
//		if (piece.find("whiteBishop") != string::npos) B++;
//		if (piece.find("blackBishop") != string::npos) B_prime++;
//		if (piece.find("whiteKnight") != string::npos) N++;
//		if (piece.find("blackKnight") != string::npos) N_prime++;
//		if (piece.find("whitePawn") != string::npos) P++;
//		if (piece.find("blackPawn") != string::npos) P_prime++;
//	
//	
//		//add doubled and isolated pawns logic if desired ...
//	
//	}
//	
//	
//	int score = 200 * (K - K_prime) + 9 * (Q - Q_prime) + 5 * (R - R_prime) +
//		3 * (B - B_prime + N - N_prime) + (P - P_prime) -
//	
//		/*the rest of these are 0 for now ...*/
//		0.5 * (D - D_prime + S - S_prime + I - I_prime) +
//		0.1 * (M - M_prime);
//	
//	return score;
//}

int ChessGame::evaluateGameState()
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
		string position = boardImage.piecesToPositions.at(piece);
		char file = position.at(0) - 'A';
		int rank = position.at(1) - '1';

		//penalize for blunders: 
		if (piece[0] == 'w')
		{
			if (isPieceThreatened(position, 'b'))
			{
				score -= findPieceValue(piece); 
			}
		}

		else
		{
			if (isPieceThreatened(position, 'w'))
			{
				score += findPieceValue(piece); 
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

vector<string> ChessGame::orderMoves(const vector<string>& moves, const string& piece)
{
	vector<string> orderedMoves = moves; 

	sort(orderedMoves.begin(), orderedMoves.end(),
		[this, &piece](const string& a, const string& b) //better names for a and b? 
		{
			string pieceAtA = getPieceAtPosition(a); 
			string pieceAtB = getPieceAtPosition(b); 

			return findPieceValue(pieceAtA) > findPieceValue(pieceAtB);
		});

	return orderedMoves; 
}

void ChessGame::simulateMove(const string& piece, const string& newPosition, bool determineIfSelfCheck, bool determineIfFork)
{
	/**************************UPDATE THE BOARD STATE SECTION**************/

	//first, check if an enemy occupies the new position
	string contents = getPieceAtPosition(newPosition); 
	bool opponentTaken = false; 
	if (getPieceRelationship(piece, contents) == "Foe")
	{
		opponentTaken = true; 
		//if an enemy is there, remove enemy from list of pieces 
		auto pos = std::find(boardImage.pieces.begin(), boardImage.pieces.end(), contents);
		boardImage.pieces.erase(pos);

		//also remove from the positions maps: 
		boardImage.piecesToPositions.erase(contents); 
		positionsToPieces.erase(newPosition); 
	}

	//first get a copy of the currentPosition of `piece`: 
	string currentPosition = boardImage.piecesToPositions.at(piece); 

	//now update the positions maps with piece at newPosition: 
	boardImage.piecesToPositions.erase(piece); 
	boardImage.piecesToPositions.insert({ piece, newPosition });

	positionsToPieces.erase(currentPosition);
	positionsToPieces.insert({ newPosition, piece });

	//NOW update the piecesToMoves map: 
	getPiecesToMoves(); 

	//*************************perform the checks of interest (look for self check, enemy check, fork, etc)

	if (determineIfSelfCheck)
	{
		if (checkForCheck(getPieceColor(piece)))
		{
			cout << "SIMULATION says piece is in check !\n";
		}
	}


	/**************************RESET TO THE ORIGINAL BOARD STATE SECTION**************/
	if (opponentTaken)
	{
		//put him back in the list and in the maps: 
		boardImage.pieces.push_back(contents); 
		boardImage.piecesToPositions.insert({ contents, newPosition });
		positionsToPieces.insert({ newPosition, contents });
	}

	//put piece back in current (old/previous) position 
	boardImage.piecesToPositions.erase(piece); 
	boardImage.piecesToPositions.insert({ piece, currentPosition });

	positionsToPieces.erase(newPosition);
	positionsToPieces.insert({ currentPosition, piece });

	//don't forget to regenerate piecesToMoves 
	getPiecesToMoves(); 
}


void ChessGame::movePiece
	(const char oldPositionFile, const int oldPositionRank, const char newPositionFile, const int newPositionRank)
{
	string oldPosition = convertCharAndIntChessPositionToString(oldPositionFile, oldPositionRank); 
	string newPosition = convertCharAndIntChessPositionToString(newPositionFile, newPositionRank);

	auto oldPositionIterator = positionsToPieces.find(oldPosition);

	if (oldPositionIterator != positionsToPieces.end())
	{
		string pieceName = oldPositionIterator->second; 

		//system("cls");
		cout << "\n\n\n";

		if (!isThatColorTurn(pieceName))
		{
			return;
		}

		if (!isPieceOnBoard(pieceName))
		{
			cout << pieceName << " not found\n";
			return;
		}

		auto positionAsCharAndInt = convertStringChessPositionToCharAndInt(newPosition);

		if (!isPositionInBounds(positionAsCharAndInt.first, positionAsCharAndInt.second))
		{
			cout << newPosition << " not a legitimate position (legal positions are A1 through H8)\n";
			return;
		}

		//confirm that newPosition is in the list of possible positions for the given piece:
		auto newPositionIterator = std::find(piecesToMoves.at(pieceName).begin(),
			piecesToMoves.at(pieceName).end(), newPosition);

		if (newPositionIterator == piecesToMoves.at(pieceName).end())
		{
			cout << newPosition << " is not an allowed position for " << pieceName
				<< " - the position is either occupied by a friend, or " << pieceName << " cannot move in that way\n";
			return;
		}

		//cout << "\nFIRST, a SIMULATION of the move..\n";
		//simulateMove(pieceName, newPosition, true, false);
		//cout << "\nEND of simulation ...\n";

		movePieceHelper(pieceName, newPosition); 
	}

	else
	{
		cout << "No piece currently occupies the position: " << oldPosition << "\n";
	}

}


void ChessGame::getPiecesToMoves()
{
	piecesToMoves.clear(); //


	assert(boardImage.pieces.size() == boardImage.piecesToPositions.size());

	for (const auto& piece : boardImage.pieces)
	{

		string position = boardImage.piecesToPositions[piece];
		char file = position.at(0);
		int rank = position.at(1) - '0'; //'0' is 48 

		vector<string> moves;

		if (piece.find("whitePawn") != string::npos)
		{
			WhitePawnMoveRules rules; 
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("blackPawn") != string::npos)
		{
			BlackPawnMoveRules rules; 
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("King") != string::npos)
		{
			KingMoveRules rules; 
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}
		
		else if (piece.find("Knight") != string::npos)
		{
			KnightMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("Rook") != string::npos)
		{
			RookMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("Bishop") != string::npos)
		{
			BishopMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else if (piece.find("Queen") != string::npos)
		{
			QueenMoveRules rules;
			moves = rules.getMoves(file, rank, piece, *this);
			piecesToMoves.insert({ piece, moves });
		}

		else
		{
			cout << "What piece is this?\n";
			__debugbreak(); 
		}

	}

}

string ChessGame::getPieceAtPosition(const string& position) const
{

	auto possiblePositionIterator = positionsToPieces.find(position); 

	if (possiblePositionIterator == positionsToPieces.end())
	{
		return string{}; 
	}

	else
	{
		return possiblePositionIterator->second; 
	}
}



bool ChessGame::checkForCheck(const string& colorToCheckForCheck)
{
	string kingToCheckForCheck = colorToCheckForCheck + "King";
	//first, find the king's position: 

	//if any white piece has a possible position equal to the black king's position, black king is in check 

	auto kingIterator = boardImage.piecesToPositions.find(kingToCheckForCheck); 

	auto positionOfKing = kingIterator->second;

	for (const auto& currentPair : piecesToMoves)
	{
		for (const auto& possiblePosition : currentPair.second)
		{
			if (possiblePosition == positionOfKing 
				&& 
				currentPair.first.find(colorToCheckForCheck) == string::npos) //ex: white cannot put white in check 
			{
				//cout << "\033[31m"; //RED!
				setTerminalColor(TerminalColor::Red);
				cout << currentPair.first << " has " << kingToCheckForCheck << " in check\n";
				//cout << "\033[0m";
				setTerminalColor(TerminalColor::Default);

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

Tree::Tree(const map <string, vector<string>> & data)
{

	//root.data = data; //former approach 
	root = make_unique<Node>(); 
	root->data = data; 

	depth = 0;
}


//void Tree::deleteNode(Node* node)
//{
//	if (node == nullptr)
//	{
//		return; 
//	}
//
//
//	for (auto& child : node->childrenLinks)
//	{
//		deleteNode(child);
//	}
//
//	node->childrenLinks.clear(); // Clear the children links to avoid dangling pointers
//	//comment above is from Copilot 
//
//	delete node;
//}

Tree::~Tree() = default; 





#pragma endregion 

#pragma region Abstract Move Rules

vector<string> WhitePawnMoveRules::getMoves(char currentFile, int currentRank, const string& currentPiece, const ChessGame& game)
{
	vector<string> moves;
	int newRank;
	newRank = currentRank + 1;

	string newPosition = currentFile + to_string(newRank);
	string pieceAtNewPosition = getPieceAtPosition(game, newPosition);

	if (pieceAtNewPosition == "")
	{
		moves.push_back(newPosition);
	}

	if (currentRank == 2)
	{
		if (pieceAtNewPosition == "")
			//ex: A2 pawn wants to move to A4 and A3 is EMPTY 
		{
			newPosition = currentFile + to_string(newRank + 1);
				
			pieceAtNewPosition = getPieceAtPosition(game, newPosition);

			if (pieceAtNewPosition == "")
			{
				moves.push_back(newPosition);
			}
		}
	}

	//check for opponent on diagonal - 
	string up1Left1 = convertCharAndIntChessPositionToString(currentFile - 1, currentRank + 1);
	string up1Right1 = convertCharAndIntChessPositionToString(currentFile + 1, currentRank + 1);

	string pieceAtUp1Left1 = getPieceAtPosition(game, up1Left1);
	if (pieceAtUp1Left1.find("black") != string::npos)
	{
		if (game.moveCount % 2 == 1) //only display if piece can be taken to the color about to move 
		{
			displayThatAPieceCanBeTaken(currentPiece, pieceAtUp1Left1);
		}
		moves.push_back(up1Left1);
	}

	string pieceAtUp1Right1 = getPieceAtPosition(game, up1Right1);

	if (pieceAtUp1Right1.find("black") != string::npos)
	{
		if (game.moveCount % 2 == 1) //only display if piece can be taken to the color about to move 
		{
			displayThatAPieceCanBeTaken(currentPiece, pieceAtUp1Right1);
		}
		moves.push_back(up1Right1);
	}


	string currentPosition = convertCharAndIntChessPositionToString(currentFile, currentRank);

	return moves;
	

}

vector<string> BlackPawnMoveRules::getMoves(char currentFile, int currentRank, const string& piece, const ChessGame& game)
{
	int newRank;
	vector<string> moves;

	//black pawns "usually" decrement rank by 1
	newRank = currentRank - 1;
	string possiblePosition = currentFile + to_string(newRank);
	string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

	if (contentsOfPossiblePosition == "") //space is empty 
		//(not occupied by friend, and pawns cannot take unless on a diagonal) 
	{
		moves.push_back(possiblePosition);
	}

	//allow moving two positions only if rank is 7 (pawn has not yet moved): 
	if (currentRank == 7)
	{
		if (contentsOfPossiblePosition == "")
			//ex: B7 pawn wants to move to B5 and B6 is EMPTY 
		{
			possiblePosition = currentFile + to_string(newRank - 1);
			contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);
			if (contentsOfPossiblePosition == "")
			{
				moves.push_back(possiblePosition);
			}
		}
	}

	//check for opponent on diagonal - 
	string firstPossibleTakingPosition = convertCharAndIntChessPositionToString(currentFile - 1, currentRank - 1);
	string secondPossibleTakingPosition = convertCharAndIntChessPositionToString(currentFile + 1, currentRank - 1);

	string contentsOfFirstTakingPosition = getPieceAtPosition(game, firstPossibleTakingPosition);
	if (contentsOfFirstTakingPosition.find("white") != string::npos)
	{
		if (game.moveCount % 2 == 0)
		{
			displayThatAPieceCanBeTaken(piece, contentsOfFirstTakingPosition);
		}
		moves.push_back(firstPossibleTakingPosition);
	}

	string contentsOfSecondTakingPosition = getPieceAtPosition(game, secondPossibleTakingPosition);

	if (contentsOfSecondTakingPosition.find("white") != string::npos)
	{
		if (game.moveCount % 2 == 0)
		{
			displayThatAPieceCanBeTaken(piece, contentsOfSecondTakingPosition);
		}
		moves.push_back(secondPossibleTakingPosition);
	}

	return moves;
}

vector<string> KingMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)

{
	vector<string> moves;

	int newRank;
	char newFile;

	//white king can move one square in any direction as long as friend is not there (and not off of board)
	for (int row = -1; row <= 1; ++row)
	{
		for (int col = -1; col <= 1; ++col)
		{
			newFile = file + col; //N.B. newFile is a CHARACTER type
			newRank = rank + row;

			//exclude "self-check" (do not list current position in list of possible positions to move to)
			if (row == 0 && col == 0)
			{
				continue;
			}

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = newFile + to_string(newRank);
				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				if (contentsOfPossiblePosition == "") //possible position is free to move to
				{
					moves.push_back(possiblePosition);
				}

				else if ((contentsOfPossiblePosition.find("white") != string::npos
					&&
					piece.find("white") != string::npos)
					||
					(contentsOfPossiblePosition.find("black") != string::npos
						&&
						piece.find("black") != string::npos)) //friend is already there
				{
					//do not add to list of possible positions yet - 
					//not handling "castling" just yet 
					if (contentsOfPossiblePosition.find("rook"))
					{
						//...need to check for clear spaces between that rook and the king ...
					}
				}

				else //must be opponent ("black" 
				{
					displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
					//ChessGame::canTakeOpponentPiece = true; //not sure what to do with this yet 
					moves.push_back(possiblePosition);
				}
			}
		}
	}

	// Castling moves
	if (piece.find("white") != string::npos && !game.whiteKingMoved)
	{
		// Kingside castling
		if (!game.whiteRookMoved[0] && game.getPieceAtPosition("F1") == "" && game.getPieceAtPosition("G1") == "")
		{
			//
			//if (!game.checkForCheck("white")) /*&&*//* !game.simulateMoveForCheck("E1", "F1", "white") && !game.simulateMoveForCheck("E1", "G1", "white")*/
			//{
				moves.push_back("G1");
			//}
		}
		// Queenside castling
		if (!game.whiteRookMoved[1] && game.getPieceAtPosition("B1") == "" && game.getPieceAtPosition("C1") == "" && game.getPieceAtPosition("D1") == "")
		{
			//if (!game.checkForCheck("white"))// && !game.simulateMoveForCheck("E1", "D1", "white") && !game.simulateMoveForCheck("E1", "C1", "white"))
			//{
				moves.push_back("C1");
			//}
		}
	}
	else if (piece.find("black") != string::npos && !game.blackKingMoved)
	{
		// Kingside castling
		if (!game.blackRookMoved[0] && game.getPieceAtPosition("F8") == "" && game.getPieceAtPosition("G8") == "")
		{
			//if (!game.checkForCheck("black"))// && !game.simulateMoveForCheck("E8", "F8", "black") && !game.simulateMoveForCheck("E8", "G8", "black"))
			//{
				moves.push_back("G8");
			//}
		}
		// Queenside castling
		if (!game.blackRookMoved[1] && game.getPieceAtPosition("B8") == "" && game.getPieceAtPosition("C8") == "" && game.getPieceAtPosition("D8") == "")
		{
			//if (!game.checkForCheck("black"))// && !game.simulateMoveForCheck("E8", "D8", "black") && !game.simulateMoveForCheck("E8", "C8", "black"))
			//{
				moves.push_back("C8");
			//}
		}
	}


	return moves;

}

vector<string> KnightMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<string> moves;
	int newRank;
	char newFile; //potentially need to cast to a char later, but vector below perhaps suggest using int for now...

	vector<pair<int, int>> listOfDeltaRanksAndFilesForKnight =
	{
		{+1, +2}, //up 1 rank, right 2 files 
		{+1, -2}, //up 1, left 2
		{+2, +1}, //up 2, right 1 
		{+2, -1}, //up 2, left 1 

		{-1, +2}, //now the "downie equivalents" 
		{-1, -2},
		{-2, +1},
		{-2, -1},
	};

	for (const auto& currentPair : listOfDeltaRanksAndFilesForKnight)
	{
		newRank = rank + currentPair.first;
		newFile = file + currentPair.second;

		if (!isPositionInBounds(newFile, newRank))
		{
			continue; //move on to the next pair if out of bounds 
		}

		string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

		string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

		string relationship = getPieceRelationship(piece, contentsOfPossiblePosition);

		if (relationship == "Neutral" || relationship == "Foe")
		{
			moves.push_back(possiblePosition);

			if (relationship == "Foe")
			{
				displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
			}
		}
		//else do nothing
	}

	return moves;
}

vector<string> RookMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<string> moves;

	vector<string> upMoves = lookForVerticalMoves(file, rank, piece, true, game);
	vector<string> downMoves = lookForVerticalMoves(file, rank, piece, false, game);

	moves.insert(moves.end(), upMoves.begin(), upMoves.end());
	moves.insert(moves.end(), downMoves.begin(), downMoves.end());

	vector<string> rightMoves = lookForHorizontalMoves(file, rank, piece, true, game);
	vector<string> leftMoves = lookForHorizontalMoves(file, rank, piece, false, game);

	moves.insert(moves.end(), rightMoves.begin(), rightMoves.end());
	moves.insert(moves.end(), leftMoves.begin(), leftMoves.end());

	return moves;
}


vector<string> BishopMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<string> possiblePositionsForBishop = lookForDiagonalMoves(file, rank, piece, game);


	return possiblePositionsForBishop;
}


vector<string> QueenMoveRules::getMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	vector<string> upMoves = lookForVerticalMoves(file, rank, piece, true, game);
	vector<string> downMoves = lookForVerticalMoves(file, rank, piece, false, game);

	vector<string> rightMoves = lookForHorizontalMoves(file, rank, piece, true, game);
	vector<string> leftMoves = lookForHorizontalMoves(file, rank, piece, false, game);

	vector<string> diagonalMoves = lookForDiagonalMoves(file, rank, piece, game);

	vector<string> possiblePositionsForQueen;

	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), upMoves.begin(), upMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), downMoves.begin(), downMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), rightMoves.begin(), rightMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), leftMoves.begin(), leftMoves.end());
	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), diagonalMoves.begin(), diagonalMoves.end());

	//= upMoves + downMoves + rightMoves + leftMoves + diagonalMoves; //not a supported operator for std::vector

	return possiblePositionsForQueen;

}


vector<string> AbstractMoveRules::lookForVerticalMoves(char file, int rank, const string& piece, bool isUp, const ChessGame& game)
{

	vector<string> possibleVerticalMoves;

	if (isUp)
	{
		int newRank = rank;
		char newFile = file;

		//first look for positions upwards: 
		bool isUpMoveAvailable = true;

		for (int i = 1; i < 9; ++i)
		{
			if (isUpMoveAvailable == false)
			{
				break;
			}
			newRank = rank + 1 * i; //NOTE the multiplication by loop counter 

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleVerticalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleVerticalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);

						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
						isUpMoveAvailable = false;

					}
				}

				//no need to consider castling since this is vertical moving (castling is horizontal) 

				if (possibleVerticalMoves.size() != previousNumberOfPossibilities + 1) //ex: A1 rook and A2 pawn is still at A2 - no up moves possible
				{
					isUpMoveAvailable = false; //waste no more loop iterations looking 
				}
			}
		}
	}

	else
	{
		int newRank = rank;
		char newFile = file;

		bool isDownMoveAvailable = true;
		for (int i = 1; i < 9; ++i)
		{
			if (isDownMoveAvailable == false)
			{
				break;
			}
			newRank = rank - 1 * i; //NOTE the negative 1

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleVerticalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleVerticalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
						isDownMoveAvailable = false;
					}
				}

				if (possibleVerticalMoves.size() != previousNumberOfPossibilities + 1) //ex: A1 rook and A2 pawn is still at A2 - no up moves possible
				{
					isDownMoveAvailable = false; //waste no more loop iterations looking 
				}
			}
		}
	}

	return possibleVerticalMoves;
}

vector<string> AbstractMoveRules::lookForHorizontalMoves(char file, int rank, const string& piece, bool isRight, const ChessGame& game)

{
	vector<string> possibleHorizontalMoves;

	if (isRight)
	{
		int newFile = file;
		char newRank = rank;

		//first look for positions upwards: 
		bool isRightMoveAvailable = true;

		for (int i = 1; i < 9; ++i)
		{
			if (isRightMoveAvailable == false)
			{
				break;
			}
			newFile = file + 1 * i; //NOTE the multiplication by loop counter 

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleHorizontalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleHorizontalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
						isRightMoveAvailable = false;
					}
				}

				//handle castling: 
				else //relationship == "Friend"
				{
					if (contentsOfPossiblePosition.find("King") != string::npos)
					{
						//check if spaces between the rook and king are empty

						//nested if - check if rook and king movecount == 0

						//if so, add king's position to possible position for rook 
						//AND add rook's position for king 

					}
				}


				if (possibleHorizontalMoves.size() != previousNumberOfPossibilities + 1)
					//ex: A1 rook and B2 knight is still at B2 - no right moves possible
				{
					isRightMoveAvailable = false; //waste no more loop iterations looking 
				}
			}
		}
	}

	else
	{
		int newRank = rank;
		char newFile = file;

		bool isLeftMoveAvailable = true;
		for (int i = 1; i < 9; ++i)
		{
			if (isLeftMoveAvailable == false)
			{
				break;
			}
			newFile = file - 1 * i; //NOTE the negative 1

			if (isPositionInBounds(newFile, newRank))
			{
				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

				string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

				string relationship = getPieceRelationship(piece, contentsOfPossiblePosition);

				size_t previousNumberOfPossibilities = possibleHorizontalMoves.size();
				if (relationship == "Neutral" || relationship == "Foe")
				{
					possibleHorizontalMoves.push_back(possiblePosition);

					if (relationship == "Foe")
					{
						displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
						isLeftMoveAvailable = false;
					}
				}

				//handle castling: 
				else //relationship == "Friend"
				{
					if (contentsOfPossiblePosition.find("King") != string::npos)
					{
						//check if spaces between the rook and king are empty

						//nested if - check if rook and king movecount == 0

						//if so, add king's position to possible position for rook 
						//AND add rook's position for king 

					}
				}


				if (possibleHorizontalMoves.size() != previousNumberOfPossibilities + 1)
				{
					isLeftMoveAvailable = false; //waste no more loop iterations looking 
				}
			}
		}
	}

	return possibleHorizontalMoves;
}

vector<string> AbstractMoveRules::lookForDiagonalMoves(char file, int rank, const string& piece, const ChessGame& game)
{
	char newFile = file;
	int newRank = rank;

	vector<string> diagonalMoves;

	//diagonal move possiblilities are: 
	//1) up N squares, right N squares 
	//2) up N squares, left N squares 
	//3) down N, right N 
	//4) down and left N squares each 

	vector<pair<int, int>> diagonalTypeMultipliers =
	{
		{1, 1}, //up and right (deltaRank and deltaFile multiplied by +1
		{1, -1},  //up and left (deltaRank multiplied by +1, deltaFile by -1)
		{-1, 1}, //down and right 
		{-1, -1} //down and left 
	};

	bool stopLookingAlongThatDiagonal;

	for (const auto& currentPair : diagonalTypeMultipliers)
	{
		stopLookingAlongThatDiagonal = false;

		for (int deltaRank = 1; deltaRank < 8; ++deltaRank)
		{
			if (stopLookingAlongThatDiagonal)
			{
				break; //breaks to outermost loop - in which diagonalTypeMultiplier is updated
			}
			for (int deltaFile = 1; deltaFile < 8; ++deltaFile)
			{
				if (stopLookingAlongThatDiagonal)
				{
					break; //breaks to middlemost loop
				}

				if (deltaRank == deltaFile)
				{


					newFile = file + deltaFile * currentPair.second;
					newRank = rank + deltaRank * currentPair.first;

					if (isPositionInBounds(newFile, newRank))
					{
						string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);

						string contentsOfPossiblePosition = getPieceAtPosition(game, possiblePosition);

						string relationship =getPieceRelationship(piece, contentsOfPossiblePosition);

						if (relationship == "Neutral" || relationship == "Foe")
						{
							diagonalMoves.push_back(possiblePosition);
							if (relationship == "Foe")
							{
								displayThatAPieceCanBeTaken(piece, contentsOfPossiblePosition);
								//set flag to stop looking further along "that" diagonal 
								stopLookingAlongThatDiagonal = true;
							}
						}

						else //relationship is Friend
						{
							stopLookingAlongThatDiagonal = true;
						}
					}

					else //if we've gone out of bounds, no reason to continue further 
					{
						stopLookingAlongThatDiagonal = true;
					}
				}
			}
		}
	}
	return diagonalMoves;

}


#pragma endregion 
