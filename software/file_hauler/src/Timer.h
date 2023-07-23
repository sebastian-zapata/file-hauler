// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FileHauler.
// FileHauler is licensed under the GNU General Public License (GPL), Version 3. If a copy of the GPL
// was not distributed with this file, you can obtain one at https://www.gnu.org/licenses/gpl-3.0.

#pragma once

#include <chrono>

class Timer
{
public:

	Timer();

	~Timer();

public:

	void start();

	void stop();

	uint64_t elapsedMs();
	
	float elapsedS();

private:

	std::chrono::steady_clock::time_point m_time;
	
	uint64_t m_elapsed;

};

