#include "ImageBMP.h"

void ImageBMP::writeImageFile(string filename)
{
	ofstream fout{ filename, ios::binary };

	//first comes the 14-byte file header: 
	fout.write(reinterpret_cast<const char*>(fileHeader.filetype.data()), 2); //no sizeof here, since filetype is a pointer
	fout.write(reinterpret_cast<const char*>(&fileHeader.fileSize), sizeof(fileHeader.fileSize));
	fout.write(reinterpret_cast<const char*>(&fileHeader.reserved1And2), sizeof(fileHeader.reserved1And2));
	fout.write(reinterpret_cast<const char*>(&fileHeader.indexOfPixelData), sizeof(fileHeader.indexOfPixelData));
	
	//next, the 40-byte info header: 
	fout.write(reinterpret_cast<const char*>(&infoHeader.infoHeaderSize), sizeof(infoHeader.infoHeaderSize)); 
	fout.write(reinterpret_cast<const char*>(&infoHeader.imageWidth), sizeof(infoHeader.imageWidth));
	fout.write(reinterpret_cast<const char*>(&infoHeader.imageHeight), sizeof(infoHeader.imageHeight));
	fout.write(reinterpret_cast<const char*>(&infoHeader.planes), sizeof(infoHeader.planes));
	fout.write(reinterpret_cast<const char*>(&infoHeader.bitsPerPixel), sizeof(infoHeader.bitsPerPixel)); 
	fout.write(reinterpret_cast<const char*>(&infoHeader.compressionMethod), sizeof(infoHeader.compressionMethod)); 
	fout.write(reinterpret_cast<const char*>(&infoHeader.sizeOfPixelData), sizeof(infoHeader.sizeOfPixelData));
	
	for (const auto& remainingHeaderField : infoHeader.remainingHeaderFields)
	{
		fout.write(reinterpret_cast<const char*>(&remainingHeaderField), sizeof(remainingHeaderField)); 
	}

	// Calculate the number of bytes per pixel based on the bit depth
	int bytesPerPixel = infoHeader.bitsPerPixel / 8;

	// Calculate the padding bytes per row (each row is padded to a multiple of 4 bytes)
	int paddingBytes = (4 - (infoHeader.imageWidth * bytesPerPixel) % 4) % 4;

	//now the pixel data: 
	for (unsigned int row = 0; row < infoHeader.imageHeight; ++row)
	{
		for (unsigned int col = 0; col < infoHeader.imageWidth; ++col)
		{
			if (infoHeader.bitsPerPixel == 32)
			{


				unsigned int colorOfCurrentPixel = pixelData.pixelMatrix.at(row).at(col).convertToUnsignedInt();

				fout.write(reinterpret_cast<const char*>(&colorOfCurrentPixel), sizeof(colorOfCurrentPixel));
			}

			else if (infoHeader.bitsPerPixel == 24)
			{
				unsigned int colorOfCurrentPixel = pixelData.pixelMatrix.at(row).at(col).convertToUnsignedInt();
				char rgb[3] = {
					static_cast<char>((colorOfCurrentPixel >> 0) & 0xFF),
					static_cast<char>((colorOfCurrentPixel >> 8) & 0xFF),
					static_cast<char>((colorOfCurrentPixel >> 16) & 0xFF)
				};
				fout.write(rgb, 3);
			}

			else
			{
				cout << "Hey! Neither 32 nor 24 bits per pixel? What is this file?\n";
				std::cin.get();
			}
		}

		// Write the padding bytes at the end of each row
		char padding[3] = { 0, 0, 0 };
		fout.write(padding, paddingBytes);
	}

	fout.close(); 
}

