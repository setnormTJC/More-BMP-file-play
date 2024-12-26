
#pragma once
//anticipate that functions in this header will be used by Image class and Game class 
#include<filesystem>
#include<vector> 
#include<string> 
#include<iostream> 

#include<map> 


using std::vector, std::string, std::cout, std::map; 

vector<string> getPieceNames(const string& pieceColor);
vector<vector<string>> getChessPositions();

map<string, string> getPieceNamesToInitialPositions(const vector<string>& pieceNames);

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