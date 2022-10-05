#pragma once

#include "Result.h"
#include <filesystem>


enum class OverwriteMode
{
	Skip,                 /// do not transfer the file (do not replace)
	OverwriteIfNewer,     /// only replace if the file being copied is newer
	AlwaysOverwrite,      /// replace file in destination no matter what
};

typedef std::filesystem::path Path;

class Syncer
{
private:

	/// if the progress callback call interval is too small, it won't be able to update the bps accurately.
	/// please use values greater than 1000 ms..
	const size_t PROGRESS_CALLBACK_CALL_INTERVAL = 2000; // in ms

	/// size of each chunk that will be transferred at a time
	const size_t BUFFER_SIZE = 1024 * 1024; /// in bytes

	/// function to be called every time progress is updated
	typedef bool(*TransferProgressCallback)(void*  opaquePointer,
											float  progress,
											size_t elapsedMs,
											size_t bytesPerSecond);

public:

	Syncer(const Path& srcDirPath, const Path& dstDirPath, bool verbose = false);

	~Syncer();

public:

	Result sync(bool removeFilesFromSource, OverwriteMode actionDuplicateFile);

private:

	bool copyBytes(std::istream&            srcFile,
		           std::ofstream&           dstFile,
		           const uintmax_t          fileSize,
		           TransferProgressCallback progressCallback = nullptr,
		           void*                    progressCallbackOpaquePointer = nullptr);

	static void printTask(
		const std::string& name,
		const std::string& detail,
		const int          percentage = 100);

	static bool progressCallback(void* opaquePointer, float progress, size_t elapsedMs, size_t bps);

private:

	const Path& m_srcDirPath;

	const Path& m_dstDirPath;

	bool m_verbose;

};

