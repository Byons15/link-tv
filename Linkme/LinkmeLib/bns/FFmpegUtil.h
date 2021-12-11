
#ifndef _BNFFMPEGUTIL_H_
#define _BNFFMPEGUTIL_H_

extern "C"
{
#include <libavcodec\avcodec.h>
#include <libavformat\avformat.h>
#include <libavformat\avio.h>
#include <libavfilter\avfilter.h>
}

namespace bns
{
	void AVCodecContextDeleter(AVCodecContext* p) noexcept;

	using AVCodecContextDeleter_t = void(*)(AVCodecContext*) noexcept;

	void AVFrameDeleter(AVFrame* p) noexcept;

	using AVFrameDeleter_t = void(*)(AVFrame*) noexcept;

	void AVPacketDeleter(AVPacket* p) noexcept;

	using AVPacketDeleter_t = void(*)(AVPacket*) noexcept;

	void AVFilterInOutDeleter(AVFilterInOut* p) noexcept;

	using AVFilterInOutDeleter_t = void(*)(AVFilterInOut*) noexcept;

	void AVFilterGraphDeleter(AVFilterGraph* p) noexcept;

	using AVFilterGraphDeleter_t = void(*)(AVFilterGraph*) noexcept;

	void AVFilterContextDeleter(AVFilterContext* p) noexcept;

	using AVFilterContextDeleter_t = void(*)(AVFilterContext*) noexcept;

	void AVFormatContextDeleter(AVFormatContext* p) noexcept;

	using AVFormatContextDeleter_t = void(*)(AVFormatContext*) noexcept;

	void AVBSFContextDeleter(AVBSFContext* ctx) noexcept;

	using AVBSFContextDeleter_t = void(*)(AVBSFContext*) noexcept;
}

#endif // !_BNFFMPEGUTIL_H_


