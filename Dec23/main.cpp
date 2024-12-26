

//#include"ChessImageBMP.h"

#include"ChessGame.h"

#include<thread> 


int main()
{
	//int a = 2;

	//if (a == 2)
	//{
	//	__debugbreak();  //may be preferable to cin.get() for my tastes 
	//}

	//int b = 23; 

	//ChessImageBMP chessboardImage{}; 
	//
	//const char* filename = "323.bmp"; //current time
	//chessboardImage.writeImageFile(filename); 
	//system(filename); 

	ChessGame theGame{}; 

	theGame.arbitrarilyMovePiece("blackKing", "E5");

	const char* filename = "335.bmp"; 
	theGame.writeImageFile(filename); 

	system(filename); 

	return 0; 












}

