#pragma once

#include <fstream>
#include <queue>
#include <mutex>

#include<Windows.h>
#include <iostream>
#include <vector>
#include <string>


using std::vector, std::cout, std::string;


HHOOK mouseHook;
HWND targetWindow;
POINT lastClickCoordinates;
std::atomic<bool> running(true);
std::queue<POINT> clickQueue;
std::mutex queueMutex;
std::condition_variable queueCV;


//map<string, vector<string>> ChessGame::simulatePiecesToPossiblePositions()
//{
//    map<string, vector<string>> simulatedPiecesToPossiblePositions;
//
//    for (int i = 0; i < boardImage.pieceNames.size(); ++i)
//    {
//        string currentPieceName = boardImage.pieceNames.at(i);
//
//        string currentPosition = boardImage.piecesToPositions.find(currentPieceName)->second;
//
//        vector<string> possiblePositionsForCurrentPiece =
//            PieceRules::getPossiblePositionsForCurrentPiece(currentPieceName, currentPosition);
//
//        //NOTE the difference (from the other version of `generatePiecesToPossiblePositions`) here!
//        simulatedPiecesToPossiblePositions.insert({ currentPieceName, possiblePositionsForCurrentPiece });
//    }
//
//    return simulatedPiecesToPossiblePositions;
//}

class PieceMoveRules 
{
    static vector<string> getWhitePawnMoves(int currentRank, char currentFile, const string& currentPiece);
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



    PieceMoveRules();


};
//{
    //string response;
//while (true) {
//	cout << "Do you accept these as your inputs (y/n)? \n";
//	std::getline(std::cin, response);
//	
//	if (response.length() > 1) //a safety measure in case I accidentally enter "yes" 
//	{
//		setTerminalColor(TerminalColor::Red); 
//		cout << "Careful - enter one letter response - y or n\n";
//		continue; 
//	}

//	if (response != "y") {
//		cout << "How many seconds would you like this program to wait while you select the piece"
//			<< " and its new position? \n";
//		int seconds;
//		std::cin >> seconds;

//		callNodeJS(); 
//		openPort3000_andDisplayChessBoard(); 

//		for (int i = 0; i < seconds; ++i)
//		{
//			cout << seconds - i << "...";
//			std::this_thread::sleep_for(std::chrono::seconds{ 1});
//		}

//		// Clear the input buffer
//		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

//		// Re-read the coordinates
//		theTwoCoordinates = readFileAndReturnCoordinatesClickedInBrowser();

//		x0 = theTwoCoordinates.at(0).first;
//		y0 = theTwoCoordinates.at(0).second;

//		firstPosition = boardImage.convertImageCoordinatesToPosition(x0, y0);

//		xf = theTwoCoordinates.at(1).first;
//		yf = theTwoCoordinates.at(1).second;

//		secondPosition = boardImage.convertImageCoordinatesToPosition(xf, yf);

//		cout << firstPosition.first << firstPosition.second << "->"
//			<< secondPosition.first << secondPosition.second << "\n";
//	}
//	else {
//		break;
//	}
//}

//}

#pragma region PieceRules

