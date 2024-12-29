#include"ChessImageBMP.h"

ChessImageBMP::ChessImageBMP()
	:ImageBMP(boardDimension, boardDimension, ColorEnum::BoardBorder)
{

	drawEmptyChessBoard(); 

	generatePositionsToImageCoordinatesMap();


	pieceNames = mapPiecesToImages(); 

	//get INITIAL positions of all pieces 
	piecesToPositions = getPiecesToInitialPositions(pieceNames);

	//switch keys and values in the pieceToPosition map (may not be needed)
	//initialPositionsToPieceNames = switchMapKeysAndValues(pieceNamesToPositions);


	drawPieces(); 

	
}

void ChessImageBMP::drawEmptyChessBoard()
{

	for (int col = 0; col < 8; ++col)
	{
		for (int row = 0; row < 8; ++row)
		{
			ColorEnum squareColor = ((col + row) % 2 == 0) ? ColorEnum::LightSquareColor : ColorEnum::DarkSquareColor;

			fillRectangleWithColor
			(
				BORDER_SIZE + SQUARE_WIDTH * col + 10, //NOTE: the 10 here "works" for boardDimension = 720 
				BORDER_SIZE + SQUARE_WIDTH * row + 10,
				SQUARE_WIDTH,
				SQUARE_WIDTH,
				squareColor
			);
		}
	}
	setPixelToColor_withThickness(200, 100, ColorEnum::Green, 5); //thin-ish green pixels at bottom right-ish

	setPixelToColor_withThickness(100, 400, ColorEnum::Magenta, 10); //thicker magenta pixels at left, top-ish

	drawA(35, 10, ColorEnum::Black);
	drawB(120, 10, ColorEnum::Black);

	drawLetters();
	drawNumbers();

	/*Note: darkSquareColor ignored for now by drawRectangleOutline function def.*/
	//drawRectangleOutline(0 + BORDER_SIZE, 0 + BORDER_SIZE,
	//	boardDimension / 8, boardDimension / 8, Color{ 0xFF'00'00'00 });

}

