#pragma once
#include <filesystem>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>


namespace util
{

template <class Iterator>
inline void getFilesInFolder(
	const std::filesystem::path& folderPath,
	std::vector<std::filesystem::path>& foundFilePaths,
	const std::vector<std::string>& extensions,
	const bool includeDirectories)
{
	namespace fs = std::filesystem;

	for (const fs::directory_entry &p : Iterator(folderPath))
	{
		if (!includeDirectories && fs::is_directory(p.path()))
		{
			continue;
		}

		if (extensions.empty())
		{
			foundFilePaths.push_back(p.path());
		}
		else
		{
			for (const std::string& extension : extensions)
			{
				if (p.path().extension() == ("." + extension))
				{
					foundFilePaths.push_back(p.path());
				}
			}
		}
	}
}


inline void getFilesInFolder(
	const std::filesystem::path& folderPath,
	std::vector<std::filesystem::path>& foundFilePaths,
	const std::vector<std::string>& extensions,
	const bool includeDirectories,
	const bool recursive)
{
	namespace fs = std::filesystem;
	typedef fs::directory_iterator Itr;
	typedef fs::recursive_directory_iterator RecurItr;

	if (recursive)
		getFilesInFolder<RecurItr>(folderPath, foundFilePaths, extensions, includeDirectories);
	else
		getFilesInFolder<Itr>(folderPath, foundFilePaths, extensions, includeDirectories);
}


inline std::string truncateLeft(const std::string& str, const size_t maxLength)
{
	if (str.size() <= maxLength)
		return str;

	std::string result;
	size_t offset = str.size() - maxLength + 3;
	result = str.substr(offset, maxLength);
	result = "..." + result;

	return result;
}


template <typename T>
inline std::string to_string(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}


inline std::string yesNo(bool x)
{
	return x ? "yes" : "no";
}


}