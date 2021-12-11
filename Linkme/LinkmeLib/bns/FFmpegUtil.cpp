#include "FFmpegUtil.h"

namespace bns
{
	void AVCodecContextDeleter(AVCodecContext* p) noexcept
	{
		avcodec_free_context(&p);
	}

	void AVFrameDeleter(AVFrame* p) noexcept
	{
		av_frame_free(&p);
	}

	void AVPacketDeleter(AVPacket* p) noexcept
	{
		av_packet_free(&p);
	}

	void AVFilterInOutDeleter(AVFilterInOut* p) noexcept
	{
		avfilter_inout_free(&p);
	}

	void AVFilterGraphDeleter(AVFilterGraph* p) noexcept
	{
		avfilter_graph_free(&p);
	}

	void AVFilterContextDeleter(AVFilterContext* p) noexcept
	{
		avfilter_free(p);
	}

	void AVFormatContextDeleter(AVFormatContext* p) noexcept
	{
		av_write_trailer(p);
		av_freep(p->pb->buffer);
		avformat_free_context(p);
	}

	void AVBSFContextDeleter(AVBSFContext* ctx) noexcept
	{
		av_bsf_free(&ctx);
	}

}