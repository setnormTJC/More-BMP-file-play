#include "ChessImageBMP.h"


ChessImageBMP::ChessImageBMP()
	:ImageBMP(boardDimension, boardDimension, ColorEnum::BoardBorder)
{
	drawEmptyChessBoard(); 

	generatePositionsToImageCoordinatesMap(); 
	
	imageCoordinatesToPositions = 
		sharedFunctions::switchMapKeysAndValues<ChessPosition, pair<int, int>, ChessPositionHash, PairHash>
		(positionsToImageCoordinates);
 
	pieces = mapPiecesToImages_andReturnPieces(); 


	piecesToPositions = sharedFunctions::getPiecesToInitialPositions(pieces); 

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

	drawA(35, 10, ColorEnum::Black); //not in drawLetters because this was my first attempt 
	//- no big deal to leave as is, I suppose
	drawB(120, 10, ColorEnum::Black);

	drawLetters();
	drawNumbers();

}


void ChessImageBMP::generatePositionsToImageCoordinatesMap()
{
	auto chessPositions = sharedFunctions::getChessPositions(); 

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			positionsToImageCoordinates.insert
			(
				{
					chessPositions[i][j],
					std::pair<int, int>
						{SQUARE_WIDTH / 2 + SQUARE_WIDTH * i,
						SQUARE_WIDTH / 2 + SQUARE_WIDTH * j}
				}
			);
		}
	}
}

vector<string> ChessImageBMP::mapPiecesToImages_andReturnPieces()
{
	//first get names of white pieces: 
	string color = "white";
	vector<string> whitePieceNames = sharedFunctions::getPieceNames(color); 

	//now get white piece IMAGES
	string relativeWhiteFolderName = "\\resources\\pieceImages\\white";
	auto whitePieceImages = getAllImagesInFolder(relativeWhiteFolderName);
	for (int i = 0; i < whitePieceNames.size(); ++i)
	{
		piecesToImages.insert({ whitePieceNames.at(i), whitePieceImages.at(i) });
	}

	//now the same for black piece images:
	color = "black";
	auto blackPieceNames = sharedFunctions::getPieceNames(color);
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


ChessPosition ChessImageBMP::convertImageCoordinatesToPosition(int x, int y)
{

	/*NodeJS is returning "flipped" y value (y increases from top to bottom), so "convert" it*/
	y = boardDimension - y;

	//imageCoordinates in imageCoordinatesToPositions are at bottom left of square on chessboard 
	for (auto& currentPair : imageCoordinatesToPositions)
	{
		if ((x > currentPair.first.first && x < currentPair.first.first + SQUARE_WIDTH))
		{
			if (y > currentPair.first.second && y < currentPair.first.second + SQUARE_WIDTH)
			{
				char currentFile = currentPair.second.file;
				int currentRank = currentPair.second.rank;

				ChessPosition imageCoordinatesAsChessPosition{ currentFile, currentRank };
				//imageCoordinatesAsChessPosition.second = currentRank;

				return imageCoordinatesAsChessPosition;
			}
		}
	}

	return ChessPosition{}; //empty position if the if statement is never true (haven't seen this happen yet ...)
}

void ChessImageBMP::fillPositionWithColor(const ChessPosition& position)
{
	int col = position.file - 65; //char 'A' has ASCII value 65
	int row = position.rank - 1; //0-based counting 

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


void ChessImageBMP::drawPieces()
{

	for (const auto& currentPieceName : pieces) //for all pieces NOT YET TAKEN (reduce waste)
	{
		ImageBMP currentPieceImage;
		ChessPosition currentPosition;
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

