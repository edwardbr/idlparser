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


//This class is used to report to a function whether it has timed out or not

namespace xt
{
	class function_timer
	{
	public:
		function_timer();
		function_timer(long timeOut);
		function_timer(const function_timer& other);
		void operator = (const function_timer& other);
		void reset(long timeOut);

		bool is_timedOut();
		static long m_defaultTimeout;
	private:
#ifdef WIN32 //*NIX
		unsigned long m_startTime;
#else
		timeval m_startTime;
#endif		
		size_t m_timeOut;
	};
}

