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

