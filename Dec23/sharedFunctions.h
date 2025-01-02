
#pragma once
//anticipate that functions in this header will be used by Image class and Game class 
#include<filesystem>
#include<vector> 
#include<string> 
#include<iostream> 

#include<map> 

#include<time.h> //for writing image file name as current system time (Windows-specific, I think)

#include<fstream> 

#include<string>




using std::vector, std::string, std::cout, std::map;
using std::getline; 
using std::ifstream;





/*For drawing attention to checks, maybe "good" moves and "bad" moves later*/
enum class TerminalColor {
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
    Default = 0
};







void displayThatAPieceCanBeTaken(const string& currentPiece, const string& pieceThatCanBeTaken); 

vector<string> getPieceNames(const string& pieceColor);

string getPieceColor(const string& fullPieceName);

vector<vector<string>> getChessPositions();

std::pair<char, int> convertStringChessPositionToCharAndInt(const string& chessPosition);
string convertCharAndIntChessPositionToString(const char file, const int rank);


map<string, string> getPiecesToInitialPositions(const vector<string>& pieceNames);

bool isPositionInBounds(char file, int rank);

/*Ex: 3 pts for bishop and knight, 1 for pawn, etc.*/
size_t findPieceValue(string fullPieceName);





template<typename T1, typename T2> 
map<T2, T1> switchMapKeysAndValues(map<T1, T2>& originalMap)
{
	map<T2, T1> switchedMap; 

	for (auto& pair : originalMap)
	{
		switchedMap.insert({ pair.second, pair.first });
	}

	return switchedMap; 
}







vector<string> flatten2DArray(vector<vector<string>>& twoDArray);

/*ex: if current hour and minute is 3:47 pm, this method returns 15_47 (zulu time)*/
string getCurrentHourAndMinute(); 


/*For drawing attention to checks, maybe "good" moves and "bad" moves later*/
void setTerminalColor(TerminalColor color);
