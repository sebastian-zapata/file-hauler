// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FileHauler.
// FileHauler is licensed under the GNU General Public License (GPL), Version 3. If a copy of the GPL
// was not distributed with this file, you can obtain one at https://www.gnu.org/licenses/gpl-3.0.

#include "Hauler.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <tclap/CmdLine.h>
#include "operators.h"
#include "util.h"
#include "../../../modules/version.hpp"


bool parseArgs(
	int   argc,
	char* args[],
	Path& srcDirPath,
	Path& dstDirPath,
	std::vector<std::string>& fileExtensions,
	bool& removeFilesFromSource,
	bool& loop,
	OverwriteAction& overwriteMode,
	bool& verbose);


int main(int argc, char* args[])
{
	using std::cout;
	using std::cin;
	using std::string;
	using std::vector;

	cout << "Welcome to File Hauler!" << "\n";

	Path srcDirPath;
	Path dstDirPath;
	std::vector<std::string> fileExtensions;
	bool removeFilesFromSource = false;
	bool loop = false;
	OverwriteAction overwriteAction = OverwriteAction::Skip;

	bool verbose = true;


	//
	// Process arguments
	//

	if (!parseArgs(argc, args, srcDirPath, dstDirPath, fileExtensions, removeFilesFromSource, loop,
		overwriteAction, verbose))
	{
		return -1;
	}


	//
	// Print args
	//

	cout << "----------------------------------------- Settings -----------------------------------------" << "\n";
	cout << std::left << std::setw(22) << "Source folder: " << srcDirPath.string() << "\n";
	cout << std::left << std::setw(22) << "Destination folder: " << dstDirPath.string() << "\n";
	cout << std::left << std::setw(22) << "File extensions: " << fileExtensions << "\n";
	cout << std::left << std::setw(22) << "Transfer mode: " << (removeFilesFromSource ? "move" : "copy") << "\n";
	cout << std::left << std::setw(22) << "Overwrite action: " << overwriteAction << "\n";
	cout << std::left << std::setw(22) << "Run in loop: " << util::yesNo(loop) << "\n";
	cout << std::left << std::setw(22) << "Verbose: " << util::yesNo(verbose) << "\n";

	cout << "\n";
	cout << "--------------------------------------------------------------------------------------------" << "\n";


	//
	// Run program
	//

	Hauler hauler(srcDirPath, dstDirPath, fileExtensions, verbose);

	if (loop)
	{
		Result lastResult = Result::Code::Default;
		while (true)
		{
			if (lastResult != Result::Code::NoFilesToTransfer)
				cout << "Synchronizing..." << "\r";

			lastResult = hauler.sync(removeFilesFromSource, overwriteAction);

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
		Result result = hauler.sync(removeFilesFromSource, overwriteAction);
		cout << "Finished. Result: " << result << "\n";
		return 0;
	}
}


bool parseArgs(
	int   argc,
	char* args[],
	Path& srcDirPath,
	Path& dstDirPath,
	std::vector<std::string>& fileExtensions,
	bool& removeFilesFromSource,
	bool& loop,
	OverwriteAction& overwriteAction,
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
		CmdLine cmd("", '=', "FileHauler", FILEHAULER_VERSION_STRING);

		// Source directory
		ValueArg<string> argSrcDirPath("s", "src", "Path to source directory", true, "", "string");
		cmd.add(argSrcDirPath);

		// Destination directory
		ValueArg<string> argDstDirPath("d", "dst", "Path to destination directory", true, "", "string");
		cmd.add(argDstDirPath);

		// File extensions
		MultiArg<string> argFileExtensions("f", "file-extensions", "list of file extensions to copy. "
			"Example: \"tif, jpg, png\" (leave empty to copy all files)", false, "string");
		cmd.add(argFileExtensions);

		// Transfer mode
		ValuesConstraint<string> argTransferModeValues({ "move", "copy" });
		ValueArg<string> argTransferMode("t", "transfer-mode", "Transfer mode", true, "move", &argTransferModeValues);
		cmd.add(argTransferMode);

		// Overwrite mode
		ValuesConstraint<string> argOverwriteActionValues({ "skip", "overwrite-if-newer", "always-overwrite" });
		ValueArg<string> argOverwriteAction("o", "overwrite-action", "Overwrite action (what to do when a file "
			"with the same name already exists in destination)", true, "skip", &argOverwriteActionValues);
		cmd.add(argOverwriteAction);

		// Run in loop
		SwitchArg argLoop("l", "loop", "Scan and transfer files repeatedly in an infinite loop", false);
		cmd.add(argLoop);

		// Verbose
		SwitchArg argVerbose("e", "verbose", "", false);
		cmd.add(argVerbose);

		// Parse args
		{
			cmd.parse(argc, args);

			// Source directory
			srcDirPath = argSrcDirPath.getValue();

			// Destination directory
			dstDirPath = argDstDirPath.getValue();
			
			// File extensions
			fileExtensions = argFileExtensions.getValue();

			// Transfer mode
			removeFilesFromSource = argTransferMode.getValue() == "move";

			// Run in loop
			loop = argLoop.getValue();

			// Verbose
			verbose = argVerbose.getValue();

			// Overwrite mode
			if (argOverwriteAction.getValue() == "skip")
			{
				overwriteAction = OverwriteAction::Skip;
			}
			else if (argOverwriteAction.getValue() == "overwrite-if-newer")
			{
				overwriteAction = OverwriteAction::OverwriteIfNewer;
			}
			else if (argOverwriteAction.getValue() == "always-overwrite")
			{
				overwriteAction = OverwriteAction::AlwaysOverwrite;
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