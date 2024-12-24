#pragma once

#include<iostream> 
#include<vector>
#include<fstream> 
#include <vector>
#include<string> 

using std::cout, std::ifstream, std::ofstream, std::vector, std::string, std::ios, std::ostream;



class FileHeader
{
	/*will make PRIVATE all of the bmp fields that (probably) never change
	- ex: 1st two bytes should always be B and M*/

	const char* filetype = "BM"; //indices 0 and 1  
	int reserved1And2 = 0x00'00'00'00; //(unclear meaning) indices 6 - 9 
	int indexOfPixelData = 0x00'00'00'36; //indices 10 - 13

public:
	int fileSize; //indices 2 - 5 (size is in bytes)
	//should perhaps be set by the ImageBMP class //(since ImageBMP will have info on pixel data and infoheader) 

	FileHeader() = default;

	/*NOTE that the write order is CRITICAL if writing to a BMP file!*/
	friend ostream& operator << (ostream& os, const FileHeader& fileHeader);

	friend class ImageBMP; 

};

class InfoHeader
{
	int infoHeaderSize = 0x00'00'00'28; //indices 14 - 17, in bytes
	short planes = 0x00'01; //indices 26 - 27 ["always" 1 (meaning unclear)] 
	short bitsPerPixel = 0x00'20; //indices 28 - 29 (32 bits - 24 for G,B,R, and 8 for Alpha)
	int compressionMethod = 0x00'00'00'00; //indices 30 - 33
	int sizeOfPixelData;// = imageWidth * imageHeight * (bitsPerPixel / 8); //indices 34 - 37
	vector<int> remainingHeaderFields =
	{
		0x00'00'00'00,//xPixelsPerMeter
		0x00'00'00'00,//yPixelsPerMeter
		0x00'00'00'00,//color indexing thing
		0x00'00'00'00//"important" color count 
	};

public:
	int imageWidth = 0; //indices 18 - 21
	int imageHeight = 0; //indices 22 - 25

	InfoHeader() = default;

	int getInfoHeaderSize() const;
	int getSizeOfPixelData() const;

	friend class ImageBMP; 
};

struct Color
{
	//should be unsigned because 1) no "negative" colors and 2) having alpha = 255 (FF) is desirable
	unsigned int bgra = 0x00'00'00'00;

	Color() = default;
	Color(unsigned int bgra);

	//do bitshifting here
	auto alpha();

	unsigned int convertToUnsignedInt();
};

class PixelData
{
public:
	vector<vector<Color>> pixelMatrix;

	PixelData() = default;
};

class ImageBMP
{
public: 
	FileHeader fileHeader;
	InfoHeader infoHeader; 
	PixelData pixelData; 

	/*likely to be complicated*/
	void writeImageFile(string filename);

	ImageBMP() = default; 
	/*Determine filesize here (and set fileHeader.filesize)? */
	ImageBMP(int imageWidth, int imageHeight, const Color& fillColor, const Color& middleDotColor);


	void fillRectangleWithColor(int x0, int y0, int xf, int yf, const Color& color);

	void setPixelToColor(int x, int y, const Color& color);
};



