#include "Result.h"
#include <iostream>


Result::Result()
	: m_code(Code::TransferDone)
	, m_messageDetails("")
{
}


Result::Result(Code code)
	: m_code(code)
	, m_messageDetails("")
{
}


Result::Result(Code code, const std::string& messageDetails)
	: m_code(code)
	, m_messageDetails(messageDetails)
{
}


Result::~Result()
{
}


Result::Code Result::code() const
{
	return m_code;
}


std::string Result::message() const
{
	std::string message;

	switch (m_code)
	{
	case Code::NoFilesToTransfer:
		message = "No transfer was performed";
		break;
	case Code::TransferDone:
		message = "Transfer done";
		break;
	case Code::SourceDirectoryDoesNotExist:
		message = "Source directory does not exist";
		break;
	case Code::DestinationDirectoryDoesNotExist:
		message = "Destination directory does not exist";
		break;
	case Code::CouldNotOpenSourceFile:
		message = "Could not open source file";
		break;
	case Code::CouldNotCreateDestinationFile:
		message = "Could not create destination file";
		break;
	case Code::CouldNotTransferBytes:
		message = "Could not transfer bytes";
		break;
	case Code::CorruptTransfer:
		message = "Corrupt transfer";
		break;
	case Code::Exception:
		message = "Runtime exception occured";
		break;
	default:
		std::cout << "Invalid result code.\n";
	}

	return message + (m_messageDetails.empty() ? "" : ": ") + m_messageDetails;
}


void Result::setMessageDetails(const std::string& messageDetails)
{
	m_messageDetails = messageDetails;
}