// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FileHauler.
// FileHauler is licensed under the GNU General Public License (GPL), Version 3. If a copy of the GPL
// was not distributed with this file, you can obtain one at https://www.gnu.org/licenses/gpl-3.0.

#include "Hauler.h"
#include "util.h"
#include "Timer.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>


Hauler::Hauler(const Path& srcDirPath, const Path& dstDirPath, const std::vector<std::string>& fileExtensions, bool verbose)
	: m_srcDirPath(srcDirPath)
	, m_dstDirPath(dstDirPath)
	, m_fileExtensions(fileExtensions)
	, m_verbose(verbose)
{
}


Hauler::~Hauler()
{
}


Result Hauler::sync(bool removeFilesFromSource, OverwriteAction overwriteAction)
{
	namespace fs = std::filesystem;
	using std::cout;

	bool anyFileTransfered = false;
	try
	{
		if (!fs::exists(m_srcDirPath))
		{
			cout << "Source path does not exist or cannot not be accessed\n";

			return Result::Code::SourceDirectoryDoesNotExist;
		}

		if (!fs::exists(m_dstDirPath))
		{
			cout << "Destination path does not exist or cannot not be accessed\n";

			return Result::Code::DestinationDirectoryDoesNotExist;
		}

		std::vector<Path> srcFilePaths;
		util::getFilesInFolder(m_srcDirPath, srcFilePaths, m_fileExtensions, false, true);

		for (const auto& filePathInSrc : srcFilePaths)
		{
			Path filePathRelative = filePathInSrc.lexically_relative(m_srcDirPath);
			Path filePathInDst = m_dstDirPath / filePathRelative;
			Path dirPathRelative = filePathRelative.parent_path();
			Path dirPathInDst = m_dstDirPath / dirPathRelative;
			Path dirPathInSrc = m_srcDirPath / dirPathRelative;

			// if file in destination already exists:
			if (fs::exists(filePathInDst))
			{
				switch (overwriteAction)
				{
					case OverwriteAction::Skip:
					{
						continue; // skip file, continue to next iteration

						break;
					}
					case OverwriteAction::OverwriteIfNewer:
					{
						bool notNewFile = fs::last_write_time(filePathInDst) >= fs::last_write_time(filePathInSrc);
					
						if (notNewFile)
							continue; // skip file, continue to next iteration

						break;
					}
					case OverwriteAction::Overwrite:
					default:
					{
						// do nothing, continue with the transfer (replace file)
						break;
					}
				}
			}

			// create folder in destination if it doesn't exist
			if (!fs::exists(dirPathInDst))
			{
				fs::create_directories(dirPathInDst);
				printTask("Directory creation", dirPathRelative.string());
			}

			// copy file
	#if 0
			for (size_t i = 0; i <= 100; i++)
			{
				printTask("File syncing", filePathRelative.string(), static_cast<int>(i));

				std::this_thread::sleep_for(std::chrono::milliseconds(25));
			}
	#else
			// printTask("File syncing", filePathRelative.string(), 0);

			// open files
			std::ifstream srcFile;
			std::ofstream dstFile;

			// check if files were opened correctly
			srcFile = std::ifstream(filePathInSrc, std::ios::binary | std::ios::in | std::ios::out);
			if (srcFile.fail())
			{
				if (m_verbose)
					cout << "Failed to open file: " << filePathInSrc.string() << ". Skipping file...\n";

				continue;
				
				/// return Result::Code::CouldNotOpenSourceFile;
			}

			dstFile = std::ofstream(filePathInDst, std::ios::binary | std::ios::out);
			if (dstFile.fail())
			{
				if (m_verbose)
					cout << "Failed to create file: " << filePathInDst.string() << ". Skipping file...\n";

				srcFile.close();

				continue;
				/// return Result::Code::CouldNotCreateDestinationFile;
			}

			// transfer bytes in chunks
			size_t srcFileSize = static_cast<size_t>(std::filesystem::file_size(filePathInSrc));
			if (!copyBytes(srcFile, dstFile, srcFileSize, &Hauler::progressCallback,
				&filePathRelative.string()))
			{
				cout << "Failed to copy bytes from" << filePathInSrc.string() << " to "
						<< filePathInDst.string() << "\n";

				return Result::Code::CouldNotTransferBytes;
			}

			// close files
			{
				dstFile.close();
				srcFile.close();
			}

			// printTask("File syncing", filePathRelative.string(), 100);
	#endif

			// verify file integrity and delete it from source
			size_t dstFileSize = static_cast<size_t>(std::filesystem::file_size(filePathInDst));
			{
				// verify integrity
				if (srcFileSize != dstFileSize)
				{
					cout << "Transfer failed. Destination file size does not match with source file "
						 << "size. Skipping file...\n";
					
					fs::remove(filePathInDst);

					return Result::Code::CorruptTransfer;
				}

				// transfer successful!
				anyFileTransfered = true;

				// delete file from source
				if (removeFilesFromSource)
					fs::remove(filePathInSrc);
			}

			// remove folder if empty
			// ...
			if (dirPathInSrc.empty())
			{
				// I'm not sure how to approach this
			}
		}
	}
	catch (std::exception e)
	{
		std::cout << "Exception occurred: " << e.what() << "\n";
		return Result::Code::Exception;
	}

	return anyFileTransfered ? Result::Code::TransferDone : Result::Code::NoFilesToTransfer;
}