//vector<string> PieceMoveRules::getWhitePawnMoves(int currentRank, char currentFile, const string& currentPiece)
//{
//	vector<string> whitePawnMoves;
//	int newRank;
//	newRank = currentRank + 1;
//
//	string newPosition = currentFile + to_string(newRank);
//	string pieceAtNewPosition = ChessGame::getPieceAtPosition(newPosition);
//
//	if (pieceAtNewPosition == "")
//	{
//		whitePawnMoves.push_back(newPosition);
//	}
//
//	if (currentRank == 2)
//	{
//		if (pieceAtNewPosition == "")
//			//ex: A2 pawn wants to move to A4 and A3 is EMPTY 
//		{
//			newPosition = currentFile + to_string(newRank + 1);
//			pieceAtNewPosition = ChessGame::getPieceAtPosition(newPosition);
//
//			if (pieceAtNewPosition == "")
//			{
//				whitePawnMoves.push_back(newPosition);
//			}
//		}
//	}
//
//	//check for opponent on diagonal - 
//	string up1Left1 = convertCharAndIntChessPositionToString(currentFile - 1, currentRank + 1);
//	string up1Right1 = convertCharAndIntChessPositionToString(currentFile + 1, currentRank + 1);
//
//	string pieceAtUp1Left1 = ChessGame::getPieceAtPosition(up1Left1);
//	if (pieceAtUp1Left1.find("black") != string::npos)
//	{
//		if (ChessGame::moveCount % 2 == 1) //only display if piece can be taken to the color about to move 
//		{
//			displayThatAPieceCanBeTaken(currentPiece, pieceAtUp1Left1);
//		}
//		whitePawnMoves.push_back(up1Left1);
//	}
//
//	string pieceAtUp1Right1 = ChessGame::getPieceAtPosition(up1Right1);
//
//	if (pieceAtUp1Right1.find("black") != string::npos)
//	{
//		if (ChessGame::moveCount % 2 == 1) //only display if piece can be taken to the color about to move 
//		{
//			displayThatAPieceCanBeTaken(currentPiece, pieceAtUp1Right1);
//		}
//		whitePawnMoves.push_back(up1Right1);
//	}
//
//
//	string currentPosition = convertCharAndIntChessPositionToString(currentFile, currentRank);
//
//
//	return whitePawnMoves;
//}

//vector<string> PieceMoveRules::getBlackPawnPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
//{
//	int newRank;
//	vector<string> possiblePositionsForBlackPawn;
//
//	//black pawns "usually" decrement rank by 1
//	newRank = currentRank - 1;
//	string possiblePosition = currentFile + to_string(newRank);
//	string contentsOfPossiblePosition = ChessGame::getPieceAtPosition(possiblePosition);
//
//	if (contentsOfPossiblePosition == "") //space is empty 
//		//(not occupied by friend, and pawns cannot take unless on a diagonal) 
//	{
//		possiblePositionsForBlackPawn.push_back(possiblePosition);
//	}
//
//	//allow moving two positions only if rank is 7 (pawn has not yet moved): 
//	if (currentRank == 7)
//	{
//		if (contentsOfPossiblePosition == "")
//			//ex: B7 pawn wants to move to B5 and B6 is EMPTY 
//		{
//			possiblePosition = currentFile + to_string(newRank - 1);
//			contentsOfPossiblePosition = ChessGame::getPieceAtPosition(possiblePosition);
//			if (contentsOfPossiblePosition == "")
//			{
//				possiblePositionsForBlackPawn.push_back(possiblePosition);
//			}
//		}
//	}
//
//	//check for opponent on diagonal - 
//	string firstPossibleTakingPosition = convertCharAndIntChessPositionToString(currentFile - 1, currentRank - 1);
//	string secondPossibleTakingPosition = convertCharAndIntChessPositionToString(currentFile + 1, currentRank - 1);
//
//	string contentsOfFirstTakingPosition = ChessGame::getPieceAtPosition(firstPossibleTakingPosition);
//	if (contentsOfFirstTakingPosition.find("white") != string::npos)
//	{
//		if (ChessGame::moveCount % 2 == 0)  
//		{
//			displayThatAPieceCanBeTaken(currentPieceName, contentsOfFirstTakingPosition);
//		}
//		possiblePositionsForBlackPawn.push_back(firstPossibleTakingPosition);
//	}
//
//	string contentsOfSecondTakingPosition = ChessGame::getPieceAtPosition(secondPossibleTakingPosition);
//
//	if (contentsOfSecondTakingPosition.find("white") != string::npos)
//	{
//		if (ChessGame::moveCount % 2 == 0) 
//		{
//			displayThatAPieceCanBeTaken(currentPieceName, contentsOfSecondTakingPosition);
//		}
//		possiblePositionsForBlackPawn.push_back(secondPossibleTakingPosition);
//	}
//
//	return possiblePositionsForBlackPawn;
//}
//
//vector<string> PieceMoveRules::getKingPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
//{
//	vector<string> possiblePositionsForKing;
//
//	int newRank;
//	char newFile;
//
//	//white king can move one square in any direction as long as friend is not there (and not off of board)
//	for (int row = -1; row <= 1; ++row)
//	{
//		for (int col = -1; col <= 1; ++col)
//		{
//			newFile = currentFile + col; //N.B. newFile is a CHARACTER type
//			newRank = currentRank + row;
//
//			//exclude "self-check" (do not list current position in list of possible positions to move to)
//			if (row == 0 && col == 0)
//			{
//				continue;
//			}
//
//			if (isPositionInBounds(newFile, newRank))
//			{
//				string possiblePosition = newFile + to_string(newRank);
//				string contentsOfPossiblePosition = ChessGame::getPieceAtPosition(possiblePosition);
//
//				if (contentsOfPossiblePosition == "") //possible position is free to move to
//				{
//					possiblePositionsForKing.push_back(possiblePosition);
//				}
//
//				else if ((contentsOfPossiblePosition.find("white") != string::npos
//					&&
//					currentPieceName.find("white") != string::npos)
//					||
//					(contentsOfPossiblePosition.find("black") != string::npos
//						&&
//						currentPieceName.find("black") != string::npos)) //friend is already there
//				{
//					//do not add to list of possible positions yet - 
//					//not handling "castling" just yet 
//					if (contentsOfPossiblePosition.find("rook"))
//					{
//						//...need to check for clear spaces between that rook and the king ...
//					}
//				}
//
//				else //must be opponent ("black" 
//				{
//					displayThatAPieceCanBeTaken(currentPieceName, contentsOfPossiblePosition);
//					//ChessGame::canTakeOpponentPiece = true; //not sure what to do with this yet 
//					possiblePositionsForKing.push_back(possiblePosition);
//				}
//			}
//		}
//	}
//
//	return possiblePositionsForKing;
//
//}