ImageBMP::ImageBMP(unsigned int imageWidth, unsigned int imageHeight, const Color& fillColor, const Color& middleDotColor)
{
	infoHeader.imageWidth = imageWidth;
	infoHeader.imageHeight = imageHeight;
	infoHeader.sizeOfPixelData = imageWidth * imageHeight * (infoHeader.bitsPerPixel / 8);
	//NOTE: fileheader size should always be 14 (I think) 
	fileHeader.fileSize = 14 + infoHeader.getInfoHeaderSize() + infoHeader.sizeOfPixelData; 
	


	//fill pixelData with given fill color:
	for (int row = 0; row < imageHeight; ++row)
	{
		vector<Color> currentRow; 
		for (int col = 0; col < imageWidth; ++col)
		{
			currentRow.push_back(fillColor); 
		}
		pixelData.pixelMatrix.push_back(currentRow);
	}

	//add the middle dot (having different color): 
	pixelData.pixelMatrix.at(imageWidth / 2).at(imageHeight / 2) = middleDotColor; 
	
}

ImageBMP::ImageBMP(unsigned int imageWidth, unsigned int imageHeight, const Color& fillColor)
{
	infoHeader.imageWidth = imageWidth;
	infoHeader.imageHeight = imageHeight;
	infoHeader.sizeOfPixelData = imageWidth * imageHeight * (infoHeader.bitsPerPixel / 8);
	//NOTE: fileheader size should always be 14 (I think) 
	fileHeader.fileSize = 14 + infoHeader.getInfoHeaderSize() + infoHeader.sizeOfPixelData;



	//fill pixelData with given fill color:
	for (int row = 0; row < imageHeight; ++row)
	{
		vector<Color> currentRow;
		for (int col = 0; col < imageWidth; ++col)
		{
			currentRow.push_back(fillColor);
		}
		pixelData.pixelMatrix.push_back(currentRow);
	}
}

void ImageBMP::readImageBMP(string inputFilename)
{
	ifstream fin{ inputFilename, ios::binary };

	if (!fin)
	{
		cout << "File " << inputFilename << " not found.\n";
		return; 
	}

	//first read the file header info: 
	readFileHeaderFromFile(fin); 

	//now read info header: 
	readInfoHeaderFromFile(fin); 

	readPixelDataFromFile(fin); 

	//pixelData.pixelMatrix; 


	//0xb6
	//0x03'A9'B6

	fin.close(); 
}


void ImageBMP::readFileHeaderFromFile(ifstream& fin)
{
	char filetype[2];
	fin.read(filetype, 2); //NOTE: fin.GET() appends null terminator! (\0)
	fileHeader.filetype.at(0) = filetype[0];
	fileHeader.filetype.at(1) = filetype[1];

	char filesize[4];
	fin.read(filesize, 4);

	//DETAILED approach without bitshifting and bitwise OR: 
	//auto first = (unsigned char)filesize[0];
	//auto second= (unsigned char)filesize[1];
	//auto third = (unsigned char)filesize[2];
	//auto fourth = (unsigned char)filesize[3];
	//cout << first + (second * pow(2, 8)) + (third*pow(2, 16)) + (fourth*pow(2, 24)) << "\n";

	//equivalently (faster, probably): 
	fileHeader.fileSize =
		(
			(unsigned char)filesize[0] << 0 |
			(unsigned char)filesize[1] << 8 |
			(unsigned char)filesize[2] << 16 |
			(unsigned char)filesize[3] << 24
			);
	char reserved1And2[4];
	fin.read(reserved1And2, 4);

	fileHeader.reserved1And2 = (
		(unsigned char)reserved1And2[0] << 0 |
		(unsigned char)reserved1And2[1] << 8 |
		(unsigned char)reserved1And2[2] << 16 |
		(unsigned char)reserved1And2[3] << 24
		);

	char indexOfPixelData[4];
	fin.read(indexOfPixelData, 4);
	fileHeader.indexOfPixelData = (
		(unsigned char)indexOfPixelData[0] << 0 |
		(unsigned char)indexOfPixelData[1] << 8 |
		(unsigned char)indexOfPixelData[2] << 16 |
		(unsigned char)indexOfPixelData[3] << 24
		);

}

