#include "Syncer.h"
#include <iostream>


int main()
{
	using std::cout;
	using std::cin;

	cout << "Welcome to File Syncer!" << "\n";

	Path srcDirPath = "\\\\192.168.0.7\\shared";
	Path dstDirPath = "F:\\scans\\shared";
	/// Path dstRootPath = "\\\\192.168.0.6\\test_shared";

	cout << "----------------------------------------- Settings -----------------------------------------" << "\n";
	cout << std::left << std::setw(22) << "Source folder: " << srcDirPath.string() << "\n";
	cout << std::left << std::setw(22) << "Destination folder: " << dstDirPath.string() << "\n";
	cout << "--------------------------------------------------------------------------------------------" << "\n";

	Syncer syncer(srcDirPath, dstDirPath);

	while (true)
	{
		if (!syncer.sync(true, ActionDuplicateFile::AlwaysReplace))
		{
			cout << "Synchronization failed" << "\n";

			while (true)
			{
				std::string userInput;
				cout << "Continue synchronization? (Y/N)" << "\n";
				cin >> userInput;

				if (userInput == "Y" || userInput == "y")
				{
					cout << "Synchronization restarted" << "\n";
					break;
				}
				else if (userInput == "N" || userInput == "n")
				{
					return 0;
				}
				else
				{
					cout << "Invalid input. Please enter \"Y\" or \"N\"" << "\n";
				}
			}
		}
	}
}