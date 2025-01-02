#pragma once

#include"ImageBMP.h"


/*Is a CHILD of ImageBMP - has additional methods for drawing a chessboard and its pieces*/
class ChessImageBMP : public ImageBMP
{
	static const unsigned int boardDimension = 720;
	//be wary - not using static const here will pass value of 0 to parent (ImageBMP) constructor

	const int BORDER_SIZE = boardDimension / 30; //512 / 30 ~=  15
	const int SQUARE_WIDTH = boardDimension / 9; //closer to 8...


	/*Ex: A1 is at 0,0; G8 is at 720, 720*/
	map<string, pair<int, int> > positionsToImageCoordinates{};

	/*This will be occupying significant space in memory*/
	map<string, ImageBMP> piecesToImages;


	/*Anticipate this function only being called by `drawPieces`*/
	void drawPieceOnBoard(const vector<vector<Color>>& piecePixelMatrix, unsigned int x, unsigned int y);

	void generatePositionsToImageCoordinatesMap();
public:


	ChessImageBMP();
	//ChessImageBMP(bool isPie);
	void drawEmptyChessBoard();

	void drawA(unsigned int x, unsigned int y, const Color& color);
	void drawB(unsigned int x, unsigned int y, const Color& color);
	/*for labeling files on chessboard*/
	void drawLetters();

	/*for labeling ranks on chessboard*/
	void drawNumbers();




	/*Uses maps of pieceNames to images and pieceNames to coordinates - NOTE that neither of the maps is ALTERED by this method*/
	void drawPieces(); 

	void fillPositionWithColor(const string& position); 


	/*NOTE: this method also returns vector<string> pieceNames*/
	vector<string> mapPiecesToImages_andReturnPieces(); 

	static vector<string> pieces;
	//vector<string> positions = getChessPositions(); 

	map <string, string> piecesToPositions;
	//map <string, string> initialPositionsToPieceNames; 

	map<pair<int, int>, string> imageCoordinatesToPositions{};

	pair<char, int> convertImageCoordinatesToPosition(int x, int y);

};
