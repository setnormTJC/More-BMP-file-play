
#include <iostream>
#include<fstream> 
#include<bitset>

#include<cassert> 

#include<filesystem> 

#include<iomanip> 

void makeABMPFile(const char* filename)

{
    std::ofstream fout{ filename, std::ios::binary };

    const int FILE_HEADER_SIZE = 14;
    const int INFO_HEADER_SIZE = 40;
    const int ROWS_IN_IMAGE = 5;
    const int COLS_IN_IMAGE = 16;

#pragma region file header info
    //file header (MUST be? 14 bytes) 


    short bfType = 0x4d'42; //as in "binary file type"
    //4D (77 in base 10) is M; 42 is B (as in BitMap)
    //2 bytes so far...

    auto ptrToData = (const char*)(&bfType);
    fout.write(ptrToData, sizeof(bfType)); //size of bm ... heh

    int bfSize = 0x00'00'00'86; //134 BYTES, in decimal -> total size of this file (perhaps "limited" to ~4GB [= 2^32] file size) 
    //bfSize = 134; 
    bfSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (ROWS_IN_IMAGE * COLS_IN_IMAGE);
    //std::cout << bfSize << "\n";
    ptrToData = (const char*)(&bfSize);
    fout.write(ptrToData, sizeof(bfSize));

    int bfReserved1And2 = 0x00'00'00'00;
    ptrToData = (const char*)(&bfReserved1And2);
    fout.write(ptrToData, sizeof(bfReserved1And2));
    //now up to 10 bytes ...

    int bfOffBits = 0x00'00'00'36; //offset to the pixel data 
    bfOffBits = FILE_HEADER_SIZE + INFO_HEADER_SIZE;

    ptrToData = (const char*)(&bfOffBits);
    fout.write(ptrToData, sizeof(bfOffBits));


    assert(FILE_HEADER_SIZE ==
        sizeof(bfType) + sizeof(bfSize) + sizeof(bfReserved1And2) + sizeof(bfOffBits));

#pragma endregion



#pragma region info header region
    //info header ("typically" 40 Bytes): 
    int biSize = INFO_HEADER_SIZE; //as in "binary info size" (28 in hex is 40 in dec.)
    fout.write((const char*)(&biSize), sizeof(biSize));

    int biWidth = 0x00'00'00'05; //image width (5 pixels wide) 
    fout.write((const char*)(&biWidth), sizeof(biWidth));

    int biHeight = 0x00'00'00'05;
    fout.write((const char*)(&biHeight), sizeof(biHeight));

    short biPlanes = 0x00'01;
    fout.write((const char*)(&biPlanes), sizeof(biPlanes));

    short biBitCount = 0x00'18; //24 bits per pixel - > modify to 32 (decimal) for alpha channel 
    fout.write((const char*)(&biBitCount), sizeof(biBitCount));

    int biCompression = 0x00'00'00'00;
    fout.write((const char*)(&biCompression), sizeof(biCompression));

    int biSizeImage = 0x00'00'00'50;//number of bytes in the pixel data (50 hex = 80 dec)
    fout.write((const char*)(&biSizeImage), sizeof(biSizeImage));

    int xPixelsPerMeter = 0x00'00'00'00;
    int yPixelsPerMeter = 0x00'00'00'00;
    int colorUsed = 0x00'00'00'00; //? 
    int importantColorCount = 0x00'00'00'00; //outdated, I think 


    assert(INFO_HEADER_SIZE ==
        sizeof(biSize) + sizeof(biWidth) + sizeof(biHeight) + sizeof(biPlanes)
        + sizeof(biBitCount) + sizeof(biCompression) + sizeof(biSizeImage) +
        sizeof(xPixelsPerMeter) + sizeof(yPixelsPerMeter)
        + sizeof(colorUsed) + sizeof(importantColorCount));

    fout.write((const char*)(&xPixelsPerMeter), sizeof(int));
    fout.write((const char*)(&yPixelsPerMeter), sizeof(int));
    fout.write((const char*)(&colorUsed), sizeof(int));
    fout.write((const char*)(&importantColorCount), sizeof(int));

#pragma endregion 

    //pixel data (in BGR order -> NOT RGB order): 

    int pixelData[ROWS_IN_IMAGE][COLS_IN_IMAGE] =
    {
        //first 3 entries ([0][0] -> [0][2]) correspond to full WHITE at BOTTOM LEFT
        {0xFF, 0xFF, 0xFF,  0x00, 0xFF, 0xFF,   0x00, 0xFF, 0xFF,   0x00, 0xFF, 0xFF,   0xBB,0x00,0x00,     0x00},
        //add 0x00 padding byte in  
        //final 0x00 is PADDING -> each row of image must be multiple of 4 Bytes long 
       //0x00 makes each row of image SIXTEEN bytes long

        {0x00,0xFF,0xFF,    0x99,0x00,0x00,     0x88,0x00,0x00,     0x77,0x00,0x00,     0x00,0xff,0xff,     0x00},
        {0x55,0x00,0x00,    0x44,0x00,0x00,     0x33,0x00,0x00,     0x22,0x00,0x00,     0x11,0x00,0x00,     0x00},
        {0x11,0x11,0x00,    0x00,0xff,0xff,     0x33,0x33,0x00,     0x00,0xff,0xff,     0x55,0x55,0x00,     0x00},
        {0x66,0x66,0x00,    0x77,0x77,0x00,     0x88,0x88,0x00,     0x99,0x99,0x00,     0xAA,0xaa,0x00,     0x00}
    };

    for (int row = 0; row < ROWS_IN_IMAGE; ++row)
    {
        for (int col = 0; col < COLS_IN_IMAGE; ++col)
        {
            fout.write((const char*)(&pixelData[row][col]), 1);
        }
    }

    fout.close();
}

