#include "Syncer.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <tclap/CmdLine.h>


bool processArgs(
	int   argc,
	char* args[],
	Path& srcDirPath,
	Path& dstDirPath,
	bool& removeFilesFromSource,
	bool& loop,
	OverwriteMode& overwriteMode,
	bool& verbose);


int main(int argc, char* args[])
{
	using std::cout;
	using std::cin;
	using std::string;
	using std::vector;

	cout << "Welcome to File Syncer!" << "\n";

	Path srcDirPath;
	Path dstDirPath;
	bool removeFilesFromSource = false;
	bool loop = false;
	OverwriteMode overwriteMode = OverwriteMode::Skip;

	bool verbose = true;


	//
	// process cli
	//

	if (!processArgs(argc, args, srcDirPath, dstDirPath, removeFilesFromSource, loop, overwriteMode, verbose))
	{
		return -1;
	}


	//
	// Print args
	//

	cout << "----------------------------------------- Settings -----------------------------------------" << "\n";
	cout << std::left << std::setw(22) << "Source folder: " << srcDirPath.string() << "\n";
	cout << std::left << std::setw(22) << "Destination folder: " << dstDirPath.string() << "\n";
	cout << "--------------------------------------------------------------------------------------------" << "\n";


	//
	// run program
	//

	Syncer syncer(srcDirPath, dstDirPath, verbose);

	if (loop)
	{
		Result lastResult = Result::Code::Default;
		while (true)
		{
			if (lastResult != Result::Code::NoFilesToTransfer)
				cout << "Synchronizing..." << "\r";

			lastResult = syncer.sync(removeFilesFromSource, overwriteMode);

			if (lastResult == Result::Code::SourceDirectoryDoesNotExist
			 || lastResult == Result::Code::DestinationDirectoryDoesNotExist)
			{
				cout << "Synchronization failed" << "\n";

				while (true)
				{
					std::string userInput;
					cout << "Restart synchronization? (Y/N)" << "\n";
					cin >> userInput;

					if (userInput == "Y" || userInput == "y")
					{
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

			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
	}
	else
	{
		Result result = syncer.sync(removeFilesFromSource, overwriteMode);
		cout << "Finished. Result: " << result << "\n";
		return 0;
	}
}


bool processArgs(
	int   argc,
	char* args[],
	Path& srcDirPath,
	Path& dstDirPath,
	bool& removeFilesFromSource,
	bool& loop ,
	OverwriteMode& overwriteMode,
	bool& verbose)
{
	using std::cout;
	using std::cin;
	using std::string;
	using std::vector;

	try
	{
		using namespace TCLAP;

		// Define the command line object.
		CmdLine cmd("", '=', _OUTPUTNAME, _VERSION);

		// Source directory
		ValueArg<string> argSrcDirPath("s", "src", "Path to source directory", true, "", "string");
		cmd.add(argSrcDirPath);

		// Destination directory
		ValueArg<string> argDstDirPath("d", "dst", "Path to destination directory", true, "", "string");
		cmd.add(argDstDirPath);

		// Transfer mode
		ValuesConstraint<string> argTransferModeValues({ "move", "copy" });
		ValueArg<string> argTransferMode("t", "transfer-mode", "Transfer mode", true, "move", &argTransferModeValues);
		cmd.add(argTransferMode);

		// Overwrite mode
		ValuesConstraint<string> argOverwriteActionValues({ "skip", "overwrite-if-newer", "always-overwrite" });
		ValueArg<string> argOverwriteAction("o", "overwrite-mode", "Overwrite mode (what to do when a file "
			"with the same name already exists in destination)", true, "skip", &argOverwriteActionValues);
		cmd.add(argOverwriteAction);

		// Run in loop
		TCLAP::SwitchArg argLoop("l", "loop", "Scan and transfer files repeatedly in an infinite loop", false);
		cmd.add(argLoop);

		// Verbose
		TCLAP::SwitchArg argVerbose("e", "verbose", "", false);
		cmd.add(argVerbose);

		// Parse args
		{
			cmd.parse(argc, args);

			// Source directory
			srcDirPath = argSrcDirPath.getValue();

			// Destination directory
			dstDirPath = argDstDirPath.getValue();

			// Transfer mode
			removeFilesFromSource = argTransferMode.getValue() == "move";

			// Run in loop
			loop = argLoop.getValue();

			// Verbose
			verbose = argVerbose.getValue();

			// Overwrite mode
			if (argOverwriteAction.getValue() == "skip")
			{
				overwriteMode = OverwriteMode::Skip;
			}
			else if (argOverwriteAction.getValue() == "overwrite-if-newer")
			{
				overwriteMode = OverwriteMode::OverwriteIfNewer;
			}
			else if (argOverwriteAction.getValue() == "always-overwrite")
			{
				overwriteMode = OverwriteMode::AlwaysOverwrite;
			}
		}
	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << "\n";
		return false;
	}

	return true;
}