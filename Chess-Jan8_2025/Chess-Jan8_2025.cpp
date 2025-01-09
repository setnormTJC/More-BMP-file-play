// Chess-Jan8_2025.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include"ChessGame.h"

//#include"ChessImageBMP.h"

int main()
{
	//ChessImageBMP chessImageBMP{}; 
	//chessImageBMP.writeImageFile("out.bmp"); 

	//cout << sizeof(ChessPosition) << "\n"; //8 bytes
	//cout << sizeof(string) << "\n"; //32 bytes (in Release mode) 
	
	ChessGame theGame{}; 
	theGame.showAllPossibleMoves(); 

}