auto getFileSize(const char* filename)
{
    auto currentPath = std::filesystem::current_path();
    //std::cout << currentPath << "\n";

    unsigned long long FILE_SIZE; //using rather than int to match std::filesystem::...file_size() return type
    FILE_SIZE = -1; 

    for (auto& currentEntry : std::filesystem::directory_iterator(currentPath))
    {
        if (currentEntry.path().filename().string().find(filename) != std::string::npos)
        {
            //std::cout << currentEntry.path() << "\n";
            FILE_SIZE = currentEntry.file_size();
        }
    }

    if (FILE_SIZE == -1)
    {
        throw("file not found!");
    }

    return FILE_SIZE;
}



const char* readBMPFile(const char* filename, unsigned long long FILE_SIZE)
{
    std::ifstream fin{ filename, std::ios::binary };

    
    char* imageData = new char[FILE_SIZE];


    //int i = 0; 
    //while (!fin.eof())
    //{
    //    char currentChar; 

    //    fin.get(currentChar); 

    //    imageData[i] = currentChar;
    //    //fin >> imageData[i]; //reads up to first space (too much? )
    //    i++; 
    //}

    fin.read(imageData, FILE_SIZE); //nicer alternative to the commented-out loop above

    fin.close(); 
    
    return imageData; 
}

void myHexDump(const char* imageData, unsigned long long FILE_SIZE)
{
    for (int i = 0; i < FILE_SIZE; ++i)
    {
        std::cout << std::hex;
        //make new line every 10 Bytes (typical hex-dump format, I think)
        if (i % 16 == 0)
        {
            std::cout << "\n";
        }
        std::cout << std::setfill('0');
        std::cout << std::left << std::setw(2);
        std::cout << (unsigned int)(unsigned char)imageData[i] << " ";        //make space between every BYTE 
    }

    std::cout << "\n\n";
}


struct BMPImage
{
    char* imageData;
    unsigned int width, height;

    unsigned int offsetOfPixelData;

    friend std::ostream& operator << (std::ostream& os, const BMPImage& image)
    {
        os << "height:\twidth\toffset to pixel data:\n";
        os << image.height << image.width << image.offsetOfPixelData << "\n";
        return os; 
    }
};

BMPImage getImageData(const char* imageData, unsigned long long FILE_SIZE)
{
    BMPImage theImage;


    //theImage.height = (unsigned int)(unsigned char) imageData[21];
    //std::cout << std::hex << (unsigned int)(unsigned char)imageData[21] << "\n";

    //theImage.width = (unsigned int)(unsigned char)imageData[18] |
    //    ((unsigned int)(unsigned char)imageData[19] << 8) |
    //    ((unsigned int)(unsigned char)imageData[20] << 16) |
    //    ((unsigned int)(unsigned char)imageData[21] << 24);

    //theImage.height += (unsigned int) image

    ///
    
    return theImage;
}

int main()
{
    const char* filename = "out.bmp"; 

    makeABMPFile(filename); 

    auto FILE_SIZE = getFileSize(filename); 

    const char* imageData = readBMPFile(filename, FILE_SIZE); 

    myHexDump(imageData, FILE_SIZE); 

    BMPImage theImage = getImageData(imageData, FILE_SIZE); 
   

    std:: cout << theImage << "\n";


    delete[] imageData; 
}

