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

