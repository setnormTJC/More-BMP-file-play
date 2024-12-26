// Dec23.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include"ImageBMP.h"
#include "someImageBMPTestingFunctions.h"
#include<thread> 




auto getEmptyChessBoardImage()
{
	ChessImageBMP chessImageBMP{};

	chessImageBMP.drawEmptyChessBoard();


	chessImageBMP.setPixelToColor_withThickness(200, 100, ColorEnum::Green, 5); //thin-ish green pixels at bottom right-ish

	chessImageBMP.setPixelToColor_withThickness(100, 400, ColorEnum::Magenta, 10); //thicker magenta pixels at left, top-ish

	chessImageBMP.drawA(35, 10, ColorEnum::Black);
	chessImageBMP.drawB(120, 10, ColorEnum::Black);

	chessImageBMP.drawLetters();
	chessImageBMP.drawNumbers();

	//chessImageBMP.drawAndFillAnIrregularShape(); 

	return chessImageBMP; 

}
int main()
{


	auto chessImageBMP = getEmptyChessBoardImage(); 
	
	chessImageBMP.generatePositionsToImageCoordinatesMap(); 


	auto A8Pos = chessImageBMP.positionsToImageCoordinates.find("A8");
	if (A8Pos != chessImageBMP.positionsToImageCoordinates.end())
	{
		int x = A8Pos->second.first;
		int y = A8Pos->second.second; 

		chessImageBMP.drawA(x, y, Color{ ColorEnum::Blue });
	}


	
	auto whiteKnightImage = getWhiteKnightImage(); 
	auto whiteKnightPixelData = whiteKnightImage.pixelData.pixelMatrix; 


	auto whiteKnightQSideStartingPos = chessImageBMP.positionsToImageCoordinates.find("B1");
	if (whiteKnightQSideStartingPos != chessImageBMP.positionsToImageCoordinates.end())
	{
		//chessImageBMP.drawPieceOnBoard(whiteKnightPixelData, 50, 50);
		int x = whiteKnightQSideStartingPos->second.first; 
		int y = whiteKnightQSideStartingPos->second.second; 

		chessImageBMP.drawPieceOnBoard(whiteKnightPixelData, x, y);

	}


	const char* filename = "chessboard.bmp";
	chessImageBMP.writeImageFile(filename); 


	system(filename); 

}

