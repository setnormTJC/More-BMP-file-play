#include "NodeJSFunctions.h"

void callNodeJS()
{
	//call node server.js IN A THREAD! - program execution "stalls" otherwise 
	std::thread nodeServerThread([]()
		{
			system("node testingNodeJS/server.js");
		});
	nodeServerThread.detach();
}

void openPort3000_andDisplayChessBoard()
{
	std::thread browserThread([]()
		{
			//open port 3000 with browser (displays board image via NodeJS and an HTML file) 
			system("start http://localhost:3000");

		});

	browserThread.detach();
}



void killProcessOnPort(int port)
{

	// Create the command to find the process ID using the specified port
	std::string command = "netstat -ano | findstr :" + std::to_string(port) + " > portinfo.txt";
	//note the "cat" use! > direct contents into file 
	std::system(command.c_str());

	// Open the file containing the port information
	std::ifstream portInfoFile("portinfo.txt");
	std::string line;
	std::string processID;

	// Parse the file to find the process ID
	if (portInfoFile.is_open())
	{
		while (std::getline(portInfoFile, line))
		{
			// The process ID is the last element in the line
			size_t lastSpace = line.find_last_of(" ");
			if (lastSpace != std::string::npos)
			{
				processID = line.substr(lastSpace + 1);
				break;
			}
		}
		portInfoFile.close();
	}

	// If a process ID was found, kill the process
	if (!processID.empty())
	{
		std::string killCommand = "taskkill /PID " + processID + " /F";
		std::system(killCommand.c_str());
	}
	else
	{
		std::cout << "No process found using port " << port << std::endl;
	}

	// Clean up the temporary file
	std::remove("portinfo.txt");

}


array<pair<int, int>, 2>  readFileAndReturnCoordinatesClickedInBrowser()
{

	string coordinateFileName = "testingNodeJS/public/clickCoordinates.txt";
	array<pair<int, int>, 2> theTwoCoordinates;

	ifstream fin{ coordinateFileName };
	if (!fin)
	{
		cout << coordinateFileName << " not found\n";
		return theTwoCoordinates;
	}

	else
	{
		//cout << "file " << coordinateFileName << " opened successfully\n";
	}
	killProcessOnPort(3000); //process should be killed if file was able to be opened successfully: 

	//cout << "Coordinates in file: \n";

	fin.clear();
	fin.seekg(0, std::ios::beg);

	fin >> theTwoCoordinates.at(0).first;
	//cout << theTwoCoordinates.at(0).first << "\n";
	fin.ignore(1); //ignore the comma separating x and y
	fin >> theTwoCoordinates.at(0).second;
	//ignore newline? 
	fin.ignore(1);
	fin >> theTwoCoordinates.at(1).first;
	fin.ignore(1);
	fin >> theTwoCoordinates.at(1).second;


	fin.clear();

	//for (auto& coordinate : theTwoCoordinates)
	//{
	//	cout << coordinate.first << "," << coordinate.second << "\n";
	//}

	fin.close();

	std::remove(coordinateFileName.c_str()); //delete file so that Node JS MUST create it 


	return theTwoCoordinates;
}
