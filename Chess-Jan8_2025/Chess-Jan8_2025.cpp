// Chess-Jan8_2025.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include"AIChessGame.h"

//#include"ChessImageBMP.h"

int main()
{
	
	//ChessGame theGame{}; 

	//theGame.play(); 

	AIChessGame theGame{};
	//theGame.play(); 
	
	//theGame.getGameTreeRecursively()

	//theGame.getMinimaxMove(2); 

	/*get first suggested move (benchmarking minimaxAlphaBeta efficiency)*/
	pair<string, ChessPosition> suggestedMove = theGame.getMinimaxAlphaBetaMove(5); 

	cout << suggestedMove.first << " -> " << suggestedMove.second.file << suggestedMove.second.rank << "\n";

}
