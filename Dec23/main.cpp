


#include"ChessGame.h"


void waitForNodeJsToFinish(const std::string& lockFilePath)
{
	while (std::filesystem::exists(lockFilePath)) 
	{
		std::cout << "Lock file exists, waiting...\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}



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

	char oldPositionFile, newPositionFile;
	int oldPositionRank, newPositionRank;

	//int previousLineCountInClickCoordinatesFile = 0; 

	std::string lockFilePath = "testingNodeJS/public/lockfile";

	while (!theGame.isGameOver())
	{

		cout << "Enter the FILE and RANK of the piece you want to move (ex: A8, H5, etc.)\n";
		std::cin >> oldPositionFile >> oldPositionRank;

		cout << "Where do you want to move to ?\n";
		std::cin >> newPositionFile >> newPositionRank; 

		if (std::cin.fail()) //ex: SOMEONE (not me) enters chess piece name as first input mistakenly ...
		{
			std::cin.clear();
		}

		theGame.movePiece(oldPositionFile, oldPositionRank , newPositionFile, newPositionRank);


		soundAlertIfImagesTakingUpTooMuchSpace();

		waitForNodeJsToFinish(lockFilePath);


		ifstream fin{ "testingNodeJS/public/clickCoordinates.txt" };
		if (!fin)
		{
			cout << "FNFE\n";
			std::cin.get();
		}

		cout << "Coordinates in file: \n";
		string line; 

		while (!fin.eof())
		{
			//int x, y; 
			//fin >> x >> y; 

			//if (fin.fail())
			//{
			//	fin.clear(); 
			//}
			std::getline(fin, line); 

			cout << line << "\n";
//			cout << x << " " << y << "\n";
		}

		killProcessOnPort(3000); 


	}


	return 0; 

}