void ImageBMP::readInfoHeaderFromFile(ifstream& fin)
{
	char infoHeaderSize[4]; 
	fin.read(infoHeaderSize, 4);
	infoHeader.infoHeaderSize = 
	(
		(unsigned char)infoHeaderSize[0] << 0 |
		(unsigned char)infoHeaderSize[1] << 8 |
		(unsigned char)infoHeaderSize[2] << 16 |
		(unsigned char)infoHeaderSize[3] << 24 
	); 

	/*a "safety check" here:*/
	if (infoHeader.infoHeaderSize != 40)
	{
		cout << "Hey! Listen!\n"; 
		cout << "Info header size is not 40! - it is " << infoHeader.infoHeaderSize << "\n";
		std::cin.get(); 
	}


	char imageWidth[4]; 
	fin.read(imageWidth, 4); 
	infoHeader.imageWidth = 
	(
		(unsigned char)imageWidth[0] << 0 |
		(unsigned char)imageWidth[1] << 8 |
		(unsigned char)imageWidth[2] << 16 |
		(unsigned char)imageWidth[3] << 24
	);

	char imageHeight[4];
	fin.read(imageHeight, 4);
	infoHeader.imageHeight =
		(
			(unsigned char)imageHeight[0] << 0 |
			(unsigned char)imageHeight[1] << 8 |
			(unsigned char)imageHeight[2] << 16 |
			(unsigned char)imageHeight[3] << 24
			);

	char planes[2]; 
	fin.read(planes, 2); 
	infoHeader.planes =
		(
			(unsigned char)planes[0] << 0 |
			(unsigned char)planes[1] << 8
			);

	char bitsPerPixel[2];
	fin.read(bitsPerPixel, 2);
	infoHeader.bitsPerPixel =
		(
			(unsigned char)bitsPerPixel[0] << 0 |
			(unsigned char)bitsPerPixel[1] << 8
			);

	/*another "safety check" here:*/
	if (infoHeader.bitsPerPixel != 32)
	{
		cout << "Hey! Listen!\n";
		cout << "bitsPerPixel is not 32 (gbra)! - it is " << infoHeader.bitsPerPixel << "\n";
		//std::cin.get();
	}


	char compressionMethod[4];
	fin.read(compressionMethod, 4);
	infoHeader.compressionMethod =
		(
			(unsigned char)compressionMethod[0] << 0 |
			(unsigned char)compressionMethod[1] << 8 |
			(unsigned char)compressionMethod[2] << 16 |
			(unsigned char)compressionMethod[3] << 24
			);

	char sizeOfPixelData[4];
	fin.read(sizeOfPixelData, 4);
	infoHeader.sizeOfPixelData =
		(
			(unsigned char)sizeOfPixelData[0] << 0 |
			(unsigned char)sizeOfPixelData[1] << 8 |
			(unsigned char)sizeOfPixelData[2] << 16 |
			(unsigned char)sizeOfPixelData[3] << 24
			);


	assert(infoHeader.remainingHeaderFields.size() == 4); //useless assertion? 

	for (int i = 0; i < infoHeader.remainingHeaderFields.size(); ++i)
	{
		char remainingHeaderFields[4];
		fin.read(remainingHeaderFields, 4);

		infoHeader.remainingHeaderFields.at(i) =
			(
				(unsigned char)remainingHeaderFields[0] << 0 |
				(unsigned char)remainingHeaderFields[1] << 8 |
				(unsigned char)remainingHeaderFields[2] << 16 |
				(unsigned char)remainingHeaderFields[3] << 24 
				);
		
	}
}


