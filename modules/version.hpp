// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FileHauler.
// FileHauler is licensed under the GNU General Public License (GPL), Version 3. If a copy of the GPL
// was not distributed with this file, you can obtain one at https://www.gnu.org/licenses/gpl-3.0.

#pragma once

#include <ostream>

#define FILEHAULER_VERSION_MAJOR 1
#define FILEHAULER_VERSION_MINOR 0
#define FILEHAULER_VERSION_PATCH 0

// Preprocessor for string conversion
#define FILEHAULER_TO_STRING_HELPER(x) #x
#define FILEHAULER_TO_STRING(x) FILEHAULER_TO_STRING_HELPER(x)

// Get version as string. For example "5.2.0"
#define FILEHAULER_VERSION_STRING FILEHAULER_TO_STRING(FILEHAULER_VERSION_MAJOR) "." \
                                 FILEHAULER_TO_STRING(FILEHAULER_VERSION_MINOR) "." \
                                 FILEHAULER_TO_STRING(FILEHAULER_VERSION_PATCH)

namespace filehauler {

class Version
{
public:

	Version()
		: m_major(FILEHAULER_VERSION_MAJOR)
		, m_minor(FILEHAULER_VERSION_MINOR)
		, m_patch(FILEHAULER_VERSION_PATCH)
	{
	}

    Version(int major, int minor, int patch)
        : m_major(major)
        , m_minor(minor)
        , m_patch(patch)
    {
    }

    bool operator<(const Version& other) const
    {
		if (*this == other)
			return false;

		if (m_major < other.m_major)
			return true;

		if (m_major > other.m_major)
			return false;

		if (m_minor < other.m_minor)
			return true;

		if (m_minor > other.m_minor)
			return false;

		if (m_patch < other.m_patch)
			return true;

		if (m_patch > other.m_patch)
			return false;

		return false;
    }

	bool operator>(const Version& other) const
	{
		if (*this == other)
			return false;

		if (m_major > other.m_major)
			return true;

		if (m_major < other.m_major)
			return false;

		if (m_minor > other.m_minor)
			return true;

		if (m_minor < other.m_minor)
			return false;

		if (m_patch > other.m_patch)
			return true;

		if (m_patch < other.m_patch)
			return false;

		return false;
	}

	bool operator==(const Version& other) const
	{
		if (m_major != other.m_major
		 || m_minor != other.m_minor
		 || m_patch != other.m_patch)
		{
			return false;
		}

		return true;
	}

	bool operator!=(const Version& other) const
	{
		return !(*this == other);
	}

	friend std::ostream& operator<<(std::ostream& os, const Version& version)
	{
		os << version.m_major << "." << version.m_minor << "." << version.m_patch;
		return os;
	}

private:

    int m_major;

    int m_minor;

    int m_patch;

};

}
