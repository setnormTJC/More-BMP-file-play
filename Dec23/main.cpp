// Dec23.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include"ImageBMP.h"
#include<thread> 


auto demoSomeBMPImagePlay()
{
	ImageBMP outputImageBMP{ 200, 300, Color{0x00'22'33'44}, Color{0x55'66'77'88} };
	outputImageBMP.drawRectangleOutline(25, 50, 75, 100, Color{ 0xFF'FF'FF'00 });
	outputImageBMP.writeImageFile("out.bmp");

	ImageBMP inputImageBMP{};
	inputImageBMP.readImageBMP("out.bmp");

	inputImageBMP.writeImageFile("outCopy.bmp");
}

auto getWhiteKnightImage()
{
	ImageBMP whiteKnightBMP{};
	//whiteKnightBMP.readImageBMP("whiteKnight.bmp"); 
	whiteKnightBMP.readImageBMP("whiteKnightCopy.bmp");


	auto whiteKnightPixelData = whiteKnightBMP.pixelData.pixelMatrix;

	return whiteKnightPixelData;
}


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




	//std::cin.get(); 

	auto chessImageBMP = getEmptyChessBoardImage(); 
	
	chessImageBMP.generatePositionsToImageCoordinatesMap(); 




	auto A8Pos = chessImageBMP.positionsToImageCoordinates.find("A8");
	if (A8Pos != chessImageBMP.positionsToImageCoordinates.end())
	{
		int x = A8Pos->second.first;
		int y = A8Pos->second.second; 

		chessImageBMP.drawA(x, y, Color{ ColorEnum::Blue });
	}














	std::cin.get(); 
	
	auto whiteKnightPixelData = getWhiteKnightImage(); 

	chessImageBMP.drawPieceOnBoard(whiteKnightPixelData, 50, 50);


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

