// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FileHauler.
// FileHauler is licensed under the GNU General Public License (GPL), Version 3. If a copy of the GPL
// was not distributed with this file, you can obtain one at https://www.gnu.org/licenses/gpl-3.0.

#pragma once

#include <string>


class Result
{

public:
	
	enum class Code
	{
		Default,
		NoFilesToTransfer,
		TransferDone,
		SourceDirectoryDoesNotExist,
		DestinationDirectoryDoesNotExist,
		CouldNotOpenSourceFile,
		CouldNotCreateDestinationFile,
		CouldNotTransferBytes,
		CorruptTransfer,
		Exception,
	};

public:

	Result();

	Result(Code code);

	Result(Code code, const std::string& messageDetails);

	~Result();

public:

	bool operator==(const Result::Code rhs) const
	{
		return this->code() == rhs;
	}

	bool operator!=(const Result::Code rhs) const
	{
		return this->code() != rhs;
	}

	friend std::ostream& operator<<(std::ostream& stream, const Result& rhs)
	{
		stream << rhs.message();
		return stream;
	}

	operator std::string() const { return message(); }

public:

	Code code() const;

	std::string message() const;

private:

	void setMessageDetails(const std::string& messageDetails);

private:

	Code m_code;

	std::string m_messageDetails;

	friend class Session;

};