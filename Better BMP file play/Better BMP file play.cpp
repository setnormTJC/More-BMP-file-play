
#include"someBMPImageFunctions.h"

int main()
{
    const char* filename = "out.bmp"; 

    makeABMPFile(filename); 

    auto FILE_SIZE = getFileSize(filename); 

    const char* imageData = readBMPFile(filename, FILE_SIZE); 

    myHexDump(imageData, FILE_SIZE); 

    BMPImage theImage = getImageData(imageData, FILE_SIZE); 
   

    auto newImageData = returnToyModifiedImage(FILE_SIZE, theImage, imageData); 
    std::cout << "\n\nNew image data:\n";
    myHexDump_nonConst(newImageData, FILE_SIZE);
    const char* newFileName = "new.bmp";
    writeNewImage(newFileName, newImageData, FILE_SIZE);
    

    delete[] imageData; 

    delete[] newImageData; 

}

