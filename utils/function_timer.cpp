#include <stdio.h> 
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <fstream>

#include "function_timer.h"

namespace xt
{
	std::chrono::milliseconds function_timer::m_defaultTimeout(1000);

	function_timer::function_timer() :
		m_timeOut(std::chrono::milliseconds(m_defaultTimeout)),	
		m_startTime(std::chrono::system_clock::now())
	{}

	function_timer::function_timer(long timeOut) :
		m_timeOut(std::chrono::milliseconds(timeOut)),	
		m_startTime(std::chrono::system_clock::now())
	{}

	function_timer::function_timer(const function_timer& other) : 
		m_startTime(other.m_startTime),
		m_timeOut(other.m_timeOut)
	{}

	void function_timer::operator = (const function_timer& other)
	{
		m_startTime = other.m_startTime;
		m_timeOut = other.m_timeOut;
	}

	void function_timer::reset(long timeOut)
	{
		m_timeOut = std::chrono::milliseconds(timeOut);
		m_startTime = std::chrono::system_clock::now();
	}


	bool function_timer::is_timedOut()
	{
		if(m_timeOut == std::chrono::milliseconds(0))
		{
			return false;
		}

		auto endTime = std::chrono::system_clock::now();
		if(endTime - m_startTime > m_timeOut)
		{
			return true;
		}
		return false;
	}
}