//vector<string> PieceMoveRules::getKnightPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
//{
//	vector<string> possiblePositionsForKnight;
//	int newRank;
//	char newFile; //potentially need to cast to a char later, but vector below perhaps suggest using int for now...
//
//	vector<pair<int, int>> listOfDeltaRanksAndFilesForKnight =
//	{
//		{+1, +2}, //up 1 rank, right 2 files 
//		{+1, -2}, //up 1, left 2
//		{+2, +1}, //up 2, right 1 
//		{+2, -1}, //up 2, left 1 
//
//		{-1, +2}, //now the "downie equivalents" 
//		{-1, -2},
//		{-2, +1},
//		{-2, -1},
//	};
//
//	for (const auto& currentPair : listOfDeltaRanksAndFilesForKnight)
//	{
//		newRank = currentRank + currentPair.first;
//		newFile = currentFile + currentPair.second;
//
//		if (!isPositionInBounds(newFile, newRank))
//		{
//			continue; //move on to the next pair if out of bounds 
//		}
//
//		string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);
//
//		string contentsOfPossiblePosition = ChessGame::getPieceAtPosition(possiblePosition);
//
//		string relationship = getPieceRelationship(currentPieceName, contentsOfPossiblePosition);
//
//		if (relationship == "Neutral" || relationship == "Foe")
//		{
//			possiblePositionsForKnight.push_back(possiblePosition);
//
//			if (relationship == "Foe")
//			{
//				displayThatAPieceCanBeTaken(currentPieceName, contentsOfPossiblePosition);
//			}
//		}
//		//else do nothing
//	}
//
//	return possiblePositionsForKnight;
//}

