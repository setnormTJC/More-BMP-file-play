// Dec23.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include"ImageBMP.h"
#include<thread> 

int main()
{
	//ImageBMP outputImageBMP{ 200, 300, Color{0x00'22'33'44}, Color{0x55'66'77'88} };
	//outputImageBMP.drawRectangleOutline(25, 50, 75, 100, Color{0xFF'FF'FF'00});
	//outputImageBMP.writeImageFile("out.bmp"); 
	//
	//ImageBMP inputImageBMP{}; 
	//inputImageBMP.readImageBMP("out.bmp"); 

	//inputImageBMP.writeImageFile("outCopy.bmp");









	ChessImageBMP chessImageBMP{}; 

	chessImageBMP.drawEmptyChessBoard(); 


	chessImageBMP.setPixelToColor_withThickness(200, 100, ColorEnum::Green, 5); //thin-ish green pixels at bottom right-ish

	chessImageBMP.setPixelToColor_withThickness(100, 400, ColorEnum::Magenta, 10); //thicker magenta pixels at left, top-ish

	chessImageBMP.drawA(35, 10, ColorEnum::Black);
	chessImageBMP.drawB(120, 10, ColorEnum::Black);

	chessImageBMP.drawLetters(); 
	chessImageBMP.drawNumbers(); 

	//chessImageBMP.drawAndFillAnIrregularShape(); 





	ImageBMP whiteKnightBMP{};
	//whiteKnightBMP.readImageBMP("whiteKnight.bmp"); 
	whiteKnightBMP.readImageBMP("whiteKnightCopy.bmp");


	auto whiteKnightPixelData = whiteKnightBMP.pixelData.pixelMatrix;

	chessImageBMP.drawPieceOnBoard(whiteKnightPixelData, 50, 50);



	//whiteKnightBMP.writeImageFile("outputWhiteKnight.bmp");

	//ImageBMP outputWhiteKnight{}; 
	//outputWhiteKnight.writeImageFile("")



	const char* filename = "chessboard.bmp";
	chessImageBMP.writeImageFile(filename); 


	chessImageBMP.drawPieceOnBoard(whiteKnightPixelData, 150, 150);
	const char* filename2 = "chessboard2.bmp";
	chessImageBMP.writeImageFile(filename2);

	system(filename); 

	std::this_thread::sleep_for(std::chrono::seconds(2));


	system(filename2);









	// Close the file using taskkill
	//system("taskkill /IM Microsoft.Photos.exe /F");
}

