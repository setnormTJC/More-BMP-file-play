#include "ImageBMP.h"

void ImageBMP::writeImageFile(string filename)
{
	ofstream fout{ filename, ios::binary };

	//first comes the 14-byte file header: 
	fout.write(reinterpret_cast<const char*>(fileHeader.filetype), 2); //no sizeof here, since filetype is a pointer
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


	//now the pixel data: 
	for (int row = 0; row < infoHeader.imageWidth; ++row)
	{
		for (int col = 0; col < infoHeader.imageHeight; ++col)
		{
			unsigned int colorOfCurrentPixel = pixelData.pixelMatrix.at(row).at(col).convertToUnsignedInt();

			fout.write(reinterpret_cast<const char*>(&colorOfCurrentPixel), sizeof(colorOfCurrentPixel));
		}

	}


	fout.close(); 
}

ImageBMP::ImageBMP(int imageWidth, int imageHeight, const Color& fillColor, const Color& middleDotColor)
{
	infoHeader.imageWidth = imageWidth;
	infoHeader.imageHeight = imageHeight;
	infoHeader.sizeOfPixelData = imageWidth * imageHeight * (infoHeader.bitsPerPixel / 8);
	//NOTE: fileheader size should always be 14 (I think) 
	fileHeader.fileSize = 14 + infoHeader.getInfoHeaderSize() + infoHeader.sizeOfPixelData; 
	


	//fill pixelData with given fill color:
	for (int row = 0; row < imageWidth; ++row)
	{
		vector<Color> currentRow; 
		for (int col = 0; col < imageHeight; ++col)
		{
			currentRow.push_back(fillColor); 
		}
		pixelData.pixelMatrix.push_back(currentRow);
	}

	//add the middle dot (having different color): 
	pixelData.pixelMatrix.at(imageWidth / 2).at(imageHeight / 2) = middleDotColor; 
	
}


int InfoHeader::getInfoHeaderSize() const
{
	return infoHeaderSize;
}

int InfoHeader::getSizeOfPixelData() const
{
	//return sizeOfPixelData;
	return imageWidth * imageHeight * (bitsPerPixel / 8);
}

Color::Color(unsigned int bgra)
	:bgra(bgra)
{
}

unsigned int Color::convertToUnsignedInt()
{
	return bgra;
}

