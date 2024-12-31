


#include"ChessGame.h"
#include "NodeJSFunctions.h"


/*"Too much" means > 100 MB*/
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

	cout.imbue(std::locale{""}); //add commas every 3 digits for readability 
	cout << "\n\nSum of file sizes: " << sumOfFileSizes << "\n";
}


int main()
{


	ChessGame theGame{}; 


	while (!theGame.isGameOver())
	{
		callNodeJS();
		openPort3000_andDisplayChessBoard();
	
		cout << "Press ENTER once you've clicked the piece and its new position:\n";
		std::cin.get(); 

		auto theTwoChosenPositions = theGame.getAndConfirmChessMove(); 

		theGame.movePiece(theTwoChosenPositions.at(0).first,
			theTwoChosenPositions.at(0).second,
			theTwoChosenPositions.at(1).first,
			theTwoChosenPositions.at(1).second);

		soundAlertIfImagesTakingUpTooMuchSpace();

	}


	return 0; 

}