//vector<string>PieceMoveRules::getRookPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
//{
//
//	vector<string> possiblePositionsForRook;
//
//	vector<string> upMoves = lookForVerticalMoves(currentRank, currentFile, currentPieceName, true);
//	vector<string> downMoves = lookForVerticalMoves(currentRank, currentFile, currentPieceName, false);
//
//	possiblePositionsForRook.insert(possiblePositionsForRook.end(), upMoves.begin(), upMoves.end());
//	possiblePositionsForRook.insert(possiblePositionsForRook.end(), downMoves.begin(), downMoves.end());
//
//	vector<string> rightMoves = lookForHorizontalMoves(currentRank, currentFile, currentPieceName, true); 
//	vector<string> leftMoves = lookForHorizontalMoves(currentRank, currentFile, currentPieceName, false);
//
//	possiblePositionsForRook.insert(possiblePositionsForRook.end(), rightMoves.begin(), rightMoves.end()); 
//	possiblePositionsForRook.insert(possiblePositionsForRook.end(), leftMoves.begin(), leftMoves.end()); 
//
//	return possiblePositionsForRook;
//}
//
//vector<string> PieceMoveRules::getBishopPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
//{
//	vector<string> possiblePositionsForBishop = 
//		lookForDiagonalMoves(currentRank, currentFile, currentPieceName);
//
//
//	return possiblePositionsForBishop;
//}
//
//vector<string> PieceMoveRules::getQueenPossiblePositions(int currentRank, char currentFile, const string& currentPieceName)
//{
//	vector<string> upMoves = lookForVerticalMoves(currentRank, currentFile, currentPieceName, true);
//	vector<string> downMoves = lookForVerticalMoves(currentRank, currentFile, currentPieceName, false);
//
//	vector<string> rightMoves = lookForHorizontalMoves(currentRank, currentFile, currentPieceName, true);
//	vector<string> leftMoves = lookForHorizontalMoves(currentRank, currentFile, currentPieceName, false);
//
//	vector<string> diagonalMoves = lookForDiagonalMoves(currentRank, currentFile, currentPieceName);
//
//	vector<string> possiblePositionsForQueen; 
//
//	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), upMoves.begin(), upMoves.end());
//	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), downMoves.begin(), downMoves.end());
//	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), rightMoves.begin(), rightMoves.end()); 
//	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), leftMoves.begin(), leftMoves.end()); 
//	possiblePositionsForQueen.insert(possiblePositionsForQueen.end(), diagonalMoves.begin(), diagonalMoves.end()); 
//	
//	//= upMoves + downMoves + rightMoves + leftMoves + diagonalMoves; //not a supported operator for std::vector
//
//	return possiblePositionsForQueen; 
//
//}
//
//vector<string> PieceMoveRules::lookForDiagonalMoves(int currentRank, char currentFile, const string& currentPieceName)
//{
//	char newFile = currentFile;
//	int newRank = currentRank; 
//
//	vector<string> diagonalMoves; 
//
//	//diagonal move possiblilities are: 
//	//1) up N squares, right N squares 
//	//2) up N squares, left N squares 
//	//3) down N, right N 
//	//4) down and left N squares each 
//
//	vector<pair<int, int>> diagonalTypeMultipliers =
//	{
//		{1, 1}, //up and right (deltaRank and deltaFile multiplied by +1
//		{1, -1},  //up and left (deltaRank multiplied by +1, deltaFile by -1)
//		{-1, 1}, //down and right 
//		{-1, -1} //down and left 
//	};
//
//	bool stopLookingAlongThatDiagonal; 
//
//	for (const auto& currentPair : diagonalTypeMultipliers)
//	{
//		stopLookingAlongThatDiagonal = false; 
//
//		for (int deltaRank = 1; deltaRank < 8; ++deltaRank)
//		{
//			if (stopLookingAlongThatDiagonal)
//			{
//				break; //breaks to outermost loop - in which diagonalTypeMultiplier is updated
//			}
//			for (int deltaFile = 1; deltaFile < 8; ++deltaFile)
//			{
//				if (stopLookingAlongThatDiagonal)
//				{
//					break; //breaks to middlemost loop
//				}
//
//				if (deltaRank == deltaFile)
//				{
//
//
//					newFile = currentFile + deltaFile * currentPair.second;
//					newRank = currentRank + deltaRank * currentPair.first;
//
//					if (isPositionInBounds(newFile, newRank))
//					{
//						string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);
//
//						string contentsOfPossiblePosition = ChessGame::getPieceAtPosition(possiblePosition);
//
//						string relationship = ChessGame::getPieceRelationship(currentPieceName, contentsOfPossiblePosition);
//
//						if (relationship == "Neutral" || relationship == "Foe")
//						{
//							diagonalMoves.push_back(possiblePosition);
//							if (relationship == "Foe")
//							{
//								displayThatAPieceCanBeTaken(currentPieceName, contentsOfPossiblePosition);
//								//set flag to stop looking further along "that" diagonal 
//								stopLookingAlongThatDiagonal = true;
//							}
//						}
//
//						else //relationship is Friend
//						{
//							stopLookingAlongThatDiagonal = true; 
//						}
//					}
//
//					else //if we've gone out of bounds, no reason to continue further 
//					{
//						stopLookingAlongThatDiagonal = true;
//					}
//				}
//			}
//		}
//	}
//	return diagonalMoves; 
//}
//
//vector<string> PieceMoveRules::lookForVerticalMoves(int currentRank, char currentFile, const string& currentPieceName, 
//	bool isUp)
//{
//
//	vector<string> possibleVerticalMoves; 
//
//	if (isUp)
//	{
//		int newRank = currentRank;
//		char newFile = currentFile;
//
//		//first look for positions upwards: 
//		bool isUpMoveAvailable = true;
//
//		for (int i = 1; i < 9; ++i)
//		{
//			if (isUpMoveAvailable == false)
//			{
//				break;
//			}
//			newRank = currentRank + 1 * i; //NOTE the multiplication by loop counter 
//
//			if (isPositionInBounds(newFile, newRank))
//			{
//				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);
//
//				string contentsOfPossiblePosition = ChessGame::getPieceAtPosition(possiblePosition);
//
//				string relationship = ChessGame::getPieceRelationship(currentPieceName, contentsOfPossiblePosition);
//
//				size_t previousNumberOfPossibilities = possibleVerticalMoves.size();
//				if (relationship == "Neutral" || relationship == "Foe")
//				{
//					possibleVerticalMoves.push_back(possiblePosition);
//
//					if (relationship == "Foe")
//					{
//						displayThatAPieceCanBeTaken(currentPieceName, contentsOfPossiblePosition); 
//
//						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
//						isUpMoveAvailable = false; 
//
//					}
//				}
//
//				//no need to consider castling since this is vertical moving (castling is horizontal) 
//
//				if (possibleVerticalMoves.size() != previousNumberOfPossibilities + 1) //ex: A1 rook and A2 pawn is still at A2 - no up moves possible
//				{
//					isUpMoveAvailable = false; //waste no more loop iterations looking 
//				}
//			}
//		}
//	}
//
//	else
//	{
//		int newRank = currentRank;
//		char newFile = currentFile;
//
//		bool isDownMoveAvailable = true;
//		for (int i = 1; i < 9; ++i)
//		{
//			if (isDownMoveAvailable == false)
//			{
//				break;
//			}
//			newRank = currentRank - 1 * i; //NOTE the negative 1
//
//			if (isPositionInBounds(newFile, newRank))
//			{
//				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);
//
//				string contentsOfPossiblePosition = ChessGame::getPieceAtPosition(possiblePosition);
//
//				string relationship = ChessGame::getPieceRelationship(currentPieceName, contentsOfPossiblePosition);
//
//				size_t previousNumberOfPossibilities = possibleVerticalMoves.size();
//				if (relationship == "Neutral" || relationship == "Foe")
//				{
//					possibleVerticalMoves.push_back(possiblePosition);
//
//					if (relationship == "Foe")
//					{
//						displayThatAPieceCanBeTaken(currentPieceName, contentsOfPossiblePosition);
//						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
//						isDownMoveAvailable = false;
//					}
//				}
//
//				if (possibleVerticalMoves.size() != previousNumberOfPossibilities + 1) //ex: A1 rook and A2 pawn is still at A2 - no up moves possible
//				{
//					isDownMoveAvailable = false; //waste no more loop iterations looking 
//				}
//			}
//		}
//	}
//
//	return possibleVerticalMoves; 
//}
//
//vector<string> PieceMoveRules::lookForHorizontalMoves(int currentRank, char currentFile, const string& currentPieceName, bool isRight)
//
//{
//	vector<string> possibleHorizontalMoves;
//
//	if (isRight)
//	{
//		int newFile = currentFile;
//		char newRank = currentRank;
//
//		//first look for positions upwards: 
//		bool isRightMoveAvailable = true;
//
//		for (int i = 1; i < 9; ++i)
//		{
//			if (isRightMoveAvailable == false)
//			{
//				break;
//			}
//			newFile = currentFile + 1 * i; //NOTE the multiplication by loop counter 
//
//			if (isPositionInBounds(newFile, newRank))
//			{
//				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);
//
//				string contentsOfPossiblePosition = ChessGame::getPieceAtPosition(possiblePosition);
//
//				string relationship = ChessGame::getPieceRelationship(currentPieceName, contentsOfPossiblePosition);
//
//				size_t previousNumberOfPossibilities = possibleHorizontalMoves.size();
//				if (relationship == "Neutral" || relationship == "Foe")
//				{
//					possibleHorizontalMoves.push_back(possiblePosition);
//
//					if (relationship == "Foe")
//					{
//						displayThatAPieceCanBeTaken(currentPieceName, contentsOfPossiblePosition);
//						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
//						isRightMoveAvailable = false;
//					}
//				}
//
//				//handle castling: 
//				else //relationship == "Friend"
//				{
//					if (contentsOfPossiblePosition.find("King") != string::npos)
//					{
//						//check if spaces between the rook and king are empty
//
//						//nested if - check if rook and king movecount == 0
//
//						//if so, add king's position to possible position for rook 
//						//AND add rook's position for king 
//						
//					}
//				}
//
//				
//				if (possibleHorizontalMoves.size() != previousNumberOfPossibilities + 1)
//					//ex: A1 rook and B2 knight is still at B2 - no right moves possible
//				{
//					isRightMoveAvailable = false; //waste no more loop iterations looking 
//				}
//			}
//		}
//	}
//
//	else
//	{
//		int newRank = currentRank;
//		char newFile = currentFile;
//
//		bool isLeftMoveAvailable = true;
//		for (int i = 1; i < 9; ++i)
//		{
//			if (isLeftMoveAvailable == false)
//			{
//				break;
//			}
//			newFile = currentFile - 1 * i; //NOTE the negative 1
//
//			if (isPositionInBounds(newFile, newRank))
//			{
//				string possiblePosition = convertCharAndIntChessPositionToString(newFile, newRank);
//
//				string contentsOfPossiblePosition = ChessGame::getPieceAtPosition(possiblePosition);
//
//				string relationship = ChessGame::getPieceRelationship(currentPieceName, contentsOfPossiblePosition);
//
//				size_t previousNumberOfPossibilities = possibleHorizontalMoves.size();
//				if (relationship == "Neutral" || relationship == "Foe")
//				{
//					possibleHorizontalMoves.push_back(possiblePosition);
//
//					if (relationship == "Foe")
//					{
//						displayThatAPieceCanBeTaken(currentPieceName, contentsOfPossiblePosition);
//						//can't move any farther beyond foe piece, so set flag = false to stop looking: 
//						isLeftMoveAvailable = false;
//					}
//				}
//
//				//handle castling: 
//				else //relationship == "Friend"
//				{
//					if (contentsOfPossiblePosition.find("King") != string::npos)
//					{
//						//check if spaces between the rook and king are empty
//
//						//nested if - check if rook and king movecount == 0
//
//						//if so, add king's position to possible position for rook 
//						//AND add rook's position for king 
//
//					}
//				}
//
//
//				if (possibleHorizontalMoves.size() != previousNumberOfPossibilities + 1) 
//				{
//					isLeftMoveAvailable = false; //waste no more loop iterations looking 
//				}
//			}
//		}
//	}
//
//	return possibleHorizontalMoves;
//}
//
//
//vector<string> PieceMoveRules::getPossiblePositionsForCurrentPiece(const string& currentPieceName, const string& currentPosition)
//{
//
//	vector<string> possiblePositionsForCurrentPiece;
//
//	int currentRank = (int)(currentPosition.at(1) - 48); //ASCII value for character '0' is 48
//	char currentFile = currentPosition.at(0);
//
//	if (currentPieceName.find("blackPawn") != string::npos)
//	{
//		possiblePositionsForCurrentPiece =
//			getBlackPawnPossiblePositions(currentRank, currentFile, currentPieceName);
//
//	}
//
//	else if (currentPieceName.find("whitePawn") != string::npos)
//	{
//		possiblePositionsForCurrentPiece =
//			getWhitePawnMoves(currentRank, currentFile, currentPieceName);
//
//	}
//
//	//IGNORING placing self in check for now ....
//	else if (currentPieceName.find("King") != string::npos)
//	{
//		possiblePositionsForCurrentPiece =
//			getKingPossiblePositions(currentRank, currentFile, currentPieceName);
//
//	}
//
//	else if (currentPieceName.find("Knight") != string::npos)
//	{
//		possiblePositionsForCurrentPiece =
//			getKnightPossiblePositions(currentRank, currentFile, currentPieceName);
//
//	}
//
//	else if (currentPieceName.find("Rook") != string::npos)
//	{
//		possiblePositionsForCurrentPiece =
//			getRookPossiblePositions(currentRank, currentFile, currentPieceName);
//	}
//	
//	else if (currentPieceName.find("Bishop") != string::npos)
//	{
//		possiblePositionsForCurrentPiece =
//			getBishopPossiblePositions(currentRank, currentFile, currentPieceName); 
//	}
//
//	else if (currentPieceName.find("Queen") != string::npos)
//	{
//		possiblePositionsForCurrentPiece =
//			getQueenPossiblePositions(currentRank, currentFile, currentPieceName);
//	}
//
//	else
//	{
//		cout << "What piece is that? \n";
//		__debugbreak(); 
//	}
//
//	return possiblePositionsForCurrentPiece;
//}
//
//PieceMoveRules::PieceMoveRules()
//{
//	//no member vars and methods are static - does this default constructor even need to exist? 
//}


