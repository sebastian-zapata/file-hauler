#pragma once

#include <vector>
#include <string>
#include <iostream>


template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const std::vector<T>& rhs)
{
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
	case OverwriteAction::AlwaysOverwrite:
		stream << "always-overwrite";
		break;
	default:
		stream << "Invalid OverwriteAction enum\n";
		abort();
		break;
	}

	return stream;
}