/*Note: method is made ghastly long by handling 24 and 32 bit color*/
void ImageBMP::readPixelDataFromFile(ifstream& fin)
{

	if (infoHeader.bitsPerPixel == 32)
	{
		pixelData.pixelMatrix.resize(infoHeader.imageHeight,
			vector<Color>(infoHeader.imageWidth));	//CAREFUL to resize as a TWO-d array - NOT 1D!

		for (int row = 0; row < infoHeader.imageHeight; ++row)
		{
			for (int col = 0; col < infoHeader.imageWidth; ++col)
			{
				char bgra[4];
				fin.read(bgra, 4);

				if (fin.fail())
					//fin.fail gets set to true if, for example, ... the `row` counter variable goes too far
					//ex: 	for (int row = 0; row < infoHeader.imageHeight + 1; ++row)
				{
					cout << "Error: Attempted to read beyond the end of the file at row " << row << ", col " << col << ".\n";
					std::cin.get();
					return;
				}

				Color currentPixelColor
				{ (unsigned int)(unsigned char)bgra[0], (unsigned int)(unsigned char)bgra[1] ,
					(unsigned int)(unsigned char)bgra[2] , (unsigned int)(unsigned char)bgra[3] };
				if (col < infoHeader.imageWidth)
				{
					pixelData.pixelMatrix.at(row).at(col) = currentPixelColor;
				}

				else
				{
					cout << "Error: Attempted to access out-of-bounds pixel at row " << row << ", col " << col << ".\n";
					std::cin.get();
					return;
				}

			}
		}

		int lastThingInFile = fin.get(); //should be -1, I think

		//confirm that the end of the file was reached:
		if (!fin.eof())
		{
			cout << "Hey!\nListen\n EOF was not reached? Is there more pixel data? \n";
			std::cin.get();
		}
	}

	else
	{
		// Calculate the number of bytes per pixel based on the bit depth
		int bytesPerPixel = infoHeader.bitsPerPixel / 8;

		// Calculate the padding bytes per row (each row is padded to a multiple of 4 bytes)
		int paddingBytes = (4 - (infoHeader.imageWidth * bytesPerPixel) % 4) % 4;

		//cout << "not 32 bits per pixel\n";
		pixelData.pixelMatrix.resize(infoHeader.imageHeight,
			vector<Color>(infoHeader.imageWidth));	//CAREFUL to resize as a TWO-d array - NOT 1D!

		for (int row = 0; row < infoHeader.imageHeight; ++row)
		{
			for (int col = 0; col < infoHeader.imageWidth; ++col)
			{
				char bgr[3];
				fin.read(bgr, 3);

				if (fin.fail())
					//fin.fail gets set to true if, for example, ... the `row` counter variable goes too far
					//ex: 	for (int row = 0; row < infoHeader.imageHeight + 1; ++row)
				{
					cout << "Error: Attempted to read beyond the end of the file at row " << row << ", col " << col << ".\n";
					std::cin.get();
					return;
				}

				Color currentPixelColor
				{ (unsigned int)(unsigned char)bgr[0], (unsigned int)(unsigned char)bgr[1] ,
					(unsigned int)(unsigned char)bgr[2] };
				if (col < infoHeader.imageWidth)
				{
					pixelData.pixelMatrix.at(row).at(col) = currentPixelColor;
				}

				else
				{
					cout << "Error: Attempted to access out-of-bounds pixel at row " << row << ", col " << col << ".\n";
					std::cin.get();
					return;
				}

			}
			// Skip the padding bytes at the end of each row
			fin.ignore(paddingBytes);

		}

		//int counter = 0; 
		//while (!fin.eof())
		//{
			//counter++; 
			int lastThingInFile = fin.get(); //should be -1, I think
		//}

		//confirm that the end of the file was reached:
		if (!fin.eof())
		{
			cout << "Hey!\nListen\n EOF was not reached? Is there more pixel data? \n";
			std::cin.get();
		}
	}
}