#pragma endregion
//void ChessGame::eraseMovesThatPutSelfInCheck(const string& pieceColor)
//{
//    //loop over all pieces of the appropriate color: 
//    for (auto& singlePieceToPossiblePositions : piecesToPossiblePositions)
//    {
//
//        string pieceName = singlePieceToPossiblePositions.first;
//
//        if (pieceName.find(pieceColor) != string::npos) //don't waste time looking at pieces ... whose turn it is not
//        {
//            //SIMULATE moving the piece to the possible position
//            string oldPosition = boardImage.piecesToPositions.at(pieceName);
//
//            //check if oldPosition != ""?
//
//            for (const auto& possiblePosition : singlePieceToPossiblePositions.second)
//            {
//                string newPosition = possiblePosition;
//                //erase previous position:
//                boardImage.piecesToPositions.erase(pieceName);
//                //insert new position: 
//                boardImage.piecesToPositions.insert({ pieceName, newPosition });
//
//                //check if opponent will be taken by the move: 
//                bool pieceTaken = false;
//                auto newPositionMapIterator = positionsToPieces.find(newPosition);
//                string opponentPieceName;
//                if (newPositionMapIterator != positionsToPieces.end()) //ENEMY must be there
//                {
//                    opponentPieceName = newPositionMapIterator->second;
//                    auto opponentVectorIterator =
//                        std::find(boardImage.pieceNames.begin(), boardImage.pieceNames.end(), opponentPieceName);
//
//                    boardImage.pieceNames.erase(opponentVectorIterator);
//
//                    pieceTaken = true;
//                }
//
//                //update OTHER map
//                positionsToPieces.erase(oldPosition);
//                positionsToPieces.insert({ newPosition, pieceName }); //potentially OVERWRITES an opponent
//
//
//                //determine NEW piecesToPossiblePositions: 
//                auto simulatedPiecesToPossiblePositions = simulatePiecesToPossiblePositions();
//
//                //find king (with pieceColor) location: 
//                string kingName = pieceColor + "King";
//
//                //check if any piece of opposite color can move to king's location: 
//                for (auto& singleSimulatedPieceToPossiblePositions : simulatedPiecesToPossiblePositions)
//                {
//                    string simulatedPieceName = singleSimulatedPieceToPossiblePositions.first;
//
//                    for (auto& simulatedPossiblePosition : singleSimulatedPieceToPossiblePositions.second)
//                    {
//                        if (positionsToPieces.at(simulatedPossiblePosition) == kingName)
//                        {
//                            cout << pieceName << " has check on " << kingName << "\n";
//
//                            //remove `possiblePosition` from possiblePositions (permanently)  
//                        }
//                    }
//
//                }
//                //optional - print a message (depending on isThatColorTurn) informing that pieceName to pos is
//                //illegal due to check 
//
//                //UNDO all changes to the board state (the maps and the vector of pieceNames) 
//                boardImage.piecesToPositions.erase(pieceName);
//                boardImage.piecesToPositions.insert({ pieceName, oldPosition });
//
//                positionsToPieces.erase(newPosition);
//                positionsToPieces.insert({ oldPosition, pieceName });
//
//                if (pieceTaken)
//                {
//                    boardImage.pieceNames.push_back(opponentPieceName);
//                }
//            }
//        }
//    }
//}


