#include "FFmpegException.h"

extern "C"
{
#include <libavutil\error.h>
}

bns::FFmpegException::FFmpegException(int errCode)
{
	char errStr[AV_ERROR_MAX_STRING_SIZE] = { 0 };
	av_make_error_string(errStr, AV_ERROR_MAX_STRING_SIZE, errCode);
	desc = errStr;
}

bns::FFmpegException::~FFmpegException()
{
}

const char* bns::FFmpegException::what() const noexcept
{
	return desc.c_str();
}