#pragma once

/*The corresponding function definitions for these declarations get VERY complicated - not going to try to use in main.cpp, but keeping here 
in case I get interested in the future*/

#include<Windows.h>
#include <iostream>
#include <vector>
#include <string>


using std::vector, std::cout, std::string; 

/*The goal of this header file in the context of this Chess project will be to get mouse click coordinates from a BMP file 
- perhaps using Microsoft Paint - and pipe those coordinates into the `movePiece` function*/

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);//CALLBACK is a macro that expands to __stdcall

vector<HWND> getAllOpenWindows();

string getWindowTitle(HWND hwnd);

HWND getWindowByTitle(const string& desiredTitle);

//mouse hook - cute name
void setMouseHook(HWND hwnd); 

void removeMouseHook(); 

//POINT is a struct in <windef.h> that consists of long x and long y
POINT getLastClickCoordinates(); 


void writeCoordinatesToFile(const POINT& pt, const std::string& filename); 
//getwindowname