/*Modifies pixelData - no change to fileHeader or infoHeader*/
void ImageBMP::drawRectangleOutline(unsigned int x0, unsigned int y0, 
	unsigned int rectangleWidth, unsigned int rectangleHeight, const Color& color)
{

	assert(x0 + rectangleWidth <= infoHeader.imageWidth);
	assert(y0 + rectangleHeight <= infoHeader.imageHeight);

	//left line 
	for (int i = x0; i < x0 + rectangleWidth; ++i)
	{
		pixelData.pixelMatrix.at(i).at(y0) = Color{ 0xFF'00'00'00 }; //Black
	}
	
	//right
	for (int i = x0; i < x0 + rectangleWidth; ++i)
	{
		pixelData.pixelMatrix.at(i).at(y0 + rectangleWidth) = Color{ 0x00'FF'00'00 }; //red
	}

	//bottom
	for (int i = y0; i < y0 + rectangleHeight; ++i)
	{
		pixelData.pixelMatrix.at(x0).at(i) = Color{ 0x00'00'00'FF };//Blue;
	}

	//top
	for (int i = y0; i < y0 + rectangleHeight; ++i)
	{
		pixelData.pixelMatrix.at(x0 + rectangleHeight).at(i) = Color{ 0x00'00'FF'00 };//green
	}

}

void ImageBMP::fillRectangleWithColor(unsigned int x0, unsigned int y0, unsigned int rectangleWidth, unsigned int rectangleHeight, const Color& color)
{
	swap(x0, y0); //stupid, but ah well -> images use image[row][col], where row is y value and col is x value

	for (int row = x0; row < x0 + rectangleWidth; ++row)
	{
		for (int col = y0; col < y0 + rectangleHeight; ++col)
		{
			pixelData.pixelMatrix.at(row).at(col) = color; 
		}
	}

}

/*NOTE: this method will be swapping x and y */
void ImageBMP::setPixelToColor_withThickness(unsigned int x, unsigned int y, const Color& color, unsigned int thickness)
{
	swap(x, y); //ridiculous thing to do ... maybe not so much - image coordinates "naturally" flip x and y

	// Ensure the center pixel is within bounds
	if (x >= infoHeader.imageWidth || y >= infoHeader.imageHeight)
	{
		cout << "Error: Center pixel out of bounds.\n";
		return;
	}

	//center pixel 
	pixelData.pixelMatrix[x][y] = color; 
	
	//neighbors within thickness:
	for (int row = -1*(int)thickness; row <= (int)thickness; ++row)
	{
		for (int col = -1 *(int) thickness; col < (int) thickness; ++col)
		{
			int newX = x + col; 
			int newY = y + row; 

			// Ensure the new coordinates are within bounds
			if (newX >= 0 
				&& newX < static_cast<int>(infoHeader.imageWidth) 
				&& newY >= 0 
				&& newY < static_cast<int>(infoHeader.imageHeight))
			{
				pixelData.pixelMatrix[newX][newY] = color;
			}

		}
	}
}


unsigned int InfoHeader::getInfoHeaderSize() const
{
	return infoHeaderSize;
}

unsigned int InfoHeader::getSizeOfPixelData() const
{
	//return sizeOfPixelData;
	return imageWidth * imageHeight * (bitsPerPixel / 8);
}

Color::Color(unsigned int bgra)
	:bgra(bgra)
{
}

Color::Color(unsigned int b, unsigned int g, unsigned int r)
{
	bgra = (b << 0) | (g << 8) | (r << 16) | (0xFF << 24); // Set alpha to 255
}

Color::Color(unsigned int b, unsigned int g, unsigned int r, unsigned int a)
{
	bgra =
		(
			b << 0 |
			g << 8 |
			r << 16 |
			a << 24
			);
}

Color::Color(ColorEnum colorEnum)
	:bgra((unsigned int)colorEnum) //note the typecast
{

}

unsigned int Color::convertToUnsignedInt()
{
	return bgra;
}


ChessImageBMP::ChessImageBMP()
	:ImageBMP(boardDimension, boardDimension,ColorEnum::BoardBorder)
{
	//just called the parent constructor: 

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

	/*Note: darkSquareColor ignored for now by drawRectangleOutline function def.*/
	drawRectangleOutline(0 + BORDER_SIZE, 0 + BORDER_SIZE, 
		boardDimension / 8, boardDimension / 8, Color{0xFF'00'00'00});

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

void ChessImageBMP::drawAndFillAnIrregularShape()
{
	//outline of irregular shape: 


	//scanline fill algo: 

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

			if ( difference > threshold) //bgrd color of piece - do not "overwrite" the board's square color 
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
	//first make positions (positions likely to be a member variable of the ChessGame class,
	// but only needed "once" in this ChessImageBMP class, I suppose 

	const string ranks = "ABCDEFGH"; 
	const string files = "12345678"; 

	/*a 2D vector*/
	vector<vector<string>> positions(ranks.size(),vector<string>{files.size()});

	//cout << positions.size() << "\t" << positions.at(0).size() << "\n";


	for (int i = 0; i < ranks.size(); ++i)
	{
		for (int j = 0; j < files.size(); ++j)
		{
			positions[i][j] = string(1, ranks[i]) + files[j];
			//positions.at(i).at(j) = to_string(ranks.at(i)) + to_string(files.at(j) ); 
			// //nope -to_string does not handle chars
			
			//cout << left << setw(3) << positions.at(i).at(j); 

			positionsToImageCoordinates.insert
			(
				{ 
					positions[i][j], 
					std::pair<int, int>
						{SQUARE_WIDTH/2 + SQUARE_WIDTH * i, 
						SQUARE_WIDTH/2 + SQUARE_WIDTH * j} 
				}
			);
		}
		//cout << "\n";
	}
	

}



#pragma region auxillary functions

/*[1, 2, 3] will become 
[
1
2
3
]
*/
vector<vector<char>> rotateMatrixClockwise(vector<vector<char>>& originalMatrix, int originalNumberOfRows, int originalNumberOfCols)
{
	vector<vector<char>> rotatedMatrix;

	//note the switched order of rows and cols: 
	rotatedMatrix.resize(originalNumberOfCols, vector<char>(originalNumberOfRows));

	for (int row = 0; row < originalNumberOfRows; ++row)
	{
		for (int col = 0; col < originalNumberOfCols; ++col)
		{
			//I didn't work through this equation - only walked through in debug mode...
			rotatedMatrix[col][originalNumberOfRows - 1 - row] = originalMatrix[row][col]; 
		}
	}


	return rotatedMatrix; 
}

/*No great need for this one - just being goofy*/
vector<vector<int>> rotateIntMatrixClockwise(vector<vector<int>>& originalMatrix, int originalNumberOfRows, int originalNumberOfCols)
{
	vector<vector<int>> rotatedMatrix;

	//note the switched order of rows and cols: 
	rotatedMatrix.resize(originalNumberOfCols, vector<int>(originalNumberOfRows));

	for (int row = 0; row < originalNumberOfRows; ++row)
	{
		for (int col = 0; col < originalNumberOfCols; ++col)
		{
			//I didn't work through this equation - only walked through in debug mode...
			rotatedMatrix[col][originalNumberOfRows - 1 - row] = originalMatrix[row][col];
		}
	}


	return rotatedMatrix;
}
map<char, vector<vector<char>>> makeMapOfPixelLetters()
{
	map<char, vector<vector<char>>> mapOfPixelLetters;

	vector<vector<char>> matrixForC =
	{
		{'C','C','C','C','C','C','C','C',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'C','C','C','C','C','C','C','C',' ',' ',' ',' ',' ',' ',' ',' '}
	};

	mapOfPixelLetters.insert({ 'C', matrixForC });

	vector<vector<char>> matrixForD =
	{
		{ 'D','D','D','D','D','D','D',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D',' ',' ',' ',' ',' ',' ','D',' ',' ',' ',' ',' ',' ',' ',' ' },
		{ 'D','D','D','D','D','D','D',' ',' ',' ',' ',' ',' ',' ',' ',' ' }
	};

	mapOfPixelLetters.insert({ 'D', matrixForD });

	vector<vector<char>> matrixForE =
	{
		{'E','E','E','E','E','E','E','E','E','E','E','E','E','E','E','E'},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E','E','E','E','E','E','E','E','E','E','E','E','E','E','E',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'E','E','E','E','E','E','E','E','E','E','E','E','E','E','E','E'}
	};

	mapOfPixelLetters.insert({ 'E', matrixForE });

	vector<vector<char>> matrixForF =
	{
		{'F','F','F','F','F','F','F','F','F','F','F','F','F','F','F','F'},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F','F','F','F','F','F','F','F','F','F','F','F','F','F','F',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'F',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '}
	};

	mapOfPixelLetters.insert({ 'F', matrixForF });

	vector<vector<char>> matrixForG =
	{
		{' ',' ',' ',' ',' ','G','G','G','G','G','G','G',' ',' ',' ',' '},
		{' ',' ',' ',' ','G',' ',' ',' ',' ',' ',' ',' ','G',' ',' ',' '},
		{' ',' ',' ','G',' ',' ',' ',' ',' ',' ',' ',' ',' ','G',' ',' '},
		{' ',' ','G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','G',' '},
		{' ','G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','G'},
		{'G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{'G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','G','G','G','G','G'},
		{'G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','G','G','G','G','G'},
		{'G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','G'},
		{'G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','G'},
		{' ','G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','G'},
		{' ',' ','G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','G',' '},
		{' ',' ',' ','G',' ',' ',' ',' ',' ',' ',' ',' ',' ','G',' ',' '},
		{' ',' ',' ',' ','G','G','G','G','G','G','G','G','G',' ',' ',' '},
		{' ',' ',' ',' ',' ','G','G','G','G','G','G','G',' ',' ',' ',' '}
	};

	mapOfPixelLetters.insert({ 'G', matrixForG });

	vector<vector<char>> matrixForH =
	{
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H','H','H','H','H','H','H','H','H','H','H','H','H','H','H','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'},
		{'H',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','H'}
	};

	mapOfPixelLetters.insert({ 'H', matrixForH });

	return mapOfPixelLetters;


}

map<int, vector<vector<int>>> makeMapOfPixelNumbers()
{
	map<int, vector<vector<int>>> mapOfPixelNumbers;

	vector<vector<int>> matrixFor1 = 
	{
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 }, 
	};

	mapOfPixelNumbers.insert({ 1, matrixFor1 });


	vector<vector<int>> matrixFor2 =
	{
		{ 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	mapOfPixelNumbers.insert({ 2, matrixFor2 });

	vector<vector<int>> matrixFor3 =
	{
		{ 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	mapOfPixelNumbers.insert({ 3, matrixFor3 });

	vector<vector<int>> matrixFor4 =
	{
		{ 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 4, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	mapOfPixelNumbers.insert({ 4, matrixFor4 });

	vector<vector<int>> matrixFor5 =
	{
		{ 0, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	mapOfPixelNumbers.insert({ 5, matrixFor5 });

	vector<vector<int>> matrixFor6 =
	{
		{ 0, 0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 6, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 6, 6, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	mapOfPixelNumbers.insert({ 6, matrixFor6 });

	vector<vector<int>> matrixFor7 =
	{
		{ 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	mapOfPixelNumbers.insert({ 7, matrixFor7 });

	vector<vector<int>> matrixFor8 =
	{
		{ 0, 0, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 8, 8, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 8, 8, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 8, 8, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 8, 8, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 8, 8, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 8, 8, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 8, 8, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 8, 8, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 8, 8, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 8, 8, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	mapOfPixelNumbers.insert({ 8, matrixFor8 });

	return mapOfPixelNumbers;

}

#pragma endregion