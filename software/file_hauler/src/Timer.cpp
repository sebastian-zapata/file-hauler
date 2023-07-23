// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FileHauler.
// FileHauler is licensed under the GNU General Public License (GPL), Version 3. If a copy of the GPL
// was not distributed with this file, you can obtain one at https://www.gnu.org/licenses/gpl-3.0.

#include "Timer.h"


Timer::Timer()
{
}


Timer::~Timer()
{
}


void Timer::start()
{
	m_time = std::chrono::steady_clock::now();
}


void Timer::stop()
{
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	m_elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (currentTime - m_time).count();
}


uint64_t Timer::elapsedMs()
{
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds> (currentTime - m_time).count();
}


float Timer::elapsedS()
{
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	return (float)(std::chrono::duration_cast<std::chrono::milliseconds> (currentTime - m_time).count()) / 1000.0f;
}