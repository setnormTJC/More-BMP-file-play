#include "sharedFunctions.h"
#include <string>

#pragma region ChessPosition 
ChessPosition::ChessPosition(char file, int rank)
	:file(file), rank(rank)
{
}

bool ChessPosition::operator==(const ChessPosition& other) const 
{
	return (rank == other.rank) && (file == other.file);
}

bool ChessPosition::isPositionInBounds(char file, int rank)
{
	
	return
		(file >= 'A' && file <= 'H')
		&&
		(rank >= 1 && rank <= 8);
}

size_t ChessPositionHash::operator()(const ChessPosition& chessPosition) const
{
	size_t fileHash = std::hash<char>()(chessPosition.file);
	size_t rankHash = std::hash<int>()(chessPosition.rank);

	return fileHash ^ (rankHash << 1);

}

#pragma endregion 






vector<vector<ChessPosition>> sharedFunctions::getChessPositions()
{

	vector<vector<ChessPosition>> chessPositions(8, vector<ChessPosition>(8));

	int row = 0; 

	for (char file = 'A'; file < 'I'; ++file)
	{
		int col = 0;
		for (int rank = 1; rank < 9; ++rank)
		{
			chessPositions.at(row).at(col) = ChessPosition{ file, rank };
			col++; 
		}
		row++; 
	}

	return chessPositions; 
}

vector<string> sharedFunctions::getPieceNames(const string& color)
{
	vector<string> pieceNames; 
	
	string relativePathOfPieceImages = "\\resources\\pieceImages\\"; 

	if (color == "white")
	{
		relativePathOfPieceImages += "white"; 
	}

	else if (color == "black")
	{
		relativePathOfPieceImages += "black"; 
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

		pieceNames.push_back(pieceName); //note the .cstr() 

	}

	//reset current path back to starting path (where main.cpp is) 
	std::filesystem::current_path(startingPath);

	return pieceNames;

}

unordered_map<string, ChessPosition> sharedFunctions::getPiecesToInitialPositions(const vector<string>& pieceNames)
{
	auto chessPositions = getChessPositions(); 

	unordered_map<string, ChessPosition> piecesToInitialPositions; 


	piecesToInitialPositions.insert({ pieceNames[0], ChessPosition{'F', 8} });
	piecesToInitialPositions.insert({ pieceNames[1], ChessPosition{'C', 8} }); 
	piecesToInitialPositions.insert({ pieceNames[2], ChessPosition{'E', 8} });
	piecesToInitialPositions.insert({ pieceNames[3], ChessPosition{'G', 8} }); 	
	piecesToInitialPositions.insert({ pieceNames[4], ChessPosition{'B', 8} });

	//black pawns 
	int indexOfA7Pawn = 5;
	for (int col = 0; col < 8; ++col)
	{
		ChessPosition currentPosition = chessPositions.at(col).at(6); //black pawns are on rank 7 (= 6 + 1) 

		string currentPawn = pieceNames[indexOfA7Pawn + col];
		piecesToInitialPositions.insert({ currentPawn, currentPosition });
	}

	piecesToInitialPositions.insert({ pieceNames[13], ChessPosition{'D', 8} });
	piecesToInitialPositions.insert({ pieceNames[14], ChessPosition{'H', 8} });
	piecesToInitialPositions.insert({ pieceNames[15], ChessPosition{'A', 8} });

	//now WHITE pieces: 
	piecesToInitialPositions.insert({ pieceNames[16], ChessPosition{'F', 1} }); //white bishop king side 
	piecesToInitialPositions.insert({ pieceNames[17], ChessPosition{'C', 1} }); //white bishop queen side
	piecesToInitialPositions.insert({ pieceNames[18], ChessPosition{'E', 1} }); //white king 
	piecesToInitialPositions.insert({ pieceNames[19], ChessPosition{'G', 1} });
	piecesToInitialPositions.insert({ pieceNames[20], ChessPosition{'B', 1} });

	int indexOfA2Pawn = 21;
	//map white pawns to rank 2 (row = 1) 
	for (int col = 0; col < 8; ++col)
	{
		ChessPosition currentPosition = chessPositions.at(col).at(1);

		//cout << chessPositions.at(col).at(1) << "\n";
		string currentPawn = pieceNames[indexOfA2Pawn + col];

		piecesToInitialPositions.insert({ currentPawn, currentPosition });
	}

	piecesToInitialPositions.insert({ pieceNames[29],ChessPosition{'D', 1} }); //white queen
	piecesToInitialPositions.insert({ pieceNames[30],ChessPosition{'H', 1} });	
	piecesToInitialPositions.insert({ pieceNames[31],ChessPosition{'A', 1} });


	return piecesToInitialPositions; 
}

string sharedFunctions::getPieceRelationship(const string& currentPiece, const string& contentsOfNewPosition)
{
	if (contentsOfNewPosition == "")
	{
		return "Neutral"; // if position to move to does not contain a piece, neither a friend nor a foe is there
	}

	else if (currentPiece.find("white") != string::npos
		&&
		contentsOfNewPosition.find("white") != string::npos
		)
	{
		return "Friend";
	}

	else if (currentPiece.find("black") != string::npos
		&&
		contentsOfNewPosition.find("black") != string::npos
		)
	{
		return "Friend";
	}

	else //IS the only other possibility that one is white and the other is black? 
	{
		return "Foe";
	}
}

string sharedFunctions::getPieceColor(const string& fullPieceName)
{
	if (fullPieceName.find("white") != string::npos)
	{
		return "white";
	}

	else
	{
		return "black";
	}
}

unsigned short sharedFunctions::findPieceValue(const string& fullPieceName)
{
	if (fullPieceName.find("Pawn") != string::npos)
	{
		return 1;
	}

	else if (fullPieceName.find("Bishop") != string::npos)
	{
		return 3;
	}

	else if (fullPieceName.find("Knight") != string::npos)
	{
		return 3;
	}

	else if (fullPieceName.find("Rook") != string::npos)
	{
		return 5;
	}

	else if (fullPieceName.find("Queen") != string::npos)
	{
		return 9;
	}

	else if (fullPieceName.find("King") != string::npos)
	{
		return 200; //arbitrary-ish "large value from Shannon paper 
	}

	else
	{
		return 0; //this is likely to trigger when `orderMoves` is called 
		//cout << "What piece is that?\n";
		//__debugbreak(); 
	}
}

string getPieceColor(const string& fullPieceName)
{
	if (fullPieceName.find("white") != string::npos)
	{
		return "white";
	}

	else
	{
		return "black";
	}
}

void sharedFunctions::setTerminalColor(TerminalColor color)
{
	cout << "\033" << static_cast<int>(color) << "m";
}

