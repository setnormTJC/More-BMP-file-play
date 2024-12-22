
#include <iostream>
#include<fstream> 
#include<bitset>

void playingAroundWithFileWriting()
{
//    const int streamSize = 65'409;//arbitrary-ish number that requires more than 1 Byte to store 
    const int streamSize = 257;//arbitrary-ish number that requires more than 1 Byte to store 


    std::ofstream binary_fout{ "binary.txt", std::ios::binary };
    for (unsigned int i = 0; i <= streamSize; ++i)
    {
        //binary_fout.write("efgh\n", 1);

        //binary_fout << i;
        const char* ptrToData;
        int theData = i;

        //ptrToData = reinterpret_cast<const char*> (&theData);

        ptrToData = (const char*)(&theData); 

        if (i == streamSize - 2)
        {
            //std::cout << (void*)(*ptrToData) << "\n";

            std::cout << std::hex;
            //if "std::bin" is desired ...
            //std::bitset<8> thing{ 34 };
            //std::cout << thing << "\n"; //gives 00100010
            std::cout << typeid(ptrToData[0]).name() << "\n";

            std::cout << "no cast to int: " << ptrToData[0] << "\n";
            std::cout << "no bitmasking:" << ((unsigned int)ptrToData[0]) << "\n";

            //std::cout << (0xFF &  ptrToData[0]) << "\n";
            std::cout << (0xFF & (unsigned int)ptrToData[0]) << "\n";
            std::cout << (0xFF & (unsigned int)ptrToData[1]) << "\n";
            std::cout << (0xFF & (unsigned int)ptrToData[2]) << "\n";
            std::cout << (0xFF & (unsigned int)ptrToData[3]) << "\n";
        }

        //binary_fout.write(ptrToData, sizeof(theData));
        binary_fout.write(ptrToData, sizeof(theData));
    }

    binary_fout.close();

}

int main()
{
    //playingAroundWithFileWriting(); 
    std::ofstream fout{ "out.bmp", std::ios::binary };

    //file header (MUST be? 14 bytes) 
    short bfType = 0x4d'42; //as in "binary file type"
    //4D (77 in base 10) is M; 42 is B (as in BitMap)
    //2 bytes so far...

    auto ptrToData = (const char*)(&bfType);
    fout.write(ptrToData, sizeof(bfType)); //size of bm ... heh

    int bfSize = 0x00'00'00'86; //134 BYTES, in decimal -> file size (perhaps "limited" to ~4GB [= 2^32] file size) 
    ptrToData = (const char*)(&bfSize);
    fout.write(ptrToData, sizeof(bfSize));

    int bfReserved1And2 = 0x00'00'00'00;
    ptrToData = (const char*)(&bfReserved1And2);
    fout.write(ptrToData, sizeof(bfReserved1And2));
    //now up to 10 bytes ...

    int bfOffBits = 0x00'00'00'36; //offset to the pixel data 
    ptrToData = (const char*)(&bfOffBits);
    fout.write(ptrToData, sizeof(bfOffBits));




    //info header ("typically" 40 Bytes): 
    int biSize = 0x00'00'00'28; //as in "binary info size" (28 in hex is 40 in dec.)
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

    fout.write((const char*)(&xPixelsPerMeter), sizeof(int));
    fout.write((const char*)(&yPixelsPerMeter), sizeof(int));
    fout.write((const char*)(&colorUsed), sizeof(int));
    fout.write((const char*)(&importantColorCount), sizeof(int));


    //pixel data (in BGR order -> NOT RGB order): 

    int pixelData[5][16] =
    {
        //first 3 entries correspond to full blue bottom left
        {0xFF, 0x00, 0x00,  0x00, 0xFF, 0xFF,   0x00, 0xFF, 0xFF,   0x00, 0xFF, 0xFF,   0xBB,0x00,0x00,     0x00},
        //add 0x00 padding byte in  
        //final 0x00 is PADDING -> each row of image must be multiple of 4 Bytes long 
       //0x00 makes each row of image SIXTEEN bytes long

        {0x00,0xFF,0xFF,    0x99,0x00,0x00,     0x88,0x00,0x00,     0x77,0x00,0x00,     0x00,0xff,0xff,     0x00},
        {0x55,0x00,0x00,    0x44,0x00,0x00,     0x33,0x00,0x00,     0x22,0x00,0x00,     0x11,0x00,0x00,     0x00},
        {0x11,0x11,0x00,    0x00,0xff,0xff,     0x33,0x33,0x00,     0x00,0xff,0xff,     0x55,0x55,0x00,     0x00},
        {0x66,0x66,0x00,    0x77,0x77,0x00,     0x88,0x88,0x00,     0x99,0x99,0x00,     0xAA,0xaa,0x00,     0x00}
    };

    for (int row = 0; row < 5; ++row)
    {
        for (int col = 0; col < 16; ++col)
        {
            fout.write((const char*)(&pixelData[row][col]), 1);
        }
    }

    fout.close();

}

