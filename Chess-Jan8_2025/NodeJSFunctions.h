#pragma once
#include<thread>  

#include<string>

#include<iostream> 

#include<fstream>

#include<array> //for returning array of 2 coordinates (x, y) clicked in chessboard image 


using std::array, std::pair, std::string, std::ifstream, std::cout;




void callNodeJS();

void openPort3000_andDisplayChessBoard();

void killProcessOnPort(int port);

array<pair<int, int>, 2>  readFileAndReturnCoordinatesClickedInBrowser();