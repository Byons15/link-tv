#pragma once
#include <functional>

namespace bns
{
	class EndCaller
	{
	public:

		EndCaller()
		{
			//pass
		}

		template<typename CloseFn, typename ...Args>
		EndCaller(CloseFn&& fn, Args&& ...args)
		{
			endCall = std::bind(std::forward<CloseFn>(fn), std::forward<Args>(args)...);
		}

		void swap(EndCaller& other) noexcept
		{
			std::swap(endCall, other.endCall);
		}

		~EndCaller()
		{
			if(endCall)
				endCall();
		}

	private:
		std::function<void()> endCall;
	};
}