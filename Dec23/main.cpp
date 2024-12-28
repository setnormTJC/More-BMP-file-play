

//#include"ChessImageBMP.h"

#include"ChessGame.h"


void soundAlertIfImagesTakingUpTooMuchSpace()
{
	auto currentPath = std::filesystem::current_path(); 

	size_t sumOfFileSizes = 0; 

	for (auto& entry : std::filesystem::directory_iterator(currentPath))
	{
		sumOfFileSizes += entry.file_size(); 
	}

	if (sumOfFileSizes > 100'000'000) //100 MB (about 50 images with 720 x 720 p with 32 bit depth)
	{
		cout << "file size over 9000!!!! (actually 100 MB)\n";
		while (true)
		{
			cout << "\a";
		}
	}

	cout << "\n\nSum of file sizes: " << sumOfFileSizes << "\n";
}

int main()
{
	ChessGame theGame{}; 


	//move E2 pawn to check if white king can possibly move there

	theGame.arbitrarilyMovePiece("whitePawnE2", "E3");


	for (int i = 0; i < 5; ++i) //5 moves for no particular reason
	{
		theGame.generatePiecesToPossiblePositions();


		auto piecesToPossiblePositions = theGame.piecesToPossiblePositions;

		for (auto& pair : piecesToPossiblePositions)
		{

			if (pair.second.size() != 0)
			{
				cout << pair.first << " can move to:\n";
				for (int i = 0; i < pair.second.size(); ++i)
				{
					cout << pair.second.at(i) << "\n";
				}
			}

		}

		string piece, position; 
		cout << "Enter piece and position to move to: \n";
		std::cin >> piece >> position; 


		theGame.arbitrarilyMovePiece(piece, position); 


		soundAlertIfImagesTakingUpTooMuchSpace();
	}


	return 0; 

}