void ChessImageBMP::drawA(unsigned int x, unsigned int y, const Color& color)
{

	const int ROWS_FOR_A = 16;
	const int COLS_FOR_A = 16;

	vector<vector<char>> matrixForA =
	{
		{' ',' ',' ',' ',' ',' ',' ','A','A',' ',' ',' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' ',' ','A',' ',' ','A',' ',' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' ','A',' ',' ',' ',' ','A',' ',' ',' ',' ',' '},
		{' ',' ',' ',' ','A',' ',' ',' ',' ',' ',' ','A',' ',' ',' ',' '},
		{' ',' ',' ','A',' ',' ',' ',' ',' ',' ',' ',' ','A',' ',' ',' '},
		{' ',' ','A',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','A',' ',' '},
		{' ','A',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','A',' '},
		{'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A'},
		{'A',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','A'},
		{'A',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','A'},
		{'A',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','A'},
		{'A',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','A'},
		{'A',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','A'},
		{'A',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','A'},
		{'A',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','A'},
		{'A',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','A'}
	};

	auto rotatedA = rotateMatrixClockwise(matrixForA, ROWS_FOR_A, COLS_FOR_A);


	for (int row = 0; row < COLS_FOR_A; ++row)
	{
		for (int col = 0; col < ROWS_FOR_A; ++col)
		{
			if (rotatedA[row][col] != ' ')
			{
				setPixelToColor_withThickness(x + row, y + col, color, 1);
			}
		}
	}

}

void ChessImageBMP::drawB(unsigned int x, unsigned int y, const Color& color)
{
	const int ROWS_FOR_B = 16;
	const int COLS_FOR_B = 16;

	vector<vector<char>> matrixForB =
	{
		{'B','B','B','B','B','B','B','B',' ',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B','B','B','B','B','B','B','B',' ',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B',' ',' ',' ',' ',' ',' ',' ','B',' ',' ',' ',' ',' ',' ',' '},
		{'B','B','B','B','B','B','B','B',' ',' ',' ',' ',' ',' ',' ',' '}
	};

	auto rotatedB = rotateMatrixClockwise(matrixForB, ROWS_FOR_B, COLS_FOR_B);

	for (int row = 0; row < COLS_FOR_B; ++row)
	{
		for (int col = 0; col < ROWS_FOR_B; ++col)
		{
			if (rotatedB[row][col] != ' ')
			{
				setPixelToColor_withThickness(x + row, y + col, color, 1);
			}
		}
	}
}

/*NOTE: this method is assuming magic number 16 for the number of rows and columns in letter bitmap*/
void ChessImageBMP::drawLetters()
{
	auto mapOfPixelLetters = makeMapOfPixelLetters();

	int i = 0;
	for (auto& pair : mapOfPixelLetters)
	{
		auto rotatedLetter = rotateMatrixClockwise(pair.second, 16, 16);

		for (int row = 0; row < 16; ++row)
		{
			for (int col = 0; col < 16; ++col)
			{
				if (rotatedLetter[row][col] != ' ')
				{
					setPixelToColor_withThickness(205 + 85 * i + row, 10 + col, ColorEnum::Black, 1);
				}
			}
		}

		i++; //using this to "move along the files" 
	}
}

/*NOTE: this method is assuming magic number 16 for the number of rows and columns in letter bitmap*/
void ChessImageBMP::drawNumbers()
{
	auto mapOfPixelNumbers = makeMapOfPixelNumbers();

	int i = 0;
	for (auto& pair : mapOfPixelNumbers)
	{
		auto rotatedNumber = rotateIntMatrixClockwise(pair.second, 16, 16);
		for (int row = 0; row < 16; ++row)
		{
			for (int col = 0; col < 16; ++col)
			{
				if (rotatedNumber[row][col] == pair.first) //different approach here (being goofy for fun) 
				{
					setPixelToColor_withThickness(20 + row, 35 + 85 * i + col, ColorEnum::Black, 1);
				}
			}
		}
		i++;
	}


}


vector<string> ChessImageBMP::mapPiecesToImages()
{
	//first get white piece names: 
	string pieceColor = "white";
	auto whitePieceNames = getPieceNames(pieceColor); 

	//now get white piece images:
	string relativeWhiteFolderName = "\\resources\\pieceImages\\white";
	auto whitePieceImages = getAllImagesInFolder(relativeWhiteFolderName);

	//now insert white piece names and images into map: 
	for (int i = 0; i < whitePieceNames.size(); ++i)
	{
		piecesToImages.insert({whitePieceNames.at(i), whitePieceImages.at(i) });
	}

	//now the same for black piece images:
	pieceColor = "black"; 
	auto blackPieceNames = getPieceNames(pieceColor);
	string relativeBlackFolderName = "\\resources\\pieceImages\\black";
	auto blackPieceImages = getAllImagesInFolder(relativeBlackFolderName);
	for (int i = 0; i < blackPieceNames.size(); ++i)
	{
		piecesToImages.insert({ blackPieceNames.at(i), blackPieceImages.at(i) });
	}


	//also return all pieceNames: 
	vector<string> pieceNames = blackPieceNames; //black first -preserves alphabetical order
	
	for (int i = 0; i < whitePieceNames.size(); ++i)
	{
		pieceNames.push_back(whitePieceNames.at(i)); 
	}

	return pieceNames; 
}

void ChessImageBMP::drawPieceOnBoard(const vector<vector<Color>>& piecePixelMatrix, unsigned int x, unsigned int y)
{
	swap(x, y);

	for (unsigned int row = 0; row < piecePixelMatrix.size(); ++row)
	{
		vector<Color> currentRow = piecePixelMatrix.at(row);
		for (unsigned int col = 0; col < currentRow.size(); ++col)
		{
			auto difference = abs((int)(currentRow.at(col).bgra - (unsigned int)ColorEnum::RedBgrd));

			auto threshold = 0x00'00'00'FF;

			if (difference > threshold) //bgrd color of piece - do not "overwrite" the board's square color 
				//if (currentRow.at(col).bgra < (unsigned int)ColorEnum::WKnightBgrdColor) //bgrd color of piece - do not "overwrite" the board's square color 
			{
				pixelData.pixelMatrix.at(row + x).at(col + y) = currentRow.at(col);
			}

			//else leave it be
		}
	}
}


void ChessImageBMP::generatePositionsToImageCoordinatesMap()
{

	auto positions = getChessPositions(); 

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			positionsToImageCoordinates.insert
			(
				{
					positions[i][j],
					std::pair<int, int>
						{SQUARE_WIDTH / 2 + SQUARE_WIDTH * i,
						SQUARE_WIDTH / 2 + SQUARE_WIDTH * j}
				}
			);
		}
		//cout << "\n";
	}


}

void ChessImageBMP::drawPieces()
{

	for (const auto& currentPieceName : pieceNames) //for all pieces NOT YET TAKEN (reduce waste)
	{
		ImageBMP currentPieceImage;
		string currentPosition;
		int currentX; 
		int currentY; 

		//find corrsponding image in map: 
		auto imageIterator = piecesToImages.find(currentPieceName);
		if (imageIterator != piecesToImages.end())
		{
			currentPieceImage = imageIterator->second; 
		}

		else 
		{ 
			cout << "image iterator not found\n"; 
			__debugbreak();
		}

		//find chess position of pieceName in map: 
		auto positionIterator = piecesToPositions.find(currentPieceName); 
		if (positionIterator != piecesToPositions.end())
		{
			currentPosition = positionIterator->second; 
		}
		else
		{
			cout << "position iterator not found\n";
			__debugbreak();
		}

		//find coordinate in map: 
		auto coordinateIterator = positionsToImageCoordinates.find(currentPosition); 
		if (coordinateIterator != positionsToImageCoordinates.end())
		{
			currentX = coordinateIterator->second.first; 
			currentY = coordinateIterator->second.second; 
		}

		else
		{
			cout << "coordinate iterator not found\n";
			__debugbreak();
		}

		//draw: 
		drawPieceOnBoard(currentPieceImage.pixelData.pixelMatrix, currentX, currentY);

	}

}

void ChessImageBMP::fillPositionWithColor(const string& position)
{
	auto positionAsCharAndInt = convertStringChessPositionToCharAndInt(position); 
	int col = positionAsCharAndInt.first - 65 ; //char 'A' has ASCII value 65
	int row = positionAsCharAndInt.second - 1; //0-based counting 

	ColorEnum squareColor = ((col + row) % 2 == 0) ? ColorEnum::LightSquareColor : ColorEnum::DarkSquareColor;

	fillRectangleWithColor
	(
		BORDER_SIZE + SQUARE_WIDTH * col + 10, //NOTE: the 10 here "works" for boardDimension = 720 
		BORDER_SIZE + SQUARE_WIDTH * row + 10,
		SQUARE_WIDTH,
		SQUARE_WIDTH,
		squareColor
	);
}


