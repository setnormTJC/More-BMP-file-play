#pragma once
#include"ImageBMP.h"

auto demoSomeBMPImagePlay()
{
	ImageBMP outputImageBMP{ 200, 300, Color{0x00'22'33'44}, Color{0x55'66'77'88} };
	outputImageBMP.drawRectangleOutline(25, 50, 75, 100, Color{ 0xFF'FF'FF'00 });
	outputImageBMP.writeImageFile("out.bmp");

	ImageBMP inputImageBMP{};
	inputImageBMP.readImageBMP("out.bmp");

	inputImageBMP.writeImageFile("outCopy.bmp");
}

auto demoImageScaling()
{
	ImageBMP originalImage{ 3, 3, ColorEnum::Red };
	//testImageToScale.setPixelToColor_withThickness(0, 0)
	const char* originalFileName = "testImageToScale.bmp";

	originalImage.setPixelToColor_withThickness(0, 0, ColorEnum::Black, 1);
	originalImage.setPixelToColor_withThickness(1, 0, ColorEnum::White, 1);
	originalImage.setPixelToColor_withThickness(2, 0, ColorEnum::Red, 1);

	originalImage.setPixelToColor_withThickness(0, 1, ColorEnum::Green, 1);
	originalImage.setPixelToColor_withThickness(1, 1, ColorEnum::Blue, 1);
	originalImage.setPixelToColor_withThickness(2, 1, ColorEnum::Yellow, 1);

	originalImage.setPixelToColor_withThickness(0, 2, ColorEnum::Cyan, 1);
	originalImage.setPixelToColor_withThickness(1, 2, ColorEnum::Magenta, 1);
	originalImage.setPixelToColor_withThickness(2, 2, ColorEnum::DarkSquareColor, 1);

	//write the original before scaling (would perhaps like to directly compare hexdumps of orignal to scaled) 
	originalImage.writeImageFile(originalFileName);


	//now scale and write the new, scaled image to a different filename: 
	originalImage.doublescaleImageBMP();

	ImageBMP scaledImage = originalImage;  //not sure how well this will work (no pointer member vars, so maybe copy assignment is okay) 
	const char* scaledFileName = "scaledImage.bmp";
	scaledImage.writeImageFile(scaledFileName);

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

auto getWhiteKnightImage()
{
	ImageBMP whiteKnightBMP{};
	//whiteKnightBMP.readImageBMP("whiteKnight.bmp"); 
	whiteKnightBMP.readImageBMP("whiteKnightQSide.bmp");
	//whiteKnightCopy has reddish bgrd that gets "ignored" by the drawPiece function in ImageBMP.cpp

	return whiteKnightBMP;
}

auto demoWhiteKnightDoubleScaling()
{
	auto whiteKnightImage = getWhiteKnightImage();


	whiteKnightImage.doublescaleImageBMP();
	const char* doubledWhiteKnightFilename = "doubledWhiteKnight.bmp";
	whiteKnightImage.writeImageFile(doubledWhiteKnightFilename);

}

auto demoVariousChessImageBMPTests()
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

auto demoPuttingWhiteBishopOnBoard()
{
	auto chessImageBMP = getEmptyChessBoardImage();
	ImageBMP whiteBishopBMP{};
	whiteBishopBMP.readImageBMP("resources/pieceImages/whites/whiteBishopQSide.bmp");

	chessImageBMP.generatePositionsToImageCoordinatesMap();
	auto c1Pos = chessImageBMP.positionsToImageCoordinates.find("C1");


	chessImageBMP.drawPieceOnBoard(whiteBishopBMP.pixelData.pixelMatrix, c1Pos->second.first, c1Pos->second.second);


	chessImageBMP.writeImageFile("whiteBishopQSideOnBoard.bmp");

}