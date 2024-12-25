#pragma once

#include<iostream> 
#include<vector>
#include<fstream> 
#include <vector>
#include<string> 
#include<cassert>
#include<array>
#include<algorithm>
#include<map> 
#include<iomanip> 

using std::cout, std::ifstream, std::ofstream, std::vector, std::string, std::ios, std::ostream, std::array; 
using std::swap, std::map, std::pair, std::setw, std::left, std::to_string; 


class FileHeader
{
	/*will make PRIVATE all of the bmp fields that (probably) never change
	- ex: 1st two bytes should always be B and M*/

	array<char, 2> filetype = { 'B', 'M' }; //indices 0 and 1  
	//const char* filetype = "BM"; //indices 0 and 1  -> problematic type!

	unsigned int reserved1And2 = 0x00'00'00'00; //(unclear meaning) indices 6 - 9 
	unsigned int indexOfPixelData = 0x00'00'00'36; //indices 10 - 13

public:
	unsigned int fileSize; //indices 2 - 5 (size is in bytes)
	//should perhaps be set by the ImageBMP class //(since ImageBMP will have info on pixel data and infoheader) 

	FileHeader() = default;

	/*NOTE that the write order is CRITICAL if writing to a BMP file!*/
	friend ostream& operator << (ostream& os, const FileHeader& fileHeader);

	friend class ImageBMP; 

};

class InfoHeader
{
	unsigned int infoHeaderSize = 0x00'00'00'28; //indices 14 - 17, in bytes
	short planes = 0x00'01; //indices 26 - 27 ["always" 1 (meaning unclear)] 
	
	short bitsPerPixel = 0x00'20; //CAREFUL! not always 32! -> indices 28 - 29 (32 bits - 24 for G,B,R, and 8 for Alpha)
	unsigned int compressionMethod = 0x00'00'00'00; //indices 30 - 33
	unsigned int sizeOfPixelData;// = imageWidth * imageHeight * (bitsPerPixel / 8); //indices 34 - 37
	vector<int> remainingHeaderFields =
	{
		0x00'00'00'00,//xPixelsPerMeter
		0x00'00'00'00,//yPixelsPerMeter
		0x00'00'00'00,//color indexing thing
		0x00'00'00'00//"important" color count 
	};

public:
	unsigned int imageWidth = 0; //indices 18 - 21
	unsigned int imageHeight = 0; //indices 22 - 25

	InfoHeader() = default;

	unsigned int getInfoHeaderSize() const;
	unsigned int getSizeOfPixelData() const;

	friend class ImageBMP; 
};

/*NOTE: little-endian BGRA order is used here*/
enum class ColorEnum : unsigned int
{
	//A, R, G, B
	Black				=	0xFF'00'00'00,
	White				=	0xFF'FF'FF'FF,

	Red					=	0xFF'00'00'FF,
	Green				=	0xFF'00'FF'00,
	Blue				=	0xFF'FF'00'00,

	Yellow				=	0xFF'00'FF'FF,
	Cyan				=	0xFF'FF'FF'00,
	Magenta				=	0xFF'FF'00'FF,

	//chessboard-specific colors: 
	DarkSquareColor		=	0xFF'BA'61'34,
	LightSquareColor	=	0xFF'EF'D7'B5,
	BoardBorder			=	0xFF'6C'1E'1C,

	//using some images from this page: https://commons.wikimedia.org/wiki/Category:Chess_bitmap_pieces
	WKnightBgrdColor	=	0xFF'FF'FF'CB,

	RedBgrd				=	0xFF'ED'1C'24
	//CopyWKnightBgrdColor = 0x00'00'00'00

};

struct Color
{
	//should be unsigned because 1) no "negative" colors and 2) having alpha = 255 (FF) is desirable
	unsigned int bgra = 0x00'00'00'00;

	Color() = default;
	Color(unsigned int bgra);
	Color(unsigned int b, unsigned int g, unsigned int r); // New constructor for 24-bit color
	Color(unsigned int b, unsigned int g, unsigned int r, unsigned int a);
	Color(ColorEnum colorEnum);

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
	/*made private, I suppose, to prevent overwhelming client with large number of functions*/
	void readFileHeaderFromFile(ifstream& fin);
	void readInfoHeaderFromFile(ifstream& fin);
	void readPixelDataFromFile(ifstream& fin);
public: 
	FileHeader fileHeader;
	InfoHeader infoHeader; 
	PixelData pixelData; 

	/*likely to be complicated*/
	void writeImageFile(string filename);

	/*Using the default constructor anticipates using an INPUT file to get the data of the image*/
	ImageBMP() = default;

	/*Determine filesize here (and set fileHeader.filesize)? */
	ImageBMP(unsigned int imageWidth, unsigned int imageHeight, const Color& fillColor, const Color& middleDotColor);

	ImageBMP(unsigned int imageWidth, unsigned int imageHeight, const Color& fillColor);


	void readImageBMP(string inputFilename);

	void drawRectangleOutline(unsigned int x0, unsigned int y0, 
		unsigned int rectangleWidth, unsigned int rectangleHeight, const Color& color);

	void fillRectangleWithColor(unsigned int x0, unsigned int y0,
		unsigned int rectangleWidth, unsigned int rectangleHeight, const Color& color);

	void setPixelToColor_withThickness(unsigned int x, unsigned int y, const Color& color, unsigned int thickness);



};


/*Is a CHILD of ImageBMP - has additional methods for drawing a chessboard and its pieces*/
class ChessImageBMP : public ImageBMP
{
	static const unsigned int boardDimension = 720; 
	//be wary - not using static const here will pass value of 0 to parent (ImageBMP) constructor

	const int BORDER_SIZE = boardDimension / 30; //512 / 30 ~=  15
	const int SQUARE_WIDTH = boardDimension / 9; //closer to 8...

public: 
	/*Ex: A1 is at 0,0; G8 is at 720, 720*/
	map<string, pair<int, int> > positionsToImageCoordinates{};

	ChessImageBMP(); 
	void drawEmptyChessBoard(); 

	void drawA(unsigned int x, unsigned int y, const Color& color);
	void drawB(unsigned int x, unsigned int y, const Color& color);
	/*for labeling files on chessboard*/
	void drawLetters(); 

	/*for labeling ranks on chessboard*/
	void drawNumbers();

	void drawAndFillAnIrregularShape(); 

	void drawPieceOnBoard(const vector<vector<Color>>& piecePixelMatrix, unsigned int x, unsigned int y);

	void generatePositionsToImageCoordinatesMap();
};




#pragma region auxillary functions 
//"auxillary" method: 
vector<vector<char>> rotateMatrixClockwise
(vector<vector<char>>& originalMatrix, int originalNumberOfRows, int originalNumberOfCols);

vector<vector<int>> rotateIntMatrixClockwise(vector<vector<int>>& originalMatrix, int originalNumberOfRows, int originalNumberOfCols);


//for pixelated letters (for labeling chessboard A1, C3, etc.)
map<char, vector<vector<char>>> makeMapOfPixelLetters();

map<int, vector<vector<int>>> makeMapOfPixelNumbers();

#pragma endregion