void workerThread()
{
    while (running)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCV.wait(lock, [] { return !clickQueue.empty() || !running; });

        while (!clickQueue.empty())
        {
            POINT pt = clickQueue.front();
            clickQueue.pop();
            lock.unlock();

            // Write coordinates to file
            writeCoordinatesToFile(pt, "click_coordinates.txt");

            lock.lock();
        }
    }
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_LBUTTONDOWN)
    {
        MSLLHOOKSTRUCT* mouseStruct = (MSLLHOOKSTRUCT*)lParam;

        if (mouseStruct != nullptr)
        {
            POINT pt = mouseStruct->pt;

            RECT rect;
            GetWindowRect(targetWindow, &rect);
            if (PtInRect(&rect, pt))
            {
                ScreenToClient(targetWindow, &pt); // updates pt values
                // ex: if window top left is at screen's {100, 100} coordinate

                lastClickCoordinates = pt;

                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    clickQueue.push(pt);
                }
                queueCV.notify_one();

                cout << "Mouse click at (" << pt.x << " , " << pt.y << ")\n";
            }
            else
            {
                cout << "Point not in window \n";
            }
        }
        else
        {
            cout << "Mouse struct is nullptr\n";
        }
    }
    else
    {
        cout << "MouseProc not processing LBUTTONDOWN\n";
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

void setMouseHook(HWND hwnd)
{
    targetWindow = hwnd;

    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, nullptr, 0);

    if (mouseHook == nullptr)
    {
        cout << "Failed to set mouse hook\n";
    }
    else
    {
        cout << "Mouse hook set \n";
        std::thread(workerThread).detach();
    }
}

