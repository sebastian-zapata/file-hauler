#include "Syncer.h"
#include "util.h"
#include "Timer.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>


Syncer::Syncer(const Path& srcDirPath, const Path& dstDirPath)
	: m_srcDirPath(srcDirPath)
	, m_dstDirPath(dstDirPath)
{
}


Syncer::~Syncer()
{
}


bool Syncer::sync(bool removeFilesFromSource, ActionDuplicateFile actionDuplicateFile)
{
	///
	/// TODO: verify if file already exists in destination. If so, verify the timestamp and if it's an
	/// old version of the file, replace it.. Or better yet, let the user decide what to do in those
	/// cases.
	///

	namespace fs = std::filesystem;
	using std::cout;

	try
	{
		if (!fs::exists(m_srcDirPath))
		{
			cout << "Source path does not exist or cannot not be accessed" << "\n";
			return false;
		}

		if (!fs::exists(m_dstDirPath))
		{
			cout << "Destination path does not exist or cannot not be accessed" << "\n";
			return false;
		}

		std::vector<Path> srcFilePaths;
		util::getFilesInFolder(m_srcDirPath, srcFilePaths, {/* all file extensions */ }, false, true);

		for (const auto& srcFilePath : srcFilePaths)
		{
			Path filePathRelative = srcFilePath.lexically_relative(m_srcDirPath);
			Path dstFilePath = m_dstDirPath / filePathRelative;
			Path dirPathRelative = filePathRelative.parent_path();
			Path dirPathInDst = m_dstDirPath / dirPathRelative;
			Path dirPathInSrc = m_srcDirPath / dirPathRelative;

			/// create folder in destination if it doesn't exist
			if (!fs::exists(dirPathInDst))
			{
				fs::create_directories(dirPathInDst);
				printTask("Directory creation", dirPathRelative.string());
			}

			/// copy file
	#if 0
			for (size_t i = 0; i <= 100; i++)
			{
				printTask("File syncing", filePathRelative.string(), static_cast<int>(i));

				std::this_thread::sleep_for(std::chrono::milliseconds(25));
			}
	#else
			/// printTask("File syncing", filePathRelative.string(), 0);

			/// open files
			std::ifstream srcFile = std::ifstream(srcFilePath, std::ios::binary);
			std::ofstream dstFile = std::ofstream(dstFilePath, std::ios::binary);

			/// check if files were opened correctly
			{
				if (srcFile.fail())
				{
					std::cout << "Failed to open file: " << srcFilePath.string() << "\n";
					return false;
				}
				if (dstFile.fail())
				{
					std::cout << "Failed to create file: " << dstFilePath.string() << "\n";
					return false;
				}
			}

			/// transfer bytes in chunks
			size_t srcFileSize = static_cast<size_t>(std::filesystem::file_size(srcFilePath));
			{
				if (!copyBytes(srcFile, dstFile, srcFileSize, &Syncer::progressCallback,
					&filePathRelative.string()))
				{
					cout << "Failed to copy bytes from" << srcFilePath.string() << " to "
							<< dstFilePath.string() << "\n";
					return false;
				}
			}

			/// close files
			{
				dstFile.close();
				srcFile.close();
			}

			/// printTask("File syncing", filePathRelative.string(), 100);
	#endif

			/// verify file integrity and delete it from source
			size_t dstFileSize = static_cast<size_t>(std::filesystem::file_size(dstFilePath));
			{
				/// verify integrity
				if (srcFileSize != dstFileSize)
				{
					cout << "Transfer failed. Destination file size does not match with source file size. File will be skipped\n";
					fs::remove(dstFilePath);
					return false;
				}

				/// delete file from source
				if (removeFilesFromSource)
					fs::remove(srcFilePath);
			}

			/// remove folder if empty
			/// ...
			if (dirPathInSrc.empty())
			{
				/// I'm not sure how to approach this
			}
		}
	}
	catch (std::exception e)
	{
		std::cout << "Exception occurred: " << e.what() << "\n";
		return false;
	}

	return true;
}


bool Syncer::copyBytes(
	std::istream&            srcFile,
	std::ofstream&           dstFile,
	const size_t             fileSize,
	TransferProgressCallback progressCallback,
	void*                    progressCallbackOpaquePointer)
{
	if (progressCallback(progressCallbackOpaquePointer, 0.0f, 0, 0))
		return false;

	if (fileSize == 0)
		return true;

	size_t buffer_size = BUFFER_SIZE;

	/// if it's larger, set the buffer size to the size of the data
	if (fileSize < buffer_size)
		buffer_size = fileSize;

	/// start timers
	Timer timerProgress;
	timerProgress.start();
	Timer timerGlobal;
	timerGlobal.start();

	/// read data
	size_t bytesTransferred = 0;
	const size_t total = fileSize - buffer_size;
	for (size_t ptr_offset = 0; ptr_offset < total; ptr_offset += buffer_size)
	{
		char* data = new char[buffer_size];
		try
		{
			srcFile.read(data, buffer_size);
			dstFile.write(data, buffer_size);
		}
		catch (std::exception e)
		{
			std::cout << "\n";
			std::cout << "\n" << "Exception occurred: " << e.what() << "\n";
			std::cout << "\n";
			return false;
		}
		delete[] data;

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

				/// restart bytes transferred for next reading
				bytesTransferred = 0;
			}
		}
	}

	/// read remaining bytes (if any)
	size_t remainder_size = fileSize % buffer_size;
	if (remainder_size == 0)
		remainder_size = buffer_size;
	size_t remainder_ptr_offset = fileSize - remainder_size;
	
	char* data = new char[remainder_size];
	try
	{
		srcFile.read(data, remainder_size);
		dstFile.write(data, remainder_size);
	}
	catch (std::exception e)
	{
		std::cout << "\n";
		std::cout << "\n" << "Exception occurred: " << e.what() << "\n";
		std::cout << "\n";
		return false;
	}
	delete[] data;

	if (progressCallback != nullptr)
	{
		if (progressCallback(progressCallbackOpaquePointer, 1.0f, timerGlobal.elapsedMs(), 0ull))
			return false;
	}

	return true;
}


void Syncer::printTask(const std::string& name, const std::string& detail, const int percentage)
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


bool Syncer::progressCallback(void *opaquePointer, float progress, size_t elapsedMs, size_t bps)
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
