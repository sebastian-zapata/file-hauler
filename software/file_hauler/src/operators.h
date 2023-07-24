// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FileHauler.
// FileHauler is licensed under the GNU General Public License (GPL), Version 3. If a copy of the GPL
// was not distributed with this file, you can obtain one at https://www.gnu.org/licenses/gpl-3.0.

#pragma once

#include <vector>
#include <string>
#include <iostream>


template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const std::vector<T>& rhs)
{
	if (rhs.empty())
	{
		stream << "empty";
		return stream;
	}

	for (auto& value : rhs)
	{
		stream << value;
		if (&rhs.back() != &value)
			stream << ", ";
	}

	return stream;
}


inline std::ostream& operator<<(std::ostream& stream, const OverwriteAction& rhs)
{
	switch (rhs)
	{
	case OverwriteAction::Skip:
		stream << "skip";
		break;
	case OverwriteAction::OverwriteIfNewer:
		stream << "overwrite-if-newer";
		break;
	case OverwriteAction::Overwrite:
		stream << "overwrite";
		break;
	default:
		stream << "Invalid OverwriteAction enum\n";
		abort();
		break;
	}

	return stream;
}