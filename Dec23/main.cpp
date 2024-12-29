

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

	cout.imbue(std::locale{""});
	cout << "\n\nSum of file sizes: " << sumOfFileSizes << "\n";
}

int main()
{
	ChessGame theGame{}; 

	char oldPositionFile, newPositionFile;
	int oldPositionRank, newPositionRank;

	while (!theGame.isGameOver())
	{

		cout << "Enter the FILE and RANK of the piece you want to move (ex: A8, H5, etc.)\n";
		std::cin >> oldPositionFile >> oldPositionRank;

		cout << "Where do you want to move to ?\n";
		std::cin >> newPositionFile >> newPositionRank; 
		
		theGame.movePiece(oldPositionFile, oldPositionRank , newPositionFile, newPositionRank);

		if (std::cin.fail()) //ex: SOMEONE enters chess piece name as first input mistakenly ...
		{
			std::cin.clear(); 
		}
		soundAlertIfImagesTakingUpTooMuchSpace();
	}


	return 0; 

}

