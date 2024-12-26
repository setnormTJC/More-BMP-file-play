/*Shared between ChessImageBMP and ChessGame*/

#include"sharedFunctions.h"


vector<string> getPieceNames(const string& pieceColor)
{

	vector<string> pieceNames; 

	string relativePathOfPieceImages = "\\resources\\pieceImages\\";

	if (pieceColor == "white")
	{
		relativePathOfPieceImages += "white\\";
	}

	else if (pieceColor == "black")
	{
		relativePathOfPieceImages += "black\\";
	}

	else
	{
		cout << "Neither black nor white piece color?\n";
		std::cin.get(); 
		exit(-1); 

		//return pieceNames; 
	}

	auto startingPath = std::filesystem::current_path(); 

	auto newPath = startingPath.string() + relativePathOfPieceImages;

	std::filesystem::current_path(newPath);

	for (auto& entry : std::filesystem::directory_iterator{ newPath })
	{
		string path = entry.path().string();
		size_t indexOfLastFileSeparator = path.find_last_of('\\');
		size_t indexOfFileExtension = path.find('.');


		string fileName = path.substr(indexOfLastFileSeparator + 1);
		string pieceName = fileName.substr(0, fileName.find('.'));

		//cout << pieceName << "\tlength:" << pieceName.length() << "\n";
		pieceNames.push_back(pieceName);
		//cout << entry.path() << "\n";
	}

	//reset current path back to starting path (where main.cpp is) 
	std::filesystem::current_path(startingPath); 

	return pieceNames;
}

vector<vector<string>> getChessPositions()
{
	const string ranks = "ABCDEFGH";
	const string files = "12345678";

	/*a 2D vector*/
	vector<vector<string>> positions(ranks.size(), vector<string>{files.size()});

	for (int i = 0; i < ranks.size(); ++i)
	{
		for (int j = 0; j < files.size(); ++j)
		{
			positions[i][j] = string(1, ranks[i]) + files[j];
			//positions.at(i).at(j) = to_string(ranks.at(i)) + to_string(files.at(j) ); 
			// //nope -to_string does not handle chars
		}
	}

	return positions; 
}

/*Add black pieces later*/
map<string,string> getPieceNamesToInitialPositions(const vector<string>& pieceNames)
{
	auto chessPositions = getChessPositions(); 

	map<string, string> pieceNamesToInitialPositions;

	//pieceNames (read from image files) are alphabetical
	// (ex: blackBishopKside, blackBishopQSide, .. whiteBishop...whiteKing...whiteRook...etc)	
	pieceNamesToInitialPositions.insert({ pieceNames[0], "F8"});
	pieceNamesToInitialPositions.insert({ pieceNames[1], "C8"});
	pieceNamesToInitialPositions.insert({ pieceNames[2], "E8" });
	pieceNamesToInitialPositions.insert({ pieceNames[3], "G8" });
	pieceNamesToInitialPositions.insert({ pieceNames[4], "B8" });

	//black pawns 
	int indexOfA7Pawn = 5; 
	for (int col = 0; col < 8; ++col)
	{
		string currentPosition = chessPositions.at(col).at(6); //black pawns are on rank 7 (= 6 + 1) 

		string currentPawn = pieceNames[indexOfA7Pawn + col]; 
		pieceNamesToInitialPositions.insert({ currentPawn, currentPosition });
	}

	pieceNamesToInitialPositions.insert({ pieceNames[13], "D8" });
	pieceNamesToInitialPositions.insert({ pieceNames[14], "H8" });
	pieceNamesToInitialPositions.insert({ pieceNames[15], "A8" });


	//now WHITE pieces: 
	pieceNamesToInitialPositions.insert({ pieceNames[16],"F1" }); //white bishop king side 
	pieceNamesToInitialPositions.insert({ pieceNames[17],"C1" }); //white bishop queen side
	pieceNamesToInitialPositions.insert({ pieceNames[18],"E1" }); //white king 
	pieceNamesToInitialPositions.insert({ pieceNames[19],"G1" }); //etc.
	pieceNamesToInitialPositions.insert({ pieceNames[20],"B1" });

	int indexOfA2Pawn = 21;
	//map white pawns to rank 2 (row = 1) 
	for (int col = 0; col < 8; ++col)
	{
		string currentPosition = chessPositions.at(col).at(1);

		//cout << chessPositions.at(col).at(1) << "\n";
		string currentPawn = pieceNames[indexOfA2Pawn + col];

		pieceNamesToInitialPositions.insert({ currentPawn, currentPosition });
	}

	pieceNamesToInitialPositions.insert({ pieceNames[29],"D1" }); //white queen
	pieceNamesToInitialPositions.insert({ pieceNames[30],"H1" });
	pieceNamesToInitialPositions.insert({ pieceNames[31],"A1" });

	return pieceNamesToInitialPositions; 
}

vector<string> flatten2DArray(vector<vector<string>>& twoDArray)
{
	vector<string> oneDArray;// { twoDArray.size()* twoDArray.at(0).size() };

	for (size_t row = 0; row < twoDArray.size(); ++row)
	{
		for (size_t col = 0; col < twoDArray.at(0).size(); ++col)
		{
			oneDArray.push_back(twoDArray.at(row).at(col));
		}
	}


	return oneDArray; 
}