void removeMouseHook()
{
    if (mouseHook != nullptr)
    {
        UnhookWindowsHookEx(mouseHook);
        mouseHook = nullptr;
        cout << "Mouse hook removed\n";
    }

    running = false;
    queueCV.notify_all();
}

POINT getLastClickCoordinates()
{
    return lastClickCoordinates;
}

void writeCoordinatesToFile(const POINT& pt, const std::string& filename)
{
    std::ofstream outFile;
    outFile.open(filename, std::ios_base::app); // Append mode
    if (outFile.is_open())
    {
        outFile << "Mouse click at (" << pt.x << ", " << pt.y << ")\n";
        outFile.close();
    }
    else
    {
        cout << "Failed to open file for writing\n";
    }
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    vector<HWND>& windows = *reinterpret_cast<vector<HWND>*>(lParam);

    windows.push_back(hwnd);

    return TRUE;
}

vector<HWND> getAllOpenWindows()
{
    vector<HWND> windows;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));

    return windows;
}

string getWindowTitle(HWND hwnd)
{
    char title[256];

    GetWindowTextA(hwnd, title, sizeof(title));

    return string(title);
}

HWND getWindowByTitle(const string& desiredTitle)
{
    vector<HWND> windows = getAllOpenWindows();

    for (auto currentWindow : windows)
    {
        if (getWindowTitle(currentWindow) == desiredTitle)
        {
            return currentWindow;
        }
    }

    return nullptr;
}