bool Hauler::copyBytes(
	std::istream&            srcFile,
	std::ofstream&           dstFile,
	const size_t             fileSize,
	TransferProgressCallback progressCallback,
	void*                    progressCallbackOpaquePointer)
{
	if (progressCallback != nullptr)
	{
		if (progressCallback(progressCallbackOpaquePointer, 0.0f, 0ull, 0ull))
			return false;
	}

	if (fileSize == 0)
	{
		if (progressCallback != nullptr)
		{
			if (progressCallback(progressCallbackOpaquePointer, 1.0f, 0ull, 0ull))
				return false;
		}

		return true;
	}

	size_t buffer_size = BUFFER_SIZE;

	// if it's larger, set the buffer size to the size of the data
	if (fileSize < buffer_size)
		buffer_size = fileSize;

	// start timers
	Timer timerProgress;
	timerProgress.start();
	Timer timerGlobal;
	timerGlobal.start();

	// read data
	size_t bytesTransferred = 0;
	const size_t total = fileSize - buffer_size;
	for (size_t ptr_offset = 0; ptr_offset < total; ptr_offset += buffer_size)
	{
		std::vector<char> data(buffer_size);
		try
		{
			srcFile.read(data.data(), buffer_size);
			dstFile.write(data.data(), buffer_size);
		}
		catch (std::exception e)
		{
			std::cout << "\n";
			std::cout << "\n" << "Exception occurred: " << e.what() << "\n";
			std::cout << "\n";
			return false;
		}

		bytesTransferred += buffer_size;

		if (progressCallback != nullptr)
		{
			if (timerProgress.elapsedMs() > PROGRESS_CALLBACK_CALL_INTERVAL)
			{
				float progress = (float)ptr_offset / (float)fileSize;
				size_t bps = (size_t)((float)bytesTransferred / ((float)PROGRESS_CALLBACK_CALL_INTERVAL / 1000.0f));
				
				if (progressCallback(progressCallbackOpaquePointer, progress, timerGlobal.elapsedMs(), bps))
					return false;

				timerProgress.start();

				// restart bytes transferred for next reading
				bytesTransferred = 0;
			}
		}
	}

	// read remaining bytes (if any)
	size_t remainder_size = fileSize % buffer_size;
	if (remainder_size == 0)
		remainder_size = buffer_size;
	size_t remainder_ptr_offset = fileSize - remainder_size;
	
	std::vector<char> data(remainder_size);
	try
	{
		srcFile.read(data.data(), remainder_size);
		dstFile.write(data.data(), remainder_size);
	}
	catch (std::exception e)
	{
		std::cout << "\n";
		std::cout << "\n" << "Exception occurred: " << e.what() << "\n";
		std::cout << "\n";
		return false;
	}

	if (progressCallback != nullptr)
	{
		if (progressCallback(progressCallbackOpaquePointer, 1.0f, timerGlobal.elapsedMs(), 0ull))
			return false;
	}

	return true;
}


void Hauler::printTask(const std::string& name, const std::string& detail, const int percentage)
{
	using std::cout;

	const size_t column0Length = 22;
	const size_t column1Length = 58;
	const size_t column1RightPadding = 7;

	cout << std::left << std::setw(column0Length) << name
		<< " | " << std::left << std::setw(column1Length)
		<< util::truncateLeft(detail, column1Length - column1RightPadding);

	if (percentage < 100)
		cout << percentage << "%" << "\r";
	else
		cout << "done" << "\n";

	std::fflush(stdout);
}


bool Hauler::progressCallback(void *opaquePointer, float progress, size_t elapsedMs, size_t bps)
{
	std::string* detail_p = static_cast<std::string*>(opaquePointer);

	if (detail_p == nullptr)
	{
		std::cout << "Task detail can not be null\n";
		abort();
	}

	const unsigned int progressPercentage = static_cast<unsigned int>(progress * 100.0f);

	const float mbps = (float)bps / 1024.0f / 1024.0f;

	const float elapsedSeconds = (float)elapsedMs / 1000.0f;
	const float elapsedMinutes = elapsedSeconds / 60.0f;
	const std::string elapsed = "time: "
		                       + util::to_string(elapsedMs >= 60000ull ? elapsedMinutes : elapsedSeconds, 1)
							   + (elapsedMs >= 60000ull ? "m" : "s");

	const std::string detailMbps = progressPercentage < 100u ? " | "
		                         + (util::to_string(mbps, 1)) + " MB/s" : "";

	std::string detail = *detail_p + " (" + elapsed + detailMbps + ")";

	printTask("File syncing", detail, progressPercentage);

	return false;
}
