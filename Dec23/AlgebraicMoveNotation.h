#pragma once

#include<vector> 

#include"sharedFunctions.h" //to generate positions A1 - H8
#include<fstream> 
#include<cassert> 
#include<string> 

using std::vector, std::getline; 

class AlgebraicMoveNotation
{
	vector<vector<string>> allPositions;
	
	vector<string> fileData; 

public: 
	AlgebraicMoveNotation();

	void readPortableGameNotationFile(const string& filename);

	/*Chess.com includes a bunch of "extra" data that is not necessary if just wanting to generate moves*/
	void cleanUpFileData();

	//getChessPositions()
};

