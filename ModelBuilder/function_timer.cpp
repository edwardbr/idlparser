/*
   Copyright (c) 2010 Edward Boggis-Rolfe.
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.*/

#include "stdafx.h"

#include "function_timer.h"
#include "time.h"

namespace xt
{
	long function_timer::m_defaultTimeout = 1000;

	function_timer::function_timer() :
		m_timeOut(m_defaultTimeout)
#ifdef WIN32 //*NIX
		,	m_startTime(GetTickCount())
#endif
	{
#ifndef WIN32 //*NIX
		gettimeofday(&m_startTime, NULL);
#endif
	}

	function_timer::function_timer(long timeOut) :
		m_timeOut(timeOut)
#ifdef WIN32 //*NIX
		,	m_startTime(GetTickCount())
#endif
	{
#ifndef WIN32 //*NIX
		gettimeofday(&m_startTime, NULL);
#endif
	}

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
		m_timeOut = timeOut;
#ifdef WIN32 //*NIX
		m_startTime = GetTickCount();
#else //*NIX
		gettimeofday(&m_startTime, NULL);
#endif
	}


	bool function_timer::is_timedOut()
	{
		if(m_timeOut == 0)
		{
			return false;
		}

#ifdef WIN32 //*NIX
		unsigned long endTime = GetTickCount();
		if(endTime - m_startTime > m_timeOut)
		{
			return true;
		}
#else
		timeval endTime;
		gettimeofday(&endTime, NULL);
		long diff = (endTime.tv_sec - m_startTime.tv_sec)*1000 + endTime.tv_usec/1000 - m_startTime.tv_usec/1000;
		if(diff > m_timeOut)
		{
			return true;
		}
#endif
		return false;
	}
}
