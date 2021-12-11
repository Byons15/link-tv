#pragma once
#include <stdexcept>
#include <string>

namespace bns 
{
	class FFmpegException
		:std::exception
	{
	public:
		FFmpegException(int errCode);

		~FFmpegException();

		virtual const char* what() const noexcept override;

	private:
		std::string desc;
	};

	inline void CheckAVResult(int result)
	{
		if (result < 0)
			throw FFmpegException(result);
	}
}
