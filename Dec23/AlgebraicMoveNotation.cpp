#include "AlgebraicMoveNotation.h"

AlgebraicMoveNotation::AlgebraicMoveNotation()
{
	allPositions = getChessPositions(); 

	for (vector<string> & rowOfPositions : allPositions)
	{
		for (string& position : rowOfPositions)
		{
			//convert uppercase file to lowercase (as is conventional in chess algebraic notation) 
			position.at(0) = (char)std::tolower(position.at(0));
		}
	}
}

void AlgebraicMoveNotation::readPortableGameNotationFile(const string& filename)
{
	assert(filename.find(".pgn") != string::npos); //MUST be the filetype for chess game notation 

	ifstream fin{};
	fin.open(filename, std::ios::binary); 

	if (!fin.is_open())
	{
		cout << filename << " not found\n";
		return; 
	}

	/*Surely ... one of the dumbest ways to do this :) */
	string line;
	while (line.find("1.") == string::npos) //moves start at text 1. - don't store until then 
	{
		getline(fin, line);
		continue; //to the next line 
	}

	while (!fin.eof())
	{	
		fileData.push_back(line); 
		getline(fin, line);
	}

	fileData.push_back(line);

	fin.close(); 
	
}

void AlgebraicMoveNotation::cleanUpFileData()
{
	int lineNumber = 1; 
	for (auto& line : fileData)
	{
		auto iteratorOfOpenBrace = std::find(line.begin(), line.end(), '{');
		if (iteratorOfOpenBrace != line.end()) //brace inidicates "optional" content on chess.com 
		{
			auto iteratorOfCloseBrace = std::find(line.begin(), line.end(), '}');
			//using iterators rather than indices in hopes of simplifying the erase arguments 
			// (ex: not having to fool with offset and count) 
			
			//erase all text between the braces (inclusive of braces)
			line.erase(iteratorOfOpenBrace, iteratorOfCloseBrace + 2);

			//NOW, move resumes after moveNumber (lineNumber)... (3 dots/ellipses)
			//string nextTokenToSkip = std::to_string(lineNumber) + "...";
			string moveNumberAsString = std::to_string(lineNumber); 

			int startIndexOfNextTokenToSkip = line.find(moveNumberAsString + "...");

			if (startIndexOfNextTokenToSkip != string::npos)
			{
				line.erase(startIndexOfNextTokenToSkip,moveNumberAsString.length() + 4);
			}


			//line occasionally might STILL have {optional stuff}, so check: 
			if (std::find(line.begin(), line.end(), '{') != line.end())
			{
				line.erase(
					std::find(line.begin(), line.end(), '{'),
					std::find(line.begin(), line.end(), '}' + 1)
					);
			}

		}
		
		//finally, remove \r character if present 
		//this occurred when I wrote fool's mate by hand and read ifstream in binary mode: 
		if (line.find('\r') != string::npos)
		{
			line = line.substr(0, line.size() - 1);
		}

		cout << line << "\n";

		lineNumber++; //for example: 1. d4 {optional stuff} 1... Nf6 
		//(need to handle 1... -> means first move continued, after optional stuff)
	}

}

