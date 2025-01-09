#pragma once

#include<vector> 
#include<iostream> 
#include<unordered_map>
#include<filesystem> 


using std::vector, std::cout, std::pair, std::hash, std::unordered_map;
using std::string; 

struct ChessPosition
{
	char file;
	int rank;

	ChessPosition() = default;
	ChessPosition(char file, int rank);

	//bool operator < (const ChessPosition& other); 
	bool operator == (const ChessPosition& other) const;

	/*Allow calling without instantiating object*/
	static bool isPositionInBounds(char file, int rank);

};

struct ChessPositionHash //REQUIRED if ChessPosition is to be a key in unordered_map
{
	size_t operator() (const ChessPosition& chessPosition) const;
};


struct PairHash
{
	template<typename T1, typename T2> 
	size_t operator()(const pair<T1, T2>& thePair) const
	{
		auto hash1 = hash<T1>{}(thePair.first);
		auto hash2 = hash<T2>{}(thePair.second);

		return hash1 ^ (hash2 << 1);
	}
};


namespace sharedFunctions
{
	vector<vector<ChessPosition>> getChessPositions();

	template<typename T1, typename T2, typename OriginalKeyHash, typename SwitchedKeyHash>
	unordered_map<T2, T1, SwitchedKeyHash> switchMapKeysAndValues(const unordered_map<T1, T2, OriginalKeyHash>& originalMap)
	{
		unordered_map<T2, T1, SwitchedKeyHash> switchedMap;

		for (const auto& pair : originalMap)
		{
			switchedMap.insert({ pair.second, pair.first });
		}

		return switchedMap;
	}


	vector<string> getPieceNames(const string& color);

	unordered_map<string, ChessPosition> getPiecesToInitialPositions(const vector<string>& pieceNames);


	/*Returns Neutral, Friend, or Foe*/
	string getPieceRelationship(const string& currentPiece, const string& contentsOfNewPosition);

};

