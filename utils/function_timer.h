//This class is used to report to a function whether it has timed out or not

#include <chrono>

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
		static std::chrono::milliseconds m_defaultTimeout;
	private:
		std::chrono::time_point<std::chrono::system_clock> m_startTime;
		std::chrono::milliseconds m_timeOut;
	};
}

