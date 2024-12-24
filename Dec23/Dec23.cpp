// Dec23.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include"ImageBMP.h"

int main()
{
	//ImageBMP imageBMP{ 2, 3, Color{0xFF'22'33'44}, Color{0x55'66'77'88} };
	ImageBMP imageBMP{ 200, 300, Color{0x00'22'33'44}, Color{0x55'66'77'88} };
	imageBMP.writeImageFile("out.bmp"); 

	//0x18

}

